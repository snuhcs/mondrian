#include <jni.h>
#include <android/log.h>

#include <thread>
#include <vector>

#include "opencv2/videoio.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/DataType.hpp"
#include "mondrian/Log.hpp"
#include "mondrian/Mondrian.hpp"
#include "mondrian/impl/ImplConfig.hpp"

static JavaVM* vm;

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_mondrian_MondrianApp_createHandle(JNIEnv* env, jobject thiz) {
  md::IMPLConfig implConfig = md::parseIMPLConfig("/data/local/tmp/mondrian.json");
  env->GetJavaVM(&vm);
  md::MondrianConfig config = md::parseMondrianConfig("/data/local/tmp/mondrian.json");
  std::map<int, int> startIndices;
  for (int vid = 0; vid < implConfig.videoConfigs.size(); vid++) {
    startIndices[vid] = implConfig.videoConfigs[vid].FRAME_RANGE.first;
  }
  return reinterpret_cast<long>(new md::Mondrian(
      config, startIndices, vm, env, thiz));
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
