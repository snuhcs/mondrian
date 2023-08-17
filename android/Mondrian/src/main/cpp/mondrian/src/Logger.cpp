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

void Logger::logFrameHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "videoId" << DELIM
          << "frameIndex" << DELIM
          << "scheduleID" << DELIM
          << "PDExtractorID" << DELIM
          << "OFExtractorID" << DELIM
          << "numBoxes" << DELIM
          << "numROIs" << DELIM
          << "numMergedROIs" << DELIM
          << "inferenceFrameSize" << DELIM
          << "inferenceDevice" << DELIM
          << "enqueueTime" << DELIM
          << "fullInferenceStartTime" << DELIM
          << "fullInferenceEndTime" << DELIM
          << "pixelDiffROIProcessStartTime" << DELIM
          << "pixelDiffROIProcessEndTime" << DELIM
          << "opticalFlowROIProcessStartTime" << DELIM
          << "opticalFlowROIProcessEndTime" << DELIM
          << "resizeStartTime" << DELIM
          << "resizeEndTime" << DELIM
          << "mergeROIStartTime" << DELIM
          << "mergeROIEndTime" << DELIM
          << "packingStartTime" << DELIM
          << "packingEndTime" << DELIM
          << "scheduledTime" << DELIM
          << "packedInferenceStartTime" << DELIM
          << "packedInferenceEndTime" << DELIM
          << "reconstructStartTime" << DELIM
          << "reconstructEndTime" << DELIM
          << "endTime" << '\n';
  logFile.flush();
}

void Logger::logFrame(const Frame* frame) {
  if (!logFile.is_open() || frame == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << frame->vid << DELIM
          << frame->fid << DELIM
          << frame->scheduleID << DELIM
          << frame->PDExtractorID << DELIM
          << frame->OFExtractorID << DELIM
          << frame->boxes.size() << DELIM
          << frame->rois.size() << DELIM
          << frame->mergedROIs.size() << DELIM
          << frame->inferenceFrameSize << DELIM
          << str(frame->inferenceDevice) << DELIM
          << fromBaseTime(frame->enqueueTime) << DELIM
          << fromBaseTime(frame->fullInferenceStartTime) << DELIM
          << fromBaseTime(frame->fullInferenceEndTime) << DELIM
          << fromBaseTime(frame->pixelDiffROIProcessStartTime) << DELIM
          << fromBaseTime(frame->pixelDiffROIProcessEndTime) << DELIM
          << fromBaseTime(frame->opticalFlowROIProcessStartTime) << DELIM
          << fromBaseTime(frame->opticalFlowROIProcessEndTime) << DELIM
          << fromBaseTime(frame->resizeStartTime) << DELIM
          << fromBaseTime(frame->resizeEndTime) << DELIM
          << fromBaseTime(frame->mergeROIStartTime) << DELIM
          << fromBaseTime(frame->mergeROIEndTime) << DELIM
          << fromBaseTime(frame->packingStartTime) << DELIM
          << fromBaseTime(frame->packingEndTime) << DELIM
          << fromBaseTime(frame->scheduledTime) << DELIM
          << fromBaseTime(frame->packedInferenceStartTime) << DELIM
          << fromBaseTime(frame->packedInferenceEndTime) << DELIM
          << fromBaseTime(frame->reconstructStartTime) << DELIM
          << fromBaseTime(frame->reconstructEndTime) << DELIM
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
      << "videoId" << DELIM
      << "frameIndex" << DELIM

      // origLoc
      << "origLoc_l" << DELIM
      << "origLoc_t" << DELIM
      << "origLoc_r" << DELIM
      << "origLoc_b" << DELIM

      // paddedLoc
      << "paddedLoc_l" << DELIM
      << "paddedLoc_t" << DELIM
      << "paddedLoc_r" << DELIM
      << "paddedLoc_b" << DELIM

      // type & origin
      << "type" << DELIM
      << "origin" << DELIM

      // features
      << "width" << DELIM
      << "height" << DELIM
      << "xyRatio" << DELIM
      << "confidence" << DELIM

      // OF features
      << "shiftAvgX" << DELIM
      << "shiftAvgY" << DELIM
      << "shiftStdX" << DELIM
      << "shiftStdY" << DELIM
      << "shiftNcc" << DELIM
      << "avgErr" << DELIM

      << "numProbingROIs" << DELIM
      << "oid" << DELIM

      // Packing info
      << "mergedLoc_l" << DELIM
      << "mergedLoc_t" << DELIM
      << "mergedLoc_r" << DELIM
      << "mergedLoc_b" << DELIM
      << "packedXY_x" << DELIM
      << "packedXY_y" << DELIM
      << "mergedScale" << DELIM
      << "pid" << DELIM
      << "packedCanvasSize" << DELIM

      << "box" << DELIM

      << "maxEdgeLength" << DELIM
      << "targetScale" << DELIM
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
      << roi->frame->vid << DELIM
      << roi->frame->fid << DELIM

      // origLoc
      << roi->origLoc.l << DELIM
      << roi->origLoc.t << DELIM
      << roi->origLoc.r << DELIM
      << roi->origLoc.b << DELIM

      // paddedLoc
      << roi->paddedLoc.l << DELIM
      << roi->paddedLoc.t << DELIM
      << roi->paddedLoc.r << DELIM
      << roi->paddedLoc.b << DELIM

      // type & origin
      << str(roi->type) << DELIM
      << str(roi->origin) << DELIM

      // features
      << roi->features.width << DELIM
      << roi->features.height << DELIM
      << roi->features.xyRatio << DELIM
      << roi->features.confidence << DELIM

      // OF features
      << roi->features.ofFeatures.shiftAvg.first << DELIM
      << roi->features.ofFeatures.shiftAvg.second << DELIM
      << roi->features.ofFeatures.shiftStd.first << DELIM
      << roi->features.ofFeatures.shiftStd.second << DELIM
      << roi->features.ofFeatures.shiftNcc << DELIM
      << roi->features.ofFeatures.avgErr << DELIM

      << roi->roisForProbing.size() << DELIM
      << roi->oid << DELIM

      << mergedROI->loc().l << DELIM
      << mergedROI->loc().t << DELIM
      << mergedROI->loc().r << DELIM
      << mergedROI->loc().b << DELIM
      << mergedROI->packedXY().first << DELIM
      << mergedROI->packedXY().second << DELIM
      << mergedROI->targetScale() << DELIM
      << mergedROI->absolutePackedCanvasIndex() << DELIM
      << mergedROI->packedCanvasSize() << DELIM

      << "X" << DELIM  // TODO: handle roi->box validity

      << roi->maxEdgeLength << DELIM
      << roi->targetScale() << DELIM
      << roi->scaleLevel() << '\n';
  logFile.flush();
}

void Logger::logBoxesHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "videoId" << DELIM
          << "frameIndex" << DELIM
          << BoundingBox::header() << '\n';
  logFile.flush();
}

void Logger::logBoxes(VID vid, FID fid, const std::vector<BoundingBox>& boxes) {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  for (const auto& box : boxes) {
    logFile << vid << DELIM
            << fid << DELIM
            << box.str() << '\n';
  }
  logFile.flush();
}

} // namespace md
