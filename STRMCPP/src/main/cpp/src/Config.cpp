#include "strm/Config.hpp"

#include <json/json.h>
#include <fstream>

namespace rm {

DispatcherConfig parseDispatcherConfig(const Json::Value& json) {

}

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json) {

}

PatchMixerConfig parsePatchMixerConfig(const Json::Value& json) {

}

PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json) {

}

STRMConfig parseSTRMConfig(const std::string& jsonPath) {
  STRMConfig strmConfig;
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  if (!jsonFile.is_open()) {
    return strmConfig;
  }
  Json::Value root;
  jsonFile >> root;
  if (!root.isObject()) {
    return strmConfig;
  }
  if (!root["dispatcher"].isNull()) {
    strmConfig.dispatcherConfig = parseDispatcherConfig(root["dispatcher"]);
  }
  if (!root["roi_extractor"].isNull()) {
    strmConfig.roIExtractorConfig = parseRoIExtractorConfig(root["roi_extractor"]);
  }
  if (!root["patch_mixer"].isNull()) {
    strmConfig.patchMixerConfig = parsePatchMixerConfig(root["patch_mixer"]);
  }
  if (!root["patch_reconstructor"].isNull()) {
    strmConfig.patchReconstructorConfig = parsePatchReconstructorConfig(root["patch_reconstructor"]);
  }
  return strmConfig;
}

} // namespace rm

