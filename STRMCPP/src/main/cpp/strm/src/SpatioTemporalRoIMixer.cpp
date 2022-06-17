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
                                               const ResizeProfile* resizeProfile,
                                               InferenceEngine* inferenceEngine)
    : mConfig(config), mbStop(false),
      mLogger(new Logger("/data/data/hcs.offloading.edgedevicecpp/execution_log.csv")),
      mInferenceEngine(inferenceEngine), mRoIExtractor(new RoIExtractor(config.roIExtractorConfig, resizeProfile, inferenceEngine->getInputSizes()[0])) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mPatchReconstructor = std::make_unique<PatchReconstructor>(config.patchReconstructorConfig);
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

std::set<idType> getRoIIds(std::vector<Frame*> frames) {
  std::set<idType> ids;
  for(const Frame* frame : frames) {
      for(const RoI& roi : frame->origRoIs) {
        ids.insert(roi.id);
      }
  }
  return ids;
}

std::vector<RoI*> getRoIStream(std::vector<Frame*> frames, idType roIId) {
  std::vector<RoI*> rois;
  for(const Frame* frame : frames) {
    for(RoI roi : frame->origRoIs) {
      if(roi.id == roIId) {
        rois.push_back(&roi);
      }
    }
  }
  return rois;
}

std::vector<int> findValidRoIs(std::vector<RoI*> rois) {
  std::vector<int> indices;
  for(int i = 0; i < rois.size(); i++) {
    if(!rois.at(i)->isDropped()) {
      indices.push_back(i);
    }
  }
  return indices;
}

std::pair<int, int> sumMotionVectors(std::vector<RoI*> rois, int start, int end) {
  int xShift = 0, yShift = 0;
  for(int i = start+1; i <= end; i++) {
    xShift += rois.at(i)->mv.first;
    yShift += rois.at(i)->mv.second;
  }
  return std::make_pair(xShift, yShift);
}

std::pair<int, int> getBbxShift(std::vector<RoI*> rois, int start, int end) {
  BoundingBox* bbx1 = rois.at(start)->getMatchedBbx();
  std::pair<int, int> c1 = bbx1->location.center();
  BoundingBox* bbx2 = rois.at(end)->getMatchedBbx();
  std::pair<int, int> c2 = bbx2->location.center();
  return std::make_pair(c2.first - c1.first, c2.second - c2.first);
}

void extrapolateLeft(std::vector<RoI*> rois, int idx) {
  RoI* prevRoI = rois.at(idx);
  std::pair<int, int> prevCenter = prevRoI->getMatchedBbx()->location.center();
  for(int current = idx-1; current >= 0; current--) {
    RoI* currRoI = rois.at(current);
    std::pair<int, int> shift = prevRoI->mv;
    std::pair<int, int> newCenter = std::make_pair(prevCenter.first - shift.first, prevCenter.second - shift.second);
    int newWidth = prevRoI->getMatchedBbx()->location.width();
    int newHeight = prevRoI->getMatchedBbx()->location.height();
    Rect newBox(newCenter, newWidth, newHeight);
    currRoI->boxes.emplace_back(prevRoI->id, newBox, 1, prevRoI->labelName);
    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

void extrapolateRight(std::vector<RoI*> rois, int idx) {
  RoI* prevRoI = rois.at(idx);
  std::pair<int, int> prevCenter = prevRoI->getMatchedBbx()->location.center();
  for(int current = idx+1; current < rois.size(); current++) {
    RoI* currRoI = rois.at(current);
    std::pair<int, int> shift = currRoI->mv;
    std::pair<int, int> newCenter = std::make_pair(prevCenter.first+shift.first, prevCenter.second+shift.second);
    int newWidth = prevRoI->getMatchedBbx()->location.width();
    int newHeight = prevRoI->getMatchedBbx()->location.height();
    Rect newBox(newCenter, newWidth, newHeight);
    currRoI->boxes.emplace_back(prevRoI->id, newBox, 1, prevRoI->labelName);
    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

void interpolateBetween(std::vector<RoI*> rois, int leftIdx, int rightIdx) {
  std::pair<int, int> totalShift = sumMotionVectors(rois, leftIdx, rightIdx);
  std::pair<int, int> bbxShift = getBbxShift(rois, leftIdx, rightIdx);
  float xRatio = bbxShift.first / totalShift.first;
  float yRatio = bbxShift.second / totalShift.second;

  RoI* prevRoI = rois.at(leftIdx);
  std::pair<int, int> prevCenter = prevRoI->getMatchedBbx()->location.center();
  for(int current = leftIdx+1; current < rightIdx; current++) {
    RoI* currRoI = rois.at(current);
    std::pair<int, int> shift = currRoI->mv;
    std::pair<int, int> newCenter = std::make_pair(prevCenter.first + shift.first * xRatio, prevCenter.second + shift.second * yRatio);
    int newWidth = prevRoI->getMatchedBbx()->location.width();
    int newHeight = prevRoI->getMatchedBbx()->location.height();
    Rect newBox(newCenter, newWidth, newHeight);
    currRoI->boxes.emplace_back(currRoI->id, newBox, 1, currRoI->labelName);
    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

void SpatioTemporalRoIMixer::interpolate(std::set<Frame*> frameSet) {
  std::vector<Frame*> frameVector(frameSet.begin(), frameSet.end());
  std::sort(frameVector.begin(), frameVector.end(),[](const Frame* a, const Frame* b) -> bool {return a->frameIndex < b->frameIndex;});
  std::set<idType> roIIds = getRoIIds(frameVector);
  for(auto id : roIIds) {
    std::vector<RoI*> rois = getRoIStream(frameVector, id);
    std::vector<int> validIndices = findValidRoIs(rois);
    if(!validIndices.empty()) {
      extrapolateLeft(rois, validIndices.at(0));
      for(int i = 0; i < validIndices.size() - 1; i++) {
        int leftIdx = validIndices.at(i);
        int rightIdx = validIndices.at(i+1);
        if(rightIdx - leftIdx == 1) continue;
        interpolateBetween(rois, leftIdx, rightIdx);
      }
      extrapolateRight(rois, validIndices.at(validIndices.size()-1));
    }
  }
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
