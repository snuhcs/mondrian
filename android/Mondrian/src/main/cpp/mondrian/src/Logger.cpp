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

  logFile_ = std::ofstream(logPath, std::ofstream::app);
  if (logFile_.is_open()) {
    LOGD("Logger %s create success", logPath);
  } else {
    LOGE("Logger %s create failed", logPath);
  }
}

Logger::~Logger() {
  if (logFile_.is_open()) {
    logFile_.close();
  }
}

void Logger::logFrameHeader() {
  if (!logFile_.is_open()) return;
  std::lock_guard<std::mutex> lock(mtx_);
  logFile_ << Frame::header() << '\n';
  logFile_.flush();
}

void Logger::logROIHeader() {
  if (!logFile_.is_open()) return;
  std::lock_guard<std::mutex> lock(mtx_);
  logFile_ << ROI::header() << '\n';
  logFile_.flush();
}

void Logger::logBoxesHeader() {
  if (!logFile_.is_open()) return;
  std::lock_guard<std::mutex> lock(mtx_);
  logFile_ << "vid" << DELIM
           << "fid" << DELIM
           << BoundingBox::header() << '\n';
  logFile_.flush();
}

void Logger::logFrame(const Frame* frame) {
  if (!logFile_.is_open() || frame == nullptr) return;
  logFile_ << frame->str(baseTime) << '\n';
}

void Logger::logROIs(const Frame* frame) {
  if (!logFile_.is_open() || frame == nullptr) return;
  for (const auto& roi : frame->rois) {
    logFile_ << roi->str() << '\n';
  }
}

void Logger::logBoxes(const Frame* frame) {
  if (!logFile_.is_open() || frame == nullptr) return;
  for (const auto& box : frame->boxes) {
    logFile_ << frame->vid << DELIM
             << frame->fid << DELIM
             << box->str() << '\n';
  }
}

void Logger::flush() {
  if (!logFile_.is_open()) return;
  logFile_.flush();
}

} // namespace md
