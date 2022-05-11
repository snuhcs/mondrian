#include "strm/Logger.hpp"
#include "strm/Log.hpp"

#include <utility>

namespace rm {

Logger::Logger(const char* logPath): baseTime(NowMicros()) {
  if (std::remove(logPath) == 0) {
    LOGD("%s remove success", logPath);
  } else {
    LOGE("%s remove fail", logPath);
  }

  logFile = std::ofstream(logPath, std::ofstream::app);
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
          << "of0" << delim
          << "of1" << delim
          << "of2" << delim
          << "of3" << delim
          << "of4" << delim
          << "of5" << delim
          << "of6" << delim
          << "pixelDiffRoIProcessStartTime" << delim
          << "pixelDiffRoIProcessEndTime" << delim
          << "pd0" << delim
          << "pd1" << delim
          << "pd2" << delim
          << "pd3" << delim
          << "pd4" << delim
          << "pd5" << delim
          << "pd6" << delim
          << "pd7" << delim
          << "pd8" << delim
          << "pd9" << delim
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
          << fromBaseTime(frame->of0) << delim
          << fromBaseTime(frame->of1) << delim
          << fromBaseTime(frame->of2) << delim
          << fromBaseTime(frame->of3) << delim
          << fromBaseTime(frame->of4) << delim
          << fromBaseTime(frame->of5) << delim
          << fromBaseTime(frame->of6) << delim
          << fromBaseTime(frame->pixelDiffRoIProcessStartTime) << delim
          << fromBaseTime(frame->pixelDiffRoIProcessEndTime) << delim
          << fromBaseTime(frame->pd0) << delim
          << fromBaseTime(frame->pd1) << delim
          << fromBaseTime(frame->pd2) << delim
          << fromBaseTime(frame->pd3) << delim
          << fromBaseTime(frame->pd4) << delim
          << fromBaseTime(frame->pd5) << delim
          << fromBaseTime(frame->pd6) << delim
          << fromBaseTime(frame->pd7) << delim
          << fromBaseTime(frame->pd8) << delim
          << fromBaseTime(frame->pd9) << delim
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
