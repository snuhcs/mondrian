#include "strm/Logger.hpp"

#include <utility>

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

Logger::Logger(const char* logPath) : baseTime(NowMicros()) {
  if (std::remove(logPath) == 0) {
    LOGD("Logger %s remove success", logPath);
  } else {
    LOGE("Logger %s remove failed", logPath);
    assert(false);
  }

  logFile = std::ofstream(logPath, std::ofstream::app);
  if (logFile.is_open()) {
    LOGD("Logger %s create success", logPath);
  } else {
    LOGE("Logger %s create failed", logPath);
    assert(false);
  }
}

Logger::~Logger() {
  if (logFile.is_open()) {
    logFile.close();
  }
}

void Logger::logHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "key" << delim
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
          << "mixedInferenceStartTime" << delim
          << "mixedInferenceEndTime" << delim
          << "reconstructStartTime" << delim
          << "reconstructEndTime" << delim
          << "endTime" << '\n';
  logFile.flush();
}

void Logger::log(Frame* frame) {
  if (!logFile.is_open() || frame == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << frame->key << delim
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
          << fromBaseTime(frame->mixedInferenceStartTime) << delim
          << fromBaseTime(frame->mixedInferenceEndTime) << delim
          << fromBaseTime(frame->reconstructStartTime) << delim
          << fromBaseTime(frame->reconstructEndTime) << delim
          << fromBaseTime(NowMicros()) << '\n';
  logFile.flush();
}

time_us Logger::fromBaseTime(const time_us& time) const {
  return time != 0 ? time - baseTime : 0;
}

void Logger::logResult(const std::string& key, int frameIndex, time_us time,
                       const std::vector<BoundingBox>& boxes) {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << key << ','
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
      << "key" << delim
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

      // OF features
      << "avgShiftX" << delim
      << "avgShiftY" << delim
      << "stdShiftX" << delim
      << "stdShiftY" << delim
      << "avgErr" << delim
      << "ncc" << delim

      << "numProbingRoIs" << delim
      << "priority" << delim
      << "id" << delim

      << "packedLocationX" << delim
      << "packedLocationY" << delim
      << "packedAbsMixedFrameIndex" << delim

      << "maxEdgeLength" << delim
      << "targetScale" << '\n';
  logFile.flush();
}

void Logger::logRoI(RoI* roi) {
  if (!logFile.is_open() || roi == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile
      // frame
      << roi->frame->key << delim
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

      // OF features
      << roi->features.ofFeatures.avgShift.first << delim
      << roi->features.ofFeatures.avgShift.second << delim
      << roi->features.ofFeatures.stdShift.first << delim
      << roi->features.ofFeatures.stdShift.second << delim
      << roi->features.ofFeatures.avgErr << delim
      << roi->features.ofFeatures.ncc << delim

      << roi->roisForProbing.size() << delim
      << roi->parentRoI->priority << delim
      << roi->id << delim

      << roi->parentRoI->packedLocation.first << delim
      << roi->parentRoI->packedLocation.second << delim
      << roi->parentRoI->packedAbsMixedFrameIndex << delim

      << roi->maxEdgeLength << delim
      << roi->getTargetScale() << '\n';
  logFile.flush();
}

} // namespace rm
