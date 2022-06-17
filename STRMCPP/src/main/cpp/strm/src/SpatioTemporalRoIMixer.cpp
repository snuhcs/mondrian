#include "strm/SpatioTemporalRoIMixer.hpp"

#include <memory>
#include <utility>

#include "strm/Interpolator.hpp"

namespace rm {

FrameBuffer::FrameBuffer(const std::string& key, int capacity)
    : key(key), shortKey(key.substr(key.size() - 5, 1)), capacity(capacity), begin(0), end(0) {
  frames.resize(capacity);
}

int FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this]() { return end - begin < capacity; });
  int frameIndex = end++;
  Frame* prevFrame = frameIndex == 0 ? nullptr : frames[(frameIndex - 1) % capacity].get();
  frames[frameIndex % capacity] = std::make_unique<Frame>(key, frameIndex, mat, prevFrame,
                                                          NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[frameIndex % capacity].get();
  }
  lock.unlock();
  LOGD("FrameBuffer%s::enqueue ()                       // frameIndex = %d", shortKey.c_str(),
       frameIndex);
  return frameIndex;
}

void FrameBuffer::pop(int numFrames) {
  std::unique_lock<std::mutex> lock(mtx);
  for (int frameIndex = begin; frameIndex < begin + numFrames; frameIndex++) {
    frames[frameIndex % capacity].reset();
  }
  begin += numFrames;
  lock.unlock();
  cv.notify_all();
  LOGD("FrameBuffer%s::pop(%2d)                          // begin = %d, end = %d", shortKey.c_str(),
       numFrames, begin, end);
}

Frame* FrameBuffer::getFrame(int frameIndex) {
  std::unique_lock<std::mutex> lock(mtx);
  assert(begin <= frameIndex && frameIndex < end);
  Frame* frame = frames[frameIndex % capacity].get();
  lock.unlock();
  LOGD("FrameBuffer%s::getFrame()                       // frameIndex = %d", shortKey.c_str(),
       frameIndex);
  return frame;
}

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               ResizeProfile* resizeProfile,
                                               InferenceEngine* inferenceEngine)
    : mConfig(config), mbStop(false),
      mLogger(new Logger("/data/data/hcs.offloading.edgedevicecpp/execution_log.csv")),
      mInferenceEngine(inferenceEngine),
      mRoIExtractor(new RoIExtractor(config.roIExtractorConfig, resizeProfile,
                                     inferenceEngine->getInputSizes()[0])),
      mPatchReconstructor(new PatchReconstructor(config.patchReconstructorConfig, resizeProfile)) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mThread = std::thread([this]() { work(); });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mThread.join();
}

