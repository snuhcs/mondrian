#include "strm/impl/ImplConfig.hpp"

#include <fstream>

#include "strm/Log.hpp"

namespace rm {

std::vector<VideoConfig> parseVideoConfigs(const Json::Value& json) {
  std::vector<VideoConfig> configs;
  for (const auto& j: json) {
    VideoConfig config;
    if (!j["path"].isNull()) {
      config.PATH = j["path"].asString();
    }
    if (!j["fps"].isNull()) {
      config.FPS = j["fps"].asInt();
    }
    configs.push_back(config);
  };
  return configs;
}

InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json) {
  InferenceEngineConfig config;
  if (!json["model"].isNull()) {
    config.MODEL = json["model"].asString();
  }
  if (!json["runtime"].isNull()) {
    config.RUNTIME = json["runtime"].asString();
  }
  if (!json["use_tiny"].isNull()) {
    config.USE_TINY = json["use_tiny"].asBool();
  }
  if (!json["conf_threshold"].isNull()) {
    config.CONF_THRESHOLD = json["conf_threshold"].asFloat();
  }
  if (!json["iou_threshold"].isNull()) {
    config.IOU_THRESHOLD = json["iou_threshold"].asFloat();
  }
  if (!json["num_workers"].isNull()) {
    config.NUM_WORKERS = json["num_workers"].asInt();
  }
  if (!json["input_sizes"].isNull()) {
    const Json::Value inputSizes = json["input_sizes"];
    config.INPUT_SIZES.clear();
    for (const auto& size : inputSizes) {
      config.INPUT_SIZES.push_back(size.asInt());
    }
    std::sort(config.INPUT_SIZES.begin(), config.INPUT_SIZES.end());
  }
  return config;
}

IMPLConfig parseIMPLConfig(const std::string& jsonPath) {
  IMPLConfig implConfig;
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  if (!jsonFile.is_open()) {
    LOGE("Cannot open config file");
    return implConfig;
  }
  Json::Value json;
  jsonFile >> json;
  if (!json.isObject()) {
    LOGE("Json parsing failed");
    return implConfig;
  }
  if (!json["draw_output"].isNull()) {
    implConfig.DRAW_OUTPUT = json["draw_output"].asBool();
  }
  if (!json["draw_inference_result"].isNull()) {
    implConfig.DRAW_INFERENCE_RESULT = json["draw_inference_result"].asBool();
  }
  if (!json["video_configs"].isNull()) {
    implConfig.videoConfigs = parseVideoConfigs(json["video_configs"]);
  }
  if (!json["inference_engine"].isNull()) {
    implConfig.inferenceEngineConfig = parseInferenceEngineConfig(json["inference_engine"]);
  }
  return implConfig;
}

} // namespace rm
