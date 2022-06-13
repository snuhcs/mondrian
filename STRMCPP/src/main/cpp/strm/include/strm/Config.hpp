#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>

#include <json/json.h>

namespace rm {

constexpr int NUM_LABELS = 80;

struct RoIExtractorConfig {
  int NUM_WORKERS = 1;
  int EXTRACTION_RESIZE_WIDTH = 640;
  int EXTRACTION_RESIZE_HEIGHT = 360;
  int MIN_ROI_AREA = 5000;
  float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = 0.1;
  float MERGE_THRESHOLD = 0.5;
  int MAX_MERGED_ROI_SIZE = 800;
  bool OF_ROI = true;
  bool PD_ROI = true;
  bool MERGE_ROI = true;
};

struct PatchMixerConfig {
  bool PACKING = true;
  int MAX_PACKED_FRAMES = 30;
  int MIXED_FRAME_SIZE = 800;
};

struct PatchReconstructorConfig {
  int MAX_QUEUE_SIZE = 2;
  float FRAME_BOXES_IOU_THRESHOLD = 0.5;
  float OVERLAP_THRESHOLD = 0.8;
};

struct STRMConfig {
  int MAX_QUEUE_SIZE = 2;
  int FULL_INFERENCE_INTERVAL = 4;
  int ROI_PADDING = 10;
  int LATENCY_SLO_MS = 5000;
  RoIExtractorConfig roIExtractorConfig;
  PatchMixerConfig patchMixerConfig;
  PatchReconstructorConfig patchReconstructorConfig;
};

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json);
PatchMixerConfig parsePatchMixerConfig(const Json::Value& json);
PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json);
STRMConfig parseSTRMConfig(const std::string& jsonPath);

} // namespace rm

#endif // CONFIG_HPP_
