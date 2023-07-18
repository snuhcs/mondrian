#include "mondrian/Time.hpp"

#include <chrono>

namespace md {

// https://github.com/tensorflow/tensorflow/blob/v2.8.0/tensorflow/lite/profiling/time.cc
time_us NowMicros() {
  return static_cast<time_us>(
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

} // namespace md
