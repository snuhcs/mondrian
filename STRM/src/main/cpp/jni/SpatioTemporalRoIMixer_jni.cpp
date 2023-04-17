#include <jni.h>
#include <android/log.h>

#include <thread>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/Log.hpp"
#include "strm/SpatioTemporalRoIMixer.hpp"
#include "strm/impl/ImplConfig.hpp"

static JavaVM* vm;

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strm_Emulator_createSpatioTemporalRoIMixer(JNIEnv* env, jobject thiz) {
  rm::IMPLConfig implConfig = rm::parseIMPLConfig("/data/local/tmp/strm.json");
  env->GetJavaVM(&vm);
  rm::STRMConfig config = rm::parseSTRMConfig("/data/local/tmp/strm.json");
  std::map<int, int> startIndices;
  for (int vid = 0; vid < implConfig.videoConfigs.size(); vid++) {
    startIndices[vid] = implConfig.videoConfigs[vid].FRAME_RANGE.first;
  }
  return reinterpret_cast<long>(new rm::SpatioTemporalRoIMixer(
      config, startIndices, vm, env, thiz));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strm_Emulator_enqueueImage(JNIEnv* env, jobject thiz,
                                               jlong handle, jint vid, jlong yuvMatAddr) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* yuvMat = (cv::Mat*) yuvMatAddr;
  strm->enqueueImage(vid, *yuvMat);
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strm_Emulator_close(JNIEnv* env, jobject thiz, jlong handle) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  delete strm;
}
