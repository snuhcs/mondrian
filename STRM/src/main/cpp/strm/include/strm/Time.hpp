#ifndef TIME_HPP_
#define TIME_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace rm {

using time_us = long long;

time_us NowMicros();

class TimeLogger {
 public:
  time_us getDuration(const char* next, const char* prev = nullptr) const;

  time_us getElapsedTime() const;

  std::string getLog() const;

  void start();

  void step(const char* name);

 private:
  void reset();

  int index = 0;
  std::vector<const char*> names;
  std::map<const char*, int> nameIndices;
  std::map<const char*, time_us> timestamps;
};

} // namespace rm

#endif // TIME_HPP_
