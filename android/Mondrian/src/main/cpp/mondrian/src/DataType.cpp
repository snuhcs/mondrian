#include "mondrian/DataType.hpp"

#include <sstream>

#include "mondrian/Frame.hpp"
#include "mondrian/ROIExtractor.hpp"
#include "mondrian/Log.hpp"

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
  } else if (executionTypeStr == "frame_wise_inference") {
    return FRAME_WISE_INFERENCE;
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
  } else if (executionType == FRAME_WISE_INFERENCE) {
    return "frame_wise_inference";
  } else {
    LOGE("Unknown execution type: %d", executionType);
    assert(false);
  }
}

ROIPrioritizerType roiPrioritizerTypeOf(const std::string& roiPrioritizerTypeStr) {
  if (roiPrioritizerTypeStr == "min_max_propagation") {
    return MIN_MAX_PROPAGATION;
  } else if (roiPrioritizerTypeStr == "of_confidence") {
    return OF_CONFIDENCE;
  } else {
    LOGE("Unknown ROI prioritizer type: %s", roiPrioritizerTypeStr.c_str());
    assert(false);
  }
}

std::string str(const ROIPrioritizerType& roiPrioritizerType) {
  if (roiPrioritizerType == MIN_MAX_PROPAGATION) {
    return "min_max_propagation";
  } else if (roiPrioritizerType == OF_CONFIDENCE) {
    return "of_confidence";
  } else {
    LOGE("Unknown ROI prioritizer type: %d", roiPrioritizerType);
    assert(false);
  }
}

std::string str(const std::vector<InferenceInfo>& inferencePlan) {
  std::stringstream ss;
  for (int i = int(inferencePlan.size()) - 1; i >= 0; i--) {
    const InferenceInfo& info = inferencePlan[i];
    // TODO: support other processors
    ss << "(" << (info.device == GPU ? "GPU" : "DSP") << ", "
       << info.size << ")";
    if (i != 0) {
      ss << ", ";
    }
  }
  return ss.str();
}

template<typename T>
BlockingQueue<T>::BlockingQueue(int maxElem) : maxElem_(maxElem) {
  assert(maxElem > 0);
}

template<typename T>
void BlockingQueue<T>::put(const T& v) {
  std::unique_lock<std::mutex> lock(mtx_);
  cv_.wait(lock, [this] { return queue_.size() < maxElem_; });
  queue_.push(v);
  lock.unlock();
  cv_.notify_one();
}

template<typename T>
T BlockingQueue<T>::take() {
  std::unique_lock<std::mutex> lock(mtx_);
  cv_.wait(lock, [this] { return !queue_.empty(); });
  T v = queue_.front();
  queue_.pop();
  lock.unlock();
  cv_.notify_one();
  return v;
}

template
class BlockingQueue<Frame*>;

template
class BlockingQueue<PackingResult>;

} // namespace md
