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

ExecutionType executionTypeOf(const std::string& executionTypeStr) {
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

std::string str(const ExecutionType& executionType) {
  if (executionType == MONDRIAN) {
    return "mondrian";
  } else if (executionType == EMULATED_BATCH) {
    return "emulated_batch";
  } else if (executionType == ROI_WISE_INFERENCE) {
    return "roi_wise_inference";
  } else {
    LOGE("Unknown execution type: %d", executionType);
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
    config.DEVICES.push_back(deviceOf(deviceJson.asString()));
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
  config.EXECUTION_TYPE = executionTypeOf(parseString(json, "execution_type"));
  config.INTERPOLATION_THRESHOLD = parseFloat(json, "interpolation_threshold");
  config.FULL_FRAME_INTERVAL = parseInt(json, "full_frame_interval");
  config.FULL_FRAME_SIZE = parseInt(json, "full_frame_size");
  config.FULL_DEVICE = deviceOf(parseString(json, "full_device"));
  config.BUFFER_SIZE = parseInt(json, "buffer_size");
  config.LATENCY_SLO_MS = parseInt(json, "latency_slo_ms");
  config.ROI_SIZE = parseInt(json, "roi_size");

  config.roiExtractorConfig = parseROIExtractorConfig(json["roi_extractor"]);
  config.roiResizerConfig = parseROIResizerConfig(json["roi_resizer"]);
  config.inferenceEngineConfig = parseInferenceEngineConfig(json["inference_engine"]);
  config.patchReconstructorConfig = parsePatchReconstructorConfig(json["patch_reconstructor"]);
  return config;
}

bool MondrianConfig::isValid() const {
  std::set<std::string> datasets = {"virat", "mta"};

  // ROIResizer
  if (datasets.find(roiResizerConfig.DATASET) == datasets.end()) return false;
  if (roiResizerConfig.PROBE_STEP_SIZE <= 0) return false;
  if (EXECUTION_TYPE != MONDRIAN && roiResizerConfig.NUM_PROBE_STEPS != 0) return false;

  // InferenceEngine
  if (inferenceEngineConfig.DATASET != roiResizerConfig.DATASET) return false;
  if (inferenceEngineConfig.DEVICES.empty()) return false;
  if (inferenceEngineConfig.INPUT_SIZES.empty()) return false;
  if (EXECUTION_TYPE == ROI_WISE_INFERENCE) {
    if (inferenceEngineConfig.INPUT_SIZES.size() != 1) return false;
    if (ROI_SIZE != inferenceEngineConfig.INPUT_SIZES[0]) return false;
  }
  bool isInputSizeSorted = std::is_sorted(inferenceEngineConfig.INPUT_SIZES.begin(),
                                          inferenceEngineConfig.INPUT_SIZES.end());
  if (!isInputSizeSorted) return false;
  if (inferenceEngineConfig.FULL_FRAME_SIZE != FULL_FRAME_SIZE) return false;
  if (std::find(inferenceEngineConfig.DEVICES.begin(),
                inferenceEngineConfig.DEVICES.end(),
                FULL_DEVICE) == inferenceEngineConfig.DEVICES.end())
    return false;
  bool isDivisible = std::all_of(
      inferenceEngineConfig.INPUT_SIZES.begin(), inferenceEngineConfig.INPUT_SIZES.end(),
      [this](int input_size) { return input_size % ROI_SIZE == 0; });
  if (EXECUTION_TYPE == EMULATED_BATCH && !isDivisible) return false;
  return true;
}

void MondrianConfig::print() const {
  std::stringstream ss;
  ss << "========== MondrianConfig ==========" << std::endl;
  ss << "LOG_EXECUTION: " << LOG_EXECUTION << std::endl;
  ss << "LOG_ROI: " << LOG_ROI << std::endl;
  ss << "EXECUTION_TYPE: " << md::str(EXECUTION_TYPE) << std::endl;
  ss << "INTERPOLATION_THRESHOLD: " << INTERPOLATION_THRESHOLD << std::endl;
  ss << "FULL_FRAME_INTERVAL: " << FULL_FRAME_INTERVAL << std::endl;
  ss << "FULL_FRAME_SIZE: " << FULL_FRAME_SIZE << std::endl;
  ss << "FULL_DEVICE: " << md::str(FULL_DEVICE) << std::endl;
  ss << "BUFFER_SIZE: " << BUFFER_SIZE << std::endl;
  ss << "LATENCY_SLO_MS: " << LATENCY_SLO_MS << std::endl;
  ss << "ROI_SIZE: " << ROI_SIZE << std::endl;
  LOGD("%s", ss.str().c_str());
  roiExtractorConfig.print();
  roiResizerConfig.print();
  inferenceEngineConfig.print();
  patchReconstructorConfig.print();
  LOGD("====================================");
}

void ROIExtractorConfig::print() const {
  std::stringstream ss;
  ss << "========== ROIExtractorConfig ==========" << std::endl;
  ss << "MAX_QUEUE_SIZE: " << MAX_QUEUE_SIZE << std::endl;
  ss << "NUM_WORKERS: " << NUM_WORKERS << std::endl;
  ss << "EXTRACTION_RESIZE_WIDTH: " << EXTRACTION_RESIZE_WIDTH << std::endl;
  ss << "EXTRACTION_RESIZE_HEIGHT: " << EXTRACTION_RESIZE_HEIGHT << std::endl;
  ss << "MAX_PD_ROI_SIZE: " << MAX_PD_ROI_SIZE << std::endl;
  ss << "MIN_PD_ROI_SIZE: " << MIN_PD_ROI_SIZE << std::endl;
  ss << "EAT_PD: " << EAT_PD << std::endl;
  ss << "ROI_PADDING: " << ROI_PADDING << std::endl;
  ss << "ROI_BORDER: " << ROI_BORDER << std::endl;
  ss << "OF_CONF_THRESHOLD: " << OF_CONF_THRESHOLD << std::endl;
  ss << "PD_FILTER_THRESHOLD: " << PD_FILTER_THRESHOLD << std::endl;
  ss << "PD_INTERVAL: " << PD_INTERVAL << std::endl;
  ss << "MERGE: " << MERGE << std::endl;
  ss << "NO_DOWNSAMPLING_FOR_LAST_FRAME: " << NO_DOWNSAMPLING_FOR_LAST_FRAME << std::endl;
  LOGD("%s", ss.str().c_str());
}

void ROIResizerConfig::print() const {
  std::stringstream ss;
  ss << "========== ROIResizerConfig ==========" << std::endl;
  ss << "DATASET: " << DATASET << std::endl;
  ss << "VOTING_WINDOW: " << VOTING_WINDOW << std::endl;
  ss << "SCALE_SHIFT: " << SCALE_SHIFT << std::endl;
  ss << "AREA_SHIFT: " << AREA_SHIFT << std::endl;
  ss << "STATIC_SCALE: " << STATIC_SCALE << std::endl;
  ss << "STATIC_TARGET_SCALE: " << STATIC_TARGET_SCALE << std::endl;
  ss << "MAX_OF_ROI_SIZE: " << MAX_OF_ROI_SIZE << std::endl;
  ss << "PROBE_STEP_SIZE: " << PROBE_STEP_SIZE << std::endl;
  ss << "NUM_PROBE_STEPS: " << NUM_PROBE_STEPS << std::endl;
  ss << "PROBE_CONF_THRESHOLD: " << PROBE_CONF_THRESHOLD << std::endl;
  ss << "PROBE_IOU_THRESHOLD: " << PROBE_IOU_THRESHOLD << std::endl;
  LOGD("%s", ss.str().c_str());
}

void InferenceEngineConfig::print() const {
  std::stringstream ss;
  ss << "========== InferenceEngineConfig ==========" << std::endl;
  ss << "DRAW_INFERENCE_RESULT: " << DRAW_INFERENCE_RESULT << std::endl;
  ss << "DATASET: " << DATASET << std::endl;
  ss << "MODEL: " << MODEL << std::endl;
  ss << "RUNTIME: " << RUNTIME << std::endl;
  ss << "USE_TINY: " << USE_TINY << std::endl;
  ss << "CONF_THRESHOLD: " << CONF_THRESHOLD << std::endl;
  ss << "IOU_THRESHOLD: " << IOU_THRESHOLD << std::endl;
  ss << "PROFILE_WARMUPS: " << PROFILE_WARMUPS << std::endl;
  ss << "PROFILE_RUNS: " << PROFILE_RUNS << std::endl;
  ss << "FULL_FRAME_SIZE: " << FULL_FRAME_SIZE << std::endl;
  ss << "INPUT_SIZES: ";
  for (auto& inputSize: INPUT_SIZES) {
    ss << inputSize << " ";
  }
  ss << std::endl;
  ss << "DEVICES: ";
  for (auto& device: DEVICES) {
    ss << md::str(device) << " ";
  }
  ss << std::endl;
  LOGD("%s", ss.str().c_str());
}

void PatchReconstructorConfig::print() const {
  std::stringstream ss;
  ss << "========== PatchReconstructorConfig ==========" << std::endl;
  ss << "FRAME_BOXES_IOU_THRESHOLD: " << FRAME_BOXES_IOU_THRESHOLD << std::endl;
  ss << "BOX_FILTER_OVERLAP_THRESHOLD: " << BOX_FILTER_OVERLAP_THRESHOLD << std::endl;
  ss << "ID_MAPPING_IOU_THRESHOLD: " << ID_MAPPING_IOU_THRESHOLD << std::endl;
  LOGD("%s", ss.str().c_str());
}

} // namespace md
