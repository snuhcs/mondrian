#include <jni.h>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/SpatioTemporalRoIMixer.hpp"
#include "CustomResizeProfile.hpp"
#include "CustomRoIPrioritizer.hpp"
#include "CppInferenceEngine.h"
#include "JavaInferenceEngine.hpp"

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_createSpatioTemporalRoIMixer(JNIEnv* env,
                                                                                jobject thiz,
                                                                                jobject inferenceEngine) {
  std::string jsonPath = "/data/local/tmp/strm.json";
  return reinterpret_cast<jlong>(new rm::SpatioTemporalRoIMixer(rm::parseSTRMConfig(jsonPath),
                                                                new rm::CustomResizeProfile(),
                                                                new rm::CustomRoIPrioritizer(),
                                                                new rm::CppInferenceEngine()));
//                                                                new rm::JavaInferenceEngine(env, inferenceEngine)));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_enqueueImage(JNIEnv* env, jobject thiz,
                                                                jlong strm_handle, jstring key,
                                                                jint frameIndex, jlong matAddr) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) strm_handle;
  auto* image = (cv::Mat*)matAddr;
  const char* k = env->GetStringUTFChars(key, JNI_FALSE);
  strm->enqueueImage(std::string(k), frameIndex, image);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_getResults(JNIEnv* env, jobject thiz,
                                                              jlong strm_handle, jstring key,
                                                              jint frame_index) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) strm_handle;
  const char* k = env->GetStringUTFChars(key, JNI_FALSE);
  std::vector<rm::BoundingBox> results = strm->getResults(std::string(k), frame_index);
  jclass arrayListClass = env->FindClass("java/util/ArrayList");
  jmethodID constructor = env->GetMethodID(arrayListClass, "<init>", "()V");
  jobject arrayList = env->NewObject(arrayListClass, constructor);
  jmethodID add = env->GetMethodID(arrayListClass, "add", "(ILjava/lang/Object;)V");
  jclass boxClass = env->FindClass("hcs/offloading/strmcpp/BoundingBox");
  jmethodID box_init = env->GetMethodID(boxClass, "<init>", "(IIIIFLjava/lang/String;)V");
  for (int i = 0; i < results.size(); i++) {
    const rm::BoundingBox& b = results.at(i);
    jstring labelName = env->NewStringUTF(b.labelName.c_str());
    jobject box = env->NewObject(boxClass, box_init, b.location.left, b.location.top, b.location.right, b.location.bottom, b.confidence, labelName);
    env->CallVoidMethod(arrayList, add, i, box);
  }
  return arrayList;
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_addSource(JNIEnv* env, jobject thiz,
                                                             jlong strm_handle, jstring key) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) strm_handle;
  const char* k = env->GetStringUTFChars(key, JNI_FALSE);
  strm->addSource(std::string(k));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_removeSource(JNIEnv* env, jobject thiz,
                                                                jlong strm_handle, jstring key) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) strm_handle;
  const char* k = env->GetStringUTFChars(key, JNI_FALSE);
  strm->removeSource(std::string(k));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_close(JNIEnv* env, jobject thiz,
                                                         jlong strm_handle) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) strm_handle;
  strm->close();
}