#include <jni.h>
#include <android/log.h>

#include <fstream>

#include "json/json.h"
#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/Mondrian.hpp"
#include "mondrian/Utils.hpp"

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_mondrian_MondrianApp_createHandle(JNIEnv* env, jobject thiz, jint numVideos) {
  auto configPath = "/data/local/tmp/config.json";
  auto config = md::parseMondrianConfig(configPath);
  return reinterpret_cast<long>(new md::Mondrian(config, numVideos, env, thiz));
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
Java_hcs_offloading_mondrian_MondrianApp_close(JNIEnv* env, jobject thiz, jlong handle) {
  auto* mondrian = (md::Mondrian*) handle;
  delete mondrian;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hcs_offloading_mondrian_Utils_schedSetAffinityPrimary(JNIEnv* env, jclass clazz) {
  return md::sched_setaffinity_primary();
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
