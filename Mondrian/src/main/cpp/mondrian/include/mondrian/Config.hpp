#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <vector>

#include "json/json.h"

#include "mondrian/DataType.hpp"

namespace md {

constexpr int NUM_LABELS = 80;

struct ROIExtractorConfig {
  int MAX_QUEUE_SIZE;
  int NUM_WORKERS;
  float EXTRACTION_RESIZE_WIDTH;
  float EXTRACTION_RESIZE_HEIGHT;
  float MAX_PD_ROI_SIZE;
  float MIN_PD_ROI_SIZE;
  bool EAT_PD;
  float ROI_PADDING;
  int ROI_BORDER;
  float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD;
  float PD_FILTER_THRESHOLD;
  int PD_INTERVAL;
  bool MERGE;
  bool NO_DOWNSAMPLING_FOR_LAST_FRAME;
};

struct ROIResizerConfig {
  // Predictive model configs
  std::string DATASET; // TODO : remove redundancy
  int VOTING_WINDOW;
  float SCALE_SHIFT;
  float AREA_SHIFT;
  bool STATIC_SCALE;
  float STATIC_TARGET_SCALE;
  float MAX_OF_ROI_SIZE;

  // Reactive probing configs
  float PROBE_STEP_SIZE;
  int NUM_PROBE_STEPS;
  float PROBE_CONF_THRESHOLD;
  float PROBE_IOU_THRESHOLD;
};

struct InferenceEngineConfig {
  bool DRAW_INFERENCE_RESULT;
  std::string DATASET; // TODO : remove redundancy
  std::string MODEL;
  std::string RUNTIME;
  bool USE_TINY;
  float CONF_THRESHOLD;
  float IOU_THRESHOLD;
  int PROFILE_WARMUPS;
  int PROFILE_RUNS;
  int FULL_FRAME_SIZE; // TODO : remove redundancy
  std::vector<int> INPUT_SIZES; // TODO: Change to std::set
  std::vector<Device> DEVICES;
};

struct PatchReconstructorConfig {
  float FRAME_BOXES_IOU_THRESHOLD;
  float BOX_FILTER_OVERLAP_THRESHOLD;
  float ID_MAPPING_IOU_THRESHOLD;
};

extern bool LOG_INTERNAL;

struct MondrianConfig {
  bool LOG_EXECUTION;
  bool LOG_ROI;
  float INTERPOLATION_THRESHOLD;
  int FULL_FRAME_INTERVAL; // If FULL_FRAME_INTERVAL == 0, always run full frame inference
  int FULL_FRAME_SIZE; // TODO : remove redundancy
  Device FULL_DEVICE;
  int BUFFER_SIZE;
  int LATENCY_SLO_MS;
  bool USE_EMULATED_BATCH;
  bool USE_ROI_WISE_INFERENCE;
  int ROI_SIZE; // Used for Emulated Batch or ROI-wise Inference
  ROIExtractorConfig roiExtractorConfig;
  ROIResizerConfig roiResizerConfig;
  InferenceEngineConfig inferenceEngineConfig;
  PatchReconstructorConfig patchReconstructorConfig;

  std::string str() const;
};

ROIExtractorConfig parseROIExtractorConfig(const Json::Value& json);
ROIResizerConfig parseROIResizerConfig(const Json::Value& json);
InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json);
PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json);
MondrianConfig parseMondrianConfig(const std::string& jsonPath);

} // namespace md

#endif // CONFIG_HPP_
