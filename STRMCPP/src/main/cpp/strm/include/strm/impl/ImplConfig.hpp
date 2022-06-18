#ifndef IMPL_CONFIG_HPP_
#define IMPL_CONFIG_HPP_

#include <string>

#include <json/json.h>

namespace rm {

struct ResizeProfileConfig {
  bool ACCURACY_AWARE_RESIZE = true;
  int RESIZE_MARGIN = 10;
  int STATIC_TARGET_SIZE = 80;
};

struct InferenceEngineConfig {
  std::string MODEL = "YOLO_V4";
  std::string RUNTIME = "TFLITE";
  bool USE_TINY = false;
  float CONF_THRESHOLD = 0.1;
  float IOU_THRESHOLD = 0.5;
  int NUM_WORKERS = 1;
  std::vector<int> INPUT_SIZES = {800};
};

struct IMPLConfig {
  int NUM_VIDEOS;
  ResizeProfileConfig resizeProfileConfig;
  InferenceEngineConfig inferenceEngineConfig;
};

ResizeProfileConfig parseResizeProfileConfig(const Json::Value& json);
InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json);
IMPLConfig parseIMPLConfig(const std::string& jsonPath);

} // namespace rm

#endif // IMPL_CONFIG_HPP_
