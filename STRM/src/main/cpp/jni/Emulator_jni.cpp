#include <jni.h>

#include <thread>

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
static long resizeProfileHandle = (long) nullptr;
static long inferenceEngineHandle = (long) nullptr;

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strm_Emulator_createSpatioTemporalRoIMixer(JNIEnv* env,
                                                               jobject thiz) {
  rm::STRMConfig config = rm::parseSTRMConfig("/data/local/tmp/strm.json");
  rm::IMPLConfig implConfig = rm::parseIMPLConfig("/data/local/tmp/strm.json");
  rm::ResizeProfile* resizeProfile;
  if (implConfig.resizeProfileConfig.ACCURACY_AWARE_RESIZE) {
    resizeProfile = reinterpret_cast<rm::ResizeProfile*>(new rm::AccuracyAwareResizeProfile(
        implConfig.resizeProfileConfig.RESIZE_MARGIN,
        implConfig.resizeProfileConfig.RESIZE_SMOOTHING_FACTOR,
        implConfig.resizeProfileConfig.PROBING_STEP));
  } else {
    resizeProfile = reinterpret_cast<rm::ResizeProfile*>(new rm::StaticResizeProfile(
        implConfig.resizeProfileConfig.STATIC_TARGET_SIZE));
  }
  env->GetJavaVM(&vm);
  auto* inferenceEngine = reinterpret_cast<rm::InferenceEngine*>(new rm::CustomInferenceEngine(
      implConfig.inferenceEngineConfig, vm, env, thiz, implConfig.DRAW_INFERENCE_RESULT));
  resizeProfileHandle = reinterpret_cast<long>(resizeProfile);
  inferenceEngineHandle = reinterpret_cast<long>(inferenceEngine);
  return reinterpret_cast<long>(new rm::SpatioTemporalRoIMixer(
      config, resizeProfile, inferenceEngine, implConfig.NUM_VIDEOS,
      vm, env, thiz, implConfig.DRAW_OUTPUT, implConfig.resizeProfileConfig.PROBING));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strm_Emulator_close(JNIEnv* env, jobject thiz,
                                        jlong handle) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* resizeProfile = (rm::ResizeProfile*) resizeProfileHandle;
  auto* inferenceEngine = (rm::InferenceEngine*) inferenceEngineHandle;
  delete strm;
  delete resizeProfile;
  delete inferenceEngine;
}
