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

using Predictor = std::function<int(
    float, float, float, float, float, float, float, float)>;

class RoIResizer {
 public:
  static const int INVALID_LEVEL;

  RoIResizer(const RoIResizerConfig& config);

  std::pair<float, int> getTargetScale(const idType id, const RoI::Features& features);

  void updateTable(RoI* roi);

  int getNumProbeSteps() const {
    return mConfig.NUM_PROBE_STEPS;
  }

  int isProbing() const {
    return mConfig.PROBE_STEP_SIZE != 0;
  }

  std::vector<float> getProbingCandidates(
      float scale, int level, int numProbeSteps);

 private:
  class CircularBuffer {
   public:
    CircularBuffer();

    void push(int data);

    int maxVote();

   private:
    size_t capacity_, oldest_index, size_;
    std::vector<int> data_;
  };

  int getMaxVotedLevel(const idType id, const RoI::Features& features);

  int predictLevelWithFeatures(const RoI::Features& features) const;

  bool isUsable(BoundingBox* targetBox, BoundingBox* baseBox) const;

  static float getOverlap(Rect& targetRect, Rect& baseRect);

  static const std::map<std::string, Predictor> candidatePredictors;
  static const std::map<std::string, std::vector<float>> scalesForLevels;

  const RoIResizerConfig mConfig;
  const Predictor mPredictor;
  const std::vector<float> mTargetSize;

  // Save prev prediction to smooth the predicted size
  std::map<idType, CircularBuffer> prevPredictionBuffer;

  // Save <targetScale of RoI, calibration for that targetScale>
  std::map<idType, std::pair<int, float>> calibrationTable;
};

} // namespace rm

#endif // ROI_RESIZER_HPP_
