#include "strm/Time.hpp"

#include <chrono>
#include <sstream>

namespace rm {

// https://github.com/tensorflow/tensorflow/blob/v2.8.0/tensorflow/lite/profiling/time.cc
time_us NowMicros() {
  return static_cast<time_us>(
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

const char* TimeLogger::startName = "start";

time_us TimeLogger::getElapsedTime() const {
  assert(timestamps.find(startName) != timestamps.cend());
  return NowMicros() - timestamps.at("start");
}

time_us TimeLogger::getDuration(const char* next, const char* prev) const {
  assert(nameIndices.find(next) != nameIndices.end());
  assert(timestamps.find(next) != timestamps.end());
  int nextIndex = nameIndices.at(next);
  assert(nextIndex >= 1);
  int prevIndex = nextIndex - 1;
  if (prev != nullptr) {
    assert(nameIndices.find(prev) != nameIndices.end());
    assert(timestamps.find(prev) != timestamps.end());
    prevIndex = nameIndices.at(prev);
  }
  return timestamps.at(names[nextIndex]) - timestamps.at(names[prevIndex]);
}

std::string TimeLogger::getLog() const {
  std::stringstream ss;
  for (const auto& name : names) {
    assert(timestamps.find(name) != timestamps.end());
    ss << name << ": " << (timestamps.at(name) - timestamps.at(startName));
    if (name != names.back()) {
      ss << ", ";
    }
  }
  return ss.str();
}

void TimeLogger::start() {
  reset();
  step(startName);
}

void TimeLogger::step(const char* name) {
  assert(timestamps.find(name) == timestamps.end());
  names.push_back(name);
  nameIndices[name] = index++;
  timestamps[name] = NowMicros();
}

void TimeLogger::reset() {
  index = 0;
  names.clear();
  nameIndices.clear();
  timestamps.clear();
}

} // namespace rm
