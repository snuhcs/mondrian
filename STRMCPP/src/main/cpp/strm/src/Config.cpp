#include "strm/Config.hpp"

#include <fstream>

#include "strm/Log.hpp"

namespace rm {

bool LOG_INTERNAL = true;

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json) {
  RoIExtractorConfig config;
  if (!json["num_workers"].isNull()) {
    config.NUM_WORKERS = json["num_workers"].asInt();
  }
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
  if (!json["mixed_frame_size"].isNull()) {
    config.MIXED_FRAME_SIZE = json["mixed_frame_size"].asInt();
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

STRMConfig parseSTRMConfig(const std::string& jsonPath) {
  STRMConfig config;
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  if (!jsonFile.is_open()) {
    LOGE("Cannot open config file");
    return config;
  }
  Json::Value json;
  jsonFile >> json;
  if (!json.isObject()) {
    LOGE("Json parsing failed");
    return config;
  }
  LOGD("STRMConfig : %s", json.toStyledString().c_str());
  if (!json["log_internal"].isNull()) {
    LOG_INTERNAL = json["log_internal"].asBool();
  }
  if (!json["buffer_size"].isNull()) {
    config.BUFFER_SIZE = json["buffer_size"].asInt();
  }
  if (!json["roi_padding"].isNull()) {
    config.ROI_PADDING = json["roi_padding"].asInt();
  }
  if (!json["latency_slo_ms"].isNull()) {
    config.LATENCY_SLO_MS = json["latency_slo_ms"].asInt();
  }

  if (!json["roi_extractor"].isNull()) {
    config.roIExtractorConfig = parseRoIExtractorConfig(json["roi_extractor"]);
  }
  if (!json["patch_mixer"].isNull()) {
    config.patchMixerConfig = parsePatchMixerConfig(json["patch_mixer"]);
  }
  if (!json["patch_reconstructor"].isNull()) {
    config.patchReconstructorConfig = parsePatchReconstructorConfig(
        json["patch_reconstructor"]);
  }
  return config;
}

} // namespace rm
