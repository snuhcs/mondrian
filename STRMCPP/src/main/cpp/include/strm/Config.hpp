#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>

namespace rm {

struct DispatcherConfig {
  int MAX_QUEUE_SIZE = 2;
  int FULL_INFERENCE_INTERVAL = 4;
  int ROI_PADDING = 10;
};

struct RoIExtractorConfig {
  int EXTRACTION_RESIZE_WIDTH = 640;
  int EXTRACTION_RESIZE_HEIGHT = 360;
  float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = 0.3f;
  float MERGE_THRESHOLD = 0.5f;
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
  int MATCH_PADDING = 40;
  float USE_IOU_THRESHOLD = 0.1f;
};

struct STRMConfig {
  DispatcherConfig dispatcherConfig;
  RoIExtractorConfig roIExtractorConfig;
  PatchMixerConfig patchMixerConfig;
  PatchReconstructorConfig patchReconstructorConfig;
};

STRMConfig parseSTRMConfig(const std::string& jsonPath);

} // namespace rm

#endif // CONFIG_HPP_
