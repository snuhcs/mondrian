#include "strm/DataType.hpp"

#include <sstream>

namespace rm {

const idType UNASSIGNED_ID = -1;
const idType MERGED_ROI_ID = -2;

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

} // namespace rm
