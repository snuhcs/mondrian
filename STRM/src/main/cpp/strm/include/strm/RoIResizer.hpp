#ifndef ROI_RESIZER_HPP_
#define ROI_RESIZER_HPP_

#include <string>
#include <map>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/tree/VIRAT.hpp"
#include "strm/tree/MTA.hpp"
#include "strm/tree/YouTube.hpp"

namespace rm {

class RoIResizer {
 public:
  RoIResizer(const RoIResizerConfig& config);

  float getTargetSize(const idType id, const RoI::Features& features);

  void updateTable(RoI* roi);

  int getNumProbeSteps() const {
    return mConfig.NUM_PROBE_STEPS;
  }

  int getProbeStepSize() const {
    return mConfig.PROBE_STEP_SIZE;
  }

  int isProbing() const {
    return mConfig.PROBE_STEP_SIZE != 0;
  }

 private:
  float getSmoothedTargetSize(const idType id, const RoI::Features& features);

  float getSizeWithFeature(const RoI::Features& features) const;

  bool isUsable(BoundingBox* targetBox, BoundingBox* baseBox) const;

  static float getOverlap(Rect& targetRect, Rect& baseRect);

  static const std::map<std::string, std::function<float(
      float, float, float, float, float, float,
      float, float, float, float, float, float)>> candidatePredictors;
  static const std::map<std::string, std::vector<float>> candidateResizeTargets;

  const RoIResizerConfig mConfig;
  const std::function<float(
      float, float, float, float, float, float,
      float, float, float, float, float, float)> mPredictor;
  const std::vector<float> mResizeTargets;

  // Save prev prediction to smooth the predicted size
  std::map<idType, float> prevTargetSizeTable;

  // Save probing start size to reset the reactive calibration
  std::map<idType, float> calibrationStartSizeTable;
  float calibration;
};

} // namespace rm

#endif // ROI_RESIZER_HPP_
