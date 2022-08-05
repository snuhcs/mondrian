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
  float PD_FILTER_THRESHOLD = 0.5;
};

struct RoIResizerConfig {
  // Margin to add after resize target estimation
  float RESIZE_MARGIN = 10;

  // Predictive model configs
  float RESIZE_SMOOTHING_FACTOR = 0.1; // static resize when RESIZE_SMOOTHING_FACTOR == 0.0
  float STATIC_RESIZE_TARGET = 100;

  // Reactive probing configs
  int PROBE_STEP_SIZE = 5; // No probing when PROBE_STEP_SIZE == 0
  int NUM_PROBE_STEPS = 1; // Num probes == 2 * NUM_PROBE_STEPS + 1
  float PROBE_RESET_THRESHOLD = 10.0;
  float OVERLAP_THRESHOLD = 0.8;
  float ABSOLUTE_CONFIDENCE_THRESHOLD = 0.3;
  float RELATIVE_CONFIDENCE_THRESHOLD = 0.1;
};

struct PatchMixerConfig {
  bool MERGE = true;
  float MERGE_THRESHOLD = 0.5;
  bool N_WAY_MIXING = true;
  bool PRIORITY_MIXING = true;
};

struct PatchReconstructorConfig {
  float FRAME_BOXES_IOU_THRESHOLD = 0.5;
  float OVERLAP_THRESHOLD = 0.8;
};

struct STRMConfig {
  bool ROI_WISE_INFERENCE = false;
  bool EMULATED_BATCH = false;
  int FULL_FRAME_INTERVAL = 1; // If FULL_FRAME_INTERVAL == 0, always run full frame inference
  int BUFFER_SIZE = 1000;
  int LATENCY_SLO_MS = 10000;
  RoIExtractorConfig roIExtractorConfig;
  RoIResizerConfig roIResizerConfig;
  PatchMixerConfig patchMixerConfig;
  PatchReconstructorConfig patchReconstructorConfig;
};

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json);
RoIResizerConfig parseRoIResizerConfig(const Json::Value& json);
PatchMixerConfig parsePatchMixerConfig(const Json::Value& json);
PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json);
STRMConfig parseSTRMConfig(const std::string& jsonPath);

} // namespace rm

#endif // CONFIG_HPP_
