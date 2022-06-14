#ifndef LOG_HPP_
#define LOG_HPP_

#include <android/log.h>

#include "strm/Config.hpp"

#define TAG "STRMCPP"

namespace rm {

#define LOG(prio, ...)                             \
  do {                                             \
    if (LOG_INTERNAL) {                            \
      __android_log_print(prio, TAG, __VA_ARGS__); \
    }                                              \
  } while (false)

#define LOGV(...) LOG(ANDROID_LOG_VERBOSE, __VA_ARGS__)
#define LOGD(...) LOG(ANDROID_LOG_DEBUG, __VA_ARGS__)
#define LOGI(...) LOG(ANDROID_LOG_INFO, __VA_ARGS__)
#define LOGW(...) LOG(ANDROID_LOG_WARNING, __VA_ARGS__)
#define LOGE(...) LOG(ANDROID_LOG_ERROR, __VA_ARGS__)

} // namespace rm

#endif // LOG_HPP_
