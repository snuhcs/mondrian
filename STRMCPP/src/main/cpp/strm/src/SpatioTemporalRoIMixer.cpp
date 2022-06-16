#include "strm/SpatioTemporalRoIMixer.hpp"

#include <memory>
#include <utility>

namespace rm {

FrameBuffer::FrameBuffer(std::string key, int capacity)
    : key(std::move(key)), capacity(capacity), begin(0), end(0) {
  frames.resize(capacity);
}

int FrameBuffer::enqueue(const cv::Mat& mat) {
  LOGD("FrameBuffer::enqueue() start");
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this]() { return end - begin < capacity; });
  Frame* prevFrame = end == 0 ? nullptr : frames[(end - 1) % capacity].get();
  frames[end % capacity] = std::make_unique<Frame>(key, end, mat, prevFrame, NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[end % capacity].get();
  }
  LOGD("FrameBuffer::enqueue() end %d %d", begin, end);
  return end++;
}

void FrameBuffer::pop() {
  LOGD("FrameBuffer::pop() start");
  std::lock_guard<std::mutex> lock(mtx);
  frames[begin % capacity].reset();
  begin++;
  cv.notify_all();
  LOGD("FrameBuffer::pop() end");
}

Frame* FrameBuffer::getFrame(int frameIndex) {
  LOGD("FrameBuffer::getFrame(%d) start", frameIndex);
  std::lock_guard<std::mutex> lock(mtx);
  return frames[frameIndex % capacity].get();
  LOGD("FrameBuffer::getFrame(%d) end", frameIndex);
}

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               ResizeProfile* resizeProfile,
                                               InferenceEngine* inferenceEngine)
    : mConfig(config), mbStop(false),
      mLogger(new Logger("/data/data/hcs.offloading.edgedevicecpp/execution_log.csv")),
      mInferenceEngine(inferenceEngine), mRoIExtractor(new RoIExtractor(config.roIExtractorConfig, resizeProfile, inferenceEngine->getInputSizes()[0])) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mPatchReconstructor = std::make_unique<PatchReconstructor>(config.patchReconstructorConfig, resizeProfile);
  RoI::lastId = 1;

  mThread = std::thread([this]() { work(); });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mThread.join();
}

void SpatioTemporalRoIMixer::work() {
  LOGD("SpatioTemporalRoIMixer::work(), %lu us", NowMicros());
  int fullFrameInferenceStreamIndex = 0;
  time_us startTime, roiGettingTime, fullFrameInferenceEndTime;
  while (!mbStop) {
    startTime = NowMicros();
    std::vector<Frame*> frames = mRoIExtractor->getExtractedFrames();
    roiGettingTime = NowMicros();
    LOGD("The number of extracted frames: %lu", frames.size());

    if (frames.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(mConfig.LATENCY_SLO_MS / 2));
      continue;
    }

    fullFrameInference(getFullFrameInferenceFrame(frames, fullFrameInferenceStreamIndex++));
    fullFrameInferenceEndTime = NowMicros();

    int numMixedFrames = (mConfig.LATENCY_SLO_MS / 2 - (fullFrameInferenceEndTime - startTime) / 1000) / mInferenceEngine->getInferenceTimeMs();
    if (numMixedFrames <= 0) numMixedFrames = 1; // TODO : handle numMixedFrames <= 0 case
    std::vector<MixedFrame> mixedFrames = PatchMixer::pack(frames, mInferenceEngine->getInputSizes()[0], numMixedFrames);
    std::vector<int> handles;
    std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                   [this](const MixedFrame& mixedFrame) {
                     return mInferenceEngine->enqueue(mixedFrame.packedMat);
                   });
    for (int i = 0; i < mixedFrames.size(); i++) {
      std::vector<BoundingBox> results = mInferenceEngine->getResults(handles[i]);
      mixedFrames[i].packedMat.release();
      mPatchReconstructor->reconstructResults(mixedFrames[i], results);
    }

    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    for (auto& mixedFrame : mixedFrames) {
      for (Frame* frame : mixedFrame.getPackedFrames()) {
        mResults[{frame->key, frame->frameIndex}] = frame->boxes;
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    for (auto& mixedFrame : mixedFrames) {
      std::map<std::string, Frame*> lastFrames;
      for (Frame* frame : mixedFrame.getPackedFrames()) {
        if (lastFrames.find(frame->key) == lastFrames.end() ||
            lastFrames.at(frame->key)->frameIndex < frame->frameIndex) {
          lastFrames[frame->key] = frame;
        }
      }
      for (auto& lastFrame : lastFrames) {
        lastFrame.second->updateBoxesToTrackWithInferenceResult();
      }
    }

    for (auto& mixedFrame : mixedFrames) {
      std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
      for (Frame* frame : mixedFrame.getPackedFrames()) {
        mFrameBuffers.at(frame->key)->pop();
      }
      framesLock.unlock();
    }

    std::this_thread::sleep_for(
        std::chrono::microseconds(mConfig.LATENCY_SLO_MS * 1000 / 2 - (NowMicros() - startTime)));
  }
}

