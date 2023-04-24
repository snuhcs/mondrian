#include <jni.h>
#include <android/log.h>

#include <fstream>

#include "json/json.h"
#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/Mondrian.hpp"

static std::map<int, int> parseStartIndexMap(const std::string& jsonPath) {
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  assert(jsonFile.is_open());
  Json::Value json;
  jsonFile >> json;
  assert(json.isObject());
  std::map<int, int> startIndexMap;
  for (int vid = 0; vid < json["video_configs"].size(); vid++) {
    startIndexMap[vid] = json["video_configs"][vid]["frame_range"][0].asInt();
  }
  return startIndexMap;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_mondrian_MondrianApp_createHandle(JNIEnv* env, jobject thiz) {
  auto configPath = "/data/local/tmp/config.json";
  auto config = md::parseMondrianConfig(configPath);
  auto startIndexMap = parseStartIndexMap(configPath);
  return reinterpret_cast<long>(new md::Mondrian(config, startIndexMap, env, thiz));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_mondrian_MondrianApp_enqueueImage(JNIEnv* env, jobject thiz,
                                                      jlong handle, jint vid, jlong yuvMatAddr) {
  auto* mondrian = (md::Mondrian*) handle;
  auto* yuvMat = (cv::Mat*) yuvMatAddr;
  mondrian->enqueueImage(vid, *yuvMat);
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_mondrian_MondrianApp_close(JNIEnv* env, jobject thiz, jlong handle) {
  auto* mondrian = (md::Mondrian*) handle;
  delete mondrian;
}
