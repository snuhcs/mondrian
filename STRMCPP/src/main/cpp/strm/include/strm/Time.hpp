#ifndef TIME_HPP_
#define TIME_HPP_

#include <cstdint>

namespace rm {

using time_us = uint64_t;
time_us NowMicros();

} // namespace rm

#endif // TIME_HPP_
