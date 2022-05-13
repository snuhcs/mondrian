#ifndef LOG_HPP_
#define LOG_HPP_

#include <android/log.h>

#define TAG "STRMCPP"

//#define INTERNAL_LOG // Log internal or not

#ifdef INTERNAL_LOG
#define LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#else
#define LOGV(...)  do {} while(0)
#define LOGD(...)  do {} while(0)
#define LOGI(...)  do {} while(0)
#define LOGW(...)  do {} while(0)
#define LOGE(...)  do {} while(0)
#endif

#endif // LOG_HPP_
