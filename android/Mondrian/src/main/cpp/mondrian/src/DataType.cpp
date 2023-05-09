#include "mondrian/DataType.hpp"
#include "mondrian/Log.hpp"

#include <sstream>

namespace md {

const ID INVALID_ID = -1;

Device deviceOf(const std::string& deviceStr) {
  if (deviceStr == "GPU") {
    return GPU;
  } else if (deviceStr == "DSP") {
    return DSP;
  } else if (deviceStr == "NO_DEVICE") {
    return NO_DEVICE;
  } else {
    LOGE("Unknown device: %s", deviceStr.c_str());
    assert(false);
  }
}

std::string str(const Device& device) {
  if (device == GPU) {
    return "GPU";
  } else if (device == DSP) {
    return "DSP";
  } else if (device == NO_DEVICE) {
    return "NO_DEVICE";
  } else {
    LOGE("Unknown device: %d", device);
    assert(false);
  }
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

std::string str(const std::vector<InferenceInfo>& inferencePlan) {
  std::stringstream ss;
  for (int i = int(inferencePlan.size()) - 1; i >= 0; i--) {
    const InferenceInfo& info = inferencePlan[i];
    // TODO: support other processors
    ss << "(" << (info.device == GPU ? "GPU" : "DSP") << ", "
       << info.size << ", "
       << info.accumulatedLatency << ")";
    if (i != 0) {
      ss << ", ";
    }
  }
  return ss.str();
}

} // namespace md
