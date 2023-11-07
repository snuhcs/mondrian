#include "mondrian/Config.hpp"

#include <fstream>

#include "mondrian/Log.hpp"

namespace md {

static int parseInt(const Json::Value& json, const std::string& key) {
  if (json[key].isNull()) {
    LOGE("[Config] parseInt fail // key=%s json=%s", key.c_str(), json.toStyledString().c_str());
    assert(false);
  }
  return json[key].asInt();
}

static std::vector<int> parseInts(const Json::Value& json, const std::string& key, const int size) {
  if (json[key].isNull() || json[key].size() != size) {
    LOGE("[Config] parseInts fail // key=%s json=%s", key.c_str(), json.toStyledString().c_str());
    assert(false);
  }
  std::vector<int> values;
  for (const auto& value : json[key]) {
    values.push_back(value.asInt());
  }
  return values;
}

static float parseFloat(const Json::Value& json, const std::string& key) {
  if (json[key].isNull()) {
    LOGE("[Config] parseFloat fail // key=%s json=%s", key.c_str(), json.toStyledString().c_str());
    assert(false);
  }
  return json[key].asFloat();
}

static bool parseBool(const Json::Value& json, const std::string& key) {
  if (json[key].isNull()) {
    LOGE("[Config] parseBool fail // key=%s json=%s", key.c_str(), json.toStyledString().c_str());
    assert(false);
  }
  return json[key].asBool();
}

static std::string parseString(const Json::Value& json, const std::string& key) {
  if (json[key].isNull()) {
    LOGE("[Config] parseString fail // key=%s json=%s", key.c_str(), json.toStyledString().c_str());
    assert(false);
  }
  return json[key].asString();
}

ROIExtractorConfig parseROIExtractorConfig(const Json::Value& json) {
  ROIExtractorConfig config = {};
  auto extraction_size = parseInts(json, "extraction_size", 2);
  config.EXTRACTION_SIZE = cv::Size(extraction_size[0], extraction_size[1]);
  config.PD_INTERVAL = parseInt(json, "pd_interval");
  config.MIN_PD_ROI_SIZE = parseFloat(json, "min_pd_roi_size");
  config.MAX_PD_ROI_SIZE = parseFloat(json, "max_pd_roi_size");
  config.OF_TRACK_CENTER = parseBool(json, "of_track_center");
  config.OF_CONF_THRES = parseFloat(json, "of_conf_thres");
  config.OF_ROI_PADDING = parseFloat(json, "of_roi_padding");
  config.PD_EAT_OVERLAP_THRES = parseFloat(json, "pd_eat_overlap_thres");
  config.PD_FILTER_OVERLAP_THRES = parseFloat(json, "pd_filter_overlap_thres");
  config.MERGE = parseBool(json, "merge");
  return config;
}

ROIResizerConfig parseROIResizerConfig(const Json::Value& json) {
  ROIResizerConfig config = {};
  config.STATIC_AREA = parseBool(json, "static_area");
  config.STATIC_TARGET_AREA = parseFloat(json, "static_target_area");
  config.DATASET = parseString(json, "dataset");
  config.VOTING_WINDOW_SIZE = parseInt(json, "voting_window_size");
  config.AREA_SHIFT = parseFloat(json, "area_shift");
  config.SCALE_SHIFT = parseFloat(json, "scale_shift");
  config.NUM_PROBE_STEPS = parseInt(json, "num_probe_steps");
  config.ONLY_SMALLER_PROBING = parseBool(json, "only_smaller_probing");
  config.PROBE_STEP_SIZE = parseFloat(json, "probe_step_size");
  config.PROBE_CONF_THRES = parseFloat(json, "probe_conf_thres");
  config.PROBE_IOU_THRES = parseFloat(json, "probe_iou_thres");
  return config;
}

ROIPackerConfig parseROIPackerConfig(const Json::Value& json) {
  ROIPackerConfig config = {};
  config.NO_DOWNSAMPLING_FOR_LAST_FRAME = parseBool(json, "no_downsampling_for_last_frame");
  config.TYPE = roiPackerTypeOf(parseString(json, "type"));
  return config;
}

static std::map<Device, WorkerConfig> parseWorkerConfigs(const Json::Value& json) {
  std::map<Device, WorkerConfig> workerConfigs;
  for (auto it = json.begin(); it != json.end(); it++) {
    Device device = deviceOf(it.key().asString());
    const Json::Value& workerConfigJson = *it;
    WorkerConfig workerConfig = {};
    workerConfig.MODEL = parseString(workerConfigJson, "model");
    workerConfig.DATASET = parseString(workerConfigJson, "dataset");
    for (const auto& value : workerConfigJson["input_sizes"]) {
      workerConfig.INPUT_SIZES.push_back(value.asInt());
    }
    workerConfigs[device] = workerConfig;
  }
  return workerConfigs;
}

InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json) {
  InferenceEngineConfig config = {};
  config.FULL_DEVICE = deviceOf(parseString(json, "full_device"));
  config.FULL_MODEL = parseString(json, "full_model");
  config.FULL_DATASET = parseString(json, "full_dataset");
  config.FULL_FRAME_SIZE = parseInt(json, "full_frame_size");
  config.WORKER_CONFIGS = parseWorkerConfigs(json["worker_configs"]);
  config.DRAW_INFERENCE_RESULT = parseBool(json, "draw_inference_result");
  config.CONF_THRES = parseFloat(json, "conf_thres");
  config.IOU_THRES = parseFloat(json, "iou_thres");
  config.PROFILE_WARMUPS = parseInt(json, "profile_warmups");
  config.PROFILE_RUNS = parseInt(json, "profile_runs");
  return config;
}

PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json) {
  PatchReconstructorConfig config = {};
  config.FRAME_BOXES_IOU_THRES = parseFloat(json, "frame_boxes_iou_thres");
  config.BOX_FILTER_OVERLAP_THRES = parseFloat(json, "box_filter_overlap_thres");
  config.ID_MAPPING_IOU_THRES = parseFloat(json, "id_mapping_iou_thres");
  return config;
}

MondrianConfig parseMondrianConfig(const std::string& jsonPath) {
  MondrianConfig config;
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  assert(jsonFile.is_open());
  Json::Value json;
  jsonFile >> json;
  assert(json.isObject());

  config.EXECUTION_TYPE = executionTypeOf(parseString(json, "execution_type"));
  config.LOG_BOXES = parseBool(json, "log_boxes");
  config.LOG_ROI = parseBool(json, "log_roi");
  config.LOG_FRAME = parseBool(json, "log_frame");
  config.INTERPOLATION_THRES = parseInt(json, "interpolation_thres");
  config.FULL_FRAME_INTERVAL = parseInt(json, "full_frame_interval");
  config.LATENCY_SLO_MS = parseInt(json, "latency_slo_ms");
  config.ROI_SIZE = parseInt(json, "roi_size");

  config.roiExtractorConfig = parseROIExtractorConfig(json["roi_extractor"]);
  config.roiResizerConfig = parseROIResizerConfig(json["roi_resizer"]);
  config.roiPackerConfig = parseROIPackerConfig(json["roi_packer"]);
  config.inferenceEngineConfig = parseInferenceEngineConfig(json["inference_engine"]);
  config.patchReconstructorConfig = parsePatchReconstructorConfig(json["patch_reconstructor"]);

  return config;
}

