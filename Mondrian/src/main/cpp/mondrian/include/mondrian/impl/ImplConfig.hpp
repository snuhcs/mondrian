#ifndef IMPL_CONFIG_HPP_
#define IMPL_CONFIG_HPP_

#include <string>
#include <vector>

#include <json/json.h>
#include "mondrian/DataType.hpp"

namespace md {

struct VideoConfig {
  std::string PATH;
  std::pair<int, int> FRAME_RANGE;
  int FPS = 0;
};

struct IMPLConfig {
  std::vector<VideoConfig> videoConfigs;
};

std::vector<VideoConfig> parseVideoConfigs(const Json::Value& json);
IMPLConfig parseIMPLConfig(const std::string& jsonPath);

} // namespace md

#endif // IMPL_CONFIG_HPP_
