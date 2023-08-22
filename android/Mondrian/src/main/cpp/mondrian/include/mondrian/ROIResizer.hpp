#ifndef ROI_RESIZER_HPP_
#define ROI_RESIZER_HPP_

#include <string>
#include <map>

#include "mondrian/Config.hpp"
#include "mondrian/Features.hpp"
#include "mondrian/ROI.hpp"
#include "mondrian/tree/VIRAT.hpp"
#include "mondrian/tree/MTA.hpp"

namespace md {

class ROI;

using Predictor = std::function<int(
    float, float, float, float, float, float, float)>;

class ROIResizer {
 public:
  static const int STATIC_LEVEL;
  static const int INVALID_LEVEL;

  ROIResizer(const ROIResizerConfig& config);

  std::pair<float, int> getTargetScale(const OID oid,
                                       const Features& features,
                                       const float area);

  void updateTable(ROI* roi);

  std::vector<float> getProbingCandidates(float scale, int level, float area) const;

 private:
  class CircularBuffer {
   public:
    CircularBuffer() {}; // Default ctor for std::map

    CircularBuffer(int numLevels, int capacity);

    void push(int data);

    int maxVote();

   private:
    int numLevels_;
    size_t capacity_;
    size_t oldest_index_;
    size_t size_;
    std::vector<int> data_;
  };

  float calcTargetScale(const int scaleLevel, const float originalArea) const;

  bool isCalibrated(const OID oid) const;

  int getMaxVotedLevel(const OID oid, const Features& features);

  int predictLevelWithFeatures(const Features& features) const;

  bool isUsable(BoundingBox* targetBox, BoundingBox* baseBox) const;

  static const std::map<std::string, Predictor> CANDIDATE_PREDICTORS;
  static const std::map<std::string, std::vector<float>> AREA_LEVELS;

  const ROIResizerConfig config_;
  std::map<Device, Predictor> predictorTable_;
  std::map<Device, std::vector<float>> targetAreasTable_;

  // Save prev prediction to smooth the predicted size
  std::map<Device, std::map<OID, CircularBuffer>> prevPredictionBufferTable_;

  // Save <targetScale_ of ROI, calibration for that targetScale_>
  std::map<Device, std::map<OID, std::pair<int, float>>> calibrationTableTable_;
};

} // namespace md

#endif // ROI_RESIZER_HPP_
