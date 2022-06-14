#include "strm/Config.hpp"

#include <fstream>

#include "strm/Log.hpp"

namespace rm {

bool LOG_INTERNAL = true;

DispatcherConfig parseDispatcherConfig(const Json::Value& json) {
  DispatcherConfig config;
  if (!json["max_queue_size"].isNull()) {
    config.MAX_QUEUE_SIZE = json["max_queue_size"].asInt();
  }
  if (!json["full_inference_interval"].isNull()) {
    config.FULL_INFERENCE_INTERVAL = json["full_inference_interval"].asInt();
  }
  if (!json["roi_padding"].isNull()) {
    config.ROI_PADDING = json["roi_padding"].asInt();
  }
  return config;
}

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json) {
  RoIExtractorConfig config;
  if (!json["extraction_resize_width"].isNull()) {
    config.EXTRACTION_RESIZE_WIDTH = json["extraction_resize_width"].asInt();
  }
  if (!json["extraction_resize_height"].isNull()) {
    config.EXTRACTION_RESIZE_HEIGHT = json["extraction_resize_height"].asInt();
  }
  if (!json["min_roi_area"].isNull()) {
    config.MIN_ROI_AREA = json["min_roi_area"].asInt();
  }
  if (!json["optical_flow_roi_confidence_threshold"].isNull()) {
    config.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = json["optical_flow_roi_confidence_threshold"].asFloat();
  }
  if (!json["merge_threshold"].isNull()) {
    config.MERGE_THRESHOLD = json["merge_threshold"].asFloat();
  }
  if (!json["max_merged_roi_size"].isNull()) {
    config.MAX_MERGED_ROI_SIZE = json["max_merged_roi_size"].asInt();
  }
  if (!json["of_roi"].isNull()) {
    config.OF_ROI = json["of_roi"].asBool();
  }
  if (!json["pd_roi"].isNull()) {
    config.PD_ROI = json["pd_roi"].asBool();
  }
  if (!json["merge_roi"].isNull()) {
    config.MERGE_ROI = json["merge_roi"].asBool();
  }
  return config;
}

PatchMixerConfig parsePatchMixerConfig(const Json::Value& json) {
  PatchMixerConfig config;
  if (!json["packing"].isNull()) {
    config.PACKING = json["packing"].asBool();
  }
  if (!json["max_packed_frames"].isNull()) {
    config.MAX_PACKED_FRAMES = json["max_packed_frames"].asInt();
  }

  // mixed_frame_sizes is set at parseSTRMConfig()

  if (!json["latency_slo_ms"].isNull()) {
    config.LATENCY_SLO_MS = json["latency_slo_ms"].asInt();
  }
  return config;
}

PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json) {
  PatchReconstructorConfig config;
  if (!json["max_queue_size"].isNull()) {
    config.MAX_QUEUE_SIZE = json["max_queue_size"].asInt();
  }
  if (!json["frame_boxes_iou_threshold"].isNull()) {
    config.FRAME_BOXES_IOU_THRESHOLD = json["frame_boxes_iou_threshold"].asFloat();
  }
  if (!json["overlap_threshold"].isNull()) {
    config.OVERLAP_THRESHOLD = json["overlap_threshold"].asFloat();
  }
  return config;
}

STRMConfig parseSTRMConfig(const std::string& jsonPath, const std::vector<int>& inputSizes) {
  STRMConfig strmConfig;
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  if (!jsonFile.is_open()) {
    LOGE("Cannot open config file");
    return strmConfig;
  }
  Json::Value json;
  jsonFile >> json;
  if (!json.isObject()) {
    LOGE("Json parsing failed");
    return strmConfig;
  }
  LOGD("STRMConfig : %s", json.toStyledString().c_str());
  if (!json["log_internal"].isNull()) {
    LOG_INTERNAL = json["log_internal"].asBool();
  }
  if (!json["dispatcher"].isNull()) {
    strmConfig.dispatcherConfig = parseDispatcherConfig(json["dispatcher"]);
  }
  if (!json["roi_extractor"].isNull()) {
    strmConfig.roIExtractorConfig = parseRoIExtractorConfig(json["roi_extractor"]);
  }
  if (!json["patch_mixer"].isNull()) {
    strmConfig.patchMixerConfig = parsePatchMixerConfig(json["patch_mixer"]);
    strmConfig.patchMixerConfig.MIXED_FRAME_SIZES = inputSizes;
  }
  if (!json["patch_reconstructor"].isNull()) {
    strmConfig.patchReconstructorConfig = parsePatchReconstructorConfig(
        json["patch_reconstructor"]);
  }
  return strmConfig;
}

} // namespace rm
