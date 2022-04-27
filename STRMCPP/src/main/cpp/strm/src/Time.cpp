#include "strm/Time.hpp"

#include <chrono>

namespace rm {

// https://github.com/tensorflow/tensorflow/blob/v2.8.0/tensorflow/lite/profiling/time.cc
time_us NowMicros() {
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

} // namespace rm