void MondrianConfig::test() const {
  std::set<std::string> datasets = {"pretrained", "virat", "mta"};

  // Common
  if (EXECUTION_TYPE == ExecutionType::FRAME_WISE_INFERENCE) {
    assert(FULL_FRAME_INTERVAL == 0);
  } else {
    assert(FULL_FRAME_INTERVAL > 0);
  }

  // ROIResizer
  if (roiResizerConfig.STATIC_AREA) {
    assert(roiResizerConfig.AREA_SHIFT == 0.0f);
    assert(roiResizerConfig.SCALE_SHIFT == 0.0f);
    assert(roiResizerConfig.STATIC_TARGET_AREA > 0);
  } else {
    assert(roiResizerConfig.NUM_PROBE_STEPS == 0 || roiResizerConfig.PROBE_STEP_SIZE > 0);
  }
  if (EXECUTION_TYPE == ExecutionType::ROI_WISE_INFERENCE
      || EXECUTION_TYPE == ExecutionType::EMULATED_BATCH) {
    assert(roiResizerConfig.NUM_PROBE_STEPS == 0);
  }

  // InferenceEngine
  if (EXECUTION_TYPE != ExecutionType::FRAME_WISE_INFERENCE) {
    assert(std::any_of(
        inferenceEngineConfig.WORKER_CONFIGS.begin(), inferenceEngineConfig.WORKER_CONFIGS.end(),
        [](const auto& pair) { return !pair.second.INPUT_SIZES.empty(); }));
  }
  if (EXECUTION_TYPE == ExecutionType::ROI_WISE_INFERENCE) {
    for (const auto& [device, workerConfig] : inferenceEngineConfig.WORKER_CONFIGS) {
      assert(workerConfig.INPUT_SIZES.size() <= 1);
      if (!workerConfig.INPUT_SIZES.empty()) {
        assert(ROI_SIZE == *workerConfig.INPUT_SIZES.begin());
      }
    }
  }
  if (EXECUTION_TYPE == ExecutionType::EMULATED_BATCH) {
    for (const auto& [device, workerConfig] : inferenceEngineConfig.WORKER_CONFIGS) {
      assert(std::all_of(
          workerConfig.INPUT_SIZES.begin(), workerConfig.INPUT_SIZES.end(),
          [this](int input_size) { return input_size % ROI_SIZE == 0; }));
    }
  }
}

void MondrianConfig::print() const {
  std::stringstream ss;
  ss << "========== MondrianConfig ==========" << std::endl;
  ss << "EXECUTION_TYPE: " << md::str(EXECUTION_TYPE) << std::endl;
  ss << "LOG_BOXES: " << LOG_BOXES << std::endl;
  ss << "LOG_ROI: " << LOG_ROI << std::endl;
  ss << "LOG_FRAME: " << LOG_FRAME << std::endl;
  ss << "INTERPOLATION_THRES: " << INTERPOLATION_THRES << std::endl;
  ss << "FULL_FRAME_INTERVAL: " << FULL_FRAME_INTERVAL << std::endl;
  ss << "LATENCY_SLO_MS: " << LATENCY_SLO_MS << std::endl;
  ss << "ROI_SIZE: " << ROI_SIZE << std::endl;
  LOGD("%s", ss.str().c_str());
  roiExtractorConfig.print();
  roiResizerConfig.print();
  roiPackerConfig.print();
  inferenceEngineConfig.print();
  patchReconstructorConfig.print();
  LOGD("====================================");
}

void ROIExtractorConfig::print() const {
  std::stringstream ss;
  ss << "========== ROIExtractorConfig ==========" << std::endl;
  ss << "EXTRACTION_SIZE: ["
     << EXTRACTION_SIZE.width << ", "
     << EXTRACTION_SIZE.height << "]" << std::endl;
  ss << "PD_INTERVAL: " << PD_INTERVAL << std::endl;
  ss << "MIN_PD_ROI_SIZE: " << MIN_PD_ROI_SIZE << std::endl;
  ss << "MAX_PD_ROI_SIZE: " << MAX_PD_ROI_SIZE << std::endl;
  ss << "OF_TRACK_CENTER: " << OF_TRACK_CENTER << std::endl;
  ss << "OF_CONF_THRES: " << OF_CONF_THRES << std::endl;
  ss << "OF_ROI_PADDING: " << OF_ROI_PADDING << std::endl;
  ss << "PD_EAT_OVERLAP_THRES: " << PD_EAT_OVERLAP_THRES << std::endl;
  ss << "PD_FILTER_OVERLAP_THRES: " << PD_FILTER_OVERLAP_THRES << std::endl;
  ss << "MERGE: " << MERGE << std::endl;
  LOGD("%s", ss.str().c_str());
}

