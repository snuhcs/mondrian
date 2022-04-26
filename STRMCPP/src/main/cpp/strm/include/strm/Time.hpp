#ifndef TIME_HPP_
#define TIME_HPP_

#include <cstdint>

namespace rm {

using time_ms = uint64_t;
time_ms NowMicros();

} // namespace rm

#endif // TIME_HPP_