void SpatioTemporalRoIMixer::work() {
  int scheduleID = 0;
  int fullFrameInferenceStreamIndex = 0;
  time_us scheduleInterval = mConfig.LATENCY_SLO_MS * 1000 / 2;
  time_us startTime, roiGettingTime, fullFrameInferenceTime, mixedFrameInferenceTime;
  time_us remainingTime;
  while (!mbStop) {
    LOGD("========== Schedule %d Start ==========", scheduleID);
    startTime = NowMicros();
    std::set<Frame*> frames = mRoIExtractor->getExtractedFrames();
    roiGettingTime = NowMicros();
    LOGD("SpatioTemporalRoIMixer::work() getExtractedFrames() took %lu us  // %lu frames",
         roiGettingTime - startTime, frames.size());

    if (frames.empty()) {
      if (scheduleInterval > roiGettingTime - startTime) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(scheduleInterval - (roiGettingTime - startTime)));
      }
      LOGD("========== Schedule %d End   ==========", scheduleID++);
      continue;
    }

    // Prepare for free
    std::map<std::string, int> frameCounts;
    for (Frame* frame : frames) {
      if (frameCounts.find(frame->key) == frameCounts.end()) {
        frameCounts[frame->key] = 0;
      }
      frameCounts[frame->key]++;
    }

    // Full Frame Inference
    std::set<Frame*> lastFrames = filterLastFrames(frames);
    for (Frame* frame : lastFrames) {
      frame->boxesToTrack.clear();
    }
    Frame* fullFrameTarget = getFullFrameInferenceFrame(lastFrames,
                                                        fullFrameInferenceStreamIndex++);
    lastFrames.erase(lastFrames.find(fullFrameTarget));
    frames.erase(frames.find(fullFrameTarget));
    fullFrameInference(fullFrameTarget);
    fullFrameInferenceTime = NowMicros();
    remainingTime = scheduleInterval < (fullFrameInferenceTime - startTime) ? 0 :
                    scheduleInterval - (fullFrameInferenceTime - startTime);
    long long inferenceTimeUs = mInferenceEngine->getInferenceTimeMs() * 1000;
    LOGD(
        "SpatioTemporalRoIMixer::work() fullFrameInference() took %lu us, %lu us remains %lld us for inference  // %lu boxes",
        fullFrameInferenceTime - roiGettingTime, remainingTime, inferenceTimeUs,
        fullFrameTarget->boxes.size());

    // TODO : handle numMixedFrames <= 0 case
    int numMixedFrames = remainingTime > inferenceTimeUs ? (int) (remainingTime / inferenceTimeUs) : 1;
    std::vector<MixedFrame> mixedFrames = PatchMixer::pack(
        frames, lastFrames, mInferenceEngine->getInputSizes()[0], numMixedFrames);

    // Inference Mixed Frames
    std::vector<int> handles;
    std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                   [this](const MixedFrame& mixedFrame) {
                     return mInferenceEngine->enqueue(mixedFrame.packedMat);
                   });

    std::set<Frame*> processedFrames;
    for (int i = 0; i < mixedFrames.size(); i++) {
      std::vector<BoundingBox> results = mInferenceEngine->getResults(handles[i]);
      mixedFrames[i].packedMat.release();
      mPatchReconstructor->reconstructResults(mixedFrames[i], results);

      for (Frame* frame : mixedFrames[i].getPackedFrames()) {
        if (frame->isAllRoIPrepared() && processedFrames.find(frame) == processedFrames.end()) {
          processedFrames.insert(frame);
          if (lastFrames.find(frame) != lastFrames.end()) {
            frame->updateBoxesToTrackWithInferenceResult();
          }
        }
      }
    }

    frames.insert(fullFrameTarget);
    Interpolator::interpolate(frames);

    // TODO: Remove this part after implementing interpolation
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    for (Frame* frame : frames) {
      if (processedFrames.find(frame) == processedFrames.end()) {
        mResults[{frame->key, frame->frameIndex}] = frame->boxes;
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
    for (auto& it : frameCounts) {
      mFrameBuffers.at(it.first)->pop(it.second);
    }
    framesLock.unlock();

    mixedFrameInferenceTime = NowMicros();
    LOGD("SpatioTemporalRoIMixer::work() %lu mixedFrameInferences took %lu us",
         mixedFrames.size(), mixedFrameInferenceTime - fullFrameInferenceTime);

    LOGD("========== Schedule %d End (getRoI %lu | full %lu | mix %lu) ==========", scheduleID++,
         roiGettingTime - startTime, fullFrameInferenceTime - roiGettingTime,
         mixedFrameInferenceTime - fullFrameInferenceTime);

    if (scheduleInterval > mixedFrameInferenceTime - startTime) {
      std::this_thread::sleep_for(
          std::chrono::microseconds(scheduleInterval - (mixedFrameInferenceTime - startTime)));
    }
  }
}

void SpatioTemporalRoIMixer::fullFrameInference(Frame* frame) {
  mRoIExtractor->preprocess(frame);
  std::vector<RoI> emptyRoIs;
  // TODO: use currFrame->origRoIs for ID mapping
  frame->boxes = assignIdsToBoxes(
      mInferenceEngine->getResults(mInferenceEngine->enqueue(frame->mat)),
      frame->prevFrame == nullptr ? emptyRoIs : frame->prevFrame->origRoIs,
      mConfig.patchReconstructorConfig.OVERLAP_THRESHOLD);
  frame->updateBoxesToTrackWithInferenceResult();

  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  mResults[{frame->key, frame->frameIndex}] = frame->boxes;
  resultLock.unlock();
  mResultsCv.notify_all();
}

