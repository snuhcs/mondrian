#include <jni.h>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/Log.hpp"
#include "strm/SpatioTemporalRoIMixer.hpp"
#include "strm/ResizeProfile.hpp"
#include "strm/impl/ImplConfig.hpp"
#include "strm/impl/AccuracyAwareResizeProfile.hpp"
#include "strm/impl/StaticResizeProfile.hpp"
#include "strm/impl/CustomInferenceEngine.hpp"

static JavaVM* vm;
static jboolean isCopy = JNI_TRUE;
static long resizeProfileHandle = (long) nullptr;
static long inferenceEngineHandle = (long) nullptr;

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_createSpatioTemporalRoIMixer(JNIEnv* env,
                                                                                jobject thiz) {
  std::string jsonPath = "/data/local/tmp/strmcpp.json";
  std::string implJsonPath = "/data/local/tmp/edgedevicecpp.json";
  rm::IMPLConfig config = rm::parseIMPLConfig(implJsonPath);
  rm::ResizeProfile* resizeProfile;
  if (config.resizeProfileConfig.ACCURACY_AWARE_RESIZE) {
    resizeProfile = reinterpret_cast<rm::ResizeProfile*>(new rm::AccuracyAwareResizeProfile(
        config.resizeProfileConfig.RESIZE_MARGIN,
        config.resizeProfileConfig.RESIZE_SMOOTHING_FACTOR,
        config.resizeProfileConfig.PROBING_STEP));
  } else {
    resizeProfile = reinterpret_cast<rm::ResizeProfile*>(new rm::StaticResizeProfile(
        config.resizeProfileConfig.STATIC_TARGET_SIZE));
  }
  env->GetJavaVM(&vm);
  auto* inferenceEngine = reinterpret_cast<rm::InferenceEngine*>(new rm::CustomInferenceEngine(
      config.inferenceEngineConfig, vm, env, thiz, config.DRAW_INFERENCE_RESULT));
  resizeProfileHandle = reinterpret_cast<long>(resizeProfile);
  inferenceEngineHandle = reinterpret_cast<long>(inferenceEngine);
  return reinterpret_cast<long>(new rm::SpatioTemporalRoIMixer(
      rm::parseSTRMConfig(jsonPath), resizeProfile, inferenceEngine, config.NUM_VIDEOS,
      vm, env, thiz, config.DRAW_OUTPUT, config.resizeProfileConfig.PROBING));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_enqueueImage(JNIEnv* env, jobject thiz,
                                                                jlong handle, jstring key,
                                                                jlong matAddr) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* image = (cv::Mat*) matAddr;
  const char* k = env->GetStringUTFChars(key, &isCopy);
  strm->enqueueImage(std::string(k), *image);
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_close(JNIEnv* env, jobject thiz,
                                                         jlong handle) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* resizeProfile = (rm::ResizeProfile*) resizeProfileHandle;
  auto* inferenceEngine = (rm::InferenceEngine*) inferenceEngineHandle;
  delete strm;
  delete resizeProfile;
  delete inferenceEngine;
}
