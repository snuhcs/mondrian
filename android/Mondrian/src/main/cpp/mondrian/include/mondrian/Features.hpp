#ifndef FEATURES_HPP_
#define FEATURES_HPP_

#include "opencv2/opencv.hpp"

#include "mondrian/DataType.hpp"

namespace md {

using Shift = std::pair<float, float>;

class OFFeatures {
 public:
  Shift shiftAvg;
  Shift shiftStd;
  float shiftNcc;
  float avgErr;

  OFFeatures();

  OFFeatures(const std::vector<Shift>& shifts,
             const std::vector<int>& statuses,
             const std::vector<float>& errs);

 private:
  static std::pair<std::vector<Shift>, std::vector<float>>
  filterInvalid(const std::vector<Shift>& shifts,
                const std::vector<int>& statuses,
                const std::vector<float>& errs);

  static std::pair<std::vector<Shift>, std::vector<float>>
  filterOutlier(const std::vector<Shift>& shifts,
                const std::vector<float>& errs);

  static Shift avgOf(const std::vector<Shift>& shifts);

  static Shift stdOf(const std::vector<Shift>& shifts);

  static float nccOf(const std::vector<Shift>& shifts);

  static float avgOf(const std::vector<float>& errs);
};

struct Features {
  float width;
  float height;
  int label;
  ROIType type;
  Origin origin;
  float xyRatio;
  float confidence;
  OFFeatures ofFeatures;

  static std::string header();

  std::string str() const;
};

} // namespace md

#endif // FEATURES_HPP_