Frame* SpatioTemporalRoIMixer::getFullFrameInferenceFrame(const std::set<Frame*>& lastFrames,
                                                          int fullFrameInferenceStreamIndex) {
  auto it = lastFrames.begin();
  for (int i = 0; i < fullFrameInferenceStreamIndex % lastFrames.size(); i++) {
    it++;
  }
  return *it;
}

int SpatioTemporalRoIMixer::enqueueImage(const std::string& key, const cv::Mat& mat) {
  assert(!mat.empty());
  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(key) == mFrameBuffers.end()) {
    mFrameBuffers[key] = std::make_unique<FrameBuffer>(key, mConfig.BUFFER_SIZE);
  }
  int frameIndex = mFrameBuffers.at(key)->enqueue(mat);

  Frame* frame = mFrameBuffers.at(key)->getFrame(frameIndex);
  if (frameIndex == 0) {
    mRoIExtractor->preprocess(frame);
    fullFrameInference(frame);
  } else {
    mRoIExtractor->enqueue(frame);
  }
  return frameIndex;
}

std::vector<BoundingBox> SpatioTemporalRoIMixer::getResults(const std::string& key,
                                                            int frameIndex) {
  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  std::pair<std::string, int> resultKey = std::make_pair(key, frameIndex);
  auto it = mResults.find(resultKey);
  mResultsCv.wait(resultLock,
                  [this, &resultKey]() { return mResults.find(resultKey) != mResults.end(); });
  std::vector<BoundingBox> results = std::move(mResults.at(resultKey));
  mResults.erase(mResults.find(resultKey));
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %4d)    // %lu boxes",
       key.substr(key.size() - 5, 1).c_str(), frameIndex, results.size());
  return results;
}

std::vector<BoundingBox> SpatioTemporalRoIMixer::assignIdsToBoxes(
    const std::vector<BoundingBox>& boxes, std::vector<RoI>& rois, float overlapThreshold) {
  std::vector<BoundingBox> unassignedBoxes;
  std::vector<BoundingBox> assignedBoxes;

  // Match Boxes to RoI. Boxes can be unmatched. (if overlap ratio lower than threshold)
  for (const BoundingBox& box : boxes) {
    float maxOverlap = -1;
    RoI* maxRoI = nullptr;
    for (RoI& roi : rois) {
      int intersection = roi.location.intersection(box.location);
      assert(box.location.area() != 0);
      float overlapRatio = (float) intersection / (float) box.location.area();
      if (maxOverlap < overlapRatio) {
        maxOverlap = overlapRatio;
        maxRoI = &roi;
      }
    }
    if (maxRoI != nullptr && maxOverlap >= overlapThreshold) {
      maxRoI->boxes.emplace_back(UNASSIGNED_ID, box.location, box.confidence, box.labelName);
    } else {
      unassignedBoxes.emplace_back(UNASSIGNED_ID, box.location, box.confidence, box.labelName);
    }
  }

  // Let RoIs find their most well corresponding Box
  for (RoI& roi : rois) {
    int maxIntersection = -1;
    int maxIndex = -1;
    for (int i = 0; i < roi.boxes.size(); ++i) {
      BoundingBox& box = roi.boxes[i];
      int intersection = roi.location.intersection(box.location);
      if (maxIntersection < intersection) {
        maxIntersection = intersection;
        maxIndex = i;
      }
    }
    if (maxIndex != -1) {
      BoundingBox& box = roi.boxes[maxIndex];
      assignedBoxes.emplace_back(roi.id, box.location, box.confidence, box.labelName);
      for (int i = 0; i < roi.boxes.size(); ++i) {
        if (i == maxIndex) continue;
        unassignedBoxes.emplace_back(UNASSIGNED_ID, box.location, box.confidence, box.labelName);
      }
    }
  }


  // If unassigned Boxes exist (1. those who does not match with any RoI 2. those who lost competition between other Boxes in single RoI),
  // classify them as newly appeared objects and assign new Id
  if (!unassignedBoxes.empty()) {
    std::pair<idType, idType> idRange = RoI::getNewIds(unassignedBoxes.size());
    idType id = idRange.first;
    for (const BoundingBox& box : unassignedBoxes) {
      assert(id < idRange.second);
      assignedBoxes.emplace_back(id++, box.location, box.confidence, box.labelName);
    }
  }

  return assignedBoxes;
}

} // namespace rm
