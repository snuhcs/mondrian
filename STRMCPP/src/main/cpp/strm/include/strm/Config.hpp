#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <vector>

#include <json/json.h>

namespace rm {

constexpr int NUM_LABELS = 80;

extern bool LOG_INTERNAL;

struct RoIExtractorConfig {
  int NUM_WORKERS = 2;
  int EXTRACTION_RESIZE_WIDTH = 640;
  int EXTRACTION_RESIZE_HEIGHT = 360;
  int MIN_ROI_AREA = 5000;
  int ROI_PADDING = 10;
  float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = 0.1;
  float MERGE_THRESHOLD = 0.5;
};

struct PatchReconstructorConfig {
  float FRAME_BOXES_IOU_THRESHOLD = 0.5;
  float OVERLAP_THRESHOLD = 0.8;
};

struct STRMConfig {
  int BUFFER_SIZE = 1000;
  int LATENCY_SLO_MS = 10000;
  RoIExtractorConfig roIExtractorConfig;
  PatchReconstructorConfig patchReconstructorConfig;
};

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json);
PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json);
STRMConfig parseSTRMConfig(const std::string& jsonPath);

} // namespace rm

#endif // CONFIG_HPP_
