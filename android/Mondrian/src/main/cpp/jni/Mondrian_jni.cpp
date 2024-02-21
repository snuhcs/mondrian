#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <fstream>

#include "json/json.h"
#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/Mondrian.hpp"
#include "mondrian/Utils.hpp"

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_mondrian_MondrianApp_createHandle(JNIEnv* env,
                                                      jobject thiz,
                                                      jstring logDir,
                                                      jint numVideos,
                                                      jint numTotalFrames) {
  auto configPath = "/data/local/tmp/config.json";
  auto config = md::parseMondrianConfig(configPath);
  std::string logDirStr(env->GetStringUTFChars(logDir, nullptr));
  return reinterpret_cast<long>(new md::Mondrian(logDirStr, numVideos, numTotalFrames, config, env, thiz));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_mondrian_MondrianApp_enqueue(JNIEnv* env, jobject thiz,
                                                 jlong handle, jint vid, jlong yuvMatAddr) {
  auto* mondrian = (md::Mondrian*) handle;
  auto* yuvMat = (cv::Mat*) yuvMatAddr;
  mondrian->enqueue(vid, yuvMat->clone());
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_mondrian_MondrianApp_dumpLogs(JNIEnv* env, jobject thiz, jlong handle) {
  auto* mondrian = (md::Mondrian*) handle;
  mondrian->dumpLogs();
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_mondrian_MondrianApp_close(JNIEnv* env, jobject thiz, jlong handle) {
  auto* mondrian = (md::Mondrian*) handle;
  delete mondrian;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hcs_offloading_mondrian_Utils_schedSetAffinityBig(JNIEnv* env, jclass clazz) {
  return md::sched_setaffinity_big();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hcs_offloading_mondrian_Utils_schedSetAffinityBigOrPrimary(JNIEnv* env, jclass clazz) {
  return md::sched_setaffinity_big_or_primary();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hcs_offloading_mondrian_Utils_schedSetAffinityLittle(JNIEnv* env, jclass clazz) {
  return md::sched_setaffinity_little();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hcs_offloading_mondrian_JniRenderer_drawFrameByte(JNIEnv* env,
                                                       jclass clazz,
                                                       jobject surface,
                                                       jbyteArray img,
                                                       jint width,
                                                       jint height) {
  auto imgVal = reinterpret_cast<uint8_t*>(env->GetByteArrayElements(img, JNI_FALSE));

  ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
  if (window == nullptr) return false;
  ANativeWindow_acquire(window);
  ANativeWindow_Buffer buffer;

  ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
  int32_t errLock = ANativeWindow_lock(window, &buffer, nullptr);
  if (errLock != 0) {
    LOGE("ANativeWindow_lock failed with error code: %d", errLock);
    ANativeWindow_release(window);
    return false;
  }

  auto* outPtr = reinterpret_cast<uint8_t*>(buffer.bits);
  std::memcpy(outPtr, imgVal, 4 * width * height);

  int32_t errUnlockAndPost = ANativeWindow_unlockAndPost(window);
  if (errUnlockAndPost != 0) {
    LOGE("ANativeWindow_unlockAndPost failed with error code: %d", errUnlockAndPost);
    ANativeWindow_release(window);
    return false;
  }
  ANativeWindow_release(window);
  return true;
}
