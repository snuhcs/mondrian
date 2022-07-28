#include <jni.h>
#include <android/log.h>

#include <thread>
#include <vector>

#include "opencv2/videoio.hpp"

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/Log.hpp"
#include "strm/SpatioTemporalRoIMixer.hpp"
#include "strm/impl/ImplConfig.hpp"
#include "strm/impl/CustomInferenceEngine.hpp"

static JavaVM* vm;
static jboolean isCopy = JNI_TRUE;
static rm::InferenceEngine* inferenceEngine = nullptr;

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strm_Emulator_createSpatioTemporalRoIMixer(JNIEnv* env,
                                                               jobject thiz) {
  rm::IMPLConfig implConfig = rm::parseIMPLConfig("/data/local/tmp/strm.json");
  env->GetJavaVM(&vm);
  inferenceEngine = reinterpret_cast<rm::InferenceEngine*>(new rm::CustomInferenceEngine(
      implConfig.inferenceEngineConfig, vm, env, thiz, implConfig.DRAW_INFERENCE_RESULT));

  rm::STRMConfig config = rm::parseSTRMConfig("/data/local/tmp/strm.json");
  return reinterpret_cast<long>(new rm::SpatioTemporalRoIMixer(
      config, inferenceEngine, (int) implConfig.videoConfigs.size(),
      vm, env, thiz, implConfig.DRAW_OUTPUT));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strm_Emulator_enqueueImage(JNIEnv* env, jobject thiz, jlong handle, jstring key,
                                               jlong matAddr) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* image = (cv::Mat*) matAddr;
  const char* k = env->GetStringUTFChars(key, &isCopy);
  strm->enqueueImage(std::string(k), *image);
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strm_Emulator_close(JNIEnv* env, jobject thiz, jlong handle) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  delete strm;
  delete inferenceEngine;
  inferenceEngine = nullptr;
}
