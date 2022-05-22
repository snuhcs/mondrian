#ifndef IMPL_CUSTOM_INFERENCE_ENGINE_H
#define IMPL_CUSTOM_INFERENCE_ENGINE_H

#include <map>
#include <queue>

#include "strm/InferenceEngine.hpp"
#include "strm/impl/ImplConfig.hpp"
#include "strm/impl/models/Classifier.hpp"

namespace rm {

class Worker;

class CustomInferenceEngine : public InferenceEngine {
 public:
  CustomInferenceEngine(const InferenceEngineConfig& config);

  int enqueue(const cv::Mat mat, const bool isFull) override;

  std::vector<BoundingBox> getResults(const int handle) override;

  long long getInferenceTimeMs() override;

  std::tuple<int, const cv::Mat, bool> getInput();

  void enqueueResults(const int handle, const std::vector<BoundingBox>& boxes);

 private:
  template <typename T>
  void initClassifiers(const InferenceEngineConfig& config);

  std::vector<std::unique_ptr<Worker>> workers;
  std::vector<std::unique_ptr<Classifier>> classifiers;
  std::vector<std::unique_ptr<Classifier>> fullClassifiers;

  int mHandle;
  std::queue<std::tuple<int, const cv::Mat, bool>> inputs;
  std::mutex inputMtx;
  std::condition_variable inputCv;
  std::map<int, std::vector<BoundingBox>> results;
  std::mutex resultMtx;
  std::condition_variable resultCv;
};

} // namespace rm

#endif // IMPL_CUSTOM_INFERENCE_ENGINE_H
