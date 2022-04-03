#include <jni.h>

#include "strm/Log.hpp"
#include "CppInferenceEngine.hpp"

extern "C"
JNIEXPORT jlong JNICALL
Java_hcs_offloading_strmcpp_CppInferenceEngine_createCppInferenceEngine(JNIEnv* env, jobject thiz) {
  LOGD("JNI CppInferenceEngine.createCppInferenceEngine");
  return reinterpret_cast<long>(new rm::CppInferenceEngine());
}

extern "C"
JNIEXPORT jint JNICALL
Java_hcs_offloading_strmcpp_CppInferenceEngine_enqueue(JNIEnv* env, jobject thiz, jlong handle,
                                                       jlong matAddr) {
  auto* engine = (rm::CppInferenceEngine*) handle;
  auto* image = (cv::Mat*) matAddr;
  LOGD("JNI enqueue(Mat(%d, %d, %d))", image->cols, image->rows, image->channels());
  cv::Mat clonedMat = image->clone();
  return engine->enqueue(clonedMat, true);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_hcs_offloading_strmcpp_CppInferenceEngine_getResults(JNIEnv* env, jobject thiz, jlong handle,
                                                          jint request_handle) {
  auto* engine = (rm::CppInferenceEngine*) handle;
  std::vector<rm::BoundingBox> results = engine->getResults(request_handle);

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
