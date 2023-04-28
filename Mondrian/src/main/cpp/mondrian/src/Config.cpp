#include "mondrian/Config.hpp"

#include <fstream>

#include "mondrian/DataType.hpp"
#include "mondrian/Log.hpp"

namespace md {

bool LOG_INTERNAL = true;

ROIExtractorConfig parseROIExtractorConfig(const Json::Value& json) {
  ROIExtractorConfig config = {};
  assert(!json["max_queue_size"].isNull());
  config.MAX_QUEUE_SIZE = json["max_queue_size"].asInt();
  assert(!json["num_workers"].isNull());
  config.NUM_WORKERS = json["num_workers"].asInt();
  assert(!json["extraction_resize_width"].isNull());
  config.EXTRACTION_RESIZE_WIDTH = json["extraction_resize_width"].asFloat();
  assert(!json["extraction_resize_height"].isNull());
  config.EXTRACTION_RESIZE_HEIGHT = json["extraction_resize_height"].asFloat();
  assert(!json["max_pd_roi_size"].isNull());
  config.MAX_PD_ROI_SIZE = json["max_pd_roi_size"].asFloat();
  assert(!json["min_pd_roi_size"].isNull());
  config.MIN_PD_ROI_SIZE = json["min_pd_roi_size"].asFloat();
  assert(!json["eat_pd"].isNull());
  config.EAT_PD = json["eat_pd"].asBool();
  assert(!json["roi_padding"].isNull());
  config.ROI_PADDING = json["roi_padding"].asFloat();
  assert(!json["roi_border"].isNull());
  config.ROI_BORDER = json["roi_border"].asInt();
  assert(!json["optical_flow_roi_confidence_threshold"].isNull());
  config.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = json["optical_flow_roi_confidence_threshold"].asFloat();
  assert(!json["pd_filter_threshold"].isNull());
  config.PD_FILTER_THRESHOLD = json["pd_filter_threshold"].asFloat();
  assert(!json["pd_interval"].isNull());
  config.PD_INTERVAL = json["pd_interval"].asInt();
  assert(!json["merge"].isNull());
  config.MERGE = json["merge"].asBool();
  assert(!json["no_downsampling_for_last_frame"].isNull());
  config.NO_DOWNSAMPLING_FOR_LAST_FRAME = json["no_downsampling_for_last_frame"].asBool();
  return config;
}

ROIResizerConfig parseROIResizerConfig(const Json::Value& json) {
  ROIResizerConfig config = {};
  assert(!json["dataset"].isNull());;
  config.DATASET = json["dataset"].asString();
  assert(config.DATASET == "virat" || config.DATASET == "mta");
  assert(!json["voting_window"].isNull());
  config.VOTING_WINDOW = json["voting_window"].asInt();
  assert(!json["scale_shift"].isNull());
  config.SCALE_SHIFT = json["scale_shift"].asFloat();
  assert(!json["area_shift"].isNull());
  config.AREA_SHIFT = json["area_shift"].asFloat();
  assert(!json["static_scale"].isNull());
  config.STATIC_SCALE = json["static_scale"].asBool();
  assert(!json["static_target_scale"].isNull());
  config.STATIC_TARGET_SCALE = json["static_target_scale"].asFloat();
  assert(!json["max_of_roi_size"].isNull());
  config.MAX_OF_ROI_SIZE = json["max_of_roi_size"].asFloat();
  assert(!json["probe_step_size"].isNull());
  config.PROBE_STEP_SIZE = json["probe_step_size"].asFloat();
  assert(config.PROBE_STEP_SIZE > 0);
  assert(!json["num_probe_steps"].isNull());
  config.NUM_PROBE_STEPS = json["num_probe_steps"].asInt();
  assert(!json["probe_conf_threshold"].isNull());
  config.PROBE_CONF_THRESHOLD = json["probe_conf_threshold"].asFloat();
  assert(!json["probe_iou_threshold"].isNull());
  config.PROBE_IOU_THRESHOLD = json["probe_iou_threshold"].asFloat();
  return config;
}

InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json) {
  InferenceEngineConfig config = {};
  assert(!json["draw_inference_result"].isNull());
  config.DRAW_INFERENCE_RESULT = json["draw_inference_result"].asBool();
  assert(!json["dataset"].isNull());;
  config.DATASET = json["dataset"].asString();
  assert(config.DATASET == "virat" || config.DATASET == "mta");
  assert(!json["model"].isNull());
  config.MODEL = json["model"].asString();
  assert(!json["runtime"].isNull());
  config.RUNTIME = json["runtime"].asString();
  assert(!json["use_tiny"].isNull());
  config.USE_TINY = json["use_tiny"].asBool();
  assert(!json["conf_threshold"].isNull());
  config.CONF_THRESHOLD = json["conf_threshold"].asFloat();
  assert(!json["iou_threshold"].isNull());
  config.IOU_THRESHOLD = json["iou_threshold"].asFloat();
  assert(!json["profile_warmups"].isNull());
  config.PROFILE_WARMUPS = json["profile_warmups"].asInt();
  assert(!json["profile_runs"].isNull());
  config.PROFILE_RUNS = json["profile_runs"].asInt();
  assert(!json["full_frame_size"].isNull());
  config.FULL_FRAME_SIZE = json["full_frame_size"].asInt();
  assert(!json["input_sizes"].isNull());
  for (const auto& inputSizeJson: json["input_sizes"]) {
    config.INPUT_SIZES.push_back(inputSizeJson.asInt());
  }
  std::sort(config.INPUT_SIZES.begin(), config.INPUT_SIZES.end());
  assert(!json["devices"].isNull());
  for (const auto& deviceJson: json["devices"]) {
    config.DEVICES.push_back(toDevice(deviceJson.asString()));
  }
  return config;
}

PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json) {
  PatchReconstructorConfig config = {};
  assert(!json["frame_boxes_iou_threshold"].isNull());
  config.FRAME_BOXES_IOU_THRESHOLD = json["frame_boxes_iou_threshold"].asFloat();
  assert(!json["box_filter_overlap_threshold"].isNull());
  config.BOX_FILTER_OVERLAP_THRESHOLD = json["box_filter_overlap_threshold"].asFloat();
  assert(!json["id_mapping_iou_threshold"].isNull());
  config.ID_MAPPING_IOU_THRESHOLD = json["id_mapping_iou_threshold"].asFloat();
  return config;
}

MondrianConfig parseMondrianConfig(const std::string& jsonPath) {
  MondrianConfig config;
  std::ifstream jsonFile(jsonPath, std::ifstream::binary);
  assert(jsonFile.is_open());
  Json::Value json;
  jsonFile >> json;
  assert(json.isObject());
  LOGD("Config : %s", json.toStyledString().c_str());

  assert(!json["log_internal"].isNull());
  LOG_INTERNAL = json["log_internal"].asBool();
  assert(!json["log_execution"].isNull());
  config.LOG_EXECUTION = json["log_execution"].asBool();
  assert(!json["log_roi"].isNull());
  config.LOG_ROI = json["log_roi"].asBool();
  assert(!json["allow_interpolation"].isNull());
  config.ALLOW_INTERPOLATION = json["allow_interpolation"].asBool();
  assert(!json["interpolation_threshold"].isNull());
  config.INTERPOLATION_THRESHOLD = json["interpolation_threshold"].asFloat();
  assert(!json["full_frame_interval"].isNull());
  config.FULL_FRAME_INTERVAL = json["full_frame_interval"].asInt();
  assert(!json["full_frame_size"].isNull());
  config.FULL_FRAME_SIZE = json["full_frame_size"].asInt();
  assert(!json["full_device"].isNull());
  config.FULL_DEVICE = toDevice(json["full_device"].asString());
  assert(!json["buffer_size"].isNull());
  config.BUFFER_SIZE = json["buffer_size"].asInt();
  assert(!json["latency_slo_ms"].isNull());
  config.LATENCY_SLO_MS = json["latency_slo_ms"].asInt();
  assert(!json["use_emulated_batch"].isNull());
  config.USE_EMULATED_BATCH = json["use_emulated_batch"].asBool();
  assert(!json["use_roi_wise_inference"].isNull());
  config.USE_ROI_WISE_INFERENCE = json["use_roi_wise_inference"].asBool();
  assert(!json["roi_size"].isNull());
  config.ROI_SIZE = json["roi_size"].asInt();

  assert(!json["roi_extractor"].isNull());
  config.roiExtractorConfig = parseROIExtractorConfig(json["roi_extractor"]);
  assert(!json["roi_resizer"].isNull());
  config.roiResizerConfig = parseROIResizerConfig(json["roi_resizer"]);
  if (config.USE_EMULATED_BATCH || config.USE_ROI_WISE_INFERENCE) {
    assert(config.roiResizerConfig.NUM_PROBE_STEPS == 0);
  }
  assert(!json["inference_engine"].isNull());
  config.inferenceEngineConfig = parseInferenceEngineConfig(json["inference_engine"]);
  assert(config.inferenceEngineConfig.FULL_FRAME_SIZE == config.FULL_FRAME_SIZE);
  assert(config.inferenceEngineConfig.DATASET == config.roiResizerConfig.DATASET);
  auto& devices = config.inferenceEngineConfig.DEVICES;
  assert(std::find(devices.begin(), devices.end(), config.FULL_DEVICE) != devices.end());
  auto& input_sizes = config.inferenceEngineConfig.INPUT_SIZES;
  if (config.USE_EMULATED_BATCH) {
    assert(std::all_of(input_sizes.begin(), input_sizes.end(), [&config](int input_size) {
      return input_size % config.ROI_SIZE == 0;
    }));
  }
  assert(!json["patch_reconstructor"].isNull());
  config.patchReconstructorConfig = parsePatchReconstructorConfig(json["patch_reconstructor"]);
  return config;
}

} // namespace md