void SpatioTemporalRoIMixer::fullFrameInference(Frame* frame) {
  mRoIExtractor->preprocess(frame);
  frame->boxes = mInferenceEngine->getResults(mInferenceEngine->enqueue(frame->mat));
  frame->updateBoxesToTrackWithInferenceResult();

  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  mResults[{frame->key, frame->frameIndex}] = frame->boxes;
  resultLock.unlock();
  mResultsCv.notify_all();
}

Frame* SpatioTemporalRoIMixer::getFullFrameInferenceFrame(const std::vector<Frame*>& frames,
                                                          int fullFrameInferenceStreamIndex) {
  std::map<std::string, Frame*> lastFrames;
  for (Frame* frame : frames) {
    if (lastFrames.find(frame->key) == lastFrames.end() ||
        lastFrames.at(frame->key)->frameIndex < frame->frameIndex) {
      lastFrames[frame->key] = frame;
    }
  }
  auto it = lastFrames.begin();
  for (int i = 0; i < fullFrameInferenceStreamIndex % lastFrames.size(); i++) {
    it++;
  }
  return it->second;
}

int SpatioTemporalRoIMixer::enqueueImage(
    const std::string& key, const cv::Mat& mat) {
  LOGD("SpatioTemporalRoIMixer::enqueueImage(%s, Mat(%d, %d, %d))",
       key.substr(key.size() - 8).c_str(), mat.cols, mat.rows, mat.channels());
  assert(!mat.empty());
  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(key) == mFrameBuffers.end()) {
    mFrameBuffers[key] = std::make_unique<FrameBuffer>(key, mConfig.BUFFER_SIZE);
  }
  int frameIndex = mFrameBuffers.at(key)->enqueue(mat);

  Frame* frame = mFrameBuffers.at(key)->getFrame(frameIndex);
  if (frameIndex == 0) {
    time_us startTime = NowMicros();
    fullFrameInference(frame);
    LOGD("%s first frame inference (%lu us) : %lu boxes",
         key.substr(key.size() - 8).c_str(), NowMicros() - startTime, frame->boxes.size());
  } else {
    mRoIExtractor->enqueue(mFrameBuffers.at(key)->getFrame(frameIndex));
  }
  return frameIndex;
}

std::vector<BoundingBox> SpatioTemporalRoIMixer::getResults(const std::string& key,
                                                            int frameIndex) {
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d) start",
       key.substr(key.size() - 8).c_str(), frameIndex);
  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  std::pair<std::string, int> resultKey = std::make_pair(key, frameIndex);
  auto it = mResults.find(resultKey);
  mResultsCv.wait(resultLock,
                  [this, &resultKey]() { return mResults.find(resultKey) != mResults.end(); });
  std::vector<BoundingBox> results = std::move(mResults.at(resultKey));
  mResults.erase(mResults.find(resultKey));
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d) end %lu",
       key.substr(key.size() - 8).c_str(), frameIndex, results.size());
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
