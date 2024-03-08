#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>

#include "mondrian/DataType.hpp"

namespace md {

class BoundingBox;
class Frame;
class MergedROI;
class ROI;

class Logger {
 public:
  Logger(const std::string& logPath, time_us baseTime);

  ~Logger();

  void logFrameHeader();

  void logROIHeader();

  void logBoxesHeader();

  void logFrame(const Frame* frame);

  void logROIs(const Frame* frame);

  void logBoxes(const Frame* frame);

  void flush();

  std::mutex& mtx() { return mtx_; }

 private:
  std::ofstream logFile_;
  std::mutex mtx_;

  time_us baseTime;
};

} // namespace md

#endif // LOGGER_HPP
