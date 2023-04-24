#include "mondrian/Logger.hpp"

#include <utility>

#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

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
          << "scheduleID" << delim
          << "PDExtractorID" << delim
          << "OFExtractorID" << delim
          << "numBoxes" << delim
          << "numChildROIs" << delim
          << "numParentROIs" << delim
          << "inferenceFrameSize" << delim
          << "inferenceDevice" << delim
          << "enqueueTime" << delim
          << "fullInferenceStartTime" << delim
          << "fullInferenceEndTime" << delim
          << "pixelDiffROIProcessStartTime" << delim
          << "pixelDiffROIProcessEndTime" << delim
          << "opticalFlowROIProcessStartTime" << delim
          << "opticalFlowROIProcessEndTime" << delim
          << "resizeStartTime" << delim
          << "resizeEndTime" << delim
          << "mergeROIStartTime" << delim
          << "mergeROIEndTime" << delim
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
          << frame->scheduleID << delim
          << frame->PDExtractorID << delim
          << frame->OFExtractorID << delim
          << frame->boxes.size() << delim
          << frame->childROIs.size() << delim
          << frame->parentROIs.size() << delim
          << frame->inferenceFrameSize << delim
          << frame->inferenceDevice << delim
          << fromBaseTime(frame->enqueueTime) << delim
          << fromBaseTime(frame->fullInferenceStartTime) << delim
          << fromBaseTime(frame->fullInferenceEndTime) << delim
          << fromBaseTime(frame->pixelDiffROIProcessStartTime) << delim
          << fromBaseTime(frame->pixelDiffROIProcessEndTime) << delim
          << fromBaseTime(frame->opticalFlowROIProcessStartTime) << delim
          << fromBaseTime(frame->opticalFlowROIProcessEndTime) << delim
          << fromBaseTime(frame->resizeStartTime) << delim
          << fromBaseTime(frame->resizeEndTime) << delim
          << fromBaseTime(frame->mergeROIStartTime) << delim
          << fromBaseTime(frame->mergeROIEndTime) << delim
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
    logFile << boxes[i].str();
    if (i != boxes.size() - 1) {
      logFile << ',';
    }
  }
  logFile << '\n';
  logFile.flush();
}

void Logger::logROIHeader() {
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

      << "isProbingROI" << delim
      << "numProbingROIs" << delim
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

      << "box" << delim
      << "probingBox" << delim

      << "maxEdgeLength" << delim
      << "targetScale" << delim
      << "scaleLevel" << '\n';
  logFile.flush();
}

void Logger::logROI(const ROI* roi) {
  if (!logFile.is_open() || roi == nullptr) {
    return;
  }
  std::unique_lock<std::mutex> logLock(mtx, std::defer_lock);
  if (!roi->isProbingROI) {
    logLock.lock();
    for (auto& probeROI : roi->roisForProbing) {
      assert(probeROI->isProbingROI);
      logROI(probeROI);
    }
  } else {
    assert(roi->roisForProbing.empty());
  }
  const ROI* pROI = roi->isProbingROI ? roi : roi->parentROI;

  logFile
      // frame
      << roi->frame->vid << delim
      << roi->frame->frameIndex << delim

      // origLoc
      << roi->origLoc.l << delim
      << roi->origLoc.t << delim
      << roi->origLoc.r << delim
      << roi->origLoc.b << delim

      // paddedLoc
      << roi->paddedLoc.l << delim
      << roi->paddedLoc.t << delim
      << roi->paddedLoc.r << delim
      << roi->paddedLoc.b << delim

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

      << roi->isProbingROI << delim
      << roi->roisForProbing.size() << delim
      << roi->id << delim

      << pROI->priority << delim
      << pROI->paddedLoc.l << delim
      << pROI->paddedLoc.t << delim
      << pROI->paddedLoc.r << delim
      << pROI->paddedLoc.b << delim
      << pROI->getPackedXY().first << delim
      << pROI->getPackedXY().second << delim
      << pROI->getTargetScale() << delim
      << pROI->packedAbsMixedFrameIndex << delim
      << pROI->packedMixedFrameSize << delim

      << (roi->box == nullptr ? "X" : roi->box->str().c_str()) << delim
      << (roi->probingBox == nullptr ? "X" : roi->probingBox->str().c_str()) << delim

      << roi->maxEdgeLength << delim
      << roi->getTargetScale() << delim
      << roi->getScaleLevel() << '\n';
  logFile.flush();
}

} // namespace md
