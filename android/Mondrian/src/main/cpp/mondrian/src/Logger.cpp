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

void Logger::logFrameHeader() {
  if (!logFile.is_open()) return;
  std::lock_guard<std::mutex> lock(mtx);
  logFile << Frame::header() << '\n';
  logFile.flush();
}

void Logger::logFrame(const Frame* frame) {
  if (!logFile.is_open() || frame == nullptr) return;
  std::lock_guard<std::mutex> lock(mtx);
  logFile << frame->str(baseTime) << '\n';
  logFile.flush();
}

void Logger::logROIHeader() {
  if (!logFile.is_open()) return;
  std::lock_guard<std::mutex> lock(mtx);
  logFile << ROI::header() << '\n';
  logFile.flush();
}

void Logger::logROI(const ROI* roi) {
  if (!logFile.is_open() || roi == nullptr) return;
  std::lock_guard<std::mutex> logLock(mtx);
  logFile << roi->str() << '\n';
  logFile.flush();
}

void Logger::logBoxesHeader() {
  if (!logFile.is_open()) return;
  std::lock_guard<std::mutex> lock(mtx);
  logFile << "vid" << DELIM
          << "fid" << DELIM
          << BoundingBox::header() << '\n';
  logFile.flush();
}

void Logger::logBoxes(VID vid, FID fid, const std::vector<BoundingBox>& boxes) {
  if (!logFile.is_open()) return;
  std::lock_guard<std::mutex> lock(mtx);
  for (const auto& box : boxes) {
    logFile << vid << DELIM
            << fid << DELIM
            << box.str() << '\n';
  }
  logFile.flush();
}

} // namespace md
