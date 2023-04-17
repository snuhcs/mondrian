#include "mondrian/DataType.hpp"
#include "mondrian/Log.hpp"

#include <sstream>

namespace md {

const idType UNASSIGNED_ID = -1;
const idType MERGED_ROI_ID = -2;

Device toDevice(std::string deviceStr) {
  if (deviceStr == "GPU") {
    return GPU;
  } else if (deviceStr == "DSP") {
    return DSP;
  } else if (deviceStr == "NO_DEVICE") {
    return NO_DEVICE;
  } else {
    LOGE("%s device is not supported", deviceStr.c_str());
    return NO_DEVICE;
  }
}

const char* toConstStr(Device device) {
  if (device == NO_DEVICE) {
    return "NO_DEVICE";
  } else if (device == GPU) {
    return "GPU";
  } else if (device == DSP) {
    return "DSP";
  } else {
    LOGE("%d device is not supported", device);
    return "WRONG DEVICE";
  }
}

std::string toString(const std::vector<InferenceInfo>& inferencePlan) {
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
