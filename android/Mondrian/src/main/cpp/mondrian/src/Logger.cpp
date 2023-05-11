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

time_us Logger::fromBaseTime(const time_us& time) const {
  return time != 0 ? time - baseTime : 0;
}

void Logger::logResultHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "videoId" << delim
          << "frameIndex" << delim
          << BoundingBox::header(delim) << '\n';
  logFile.flush();
}

void Logger::logResult(int vid, int frameIndex, const std::vector<BoundingBox>& boxes) {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  for (const auto& box: boxes) {
    logFile << vid << delim
            << frameIndex << delim
            << box.str(delim) << '\n';
  }
  logFile.flush();
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
          << "numROIs" << delim
          << "numMergedROIs" << delim
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
          << "packingStartTime" << delim
          << "packingEndTime" << delim
          << "scheduledTime" << delim
          << "packedInferenceStartTime" << delim
          << "packedInferenceEndTime" << delim
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
          << frame->rois.size() << delim
          << frame->mergedROIs.size() << delim
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
          << fromBaseTime(frame->packingStartTime) << delim
          << fromBaseTime(frame->packingEndTime) << delim
          << fromBaseTime(frame->scheduledTime) << delim
          << fromBaseTime(frame->packedInferenceStartTime) << delim
          << fromBaseTime(frame->packedInferenceEndTime) << delim
          << fromBaseTime(frame->reconstructStartTime) << delim
          << fromBaseTime(frame->reconstructEndTime) << delim
          << fromBaseTime(frame->endTime) << '\n';
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

      << "numProbingROIs" << delim
      << "id" << delim

      // Packing info
      << "mergedLoc_l" << delim
      << "mergedLoc_t" << delim
      << "mergedLoc_r" << delim
      << "mergedLoc_b" << delim
      << "packedXY_x" << delim
      << "packedXY_y" << delim
      << "mergedScale" << delim
      << "absolutePackedCanvasIndex" << delim
      << "packedCanvasSize" << delim

      << "box" << delim

      << "maxEdgeLength" << delim
      << "targetScale" << delim
      << "scaleLevel" << '\n';
  logFile.flush();
}

void Logger::logROI(const ROI* roi) {
  if (!logFile.is_open() || roi == nullptr) {
    return;
  }
  const MergedROI* mergedROI = roi->mergedROI;

  std::lock_guard<std::mutex> logLock(mtx);
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

      << roi->roisForProbing.size() << delim
      << roi->id << delim

      << mergedROI->loc().l << delim
      << mergedROI->loc().t << delim
      << mergedROI->loc().r << delim
      << mergedROI->loc().b << delim
      << mergedROI->packedXY().first << delim
      << mergedROI->packedXY().second << delim
      << mergedROI->targetScale() << delim
      << mergedROI->absolutePackedCanvasIndex() << delim
      << mergedROI->packedCanvasSize() << delim

      << "X" << delim  // TODO: handle roi->box validity

      << roi->maxEdgeLength << delim
      << roi->targetScale() << delim
      << roi->scaleLevel() << '\n';
  logFile.flush();
}

} // namespace md
