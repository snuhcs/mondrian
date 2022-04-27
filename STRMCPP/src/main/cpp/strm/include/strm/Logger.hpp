#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>

#include "strm/DataType.hpp"
#include "strm/Time.hpp"

namespace rm {

class Logger {
 public:
  Logger(const char* logPath);

  void logHeader();

  void log(Frame* frame);

 private:
  time_us fromBaseTime(const time_us& time) const;

  const char delimiter = '\t';

  std::ofstream logFile;
  std::mutex mtx;

  time_us baseTime;
};

} // namespace rm

#endif // LOGGER_HPP
