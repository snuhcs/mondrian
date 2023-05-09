#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>

#include "mondrian/Time.hpp"

namespace md {

class BoundingBox;
class Frame;
class ROI;

class Logger {
 public:
  Logger(const char* logPath);

  ~Logger();

  void logExecutionHeader();

  void logExecution(const Frame* frame);

  void logROIHeader();

  void logROI(const ROI* roi);

  void logResult(int vid, int frameIndex, time_us endTime,
                 const std::vector<BoundingBox>& boxes);

 private:
  time_us fromBaseTime(const time_us& time) const;

  const char delim = '\t';

  std::ofstream logFile;
  std::mutex mtx;

  time_us baseTime;
};

} // namespace md

#endif // LOGGER_HPP
