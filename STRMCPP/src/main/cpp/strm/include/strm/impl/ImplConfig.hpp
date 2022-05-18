#ifndef IMPL_CONFIG_HPP_
#define IMPL_CONFIG_HPP_

#include <string>

#include <json/json.h>

namespace rm {

struct ResizeProfileConfig {
  bool FIT_RESIZE = false;
  bool MERGED_RESIZE = true;
  int PERSON_THRESHOLD = 160;
  int CLASS_AGNOSTIC_THRESHOLD = 160;
};

struct InferenceEngineConfig {
  std::string MODEL = "YOLO_V4";
  std::string RUNTIME = "TFLITE";
  bool USE_TINY = false;
  float CONF_THRESHOLD = 0.1;
  float IOU_THRESHOLD = 0.5;
  int NUM_WORKERS = 1;
  int INPUT_SIZE = 800;
  int FULL_FRAME_INPUT_SIZE = 800;
};

struct IMPLConfig {
  ResizeProfileConfig resizeProfileConfig;
  InferenceEngineConfig inferenceEngineConfig;
};

ResizeProfileConfig parseResizeProfileConfig(const Json::Value& json);
InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json);
IMPLConfig parseIMPLConfig(const std::string& jsonPath);

} // namespace rm

#endif // IMPL_CONFIG_HPP_
