#ifndef ROI_RESIZER_HPP_
#define ROI_RESIZER_HPP_

#include <string>
#include <map>

#include "mondrian/Config.hpp"
#include "mondrian/ROI.hpp"
#include "mondrian/tree/VIRAT.hpp"
#include "mondrian/tree/MTA.hpp"

namespace md {

class ROI;

using Predictor = std::function<int(
    float, float, float, float, float, float, float, float)>;

class ROIResizer {
 public:
  static const int STATIC_LEVEL;
  static const int INVALID_LEVEL;

  ROIResizer(const ROIResizerConfig& config);

  std::pair<float, int> getTargetScale(const idType id, const Features& features,
                                       const float maxEdgeLength);

  void updateTable(ROI* cROI);

  int getNumProbeSteps() const {
    return mConfig.NUM_PROBE_STEPS;
  }

  std::vector<float>
  getProbingCandidates(float scale, int level, int numProbeSteps, float originalArea) const;

 private:
  class CircularBuffer {
   public:
    CircularBuffer() {}; // Default ctor for std::map

    CircularBuffer(int numLevels, int capacity);

    void push(int data);

    int maxVote();

   private:
    int numLevels;
    size_t capacity_, oldest_index, size_;
    std::vector<int> data_;
  };

  std::pair<float, int> getTargetScale(const idType id, const Features& features);

  float getTargetScale(const int scaleLevel, const float originalArea) const;

  bool isCalibrated(const idType id, const int scaleLevel) const;

  int getMaxVotedLevel(const idType id, const Features& features);

  int predictLevelWithFeatures(const Features& features) const;

  bool isUsable(BoundingBox* targetBox, BoundingBox* baseBox) const;

  float calculateTargetScale(float targetArea, float originalArea) const;

  static const std::map<std::string, Predictor> candidatePredictors;
  static const std::map<std::string, std::vector<float>> scalesForLevels;

  const ROIResizerConfig mConfig;
  const Predictor mPredictor;
  const std::vector<float> mTargetAreas;

  // Save prev prediction to smooth the predicted size
  std::map<idType, CircularBuffer> prevPredictionBuffer;

  // Save <targetScale of ROI, calibration for that targetScale>
  std::map<idType, std::pair<int, float>> calibrationTable;
};

} // namespace md

#endif // ROI_RESIZER_HPP_
