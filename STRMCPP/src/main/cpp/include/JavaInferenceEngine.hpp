#ifndef CUSTOM_INFERENCE_ENGINE_HPP
#define CUSTOM_INFERENCE_ENGINE_HPP

#include <jni.h>

#include "strm/InferenceEngine.hpp"

namespace rm {

class JavaInferenceEngine : public InferenceEngine {
 public:
  JavaInferenceEngine(JNIEnv* env, jobject inferenceEngine)
  : env(env), InferenceEngine_object(inferenceEngine) {
    InferenceEngine_class = env->FindClass("hcs/offloading/strmcpp/InferenceEngine");
    InferenceEngine_enqueue = env->GetMethodID(InferenceEngine_class, "enqueue", "(Lorg/opencv/core/Mat;Z)I");
    InferenceEngine_getResults = env->GetMethodID(InferenceEngine_class, "getResults", "(I)Ljava/util/List;");
    Mat_class = env->FindClass("org/opencv/core/Mat");
    Mat_init = env->GetMethodID(Mat_class, "<init>", "()V");
    Mat_dataAddr = env->GetMethodID(Mat_class, "dataAddr", "()J");
    List_class = env->FindClass("java/util/List");
    List_size = env->GetMethodID(List_class, "size", "()I");
    List_get = env->GetMethodID(List_class, "get", "(I)Ljava/lang/Object;");
    BoundingBox_class = env->FindClass("hcs/offloading/strmcpp/BoundingBox");
    left_id = env->GetFieldID(BoundingBox_class, "left", "I");
    top_id = env->GetFieldID(BoundingBox_class, "top", "I");
    right_id = env->GetFieldID(BoundingBox_class, "right", "I");
    bottom_id = env->GetFieldID(BoundingBox_class, "bottom", "I");
    confidence_id = env->GetFieldID(BoundingBox_class, "confidence", "F");
    labelName_id = env->GetFieldID(BoundingBox_class, "labelName", "Ljava/lang/String;");
  };

  int enqueue(const cv::Mat& mat, bool isFull) {
    jobject jmat = env->NewObject(Mat_class, Mat_init);
    jlong nativeAddr = env->CallLongMethod(jmat, Mat_dataAddr);
    cv::Mat* jniMat = (cv::Mat*) nativeAddr;
    jniMat->create(mat.rows, mat.cols, mat.type());
    std::memcpy(jniMat->data, mat.data, jniMat->step * jniMat->rows);
    return env->CallIntMethod(InferenceEngine_object, InferenceEngine_enqueue, jmat, isFull);
  };

  std::vector<BoundingBox> getResults(const int handle) {
    jobject List_BoundingBox = env->CallObjectMethod(InferenceEngine_object, InferenceEngine_getResults, handle);
    int size = (int) env->CallIntMethod(List_BoundingBox, List_size);
    std::vector<BoundingBox> results;
    for (int i = 0; i < size; i++) {
      jobject box = env->CallObjectMethod(List_BoundingBox, List_get, i);
      const char* labelName = env->GetStringUTFChars(env->GetObjectField(box, labelName_id));
      results.push_back(BoundingBox(
              Rect(env->GetIntField(box, left_id),
                   env->GetIntField(box, top_id),
                   env->GetIntField(box, right_id),
                   env->GetIntField(box, bottom_id)),
                   env->GetFloatField(box, confidence_id),
                   std::string(labelName)));
    }
    return results;
  }

 private:
  JNIEnv* env;
  jobject InferenceEngine_object;

  jclass InferenceEngine_class;
  jmethodID InferenceEngine_enqueue;
  jmethodID InferenceEngine_getResults;

  jclass Mat_class;
  jmethodID Mat_init;
  jmethodID Mat_dataAddr;

  jclass List_class;
  jmethodID List_size;
  jmethodID List_get;

  jclass BoundingBox_class;
  jfieldID left_id;
  jfieldID top_id;
  jfieldID right_id;
  jfieldID bottom_id;
  jfieldID confidence_id;
  jfieldID labelName_id;
};

} // namespace rm

#endif // CUSTOM_INFERENCE_ENGINE_HPP
