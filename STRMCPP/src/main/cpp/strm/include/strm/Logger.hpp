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
  time_ms fromBaseTime(const time_ms& time) const;

  const char delimiter = '\t';

  std::ofstream logFile;
  std::mutex mtx;

  time_ms baseTime;
};

} // namespace rm

#endif // LOGGER_HPP
