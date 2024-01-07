#include "mondrian/DataType.hpp"

#include <sstream>

#include "mondrian/Frame.hpp"
#include "mondrian/ROIExtractor.hpp"
#include "mondrian/Log.hpp"

namespace md {

const OID INVALID_OID = -1;
const char DELIM = '\t';
const char SUBDELIM = ',';
const int NUM_LABELS = 80;
const char* COCO_LABELS[] =
    {"person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat",
     "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog",
     "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella",
     "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite",
     "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle",
     "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
     "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "potted plant",
     "bed", "dining table", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard",
     "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
     "scissors", "teddy bear", "hair drier", "toothbrush"};

ROIType roiTypeOf(const std::string& roiTypeStr) {
  if (roiTypeStr == "OF") {
    return ROIType::OF;
  } else if (roiTypeStr == "PD") {
    return ROIType::PD;
  } else {
    LOGE("Unknown ROI type: %s", roiTypeStr.c_str());
    assert(false);
  }
}

std::string str(const ROIType& roiType) {
  switch (roiType) {
    case ROIType::OF: return "OF";
    case ROIType::PD: return "PD";
  }
  LOGE("Unknown ROI type: %d", roiType);
  assert(false);
}

Device deviceOf(const std::string& deviceStr) {
  if (deviceStr == "INVALID") {
    return Device::INVALID;
  } else if (deviceStr == "GPU") {
    return Device::GPU;
  } else if (deviceStr == "DSP") {
    return Device::DSP;
  } else {
    LOGE("Unknown device: %s", deviceStr.c_str());
    assert(false);
  }
}

std::string str(const Device& device) {
  switch (device) {
    case Device::INVALID: return "INVALID";
    case Device::GPU: return "GPU";
    case Device::DSP: return "DSP";
  }
  LOGE("Unknown device: %d", device);
  assert(false);
}

ExecutionType executionTypeOf(const std::string& executionTypeStr) {
  if (executionTypeStr == "mondrian") {
    return ExecutionType::MONDRIAN;
  } else if (executionTypeStr == "emulated_batch") {
    return ExecutionType::EMULATED_BATCH;
  } else if (executionTypeStr == "roi_wise_inference") {
    return ExecutionType::ROI_WISE_INFERENCE;
  } else if (executionTypeStr == "frame_wise_inference") {
    return ExecutionType::FRAME_WISE_INFERENCE;
  } else {
    LOGE("Unknown execution type: %s", executionTypeStr.c_str());
    assert(false);
  }
}

std::string str(const ExecutionType& executionType) {
  switch (executionType) {
    case ExecutionType::MONDRIAN: return "mondrian";
    case ExecutionType::EMULATED_BATCH: return "emulated_batch";
    case ExecutionType::ROI_WISE_INFERENCE: return "roi_wise_inference";
    case ExecutionType::FRAME_WISE_INFERENCE: return "frame_wise_inference";
  }
  LOGE("Unknown execution type: %d", executionType);
  assert(false);
}

ROIPackerType roiPackerTypeOf(const std::string& roiPackerTypeStr) {
  if (roiPackerTypeStr == "min_consecutive_drop") {
    return ROIPackerType::MIN_CONSECUTIVE_DROP;
  } else if (roiPackerTypeStr == "of_confidence") {
    return ROIPackerType::OF_CONFIDENCE;
  } else {
    LOGE("Unknown ROI packer type: %s", roiPackerTypeStr.c_str());
    assert(false);
  }
}

std::string str(const ROIPackerType& roiPackerType) {
  switch (roiPackerType) {
    case ROIPackerType::MIN_CONSECUTIVE_DROP: return "min_consecutive_drop";
    case ROIPackerType::OF_CONFIDENCE: return "of_confidence";
  }
  LOGE("Unknown ROIPackerType: %d", roiPackerType);
  assert(false);
}

BoxOrigin boxOriginOf(const std::string& boxOriginStr) {
  if (boxOriginStr == "INVALID") {
    return BoxOrigin::INVALID;
  } else if (boxOriginStr == "FULL_FRAME") {
    return BoxOrigin::FULL_FRAME;
  } else if (boxOriginStr == "NEW_FULL_FRAME") {
    return BoxOrigin::NEW_FULL_FRAME;
  } else if (boxOriginStr == "PACKED_CANVAS") {
    return BoxOrigin::PACKED_CANVAS;
  } else if (boxOriginStr == "NEW_PACKED_CANVAS") {
    return BoxOrigin::NEW_PACKED_CANVAS;
  } else if (boxOriginStr == "INTERPOLATE") {
    return BoxOrigin::INTERPOLATE;
  } else {
    LOGE("Unknown boxOrigin: %s", boxOriginStr.c_str());
    assert(false);
  }
}

std::string str(const BoxOrigin& boxOrigin) {
  switch (boxOrigin) {
    case BoxOrigin::INVALID: return "INVALID";
    case BoxOrigin::FULL_FRAME: return "FULL_FRAME";
    case BoxOrigin::NEW_FULL_FRAME: return "NEW_FULL_FRAME";
    case BoxOrigin::PACKED_CANVAS: return "PACKED_CANVAS";
    case BoxOrigin::NEW_PACKED_CANVAS: return "NEW_PACKED_CANVAS";
    case BoxOrigin::INTERPOLATE: return "INTERPOLATE";
  }
  LOGE("Unknown boxOrigin: %d", boxOrigin);
  assert(false);
}

} // namespace md
