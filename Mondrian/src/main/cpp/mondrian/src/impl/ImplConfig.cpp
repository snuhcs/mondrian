#include "mondrian/impl/ImplConfig.hpp"

#include <fstream>

#include "mondrian/Log.hpp"

namespace md {

std::vector<VideoConfig> parseVideoConfigs(const Json::Value& json) {
  std::vector<VideoConfig> configs;
  for (const auto& j: json) {
    VideoConfig config;
    if (!j["path"].isNull()) {
      config.PATH = j["path"].asString();
    }
    if (!j["frame_range"].isNull()) {
      config.FRAME_RANGE.first = j["frame_range"][0].asInt();
      config.FRAME_RANGE.second = j["frame_range"][1].asInt();
    }
    if (!j["fps"].isNull()) {
      config.FPS = j["fps"].asInt();
    }
    configs.push_back(config);
  };
  return configs;
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
  if (!json["video_configs"].isNull()) {
    implConfig.videoConfigs = parseVideoConfigs(json["video_configs"]);
  }
  return implConfig;
}

} // namespace md
