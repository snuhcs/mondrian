#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <map>
#include <memory>
#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/BoundingBox.hpp"

namespace md {

cv::Vec3b yuv2rgb(uchar Y, uchar U, uchar V);

cv::Mat extractRgbROIFromYuvMat(cv::Mat yuvMat, int l, int t, int r, int b);

struct RectTrackingResult {
  std::vector<cv::Point2f> prevPoints;
  std::vector<cv::Point2f> nextPoints;
  std::vector<int> statuses;
  std::vector<float> errors;
};

std::vector<cv::Rect2i> extractPD(const cv::Mat& prevGrayMat, const cv::Mat& nextGrayMat);

std::vector<RectTrackingResult> extractOF(const cv::Mat& prevGrayMat,
                                          const cv::Mat& nextGrayMat,
                                          const std::vector<cv::Rect2f>& prevRects,
                                          bool useCenter,
                                          int* numFeaturePoints = nullptr);

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThres);

void nms(std::vector<std::unique_ptr<BoundingBox>>& boxes,
         const int numLabels, const float iouThres);

cv::Rect2f operator*(const cv::Rect2f& rect, const float scale);

cv::Rect2f& operator*=(cv::Rect2f& rect, const float scale);

bool sched_setaffinity_primary();

bool sched_setaffinity_big();

bool sched_setaffinity_big_or_primary();

bool sched_setaffinity_little();

} // namespace md

#endif // UTILS_HPP_
