#include <jni.h>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/Log.hpp"
#include "strm/SpatioTemporalRoIMixer.hpp"
#include "strm/ResizeProfile.hpp"
#include "strm/impl/ImplConfig.hpp"
#include "strm/impl/AccuracyAwareResizeProfile.hpp"
#include "strm/impl/StaticResizeProfile.hpp"
#include "strm/impl/CustomRoIPrioritizer.hpp"
#include "strm/impl/CustomInferenceEngine.hpp"

static JavaVM* vm;
static jboolean isCopy = JNI_TRUE;
static long resizeProfileHandle = (long) nullptr;
static long roiPrioritizerHandle = (long) nullptr;
static long inferenceEngineHandle = (long) nullptr;

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_createSpatioTemporalRoIMixer(JNIEnv* env,
                                                                                jobject thiz,
                                                                                jboolean draw) {
  std::string jsonPath = "/data/local/tmp/strmcpp.json";
  std::string implJsonPath = "/data/local/tmp/edgedevicecpp.json";
  rm::IMPLConfig config = rm::parseIMPLConfig(implJsonPath);
  auto* resizeProfile = config.resizeProfileConfig.ACCURACY_AWARE_RESIZE ?
                        reinterpret_cast<rm::ResizeProfile*>(new rm::AccuracyAwareResizeProfile(
                            config.resizeProfileConfig.RESIZE_MARGIN)) :
                        reinterpret_cast<rm::ResizeProfile*>(new rm::StaticResizeProfile(
                            config.resizeProfileConfig.STATIC_TARGET_SIZE));
  auto* roIPrioritizer = reinterpret_cast<rm::RoIPrioritizer*>(new rm::CustomRoIPrioritizer());
  env->GetJavaVM(&vm);
  auto* inferenceEngine = reinterpret_cast<rm::InferenceEngine*>(new rm::CustomInferenceEngine(
      config.inferenceEngineConfig, vm, env, thiz, draw == JNI_TRUE));
  resizeProfileHandle = reinterpret_cast<long>(resizeProfile);
  roiPrioritizerHandle = reinterpret_cast<long>(roIPrioritizer);
  inferenceEngineHandle = reinterpret_cast<long>(inferenceEngine);
  return reinterpret_cast<long>(new rm::SpatioTemporalRoIMixer(
      rm::parseSTRMConfig(jsonPath), resizeProfile, roIPrioritizer, inferenceEngine));
}

extern "C"
JNIEXPORT jint JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_enqueueImage(JNIEnv* env, jobject thiz,
                                                                jlong handle, jstring key,
                                                                jlong matAddr) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* image = (cv::Mat*) matAddr;
  const char* k = env->GetStringUTFChars(key, &isCopy);
  return strm->enqueueImage(std::string(k), *image);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_getResults(JNIEnv* env, jobject thiz,
                                                              jlong handle, jstring key,
                                                              jint frame_index) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  const char* k = env->GetStringUTFChars(key, &isCopy);
  std::vector<rm::BoundingBox> results = strm->getResults(std::string(k), frame_index);

  jclass class_ArrayList = env->FindClass("java/util/ArrayList");
  jclass class_BoundingBox = env->FindClass("hcs/offloading/strmcpp/BoundingBox");
  jmethodID ArrayList_init = env->GetMethodID(class_ArrayList, "<init>", "()V");
  jmethodID ArrayList_add = env->GetMethodID(class_ArrayList, "add", "(ILjava/lang/Object;)V");
  jmethodID BoundingBox_init = env->GetMethodID(class_BoundingBox, "<init>",
                                                "(IIIIFLjava/lang/String;)V");

  jobject boxes = env->NewObject(class_ArrayList, ArrayList_init);
  for (int i = 0; i < results.size(); i++) {
    const rm::BoundingBox& b = results.at(i);
    jstring labelName = env->NewStringUTF(b.labelName.c_str());
    jobject box = env->NewObject(class_BoundingBox, BoundingBox_init,
                                 b.location.left, b.location.top, b.location.right,
                                 b.location.bottom,
                                 b.confidence, labelName);
    env->CallVoidMethod(boxes, ArrayList_add, i, box);
  }
  return boxes;
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_removeSource(JNIEnv* env, jobject thiz,
                                                                jlong handle, jstring key) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  const char* k = env->GetStringUTFChars(key, &isCopy);
  strm->removeSource(std::string(k));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_close(JNIEnv* env, jobject thiz,
                                                         jlong handle) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* resizeProfile = (rm::ResizeProfile*) resizeProfileHandle;
  auto* roiPrioritizer = (rm::RoIPrioritizer*) roiPrioritizerHandle;
  auto* inferenceEngine = (rm::InferenceEngine*) inferenceEngineHandle;
  delete strm;
  delete resizeProfile;
  delete roiPrioritizer;
  delete inferenceEngine;
}
