#include "strm/SpatioTemporalRoIMixer.hpp"

#include <cmath>
#include <memory>
#include <numeric>
#include <utility>

#include "strm/FrameBuffer.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/Interpolator.hpp"
#include "strm/Logger.hpp"
#include "strm/MixedFrame.hpp"
#include "strm/RoIExtractor.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/PatchReconstructor.hpp"

namespace rm {

const int SpatioTemporalRoIMixer::FULL_KEY_OFFSET = 1000000;

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               std::map<int, int> startIndices,
                                               JavaVM* vm, JNIEnv* env, jobject emulator)
    : mConfig(config), mbStop(false),
      mResultLogger(new Logger("/data/data/hcs.offloading.strm/boxes.txt")),
      mStartIndices(std::move(startIndices)),
      mTargetSize(int(mConfig.roIExtractorConfig.EXTRACTION_RESIZE_WIDTH),
                  int(mConfig.roIExtractorConfig.EXTRACTION_RESIZE_HEIGHT)),
      mInputSizes(mConfig.inferenceEngineConfig.INPUT_SIZES),
      mScheduleInterval(mConfig.LATENCY_SLO_MS * 1000 / 2),
      mRoIResizer(new RoIResizer(config.roiResizerConfig)),
      mPatchMixer(new PatchMixer(config.patchMixerConfig)),
      mInferenceEngine(new InferenceEngine(config.inferenceEngineConfig, vm, env, emulator)),
      mPatchReconstructor(new PatchReconstructor(
          config.patchReconstructorConfig, mRoIResizer.get())) {
  assert(!config.ROI_WISE_INFERENCE || mInputSizes.size() >= 2);
  if (config.LOG_EXECUTION) {
    mExecutionLogger = std::make_unique<Logger>("/data/data/hcs.offloading.strm/timeline.csv");
    mExecutionLogger->logExecutionHeader();
  }
  if (config.LOG_ROI) {
    mRoILogger = std::make_unique<Logger>("/data/data/hcs.offloading.strm/roi.csv");
    mRoILogger->logRoIHeader();
  }

  mThread = std::thread([this]() { work(); });
  mResultThread = std::thread([this]() { outputWork(); });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mResultsCv.notify_all();
  mThread.join();
  mResultThread.join();
}

