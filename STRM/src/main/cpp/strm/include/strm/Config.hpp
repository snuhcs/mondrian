#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <vector>

#include <json/json.h>

#include "strm/DataType.hpp"

namespace rm {

constexpr int NUM_LABELS = 80;

struct RoIExtractorConfig {
  int MAX_QUEUE_SIZE = 200;
  int NUM_WORKERS = 2;
  float EXTRACTION_RESIZE_WIDTH = 960;
  float EXTRACTION_RESIZE_HEIGHT = 540;
  float MIN_ROI_AREA = 0;
  bool EAT_PD = true;
  float ROI_PADDING = 10;
  float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = 0.1;
  float PD_FILTER_THRESHOLD = 0.5;
  int PD_INTERVAL = 5;
  bool MERGE = true;
  bool NO_DOWNSAMPLING_FOR_LAST_FRAME = true;
};

struct RoIResizerConfig {
  // Predictive model configs
  std::string TRAIN_DATA;
  float RESIZE_SMOOTHING_FACTOR = 0.1; // static resize when RESIZE_SMOOTHING_FACTOR == 0.0
  float STATIC_TARGET_SCALE = 1.0;

  // Reactive probing configs
  float PROBE_STEP_SIZE = 0.05; // No probing when PROBE_STEP_SIZE == 0
  int NUM_PROBE_STEPS = 1;
  float OVERLAP_THRESHOLD = 0.8;
  float ABSOLUTE_CONFIDENCE_THRESHOLD = 0.3;
  float RELATIVE_CONFIDENCE_THRESHOLD = 0.1;
};

struct InferenceEngineConfig {
  bool DRAW_INFERENCE_RESULT = true;
  std::string MODEL = "YOLO_V5";
  std::string RUNTIME = "TFLITE";
  bool USE_TINY = false;
  float CONF_THRESHOLD = 0.1;
  float IOU_THRESHOLD = 0.5;
  int PROFILE_WARMUPS = 5;
  int PROFILE_RUNS = 5;
  std::vector<int> INPUT_SIZES = {
      768
  };
  std::vector<Device> DEVICES = {
      GPU
  };
};

struct PatchReconstructorConfig {
  float FRAME_BOXES_IOU_THRESHOLD = 0.5;
  float BOX_FILTER_OVERLAP_THRESHOLD = 0.8;
  float ID_MAPPING_IOU_THRESHOLD = 0.1;
};

extern bool LOG_INTERNAL;

struct STRMConfig {
  bool LOG_EXECUTION = true;
  bool LOG_ROI = true;
  bool ALLOW_INTERPOLATION = false;
  float INTERPOLATION_THRESHOLD = 0.5f;
  int FULL_FRAME_INTERVAL = 1; // If FULL_FRAME_INTERVAL == 0, always run full frame inference
  int FULL_FRAME_SIZE = 768;
  int BUFFER_SIZE = 500;
  int LATENCY_SLO_MS = 10000;
  bool USE_EMULATED_BATCH = false;
  bool USE_ROI_WISE_INFERENCE = false;
  int ROI_SIZE = 64; // Used for Emulated Batch or RoI-wise Inference
  RoIExtractorConfig roIExtractorConfig;
  RoIResizerConfig roiResizerConfig;
  InferenceEngineConfig inferenceEngineConfig;
  PatchReconstructorConfig patchReconstructorConfig;
};

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json);
RoIResizerConfig parseRoIResizerConfig(const Json::Value& json);
InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json);
PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json);
STRMConfig parseSTRMConfig(const std::string& jsonPath);

} // namespace rm

#endif // CONFIG_HPP_
