#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <array>
#include <map>
#include <sstream>
#include <queue>

#include "mondrian/Time.hpp"

namespace md {

using VID = int;
using FID = int;
using PID = int;
using RID = int;
using BID = int;
using OID = int;
using Indices = std::vector<std::pair<int, int>>;

extern const OID INVALID_OID;
extern const char DELIM;
extern const char SUBDELIM;
extern const int NUM_LABELS;
extern const char* COCO_LABELS[];

enum class ROIType {
  OF = 1,
  PD,
};

ROIType roiTypeOf(const std::string& roiTypeStr);

std::string str(const ROIType& roiType);

enum class Device {
  INVALID = 0,
  GPU,
  DSP,
};

const std::array<Device, 2> DEVICES = {Device::GPU, Device::DSP};

const Device LAST_FRAME_DEVICE = Device::GPU;

const Device PROBING_DEVICE = Device::GPU;

Device deviceOf(const std::string& deviceStr);

std::string str(const Device& device);

enum class ExecutionType {
  MONDRIAN = 1,
  EMULATED_BATCH,
  ROI_WISE_INFERENCE,
  FRAME_WISE_INFERENCE,
};

ExecutionType executionTypeOf(const std::string& executionTypeStr);

std::string str(const ExecutionType& executionType);

enum class BoxOrigin {
  INVALID = 0,       // null value for initialization
  FULL_FRAME,        // Box from full frame
  NEW_FULL_FRAME,    // New box from full frame
  PACKED_CANVAS,     // Box from packed canvas
  NEW_PACKED_CANVAS, // New Box from packed canvas
  INTERPOLATE,       // Box from interpolation
};

BoxOrigin boxOriginOf(const std::string& boxOriginStr);

std::string str(const BoxOrigin& boxOrigin);

enum class ROIPackerType {
  MIN_CONSECUTIVE_DROP = 1,
  OF_CONFIDENCE,
};

ROIPackerType roiPackerTypeOf(const std::string& roiPackerTypeStr);

std::string str(const ROIPackerType& roiPackerType);

template<typename T>
std::string safeGet(const std::map<Device, T>& m, Device device) {
  if (m.find(device) != m.end()) {
    return std::to_string(m.at(device));
  } else {
    return "-1";
  }
}

template<typename T>
std::string safeGetSize(const std::map<Device, std::vector<T>>& m,
                        Device device) {
  if (m.find(device) != m.end()) {
    return std::to_string(m.at(device).size());
  } else {
    return "-1";
  }
}

} // namespace md

#endif // DATA_TYPE_HPP_
