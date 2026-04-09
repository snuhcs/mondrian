#include "mondrian/ScaleEstimator.hpp"

#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <map>

#include "mondrian/Log.hpp"

namespace md {

void ScaleEstimator::load(const std::string& jsonPath) {
  std::ifstream file(jsonPath, std::ifstream::binary);
  if (!file.is_open()) {
    LOGE("[ScaleEstimator] Failed to open: %s", jsonPath.c_str());
    assert(false);
  }

  // Read file into string and replace Infinity with a large number
  // (jsoncpp cannot parse bare Infinity)
  std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
  // Replace "Infinity" (as a JSON value, not inside strings)
  std::string search = "Infinity";
  std::string replace = "1e18";
  size_t pos = 0;
  while ((pos = content.find(search, pos)) != std::string::npos) {
    content.replace(pos, search.length(), replace);
    pos += replace.length();
  }

  Json::Value json;
  Json::CharReaderBuilder builder;
  std::string errors;
  std::istringstream stream(content);
  if (!Json::parseFromStream(builder, stream, &json, &errors)) {
    LOGE("[ScaleEstimator] Parse error: %s", errors.c_str());
    assert(false);
  }

  // Parse feature names
  featureNames_.clear();
  for (const auto& name : json["feature_names"]) {
    featureNames_.push_back(name.asString());
  }

  // Parse thresholds (area levels)
  thresholds_.clear();
  for (const auto& t : json["thresholds"]) {
    if (t.isString() && t.asString() == "Infinity") {
      thresholds_.push_back(1e10f);
    } else {
      thresholds_.push_back(t.asFloat());
    }
  }

  // Parse tree
  root_ = parseNode(json["tree"]);

  LOGD("[ScaleEstimator] Loaded from %s (%zu features, %zu levels)",
       jsonPath.c_str(), featureNames_.size(), thresholds_.size());
}

std::unique_ptr<ScaleEstimator::Node> ScaleEstimator::parseNode(const Json::Value& json) const {
  auto node = std::make_unique<Node>();

  if (json.isMember("value")) {
    node->isLeaf = true;
    node->value = json["value"].asInt();
    return node;
  }

  node->isLeaf = false;
  node->threshold = json["threshold"].asFloat();

  // Map feature name to index
  std::string featureName = json["feature"].asString();
  node->featureIndex = -1;
  for (int i = 0; i < (int)featureNames_.size(); i++) {
    if (featureNames_[i] == featureName) {
      node->featureIndex = i;
      break;
    }
  }
  assert(node->featureIndex >= 0 && "Unknown feature name in scale estimator JSON");

  node->left = parseNode(json["left"]);
  node->right = parseNode(json["right"]);
  return node;
}

int ScaleEstimator::predict(float maxEdgeLength, float area, float xyRatio,
                            float shiftAvg, float shiftStd, float shiftNcc, float avgErr) const {
  assert(root_ != nullptr && "ScaleEstimator not loaded");

  float features[] = {maxEdgeLength, area, xyRatio, shiftAvg, shiftStd, shiftNcc, avgErr};

  const Node* node = root_.get();
  while (!node->isLeaf) {
    if (features[node->featureIndex] <= node->threshold) {
      node = node->left.get();
    } else {
      node = node->right.get();
    }
  }
  return node->value;
}

} // namespace md
