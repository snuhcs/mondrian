#include "strm/DataType.hpp"

#include "strm/Log.hpp"
#include "strm/Logger.hpp"

namespace rm {

const idType UNASSIGNED_ID = -1;
const idType MERGED_ROI_ID = -2;

const std::pair<int, int> RoI::NOT_PACKED{-1, -1};

void Frame::filterPDRoIs(float threshold) {
  std::vector<RoI*> OFRoIs;
  for (auto& cRoI : childRoIs) {
    if (cRoI->type == RoI::Type::OF) {
      OFRoIs.push_back(cRoI.get());
    }
  }

  for (auto it = childRoIs.begin(); it != childRoIs.end();) {
    auto& cRoI = *it;
    if (cRoI->type == RoI::Type::PD) {
      int totalOFCoverage = 0;
      for (RoI* OFRoI : OFRoIs) {
        int intersection = cRoI->location.intersection(OFRoI->location);
        totalOFCoverage += intersection;
      }
      if ((float) totalOFCoverage / (float) cRoI->getArea() >= threshold) {
        it = childRoIs.erase(it);
        continue;
      }
    }
    it++;
  }
}

bool Frame::isReadyToMarry(int mixedFrameIndex) const {
  bool atLeastOneIndexIsSame = false;
  for (const auto& pRoI : parentRoIs) {
    if (!pRoI->isPacked()) {
      continue;
    }
    if (pRoI->packedMixedFrameIndex > mixedFrameIndex) {
      return false;
    }
    atLeastOneIndexIsSame |= (pRoI->packedMixedFrameIndex == mixedFrameIndex);
  }
  return atLeastOneIndexIsSame;
}

bool Frame::readyForOFExtraction() const {
  if (prevFrame->useInferenceResultForOF) {
    return prevFrame->isBoxesReady;
  } else {
    return prevFrame->isRoIsReady;
  }
}

FrameBuffer::FrameBuffer(const std::string& key, int capacity)
    : key(key), shortKey(key.substr(key.size() - 5, 1)), capacity(capacity), count(0) {
  frames.resize(capacity);
}

Frame* FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this, frameIndex]() { return frames[frameIndex % capacity].get() == nullptr; });
  Frame* prevFrame = frameIndex == 0 ? nullptr : frames[(frameIndex - 1) % capacity].get();
  frames[frameIndex % capacity] = std::make_unique<Frame>(key, frameIndex, mat, prevFrame,
                                                          NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[frameIndex % capacity].get();
  }
  Frame* currFrame = frames[frameIndex % capacity].get();
  lock.unlock();
  LOGD("FrameBuffer%s::enqueue  (%d)", shortKey.c_str(), frameIndex);
  return currFrame;
}

void FrameBuffer::freeImage(const std::vector<int>& frameIndices, Logger* logger) {
  std::unique_lock<std::mutex> lock(mtx);
  for (int frameIndex : frameIndices) {
    if (logger != nullptr) {
      logger->log(frames[frameIndex % capacity].get());
    }
    frames[frameIndex % capacity].reset();
  }
  lock.unlock();
  cv.notify_all();
  LOGD("FrameBuffer%s::freeImage(%lu)", shortKey.c_str(), frameIndices.size());
}

} // namespace rm
