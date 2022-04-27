#include "strm/Logger.hpp"

#include <utility>

namespace rm {

Logger::Logger(const char* logPath)
: logFile(logPath, std::ofstream::app), baseTime(NowMicros()) {}

void Logger::logHeader() {
  if (!logFile.is_open()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "key" << delimiter
          << "frameIndex" << delimiter
          << "numBoxes" << delimiter
          << "numRoIs" << delimiter
          << "enqueueTime" << delimiter
          << "dispatcherProcessStartTime" << delimiter
          << "fullFrameEnqueueTime" << delimiter
          << "fullFrameGetResultsTime" << delimiter
          << "opticalFlowRoIProcessStartTime" << delimiter
          << "opticalFlowRoIProcessEndTime" << delimiter
          << "pixelDiffRoIProcessStartTime" << delimiter
          << "pixelDiffRoIProcessEndTime" << delimiter
          << "mergeRoIStartTime" << delimiter
          << "mergeRoIEndTime" << delimiter
          << "resizeRoIStartTime" << delimiter
          << "resizeRoIEndTime" << delimiter
          << "mixingStartTime" << delimiter
          << "mixingEndTime" << delimiter
          << "mixedFrameCreateStartTime" << delimiter
          << "mixedFrameCreateEndTime" << delimiter
          << "mixedFrameEnqueueTime" << delimiter
          << "reconstructStartTime" << delimiter
          << "reconstructEndTime" << delimiter
          << "endTime" << '\n';
  logFile.flush();
}

void Logger::log(Frame* frame) {
  if (!logFile.is_open() || frame == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  logFile << frame->key << delimiter
          << frame->frameIndex << delimiter
          << frame->boxes.size() << delimiter
          << frame->rois.size() << delimiter
          << fromBaseTime(frame->enqueueTime) << delimiter
          << fromBaseTime(frame->dispatcherProcessStartTime) << delimiter
          << fromBaseTime(frame->fullFrameEnqueueTime) << delimiter
          << fromBaseTime(frame->fullFrameGetResultsTime) << delimiter
          << fromBaseTime(frame->opticalFlowRoIProcessStartTime) << delimiter
          << fromBaseTime(frame->opticalFlowRoIProcessEndTime) << delimiter
          << fromBaseTime(frame->pixelDiffRoIProcessStartTime) << delimiter
          << fromBaseTime(frame->pixelDiffRoIProcessEndTime) << delimiter
          << fromBaseTime(frame->mergeRoIStartTime) << delimiter
          << fromBaseTime(frame->mergeRoIEndTime) << delimiter
          << fromBaseTime(frame->resizeRoIStartTime) << delimiter
          << fromBaseTime(frame->resizeRoIEndTime) << delimiter
          << fromBaseTime(frame->mixingStartTime) << delimiter
          << fromBaseTime(frame->mixingEndTime) << delimiter
          << fromBaseTime(frame->mixedFrameCreateStartTime) << delimiter
          << fromBaseTime(frame->mixedFrameCreateEndTime) << delimiter
          << fromBaseTime(frame->mixedFrameEnqueueTime) << delimiter
          << fromBaseTime(frame->reconstructStartTime) << delimiter
          << fromBaseTime(frame->reconstructEndTime) << delimiter
          << fromBaseTime(frame->endTime) << '\n';
  logFile.flush();
}

time_us Logger::fromBaseTime(const time_us& time) const {
  return time != 0 ? time - baseTime : 0;
}

} // namespace rm
