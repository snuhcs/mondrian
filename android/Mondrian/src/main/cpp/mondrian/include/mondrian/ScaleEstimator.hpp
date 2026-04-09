#ifndef SCALE_ESTIMATOR_HPP_
#define SCALE_ESTIMATOR_HPP_

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "json/json.h"

namespace md {

class ScaleEstimator {
 public:
  ScaleEstimator() = default;

  void load(const std::string& jsonPath);

  int predict(float maxEdgeLength, float area, float xyRatio,
              float shiftAvg, float shiftStd, float shiftNcc, float avgErr) const;

  const std::vector<float>& thresholds() const { return thresholds_; }

  bool isLoaded() const { return root_ != nullptr; }

 private:
  struct Node {
    bool isLeaf;
    int value;          // leaf value
    int featureIndex;   // internal node: which feature to split on
    float threshold;    // internal node: threshold
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
  };

  std::unique_ptr<Node> parseNode(const Json::Value& json) const;

  std::vector<std::string> featureNames_;
  std::vector<float> thresholds_;
  std::unique_ptr<Node> root_;
};

} // namespace md

#endif // SCALE_ESTIMATOR_HPP_
