#ifndef IMPL_CONFIG_HPP_
#define IMPL_CONFIG_HPP_

#include <string>
#include <vector>

#include <json/json.h>
#include "strm/DataType.hpp"

namespace rm {

struct VideoConfig {
  std::string PATH;
  int FPS = 0;
};

struct InferenceEngineConfig {
  std::string MODEL = "YOLO_V4";
  std::string RUNTIME = "TFLITE";
  bool USE_TINY = false;
  float CONF_THRESHOLD = 0.1;
  float IOU_THRESHOLD = 0.5;
  int NUM_WORKERS = 1;
  std::vector<int> INPUT_SIZES = {
      800
  };
  std::vector<Device> DEVICES = {
      GPU
  };
};

struct IMPLConfig {
  bool DRAW_OUTPUT = false;
  bool DRAW_INFERENCE_RESULT = true;
  std::vector<VideoConfig> videoConfigs;
  InferenceEngineConfig inferenceEngineConfig;
};

std::vector<VideoConfig> parseVideoConfigs(const Json::Value& json);
InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json);
IMPLConfig parseIMPLConfig(const std::string& jsonPath);

} // namespace rm

#endif // IMPL_CONFIG_HPP_
