#include "strm/Config.hpp"

#include <fstream>

#include "strm/Log.hpp"

namespace rm {

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
  if (!json["optical_flow_roi_confidence_threshold"].isNull()) {
    config.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = json["optical_flow_roi_confidence_threshold"].asFloat();
  }
  if (!json["merge_threshold"].isNull()) {
    config.MERGE_THRESHOLD = json["merge_threshold"].asFloat();
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
  if (!json["match_padding"].isNull()) {
    config.MATCH_PADDING = json["match_padding"].asInt();
  }
  if (!json["use_iou_threshold"].isNull()) {
    config.USE_IOU_THRESHOLD = json["use_iou_threshold"].asFloat();
  }
  return config;
}

STRMConfig parseSTRMConfig(const std::string& jsonPath) {
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
  if (!json["dispatcher"].isNull()) {
    strmConfig.dispatcherConfig = parseDispatcherConfig(json["dispatcher"]);
  }
  if (!json["roi_extractor"].isNull()) {
    strmConfig.roIExtractorConfig = parseRoIExtractorConfig(json["roi_extractor"]);
  }
  if (!json["patch_mixer"].isNull()) {
    strmConfig.patchMixerConfig = parsePatchMixerConfig(json["patch_mixer"]);
  }
  if (!json["patch_reconstructor"].isNull()) {
    strmConfig.patchReconstructorConfig = parsePatchReconstructorConfig(
        json["patch_reconstructor"]);
  }
  return strmConfig;
}

} // namespace rm