void SpatioTemporalRoIMixer::work() {
  LOGD("STRM::work()");
  int scheduleID = 0;
  int fullFrameStreamIndex = 0;

  // Wait sources for synced start
  waitForStart();

  TimeLogger logger;
  logger.start();

  // When FULL_FRAME_INTERVAL == 0, always run full frame inference
  // See SpatioTemporalRoIMixer::enqueueImage(...)
  while (!mbStop && mConfig.FULL_FRAME_INTERVAL > 0) {
    scheduleID++;
    // Wait for scheduling interval
    time_us elapsedTime = logger.getElapsedTime();
    if (mConfig.ALLOW_INTERPOLATION && mScheduleInterval > elapsedTime) {
      std::this_thread::sleep_for(std::chrono::microseconds(mScheduleInterval - elapsedTime));
    }

    LOGD("===== Schedule %d start =====", scheduleID);
    // Extract RoIs
    logger.start();
    auto latencyTable = mInferenceEngine->getInferenceTimeTable();
    bool runFull = scheduleID % mConfig.FULL_FRAME_INTERVAL == 0;
    std::map<Device, std::pair<time_us, time_us>> startEndTime;
    for (Device device : mConfig.inferenceEngineConfig.DEVICES) {
      startEndTime[device] = {
          (runFull && device == GPU) ? latencyTable[GPU][mInputSizes.back()] : 0,
          mScheduleInterval};
    }
    std::vector<InferenceInfo> inferencePlan = getInferencePlan(
        startEndTime, latencyTable, mConfig.ROI_WISE_INFERENCE);
    assert(!inferencePlan.empty());
    logger.step("plan");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         "STRM::getInferencePlan", logger.getDuration("plan"), toString(inferencePlan).c_str());
    MultiStream frames = mRoIExtractor->getExtractedFrames(inferencePlan);
    logger.step("roi");
    LOGD("%-25s took %-7lld us for %s",
         "RE::getExtractedFrames", logger.getDuration("roi"), toString(frames).c_str());
    if (std::all_of(frames.begin(), frames.end(), [](auto& it) { return it.second.empty(); })) {
      LOGD("===== Schedule %d end with no RoIs =====", scheduleID);
      continue;
    }

    // Schedule
    elapsedTime = logger.getElapsedTime();
    for (Device device : mConfig.inferenceEngineConfig.DEVICES) {
      startEndTime[device] = {
          elapsedTime + ((runFull && device == GPU) ? latencyTable[GPU][mInputSizes.back()] : 0),
          mScheduleInterval};
    }
    auto[mixedFrames, fullFrameTarget, selectedFrames, droppedFrames] = mPatchMixer->packRoIs(
        frames, (runFull ? fullFrameStreamIndex++ : -1),
        getInferencePlan(startEndTime, latencyTable, mConfig.ROI_WISE_INFERENCE),
        mConfig.ALLOW_INTERPOLATION, mConfig.ROI_WISE_INFERENCE, mRoIResizer.get());
    assert(runFull == (fullFrameTarget != nullptr));
    if (!mConfig.ALLOW_INTERPOLATION) {
      testNoInterpolationPacking(frames, droppedFrames, fullFrameTarget);
    }
    mRoIExtractor->reEnqueueFrames(droppedFrames);
    logger.step("pack");
    LOGD("%-25s took %-7lld us                            // %4lu MixedFrames",
         "PatchMixer::packRoIs", logger.getDuration("pack"), mixedFrames.size());
    // Full Frame Inference Target Stream
    if (runFull) {
      fullFrameInference(fullFrameTarget);
    }
    logger.step("full");
    LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
         "STRM::fullFrameInference", logger.getDuration("full"),
         fullFrameTarget != nullptr ? fullFrameTarget->vid : -1,
         fullFrameTarget != nullptr ? fullFrameTarget->frameIndex : -1);

    // Inference
    if (mConfig.ROI_WISE_INFERENCE) {
      roiWiseInference(mixedFrames);
    } else {
      mixedInference(mixedFrames);
    }
    logger.step("inf");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         mConfig.ROI_WISE_INFERENCE ? "STRM::roiWiseInference" : "STRM:mixedFrameInference",
         logger.getDuration("inf"), toString(inferencePlan).c_str());

    // Interpolate results
    std::set<idType> droppedIDs = Interpolator::interpolate(selectedFrames);
    logger.step("itp");
    LOGD("%-25s took %-7lld us                            // %4lu droppedIDs",
         "Interpolator::interpolate", logger.getDuration("itp"), droppedIDs.size());

    // Notify results of rest of the frames
    for (auto& it : selectedFrames) {
      for (Frame* frame : it.second) {
        for (auto& cRoI : frame->childRoIs) {
          if (droppedIDs.find(cRoI->id) != droppedIDs.end()) {
            continue;
          }
          assert(cRoI->box != nullptr);
          assert(cRoI->box->srcRoI == cRoI.get());
          assert(cRoI->box->id == cRoI->id);
          assert(cRoI->box->label == cRoI->label);
        }
        frame->isBoxesReady = true;
        frame->endTime = NowMicros();
      }
    }
    mRoIExtractor->notify();

    // Update results for system output
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    for (const auto& it : selectedFrames) {
      for (Frame* frame : it.second) {
        if (frame != fullFrameTarget) {
          std::vector<BoundingBox> boxes;
          std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(boxes),
                         [](const std::unique_ptr<BoundingBox>& box) { return *box; });
          mResults[frame->vid][frame->frameIndex] = {
              frame->endTime, nms(boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold())};
        }
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    // Release used frames
    releaseFrames(selectedFrames);
    logger.step("post");

    LOGD("===== Schedule %d end (%s) =====", scheduleID, logger.getLog().c_str());
  }
}

