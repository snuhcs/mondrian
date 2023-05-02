#include "mondrian/Config.hpp"

#include <fstream>

#include "mondrian/DataType.hpp"
#include "mondrian/Log.hpp"

namespace md {

static int parseInt(const Json::Value& json, const std::string& key) {
  assert(!json[key].isNull());
  return json[key].asInt();
}

static float parseFloat(const Json::Value& json, const std::string& key) {
  assert(!json[key].isNull());
  return json[key].asFloat();
}

static bool parseBool(const Json::Value& json, const std::string& key) {
  assert(!json[key].isNull());
  return json[key].asBool();
}

static std::string parseString(const Json::Value& json, const std::string& key) {
  assert(!json[key].isNull());
  return json[key].asString();
}

ExecutionType toExecutionType(const std::string& executionTypeStr) {
  if (executionTypeStr == "mondrian") {
    return MONDRIAN;
  } else if (executionTypeStr == "emulated_batch") {
    return EMULATED_BATCH;
  } else if (executionTypeStr == "roi_wise_inference") {
    return ROI_WISE_INFERENCE;
  } else {
    LOGE("Unknown execution type: %s", executionTypeStr.c_str());
    assert(false);
  }
}

ROIExtractorConfig parseROIExtractorConfig(const Json::Value& json) {
  ROIExtractorConfig config = {};
  config.MAX_QUEUE_SIZE = parseInt(json, "max_queue_size");
  config.NUM_WORKERS = parseInt(json, "num_workers");
  config.EXTRACTION_RESIZE_WIDTH = parseFloat(json, "extraction_resize_width");
  config.EXTRACTION_RESIZE_HEIGHT = parseFloat(json, "extraction_resize_height");
  config.MAX_PD_ROI_SIZE = parseFloat(json, "max_pd_roi_size");
  config.MIN_PD_ROI_SIZE = parseFloat(json, "min_pd_roi_size");
  config.EAT_PD = parseBool(json, "eat_pd");
  config.ROI_PADDING = parseFloat(json, "roi_padding");
  config.ROI_BORDER = parseInt(json, "roi_border");
  config.OF_CONF_THRESHOLD = parseFloat(json, "of_conf_threshold");
  config.PD_FILTER_THRESHOLD = parseFloat(json, "pd_filter_threshold");
  config.PD_INTERVAL = parseInt(json, "pd_interval");
  config.MERGE = parseBool(json, "merge");
  config.NO_DOWNSAMPLING_FOR_LAST_FRAME = parseBool(json, "no_downsampling_for_last_frame");
  return config;
}

ROIResizerConfig parseROIResizerConfig(const Json::Value& json) {
  ROIResizerConfig config = {};
  config.DATASET = parseString(json, "dataset");
  config.VOTING_WINDOW = parseInt(json, "voting_window");
  config.SCALE_SHIFT = parseFloat(json, "scale_shift");
  config.AREA_SHIFT = parseFloat(json, "area_shift");
  config.STATIC_SCALE = parseBool(json, "static_scale");
  config.STATIC_TARGET_SCALE = parseFloat(json, "static_target_scale");
  config.MAX_OF_ROI_SIZE = parseFloat(json, "max_of_roi_size");
  config.PROBE_STEP_SIZE = parseFloat(json, "probe_step_size");
  config.NUM_PROBE_STEPS = parseInt(json, "num_probe_steps");
  config.PROBE_CONF_THRESHOLD = parseFloat(json, "probe_conf_threshold");
  config.PROBE_IOU_THRESHOLD = parseFloat(json, "probe_iou_threshold");
  return config;
}

InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json) {
  InferenceEngineConfig config = {};
  config.DRAW_INFERENCE_RESULT = parseBool(json, "draw_inference_result");
  config.DATASET = parseString(json, "dataset");
  config.MODEL = parseString(json, "model");
  config.RUNTIME = parseString(json, "runtime");
  config.USE_TINY = parseBool(json, "use_tiny");
  config.CONF_THRESHOLD = parseFloat(json, "conf_threshold");
  config.IOU_THRESHOLD = parseFloat(json, "iou_threshold");
  config.PROFILE_WARMUPS = parseInt(json, "profile_warmups");
  config.PROFILE_RUNS = parseInt(json, "profile_runs");
  config.FULL_FRAME_SIZE = parseInt(json, "full_frame_size");
  for (const auto& inputSizeJson: json["input_sizes"]) {
    config.INPUT_SIZES.push_back(inputSizeJson.asInt());
  }
  for (const auto& deviceJson: json["devices"]) {
    config.DEVICES.push_back(toDevice(deviceJson.asString()));
  }
  return config;
}

PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json) {
  PatchReconstructorConfig config = {};
  config.FRAME_BOXES_IOU_THRESHOLD = parseFloat(json, "frame_boxes_iou_threshold");
  config.BOX_FILTER_OVERLAP_THRESHOLD = parseFloat(json, "box_filter_overlap_threshold");
  config.ID_MAPPING_IOU_THRESHOLD = parseFloat(json, "id_mapping_iou_threshold");
  return config;
}

MondrianConfig parseMondrianConfig(const std::string& jsonPath) {
  MondrianConfig config;
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  assert(jsonFile.is_open());
  Json::Value json;
  jsonFile >> json;
  assert(json.isObject());

  config.LOG_EXECUTION = parseBool(json, "log_execution");
  config.LOG_ROI = parseBool(json, "log_roi");
  config.EXECUTION_TYPE = toExecutionType(parseString(json, "execution_type"));
  config.INTERPOLATION_THRESHOLD = parseFloat(json, "interpolation_threshold");
  config.FULL_FRAME_INTERVAL = parseInt(json, "full_frame_interval");
  config.FULL_FRAME_SIZE = parseInt(json, "full_frame_size");
  config.FULL_DEVICE = toDevice(parseString(json, "full_device"));
  config.BUFFER_SIZE = parseInt(json, "buffer_size");
  config.LATENCY_SLO_MS = parseInt(json, "latency_slo_ms");
  config.ROI_SIZE = parseInt(json, "roi_size");

  config.roiExtractorConfig = parseROIExtractorConfig(json["roi_extractor"]);
  config.roiResizerConfig = parseROIResizerConfig(json["roi_resizer"]);
  config.inferenceEngineConfig = parseInferenceEngineConfig(json["inference_engine"]);
  config.patchReconstructorConfig = parsePatchReconstructorConfig(json["patch_reconstructor"]);
  return config;
}

std::string MondrianConfig::str() const {
  return std::string();
}

} // namespace md