void ROIResizerConfig::print() const {
  std::stringstream ss;
  ss << "========== ROIResizerConfig ==========" << std::endl;
  ss << "STATIC_AREA: " << STATIC_AREA << std::endl;
  ss << "STATIC_TARGET_AREA: " << STATIC_TARGET_AREA << std::endl;
  ss << "DATASET: " << DATASET << std::endl;
  ss << "VOTING_WINDOW_SIZE: " << VOTING_WINDOW_SIZE << std::endl;
  ss << "AREA_SHIFT: " << AREA_SHIFT << std::endl;
  ss << "SCALE_SHIFT: " << SCALE_SHIFT << std::endl;
  ss << "NUM_PROBE_STEPS: " << NUM_PROBE_STEPS << std::endl;
  ss << "PROBE_STEP_SIZE: " << PROBE_STEP_SIZE << std::endl;
  ss << "PROBE_CONF_THRES: " << PROBE_CONF_THRES << std::endl;
  ss << "PROBE_IOU_THRES: " << PROBE_IOU_THRES << std::endl;
  LOGD("%s", ss.str().c_str());
}

void ROIPackerConfig::print() const {
  std::stringstream ss;
  ss << "========== ROIPackerConfig ==========" << std::endl;
  ss << "NO_DOWNSAMPLING_FOR_LAST_FRAME: " << NO_DOWNSAMPLING_FOR_LAST_FRAME << std::endl;
  ss << "TYPE: " << md::str(TYPE) << std::endl;
  LOGD("%s", ss.str().c_str());
}

std::string WorkerConfig::str() const {
  std::stringstream ss;
  ss << MODEL << " ";
  ss << DATASET << " ";
  for (int input_size : INPUT_SIZES) {
    ss << input_size << " ";
  }
  return ss.str();
}

void InferenceEngineConfig::print() const {
  std::stringstream ss;
  ss << "========== InferenceEngineConfig ==========" << std::endl;
  ss << "FULL_DEVICE: " << str(FULL_DEVICE) << std::endl;
  ss << "FULL_MODEL: " << FULL_MODEL << std::endl;
  ss << "FULL_DATASET: " << FULL_DATASET << std::endl;
  ss << "FULL_FRAME_SIZE: " << FULL_FRAME_SIZE << std::endl;
  for (const auto& [device, workerConfig]: WORKER_CONFIGS) {
    ss << "WORKER_CONFIGS [" << str(device) << "]: " << workerConfig.str() << std::endl;
  }
  ss << "DRAW_INFERENCE_RESULT: " << DRAW_INFERENCE_RESULT << std::endl;
  ss << "CONF_THRES: " << CONF_THRES << std::endl;
  ss << "IOU_THRES: " << IOU_THRES << std::endl;
  ss << "PROFILE_WARMUPS: " << PROFILE_WARMUPS << std::endl;
  ss << "PROFILE_RUNS: " << PROFILE_RUNS << std::endl;
  LOGD("%s", ss.str().c_str());
}

void PatchReconstructorConfig::print() const {
  std::stringstream ss;
  ss << "========== PatchReconstructorConfig ==========" << std::endl;
  ss << "FRAME_BOXES_IOU_THRES: " << FRAME_BOXES_IOU_THRES << std::endl;
  ss << "BOX_FILTER_OVERLAP_THRES: " << BOX_FILTER_OVERLAP_THRES << std::endl;
  ss << "ID_MAPPING_IOU_THRES: " << ID_MAPPING_IOU_THRES << std::endl;
  LOGD("%s", ss.str().c_str());
}

} // namespace md