void SpatioTemporalRoIMixer::testNoInterpolationPacking(
    const MultiStream& frames, const Stream& droppedFrames, Frame* fullFrameTarget) {
  int numWrong = 0;
  int numCorrect = 0;
  for (const auto& it: frames) {
    for (Frame* frame : it.second) {
      if (!std::all_of(frame->parentRoIs.begin(), frame->parentRoIs.end(),
                       [](const std::unique_ptr<RoI>& pRoI) { return pRoI->isPacked(); })) {
        if (droppedFrames.find(frame) == droppedFrames.end() && frame != fullFrameTarget) {
          numWrong++;
        }
      } else {
        numCorrect++;
      }
    }
  }
  if (numCorrect > 0) {
    assert(numWrong == 0);
  } else {
    assert(numWrong <= 1);
  }
}

void SpatioTemporalRoIMixer::fullFrameInference(Frame* frame) {
  assert(frame->isFullFrameTarget);
  int key = frame->frameIndex + FULL_KEY_OFFSET;
  int fullFrameSize = mInputSizes.back();
  mInferenceEngine->enqueue(frame->mat, GPU, fullFrameSize, key);
  auto[boxes, times, device] = mInferenceEngine->getResults(key);
  frame->inferenceFrameSize = fullFrameSize;
  frame->inferenceDevice = device;
  frame->fullInferenceStartTime = times.first;
  frame->fullInferenceEndTime = times.second;
  for (const BoundingBox& box : boxes) {
    frame->boxes.push_back(std::make_unique<BoundingBox>(
        UNASSIGNED_ID, box.location, box.confidence, box.label, origin_FF));
  }
  mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, true);

  for (auto& box : frame->boxes) {
    assert(box->id != UNASSIGNED_ID);
  }
  frame->isBoxesReady = true;
  if (mRoIExtractor != nullptr) {
    mRoIExtractor->notify();
  }

  assert(std::all_of(frame->boxes.begin(), frame->boxes.end(),
                     [](const std::unique_ptr<BoundingBox>& box) { return box->label == 0; }));

  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  std::vector<BoundingBox> resultBoxes;
  std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(resultBoxes),
                 [](const std::unique_ptr<BoundingBox>& box) { return *box; });
  mResults[frame->vid][frame->frameIndex] = {NowMicros(), std::move(resultBoxes)};
  resultLock.unlock();
  mResultsCv.notify_all();
}

void SpatioTemporalRoIMixer::mixedInference(std::vector<MixedFrame>& mixedFrames) {
  // Enqueue Mixed Frames
  for (const auto& mixedFrame : mixedFrames) {
    mInferenceEngine->enqueue(mixedFrame.packedMat, mixedFrame.device,
                              mixedFrame.mixedFrameSize, mixedFrame.mixedFrameIndex);
  }

  // Get results of mixed frames sequentially
  for (int i = 0; i < mixedFrames.size(); i++) {
    auto[boxes, times, device] = mInferenceEngine->getResults(mixedFrames[i].mixedFrameIndex);
    for (Frame* frame : mixedFrames[i].getPackedFrames()) {
      frame->inferenceFrameSize = mixedFrames[i].mixedFrameSize;
      frame->inferenceDevice = device;
      frame->mixedInferenceStartTime = times.first;
      frame->mixedInferenceEndTime = times.second;
    }
    mixedFrames[i].packedMat.release();
    mPatchReconstructor->assignBoxesToFrame(mixedFrames[i], boxes);

    // Notify results of processed frames
    for (Frame* frame : mixedFrames[i].getPackedFrames()) {
      if (frame->isReadyToMarry(i)) {
        // Match boxes with RoIs (per frame)
        nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
        mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);

        bool allMarried = true;
        for (auto& cRoI : frame->childRoIs) {
          allMarried &= (cRoI->box != nullptr);
        }
        if (allMarried) {
          frame->isBoxesReady = true;
        }
      }
    }
    mRoIExtractor->notify();
  }
  for (auto& mixedFrame : mixedFrames) {
    for (Frame* frame : mixedFrame.getPackedFrames()) {
      if (std::any_of(frame->boxes.begin(), frame->boxes.end(),
                      [](auto& box) { return box->id == UNASSIGNED_ID; })) {
        // Match boxes with RoIs (per frame)
        nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
        mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
      }
    }
  }
}

