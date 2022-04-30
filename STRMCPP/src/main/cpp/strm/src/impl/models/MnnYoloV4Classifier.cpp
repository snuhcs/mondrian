#include "strm/impl/models/MnnYoloV4Classifier.hpp"

#include <chrono>
#include <map>
#include <set>

#include "strm/Log.hpp"

namespace rm {

MnnYoloV4Classifier::MnnYoloV4Classifier(int size, float confThreshold, float iouThreshold, bool isTiny)
    : INPUT_SIZE(size), OUTPUT_WIDTH((size / 32) * (size / 32) * 63),
      CONF_THRESHOLD(confThreshold), IOU_THRESHOLD(iouThreshold), inferenceTimeMs(-1) {
  LOGD("YoloV4 MnnYoloV4Classifier::MnnYoloV4Classifier()");
  std::string filepath = "/data/local/tmp/models/yolov4-";
  filepath += (isTiny ? "tiny-" : "") + std::to_string(size) + "-fp16.mnn";
  interpreter = MNN::Interpreter::createFromFile(filepath.c_str());
  if (interpreter == nullptr) {
    LOGE("YoloV4 interpreter creation failed");
    return;
  } else {
    LOGD("YoloV4 interpreter created");
  }

  MNN::ScheduleConfig conf;
//  conf.type = MNN_FORWARD_OPENCL; // TODO: Fail to execute with OpenCL (GPU)
  session = interpreter->createSession(conf);
  if (session == nullptr) {
    LOGE("YoloV4 session creation failed");
    return;
  } else {
    LOGD("YoloV4 session created");
  }

  std::map<std::string, MNN::Tensor*> inputTensorMap = interpreter->getSessionInputAll(session);
  std::map<std::string, MNN::Tensor*> outputTensorMap = interpreter->getSessionOutputAll(session);
  auto getShapeString = [](const std::vector<int>& shape) {
    std::string shapeString;
    for (int i = 0; i < shape.size(); i++) {
      shapeString += std::to_string(shape[i]);
      if (i != shape.size() - 1) {
        shapeString += ", ";
      }
    }
    return shapeString;
  };
  std::string inputInfo;
  for (auto& it : inputTensorMap) {
    inputInfo += it.first + ": " + getShapeString(it.second->shape());
    inputInfo += " | ";
  }
  LOGD("YoloV4 inputs  : %s", inputInfo.c_str());

  std::string outputInfo;
  for (auto& it : outputTensorMap) {
    outputInfo += it.first + ": " + getShapeString(it.second->shape());
    outputInfo += " | ";
  }
  LOGD("YoloV4 outputs : %s", outputInfo.c_str());
}

std::vector<BoundingBox> MnnYoloV4Classifier::recognizeImage(
    const cv::Mat& mat, int originalWidth, int originalHeight) {
  return nms(getDetectionsForFull(mat, originalWidth, originalHeight));
}

std::vector<BoundingBox> MnnYoloV4Classifier::getDetectionsForFull(
    const cv::Mat& mat, int originalWidth, int originalHeight) {
  MNN::Tensor* inputTensor = interpreter->getSessionInputAll(session).at(INPUT_TENSOR_NAME);
  MNN::Tensor* outputBoxes = interpreter->getSessionOutputAll(session).at(OUTPUT_TENSOR_NAME_BOXES);
  MNN::Tensor* outputConfs = interpreter->getSessionOutputAll(session).at(OUTPUT_TENSOR_NAME_CONFS);

  assert(inputTensor->size() == mat.total() * mat.elemSize());
  std::memcpy((void*) inputTensor->host<float>(), (void*) mat.data, inputTensor->size());

  auto start = std::chrono::system_clock::now();
  interpreter->runSession(session);
  auto end = std::chrono::system_clock::now();
  inferenceTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  LOGV("YoloV4 Inference %lld ms", inferenceTimeMs);

  std::vector<BoundingBox> detections;
  auto* bboxes = outputBoxes->host<float>();
  auto* confidences = outputConfs->host<float>();
  for (int i = 0; i < OUTPUT_WIDTH; i++) {
    float maxConfidence = 0;
    int maxLabel = -1;
    for (int label = 0; label < NUM_LABELS; label++) {
      float confidence = confidences[i * NUM_LABELS + label];
      if (maxConfidence < confidence) {
        maxLabel = label;
        maxConfidence = confidence;
      }
    }
    if (maxLabel == 0 && maxConfidence > CONF_THRESHOLD) {
      float xPos = bboxes[i * 4 + 0];
      float yPos = bboxes[i * 4 + 1];
      float w = bboxes[i * 4 + 2];
      float h = bboxes[i * 4 + 3];
      detections.emplace_back(Rect(
          std::max(0, (int) ((xPos - w / 2) * (float) originalWidth / (float) INPUT_SIZE)),
          std::max(0, (int) ((yPos - h / 2) * (float) originalHeight / (float) INPUT_SIZE)),
          std::min(originalWidth,
                   (int) ((xPos + w / 2) * (float) originalWidth / (float) INPUT_SIZE)),
          std::min(originalHeight,
                   (int) ((yPos + h / 2) * (float) originalHeight / (float) INPUT_SIZE))),
                              maxConfidence, "person");
    }
  }
  return detections;
}

std::vector<BoundingBox> MnnYoloV4Classifier::nms(const std::vector<BoundingBox>& boxes) const {
  std::vector<BoundingBox> nmsList;

  auto comp = [](const BoundingBox& l, const BoundingBox& r) -> bool {
    return l.confidence > r.confidence;
  };
  for (int k = 0; k < NUM_LABELS; k++) {
    if (k != 0) {
      continue;
    }
    std::set<BoundingBox, decltype(comp)> sortedBoxes(comp);

    for (const BoundingBox& box : boxes) {
//      if (box.labelName == <labelName for k>) {
      if (box.labelName == "person") {
        sortedBoxes.insert(box);
      }
    }

    while (!sortedBoxes.empty()) {
      auto startIt = sortedBoxes.begin();
      const BoundingBox& max = *startIt;
      nmsList.push_back(max);
      sortedBoxes.erase(startIt);

      for (auto it = sortedBoxes.begin(); it != sortedBoxes.end();) {
        if (max.location.iou(it->location) >= IOU_THRESHOLD) {
          it = sortedBoxes.erase(it);
        } else {
          it++;
        }
      }
    }
  }
  return nmsList;
}

int MnnYoloV4Classifier::getInputSize() const {
  return INPUT_SIZE;
}

long long MnnYoloV4Classifier::getInferenceTimeMs() const {
  return inferenceTimeMs;
}

} // namespace rm
