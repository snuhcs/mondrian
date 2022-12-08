#ifndef ROI_RESIZER_HPP_
#define ROI_RESIZER_HPP_

#include <string>
#include <map>

#include "strm/Config.hpp"
#include "strm/RoI.hpp"
#include "strm/tree/VIRAT.hpp"
#include "strm/tree/MTA.hpp"

namespace rm {

class RoI;

using Predictor = std::function<int(
    float, float, float, float, float, float, float, float)>;

class RoIResizer {
 public:
  static const int STATIC_LEVEL;
  static const int INVALID_LEVEL;

  RoIResizer(const RoIResizerConfig& config);

  std::pair<float, int> getTargetScale(const idType id, const Features& features,
                                       const float maxEdgeLength);

  void updateTable(RoI* cRoI);

  int getNumProbeSteps() const {
    return mConfig.NUM_PROBE_STEPS;
  }

  std::vector<float> getProbingCandidates(float scale, int level, int numProbeSteps) const;

 private:
  class CircularBuffer {
   public:
    CircularBuffer() {}; // Default ctor for std::map

    CircularBuffer(int numLevels);

    void push(int data);

    int maxVote();

   private:
    int numLevels;
    size_t capacity_, oldest_index, size_;
    std::vector<int> data_;
  };

  std::pair<float, int> getTargetScale(const idType id, const Features& features);

  float getTargetScale(const int scaleLevel) const;

  bool isCalibrated(const idType id, const int scaleLevel) const;

  int getMaxVotedLevel(const idType id, const Features& features);

  int predictLevelWithFeatures(const Features& features) const;

  bool isUsable(BoundingBox* targetBox, BoundingBox* baseBox) const;

  static const std::map<std::string, Predictor> candidatePredictors;
  static const std::map<std::string, std::vector<float>> scalesForLevels;

  const RoIResizerConfig mConfig;
  const Predictor mPredictor;
  const std::vector<float> mTargetScales;

  // Save prev prediction to smooth the predicted size
  std::map<idType, CircularBuffer> prevPredictionBuffer;

  // Save <targetScale of RoI, calibration for that targetScale>
  std::map<idType, std::pair<int, float>> calibrationTable;
};

} // namespace rm

#endif // ROI_RESIZER_HPP_
