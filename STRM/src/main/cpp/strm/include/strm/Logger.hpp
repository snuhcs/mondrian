#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>

#include "strm/Time.hpp"

namespace rm {

class BoundingBox;
class Frame;
class RoI;

class Logger {
 public:
  Logger(const char* logPath);

  ~Logger();

  void logExecutionHeader();

  void logExecution(const Frame* frame);

  void logRoIHeader();

  void logRoI(const RoI* roi);

  void logResult(int vid, int frameIndex, time_us time,
                 const std::vector<BoundingBox>& boxes);

 private:
  time_us fromBaseTime(const time_us& time) const;

  const char delim = '\t';

  std::ofstream logFile;
  std::mutex mtx;

  time_us baseTime;
};

} // namespace rm

#endif // LOGGER_HPP
