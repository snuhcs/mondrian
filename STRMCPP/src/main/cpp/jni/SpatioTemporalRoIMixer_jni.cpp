#include <jni.h>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/SpatioTemporalRoIMixer.hpp"
#include "CustomResizeProfile.hpp"
#include "CustomRoIPrioritizer.hpp"
#include "CppInferenceEngine.hpp"
#include "JavaInferenceEngine.hpp"

static jboolean isCopy = JNI_TRUE;

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_createSpatioTemporalRoIMixer(JNIEnv* env,
                                                                                jobject thiz) {
  std::string jsonPath = "/data/local/tmp/strmcpp.json";
  return reinterpret_cast<jlong>(new rm::SpatioTemporalRoIMixer(rm::parseSTRMConfig(jsonPath),
                                                                new rm::CustomResizeProfile(),
                                                                new rm::CustomRoIPrioritizer(),
                                                                new rm::CppInferenceEngine()));
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_enqueueImage(JNIEnv* env, jobject thiz,
                                                                jlong handle, jstring key,
                                                                jint frameIndex, jlong matAddr) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  auto* image = (cv::Mat*) matAddr;
  const char* k = env->GetStringUTFChars(key, &isCopy);
  strm->enqueueImage(std::string(k), frameIndex, image);
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
  jmethodID BoundingBox_init = env->GetMethodID(class_BoundingBox, "<init>", "(IIIIFLjava/lang/String;)V");

  jobject boxes = env->NewObject(class_ArrayList, ArrayList_init);
  for (int i = 0; i < results.size(); i++) {
    const rm::BoundingBox& b = results.at(i);
    jstring labelName = env->NewStringUTF(b.labelName.c_str());
    jobject box = env->NewObject(class_BoundingBox, BoundingBox_init,
                                 b.location.left, b.location.top, b.location.right, b.location.bottom,
                                 b.confidence, labelName);
    env->CallVoidMethod(boxes, ArrayList_add, i, box);
  }
  return boxes;
}

extern "C"
JNIEXPORT void JNICALL
Java_hcs_offloading_strmcpp_SpatioTemporalRoIMixer_addSource(JNIEnv* env, jobject thiz,
                                                             jlong handle, jstring key) {
  auto* strm = (rm::SpatioTemporalRoIMixer*) handle;
  const char* k = env->GetStringUTFChars(key, &isCopy);
  strm->addSource(std::string(k));
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
  strm->close();
}
