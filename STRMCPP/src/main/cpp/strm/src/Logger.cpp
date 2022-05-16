#include "strm/Logger.hpp"

#include <utility>

#include "strm/Log.hpp"

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

void Logger::logHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "key" << delim
          << "frameIndex" << delim
          << "numBoxes" << delim
          << "numRoIs" << delim
          << "enqueueTime" << delim
          << "dispatcherProcessStartTime" << delim
          << "dispatcherProcessEndTime" << delim
          << "fullFrameEnqueueTime" << delim
          << "fullFrameGetResultsTime" << delim
          << "opticalFlowRoIProcessStartTime" << delim
          << "opticalFlowRoIProcessEndTime" << delim
          << "pixelDiffRoIProcessStartTime" << delim
          << "pixelDiffRoIProcessEndTime" << delim
          << "mergeRoIStartTime" << delim
          << "mergeRoIEndTime" << delim
          << "resizeRoIStartTime" << delim
          << "resizeRoIEndTime" << delim
          << "mixingStartTime" << delim
          << "mixingEndTime" << delim
          << "mixedFrameCreateStartTime" << delim
          << "mixedFrameCreateEndTime" << delim
          << "mixedFrameEnqueueTime" << delim
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
          << frame->boxes.size() << delim
          << frame->rois.size() << delim
          << fromBaseTime(frame->enqueueTime) << delim
          << fromBaseTime(frame->dispatcherProcessStartTime) << delim
          << fromBaseTime(frame->dispatcherProcessEndTime) << delim
          << fromBaseTime(frame->fullFrameEnqueueTime) << delim
          << fromBaseTime(frame->fullFrameGetResultsTime) << delim
          << fromBaseTime(frame->opticalFlowRoIProcessStartTime) << delim
          << fromBaseTime(frame->opticalFlowRoIProcessEndTime) << delim
          << fromBaseTime(frame->pixelDiffRoIProcessStartTime) << delim
          << fromBaseTime(frame->pixelDiffRoIProcessEndTime) << delim
          << fromBaseTime(frame->mergeRoIStartTime) << delim
          << fromBaseTime(frame->mergeRoIEndTime) << delim
          << fromBaseTime(frame->resizeRoIStartTime) << delim
          << fromBaseTime(frame->resizeRoIEndTime) << delim
          << fromBaseTime(frame->mixingStartTime) << delim
          << fromBaseTime(frame->mixingEndTime) << delim
          << fromBaseTime(frame->mixedFrameCreateStartTime) << delim
          << fromBaseTime(frame->mixedFrameCreateEndTime) << delim
          << fromBaseTime(frame->mixedFrameEnqueueTime) << delim
          << fromBaseTime(frame->reconstructStartTime) << delim
          << fromBaseTime(frame->reconstructEndTime) << delim
          << fromBaseTime(frame->endTime) << '\n';
  logFile.flush();
}

time_us Logger::fromBaseTime(const time_us& time) const {
  return time != 0 ? time - baseTime : 0;
}

} // namespace rm