void SpatioTemporalRoIMixer::roiWiseInference(std::vector<MixedFrame>& mixedFrames) {
  for (const auto& mixedFrame : mixedFrames) {
    mInferenceEngine->enqueue(mixedFrame.packedMat, mixedFrame.device, mixedFrame.mixedFrameSize,
                              mixedFrame.mixedFrameIndex);
  }

  Stream inferenceFrames;
  for (auto& mixedFrame : mixedFrames) {
    auto[boxes, times, device] = mInferenceEngine->getResults(mixedFrame.mixedFrameIndex);
    assert(mixedFrame.packedRoIs.size() == 1);
    auto&[x, y] = (*mixedFrame.packedRoIs.begin())->packedLocation;
    RoI* pRoI = *mixedFrame.packedRoIs.begin();
    pRoI->frame->inferenceFrameSize = mixedFrame.mixedFrameSize;
    pRoI->frame->inferenceDevice = device;
    pRoI->frame->mixedInferenceStartTime = times.first;
    pRoI->frame->mixedInferenceEndTime = times.second;
    inferenceFrames.insert(pRoI->frame);
    for (BoundingBox& b : boxes) {
      pRoI->frame->boxes.push_back(std::make_unique<BoundingBox>(
          UNASSIGNED_ID,Rect(
              (b.location.left - x) / pRoI->getTargetScale() + pRoI->origLoc.left,
              (b.location.top - y) / pRoI->getTargetScale() + pRoI->origLoc.top,
              (b.location.right - x) / pRoI->getTargetScale() + pRoI->origLoc.left,
              (b.location.bottom - y) / pRoI->getTargetScale() + pRoI->origLoc.top),
          b.confidence, b.label, pRoI->origin));
    }
  }

  for (Frame* frame : inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
    mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
  }
}

void SpatioTemporalRoIMixer::releaseFrames(const MultiStream& frames) {
  std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
  for (const auto& it : frames) {
    const int vid = it.first;
    const Stream& aStreamFrames = it.second;
    if (aStreamFrames.empty()) {
      continue;
    }
    for (Frame* frame : aStreamFrames) {
      log(frame);
    }
    Frame* firstFrame = *aStreamFrames.begin();
    Frame* lastFrame = *aStreamFrames.rbegin();
    std::vector<int> freeFrameIndices;

    Frame* handle = lastFrame;
    // Skip {pdInterval} frames
    for (int i=0; i<mConfig.roIExtractorConfig.PD_INTERVAL; ++i) {
      assert(handle != nullptr);
      handle = handle->prevFrame;
      if (handle == nullptr) {
        break;
      }
    }
    while (handle != nullptr) {
      freeFrameIndices.push_back(handle->frameIndex);
      handle = handle->prevFrame;
    }

    if (!freeFrameIndices.empty()) {
      mFrameBuffers.at(vid)->freeImage(freeFrameIndices);
    }
  }
  framesLock.unlock();
}

void SpatioTemporalRoIMixer::log(const Frame* frame) {
  if (mExecutionLogger) {
    mExecutionLogger->logExecution(frame);
  }
  if (mRoILogger) {
    for (auto& cRoI : frame->childRoIs) {
      mRoILogger->logRoI(cRoI.get());
    }
  }
}

void SpatioTemporalRoIMixer::waitForStart() {
  std::unique_lock<std::mutex> startLock(mStartMtx);
  mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mStartIndices.size(); });
  std::map<Device, std::pair<time_us, time_us>> startEndTime;
  for (Device device : mConfig.inferenceEngineConfig.DEVICES) {
    startEndTime[device] = {0, mScheduleInterval};
  }
  mRoIExtractor = std::make_unique<RoIExtractor>(
      mConfig.roIExtractorConfig, mConfig.FULL_FRAME_INTERVAL > 0, mConfig.ALLOW_INTERPOLATION,
      mConfig.ROI_WISE_INFERENCE, mPatchMixer.get(), mRoIResizer.get(),
      getInferencePlan(startEndTime, mInferenceEngine->getInferenceTimeTable(),
                       mConfig.ROI_WISE_INFERENCE));
  mbStartEnqueue = true;
  startLock.unlock();
  mEnqueueCv.notify_all();
}

