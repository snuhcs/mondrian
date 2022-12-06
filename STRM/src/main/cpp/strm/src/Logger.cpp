#include "strm/Logger.hpp"

#include <utility>

#include "strm/Frame.hpp"
#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

Logger::Logger(const char* logPath) : baseTime(NowMicros()) {
  if (std::remove(logPath) == 0) {
    LOGD("Logger %s remove success", logPath);
  } else {
    LOGE("Logger %s remove failed", logPath);
  }

  logFile = std::ofstream(logPath, std::ofstream::app);
  if (logFile.is_open()) {
    LOGD("Logger %s create success", logPath);
  } else {
    LOGE("Logger %s create failed", logPath);
  }
}

Logger::~Logger() {
  if (logFile.is_open()) {
    logFile.close();
  }
}

void Logger::logExecutionHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "videoId" << delim
          << "frameIndex" << delim
          << "PDExtractorID" << delim
          << "OFExtractorID" << delim
          << "numBoxes" << delim
          << "numChildRoIs" << delim
          << "numParentRoIs" << delim
          << "inferenceFrameSize" << delim
          << "inferenceDevice" << delim
          << "enqueueTime" << delim
          << "fullInferenceStartTime" << delim
          << "fullInferenceEndTime" << delim
          << "pixelDiffRoIProcessStartTime" << delim
          << "pixelDiffRoIProcessEndTime" << delim
          << "opticalFlowRoIProcessStartTime" << delim
          << "opticalFlowRoIProcessEndTime" << delim
          << "resizeStartTime" << delim
          << "resizeEndTime" << delim
          << "mergeRoIStartTime" << delim
          << "mergeRoIEndTime" << delim
          << "mixingStartTime" << delim
          << "mixingEndTime" << delim
          << "scheduledTime" << delim
          << "mixedInferenceStartTime" << delim
          << "mixedInferenceEndTime" << delim
          << "reconstructStartTime" << delim
          << "reconstructEndTime" << delim
          << "endTime" << '\n';
  logFile.flush();
}

void Logger::logExecution(const Frame* frame) {
  if (!logFile.is_open() || frame == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << frame->vid << delim
          << frame->frameIndex << delim
          << frame->PDExtractorID << delim
          << frame->OFExtractorID << delim
          << frame->boxes.size() << delim
          << frame->childRoIs.size() << delim
          << frame->parentRoIs.size() << delim
          << frame->inferenceFrameSize << delim
          << frame->inferenceDevice << delim
          << fromBaseTime(frame->enqueueTime) << delim
          << fromBaseTime(frame->fullInferenceStartTime) << delim
          << fromBaseTime(frame->fullInferenceEndTime) << delim
          << fromBaseTime(frame->pixelDiffRoIProcessStartTime) << delim
          << fromBaseTime(frame->pixelDiffRoIProcessEndTime) << delim
          << fromBaseTime(frame->opticalFlowRoIProcessStartTime) << delim
          << fromBaseTime(frame->opticalFlowRoIProcessEndTime) << delim
          << fromBaseTime(frame->resizeStartTime) << delim
          << fromBaseTime(frame->resizeEndTime) << delim
          << fromBaseTime(frame->mergeRoIStartTime) << delim
          << fromBaseTime(frame->mergeRoIEndTime) << delim
          << fromBaseTime(frame->mixingStartTime) << delim
          << fromBaseTime(frame->mixingEndTime) << delim
          << fromBaseTime(frame->scheduledTime) << delim
          << fromBaseTime(frame->mixedInferenceStartTime) << delim
          << fromBaseTime(frame->mixedInferenceEndTime) << delim
          << fromBaseTime(frame->reconstructStartTime) << delim
          << fromBaseTime(frame->reconstructEndTime) << delim
          << fromBaseTime(frame->endTime) << '\n';
  logFile.flush();
}

time_us Logger::fromBaseTime(const time_us& time) const {
  return time != 0 ? time - baseTime : 0;
}

void Logger::logResult(int vid, int frameIndex, time_us time,
                       const std::vector<BoundingBox>& boxes) {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << vid << ','
          << frameIndex << ','
          << fromBaseTime(time) * 1000 << ',';
  for (int i = 0; i < boxes.size(); i++) {
    const BoundingBox& box = boxes[i];
    logFile << box.id << ','
            << box.location.left << ','
            << box.location.top << ','
            << box.location.right << ','
            << box.location.bottom << ','
            << box.confidence << ','
            << box.origin << ','
            << box.choiceOfBox << ','
            << COCO_LABELS[box.label];
    if (i != boxes.size() - 1) {
      logFile << ',';
    }
  }
  logFile << '\n';
  logFile.flush();
}

void Logger::logRoIHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile
      // frame
      << "videoId" << delim
      << "frameIndex" << delim

      // origLoc
      << "origLoc_l" << delim
      << "origLoc_t" << delim
      << "origLoc_r" << delim
      << "origLoc_b" << delim

      // paddedLoc
      << "paddedLoc_l" << delim
      << "paddedLoc_t" << delim
      << "paddedLoc_r" << delim
      << "paddedLoc_b" << delim

      // type & origin
      << "type" << delim
      << "origin" << delim

      // features
      << "width" << delim
      << "height" << delim
      << "xyRatio" << delim
      << "confidence" << delim

      // OF features
      << "shiftAvgX" << delim
      << "shiftAvgY" << delim
      << "shiftStdX" << delim
      << "shiftStdY" << delim
      << "shiftNcc" << delim
      << "avgErr" << delim

      << "isProbingRoI" << delim
      << "numProbingRoIs" << delim
      << "id" << delim

      // Packing info
      << "priority" << delim
      << "packedLoc_l" << delim
      << "packedLoc_t" << delim
      << "packedLoc_r" << delim
      << "packedLoc_b" << delim
      << "packedXY_x" << delim
      << "packedXY_y" << delim
      << "packedScale" << delim
      << "packedAbsMixedFrameIndex" << delim
      << "packedMixedFrameSize" << delim

      << "maxEdgeLength" << delim
      << "targetScale" << delim
      << "scaleLevel" << '\n';
  logFile.flush();
}

