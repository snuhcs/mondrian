#include "mondrian/Logger.hpp"

#include <utility>

#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

Logger::Logger(const std::string& logPath, time_us baseTime) : baseTime(baseTime) {
  if (std::remove(logPath.c_str()) == 0) {
    LOGD("Logger %s remove success", logPath.c_str());
  } else {
    LOGE("Logger %s remove failed", logPath.c_str());
  }

  logFile_ = std::ofstream(logPath, std::ofstream::app);
  if (logFile_.is_open()) {
    LOGD("Logger %s create success", logPath.c_str());
  } else {
    LOGE("Logger %s create failed", logPath.c_str());
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
    logFile_ << roi->str(baseTime) << '\n';
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