int SpatioTemporalRoIMixer::enqueueImage(const int vid, const cv::Mat& mat) {
  assert(!mat.empty());
  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(vid) == mFrameBuffers.end()) {
    mFrameBuffers[vid] = std::make_unique<FrameBuffer>(
        vid, mConfig.BUFFER_SIZE, mStartIndices[vid]);
  }
  FrameBuffer* frameBuffer = mFrameBuffers.at(vid).get();
  lock.unlock();

  Frame* frame = frameBuffer->enqueue(mat);
  time_us startTime = NowMicros();
  preprocess(frame);
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
       "STRM::preprocess", NowMicros() - startTime, frame->vid, frame->frameIndex);
  if (mConfig.FULL_FRAME_INTERVAL == 0 || frame->frameIndex == mStartIndices[vid]) {
    frame->useInferenceResultForOF = true;
    frame->isFullFrameTarget = true;
    fullFrameInference(frame);
    if (mConfig.FULL_FRAME_INTERVAL == 0) {
      std::lock_guard<std::mutex> framesLock(mFrameBuffersMtx);
      log(frame);
      mFrameBuffers.at(frame->vid)->freeImage({frame->frameIndex});
    }
  } else {
    if (frame->frameIndex == mStartIndices[vid] + 1) {
      std::unique_lock<std::mutex> startLock(mStartMtx);
      mNumStartedFrameBuffers++;
      mStartCv.notify_one();
      mEnqueueCv.wait(startLock, [this]() { return mbStartEnqueue; });
      startLock.unlock();
      LOGD("Start %d video at %lld us", vid, NowMicros());
    }
    mRoIExtractor->enqueue(frame);
  }
  return frame->frameIndex;
}

void SpatioTemporalRoIMixer::preprocess(Frame* frame) const {
  cv::resize(frame->mat, frame->preProcessedMat, mTargetSize, 0, 0, CV_INTER_NN);
  cv::cvtColor(frame->preProcessedMat, frame->preProcessedMat, cv::COLOR_BGRA2GRAY);
}

