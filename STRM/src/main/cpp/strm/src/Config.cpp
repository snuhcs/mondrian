#include "strm/Config.hpp"

#include <fstream>

#include "strm/DataType.hpp"
#include "strm/Log.hpp"

namespace rm {

bool LOG_INTERNAL = true;

RoIExtractorConfig parseRoIExtractorConfig(const Json::Value& json) {
  RoIExtractorConfig config;
  if (!json["max_queue_size"].isNull()) {
    config.MAX_QUEUE_SIZE = json["max_queue_size"].asInt();
  }
  if (!json["extraction_resize_width"].isNull()) {
    config.EXTRACTION_RESIZE_WIDTH = json["extraction_resize_width"].asFloat();
  }
  if (!json["extraction_resize_height"].isNull()) {
    config.EXTRACTION_RESIZE_HEIGHT = json["extraction_resize_height"].asFloat();
  }
  if (!json["min_roi_area"].isNull()) {
    config.MIN_ROI_AREA = json["min_roi_area"].asFloat();
  }
  if (!json["roi_padding"].isNull()) {
    config.ROI_PADDING = json["roi_padding"].asFloat();
  }
  if (!json["optical_flow_roi_confidence_threshold"].isNull()) {
    config.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = json["optical_flow_roi_confidence_threshold"].asFloat();
  }
  if (!json["pd_filter_threshold"].isNull()) {
    config.PD_FILTER_THRESHOLD = json["pd_filter_threshold"].asFloat();
  }
  if (!json["merge"].isNull()) {
    config.MERGE = json["merge"].asBool();
  }
  if (!json["merge_threshold"].isNull()) {
    config.MERGE_THRESHOLD = json["merge_threshold"].asFloat();
  }
  return config;
}

RoIResizerConfig parseRoIResizerConfig(const Json::Value& json) {
  RoIResizerConfig config;
  assert(!json["train_data"].isNull());
  config.TRAIN_DATA = json["train_data"].asString();
  assert(config.TRAIN_DATA == "VIRAT" ||
         config.TRAIN_DATA == "MTA" ||
         config.TRAIN_DATA == "YouTube");
  if (!json["resize_smoothing_factor"].isNull()) {
    config.RESIZE_SMOOTHING_FACTOR = json["resize_smoothing_factor"].asFloat();
  }
  if (!json["static_resize_target"].isNull()) {
    config.STATIC_RESIZE_TARGET = json["static_resize_target"].asFloat();
  }
  if (!json["probe_step_size"].isNull()) {
    config.PROBE_STEP_SIZE = json["probe_step_size"].asFloat();
  }
  if (!json["num_probe_steps"].isNull()) {
    config.NUM_PROBE_STEPS = json["num_probe_steps"].asInt();
  }
  if (!json["probe_reset_threshold"].isNull()) {
    config.PROBE_RESET_THRESHOLD = json["probe_reset_threshold"].asFloat();
  }
  if (!json["overlap_threshold"].isNull()) {
    config.OVERLAP_THRESHOLD = json["overlap_threshold"].asFloat();
  }
  if (!json["absolute_confidence_threshold"].isNull()) {
    config.ABSOLUTE_CONFIDENCE_THRESHOLD = json["absolute_confidence_threshold"].asFloat();
  }
  if (!json["relative_confidence_threshold"].isNull()) {
    config.RELATIVE_CONFIDENCE_THRESHOLD = json["relative_confidence_threshold"].asFloat();
  }
  return config;
}

PatchMixerConfig parsePatchMixerConfig(const Json::Value& json) {
  PatchMixerConfig config;
  if (!json["n_way_mixing"].isNull()) {
    config.N_WAY_MIXING = json["n_way_mixing"].asBool();
  }
  if (!json["priority_mixing"].isNull()) {
    config.PRIORITY_MIXING = json["priority_mixing"].asBool();
  }
  if (!json["emulated_batch"].isNull()) {
    config.EMULATED_BATCH = json["emulated_batch"].asBool();
  }
  if (!json["batch_size"].isNull()) {
    config.BATCH_SIZE = json["batch_size"].asInt();
  }
  return config;
}

InferenceEngineConfig parseInferenceEngineConfig(const Json::Value& json) {
  InferenceEngineConfig config;
  if (!json["draw_inference_result"].isNull()) {
    config.DRAW_INFERENCE_RESULT = json["draw_inference_result"].asBool();
  }
  if (!json["model"].isNull()) {
    config.MODEL = json["model"].asString();
  }
  if (!json["runtime"].isNull()) {
    config.RUNTIME = json["runtime"].asString();
  }
  if (!json["use_tiny"].isNull()) {
    config.USE_TINY = json["use_tiny"].asBool();
  }
  if (!json["conf_threshold"].isNull()) {
    config.CONF_THRESHOLD = json["conf_threshold"].asFloat();
  }
  if (!json["iou_threshold"].isNull()) {
    config.IOU_THRESHOLD = json["iou_threshold"].asFloat();
  }
  if (!json["input_sizes"].isNull()) {
    const Json::Value inputSizes = json["input_sizes"];
    config.INPUT_SIZES.clear();
    for (const auto& size : inputSizes) {
      config.INPUT_SIZES.push_back(size.asInt());
    }
    std::sort(config.INPUT_SIZES.begin(), config.INPUT_SIZES.end());
  }
  if (!json["devices"].isNull()) {
    const Json::Value devices = json["devices"];
    config.DEVICES.clear();
    for (const auto& device : devices) {
      std::string deviceStr = device.asString();
      if (deviceStr == "GPU") {
        config.DEVICES.push_back(GPU);
      } else if (deviceStr == "DSP") {
        config.DEVICES.push_back(DSP);
      } else {
        LOGD("%s device is not supported", deviceStr.c_str());
      }
    }
    assert(std::find(config.DEVICES.begin(), config.DEVICES.end(), GPU) != config.DEVICES.end());
  }
  return config;
}

PatchReconstructorConfig parsePatchReconstructorConfig(const Json::Value& json) {
  PatchReconstructorConfig config;
  if (!json["frame_boxes_iou_threshold"].isNull()) {
    config.FRAME_BOXES_IOU_THRESHOLD = json["frame_boxes_iou_threshold"].asFloat();
  }
  if (!json["box_filter_overlap_threshold"].isNull()) {
    config.BOX_FILTER_OVERLAP_THRESHOLD = json["box_filter_overlap_threshold"].asFloat();
  }
  if (!json["id_mapping_iou_threshold"].isNull()) {
    config.ID_MAPPING_IOU_THRESHOLD = json["id_mapping_iou_threshold"].asFloat();
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
  LOGD("Config : %s", json.toStyledString().c_str());
  if (!json["log_internal"].isNull()) {
    LOG_INTERNAL = json["log_internal"].asBool();
  }
  if (!json["log_execution"].isNull()) {
    config.LOG_EXECUTION = json["log_execution"].asBool();
  }
  if (!json["log_roi"].isNull()) {
    config.LOG_ROI = json["log_roi"].asBool();
  }
  if (!json["allow_interpolation"].isNull()) {
    config.ALLOW_INTERPOLATION = json["allow_interpolation"].asBool();
  }
  if (!json["roi_wise_inference"].isNull()) {
    config.ROI_WISE_INFERENCE = json["roi_wise_inference"].asBool();
  }
  if (!json["full_frame_interval"].isNull()) {
    config.FULL_FRAME_INTERVAL = json["full_frame_interval"].asInt();
  }
  if (!json["buffer_size"].isNull()) {
    config.BUFFER_SIZE = json["buffer_size"].asInt();
  }
  if (!json["latency_slo_ms"].isNull()) {
    config.LATENCY_SLO_MS = json["latency_slo_ms"].asInt();
  }

  if (!json["roi_extractor"].isNull()) {
    config.roIExtractorConfig = parseRoIExtractorConfig(json["roi_extractor"]);
  }
  if (!json["roi_resizer"].isNull()) {
    config.roiResizerConfig = parseRoIResizerConfig(json["roi_resizer"]);
  }
  if (!json["patch_mixer"].isNull()) {
    config.patchMixerConfig = parsePatchMixerConfig(json["patch_mixer"]);
  }
  if (!json["inference_engine"].isNull()) {
    config.inferenceEngineConfig = parseInferenceEngineConfig(json["inference_engine"]);
  }
  if (!json["patch_reconstructor"].isNull()) {
    config.patchReconstructorConfig = parsePatchReconstructorConfig(
        json["patch_reconstructor"]);
  }
  return config;
}

} // namespace rm