void Logger::logRoI(const RoI* roi) {
  if (!logFile.is_open() || roi == nullptr) {
    return;
  }
  std::unique_lock<std::mutex> logLock(mtx, std::defer_lock);
  if (!roi->isProbingRoI) {
    logLock.lock();
    for (auto& probeRoI : roi->roisForProbing) {
      assert(probeRoI->isProbingRoI);
      logRoI(probeRoI);
    }
  } else {
    assert(roi->roisForProbing.empty());
  }
  const RoI* pRoI = roi->isProbingRoI ? roi : roi->parentRoI;

  logFile
      // frame
      << roi->frame->vid << delim
      << roi->frame->frameIndex << delim

      // origLoc
      << roi->origLoc.left << delim
      << roi->origLoc.top << delim
      << roi->origLoc.right << delim
      << roi->origLoc.bottom << delim

      // paddedLoc
      << roi->paddedLoc.left << delim
      << roi->paddedLoc.top << delim
      << roi->paddedLoc.right << delim
      << roi->paddedLoc.bottom << delim

      // type & origin
      << roi->type << delim
      << roi->origin << delim

      // features
      << roi->features.width << delim
      << roi->features.height << delim
      << roi->features.xyRatio << delim
      << roi->features.confidence << delim

      // OF features
      << roi->features.ofFeatures.shiftAvg.first << delim
      << roi->features.ofFeatures.shiftAvg.second << delim
      << roi->features.ofFeatures.shiftStd.first << delim
      << roi->features.ofFeatures.shiftStd.second << delim
      << roi->features.ofFeatures.shiftNcc << delim
      << roi->features.ofFeatures.avgErr << delim

      << roi->isProbingRoI << delim
      << roi->roisForProbing.size() << delim
      << roi->id << delim

      << pRoI->priority << delim
      << pRoI->paddedLoc.left << delim
      << pRoI->paddedLoc.top << delim
      << pRoI->paddedLoc.right << delim
      << pRoI->paddedLoc.bottom << delim
      << pRoI->getPackedXY().first << delim
      << pRoI->getPackedXY().second << delim
      << pRoI->getTargetScale() << delim
      << pRoI->packedAbsMixedFrameIndex << delim
      << pRoI->packedMixedFrameSize << delim

      << roi->maxEdgeLength << delim
      << roi->getTargetScale() << delim
      << roi->getScaleLevel() << '\n';
  logFile.flush();
}

} // namespace rm