void SpatioTemporalRoIMixer::outputWork() {
  while (!mbStop) {
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    mResultsCv.wait(resultLock, [this]() {
      if (mbStop) {
        return true;
      }
      for (auto& streamIt: mResults) {
        int vid = streamIt.first;
        if (mResultIndices.find(vid) == mResultIndices.end()) {
          mResultIndices[vid] = mStartIndices[vid];
        }
        int frameIndex = mResultIndices[vid];
        if (streamIt.second.find(frameIndex) != streamIt.second.end()) {
          return true;
        }
      }
      return false;
    });
    int vid;
    int frameIndex;
    FrameResult result;
    for (auto& streamIt : mResults) {
      vid = streamIt.first;
      frameIndex = mResultIndices[vid];
      auto frameIt = streamIt.second.find(frameIndex);
      if (frameIt != streamIt.second.end()) {
        mResultIndices[vid]++;
        result = frameIt->second;
        streamIt.second.erase(frameIt);
        break;
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    auto&[time, boxes] = result;
    LOGD("Logger::logResult                         for video %-5d frame %-4d // %4lu boxes",
         vid, frameIndex, boxes.size());
    mResultLogger->logResult(vid, frameIndex, time, boxes);
  }
}

double SpatioTemporalRoIMixer::weigh(const std::vector<time_us>& layout, std::map<long long, double> profile) {
  double weight = 0;
  for (auto l : layout) {
    assert (profile.find(l) != profile.end());
    weight += double(l) * profile[l];
  }
  return weight;
}

std::vector<time_us> SpatioTemporalRoIMixer::search(const std::vector<long long>& bars, long long total,
                            std::map<long long, double>& profile) {
  std::vector<time_us> layout;
  time_us left = total;

  // greedy initialization
  for (auto l : bars) {
    time_us cnt = left / l;
    for (int i = 0; i < cnt; i++) {
      layout.push_back(l);
    }
    left -= l * cnt;
  }

  // if cannot fill any bar, return empty layout
  if (layout.empty()) {
    return layout;
  }

  // select type of bar to try removing
  time_us l_a = layout[0];
  assert (profile.find(l_a) != profile.end());
  double d_a = profile[l_a];
  long c_a = std::count(layout.begin(), layout.end(), l_a);
  double alpha = weigh(layout, profile) - double(l_a) * double(c_a) * d_a;
  auto l_a_it = std::find(bars.begin(), bars.end(), l_a);

  // if selected bar is the one with the smallest density, removing is no worth
  if (l_a_it == bars.end()) {
    return layout;
  }

  // figure maximum number of removal with potential benefit
  double d_b = profile[bars[std::distance(bars.begin(), l_a_it) + 1]];
  long k_max = std::min(c_a, long((double(left) * d_b - alpha) / (double(l_a) * (d_a - d_b))));
  if (k_max < 0) {
    return layout;
  }

  // try all possible number of removals, recursively calling this function for the other area
  std::vector<std::vector<time_us>> layouts;
  std::vector<time_us> subBars{bars.begin() + 1, bars.end()};
  for (int k = 0; k <= k_max; k++) {
    std::vector<time_us> layout_left(c_a - k, l_a);
    std::vector<time_us> layout_right = search(subBars, total - std::accumulate(layout_left.begin(),
                                                                                layout_left.end(),
                                                                                0l), profile);
    layout_left.insert(layout_left.end(), layout_right.begin(), layout_right.end());
    layouts.push_back(layout_left);
  }
  std::sort(layouts.begin(), layouts.end(),
            [profile](auto& l1, auto& l2) { return weigh(l1, profile) > weigh(l2, profile); });
  return layouts[0];
}

std::vector<InferenceInfo> SpatioTemporalRoIMixer::getInferencePlan(
    const std::map<Device, std::pair<time_us, time_us>>& startEndTime,
    const std::map<Device, std::map<int, time_us>>& inferenceTimes, bool roiWiseInference) {
  std::vector<InferenceInfo> inferencePlan;
  for (const auto&[device, size_latency] : inferenceTimes) {
    std::map<time_us, double> profile;
    std::vector<time_us> bars;
    std::map<time_us, int> latency_size;
    int min_size = std::min_element(
        size_latency.begin(), size_latency.end(),
        [](const auto& it0, const auto& it1) {
          return it0.first < it1.first;
        })->first;
    for (const auto&[size, latency] : size_latency) {
      if (roiWiseInference && size != min_size) {
        continue;
      }
      profile[latency] = double(size * size) / double(latency);
      bars.push_back(latency);
      latency_size[latency] = size;
    }
    std::sort(bars.begin(), bars.end(),
              [&profile](time_us b1, time_us b2) { return (profile[b1] > profile[b2]); });
    auto&[startTime, endTime] = startEndTime.at(device);
    std::vector<time_us> layout = search(bars, endTime - startTime, profile);
    inferencePlan.reserve(layout.size());
    for (auto& l : layout) {
      inferencePlan.push_back({device, latency_size[l], l, 0});
    }
  }
  for (Device device : mConfig.inferenceEngineConfig.DEVICES) {
    std::vector<InferenceInfo*> devicePlan;
    for (auto& info : inferencePlan) {
      if (info.device == device) {
        devicePlan.push_back(&info);
      }
    }
    time_us accumulatedLatency = startEndTime.at(device).first;
    for (auto* info : devicePlan) {
      accumulatedLatency += info->latency;
      info->accumulatedLatency = accumulatedLatency;
    }
  }
  std::sort(inferencePlan.begin(), inferencePlan.end(),
            [](const InferenceInfo& l, const InferenceInfo& r) {
              return l.accumulatedLatency < r.accumulatedLatency;
            });
  return inferencePlan;
}

} // namespace rm
