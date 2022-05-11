#include "strm/impl/ImplConfig.hpp"

#include <fstream>

#include "strm/Log.hpp"

namespace rm {

ResizeProfileConfig parseResizeProfileConfig(const Json::Value& json) {
  ResizeProfileConfig config;
  if (!json["fit_resize"].isNull()) {
    config.FIT_RESIZE = json["fit_resize"].asBool();
  }
  if (!json["merged_resize"].isNull()) {
    config.MERGED_RESIZE = json["merged_resize"].asBool();
  }
  if (!json["person_threshold"].isNull()) {
    config.PERSON_THRESHOLD = json["person_threshold"].asInt();
  }
  if (!json["class_agnostic_threshold"].isNull()) {
    config.CLASS_AGNOSTIC_THRESHOLD = json["class_agnostic_threshold"].asInt();
  }
  return config;
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
  if (!json["input_size"].isNull()) {
    config.INPUT_SIZE = json["input_size"].asInt();
  }
  if (!json["full_frame_input_size"].isNull()) {
    config.FULL_FRAME_INPUT_SIZE = json["full_frame_input_size"].asInt();
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
  LOGD("IMPLConfig : %s", json.toStyledString().c_str());
  if (!json["resize_profile"].isNull()) {
    implConfig.resizeProfileConfig = parseResizeProfileConfig(json["resize_profile"]);
  }
  if (!json["inference_engine"].isNull()) {
    implConfig.inferenceEngineConfig = parseInferenceEngineConfig(json["inference_engine"]);
  }
  return implConfig;
}

} // namespace rm
