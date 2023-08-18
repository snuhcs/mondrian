#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <set>
#include <string>
#include <vector>

#include "json/json.h"
#include "opencv2/core/types.hpp"

#include "mondrian/DataType.hpp"

namespace md {

struct ROIExtractorConfig {
  int NUM_WORKERS;
  cv::Size EXTRACTION_SIZE;
  int PD_INTERVAL;
  float MIN_PD_ROI_SIZE;
  float MAX_PD_ROI_SIZE;
  float OF_CONF_THRES;
  float OF_ROI_PADDING;
  float PD_EAT_OVERLAP_THRES;
  float PD_FILTER_OVERLAP_THRES;
  bool MERGE;
  int MAX_MERGE_SIZE;

  void print() const;
};

struct ROIResizerConfig {
  // Predictive model configs
  bool STATIC_AREA;
  float STATIC_TARGET_AREA;
  std::string DATASET;
  int VOTING_WINDOW_SIZE;
  float AREA_SHIFT;
  float SCALE_SHIFT;

  // Reactive probing configs
  int NUM_PROBE_STEPS;
  float PROBE_STEP_SIZE;
  float PROBE_CONF_THRES;
  float PROBE_IOU_THRES;

  void print() const;
};

struct ROIPackerConfig {
  bool NO_DOWNSAMPLING_FOR_LAST_FRAME;
  ROIPackerType TYPE;

  void print() const;
};

struct InferenceEngineConfig {
  int FULL_FRAME_SIZE;
  std::set<int> INPUT_SIZES;
  std::set<Device> DEVICES;
  bool DRAW_INFERENCE_RESULT;
  std::string DATASET;
  std::string MODEL;
  bool USE_TINY;
  float CONF_THRES;
  float IOU_THRES;
  int PROFILE_WARMUPS;
  int PROFILE_RUNS;

  void print() const;
};

struct PatchReconstructorConfig {
  float FRAME_BOXES_IOU_THRES;
  float BOX_FILTER_OVERLAP_THRES;
  float ID_MAPPING_IOU_THRES;

  void print() const;
};

struct MondrianConfig {
  ExecutionType EXECUTION_TYPE;
  bool LOG_BOXES;
  bool LOG_FRAME;
  bool LOG_ROI;
  float INTERPOLATION_THRES;
  int FULL_FRAME_INTERVAL;
  int FULL_FRAME_SIZE;
  Device FULL_DEVICE;
  int LATENCY_SLO_MS;
  int ROI_SIZE; // Used for Emulated Batch or ROI-wise Inference
  ROIExtractorConfig roiExtractorConfig;
  ROIResizerConfig roiResizerConfig;
  ROIPackerConfig roiPackerConfig;
  InferenceEngineConfig inferenceEngineConfig;
  PatchReconstructorConfig patchReconstructorConfig;

  void print() const;

  void test() const;
};

ROIExtractorConfig parseROIExtractorConfig(const Json::Value& json);
ROIResizerConfig parseROIResizerConfig(const Json::Value& json);
ROIPackerConfig parseROIPackerConfig(const Json::Value& json);
InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json);
PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json);
MondrianConfig parseMondrianConfig(const std::string& jsonPath);

} // namespace md

#endif // CONFIG_HPP_
