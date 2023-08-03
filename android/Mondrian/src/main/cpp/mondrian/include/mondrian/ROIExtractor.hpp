#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <set>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIPacker.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

class ROIExtractor {
 public:
  ROIExtractor(const ROIExtractorConfig& config, ROIResizer* roiResizer);

  ~ROIExtractor();

  void enqueue(Frame* frame);

  std::condition_variable& cv();

  Stream collectFrames(int scheduleID);

 private:
  void work(int extractorId);

  void processPD(Frame* currFrame);

  void processOF(Frame* currFrame);

  static void getOpticalFlowROIs(const Frame* prevFrame, Frame* currFrame,
                                 const std::vector<BoundingBox>& boundingBoxes,
                                 const cv::Size& targetSize,
                                 std::vector<std::unique_ptr<ROI>>& outChildROIs);

  static std::vector<OFFeatures> opticalFlowTracking(
      const Frame* prevFrame, const Frame* currFrame, const std::vector<Rect>& boundingBoxes,
      const cv::Size& targetSize);

  void getPixelDiffROIs(Frame* currFrame, const cv::Size& targetSize,
                        const float maxPDROISize, const float minPDROISize,
                        std::vector<std::unique_ptr<ROI>>& outChildROIs) const;

  static cv::Mat calculateDiffAndThreshold(
      const cv::Mat& prevMat, const cv::Mat& currMat);

  static void cannyEdgeDetection(cv::Mat mat);

  static const cv::TermCriteria CRITERIA;

  bool stop_;
  std::vector<std::thread> threads_;

  const ROIExtractorConfig config_;

  ROIResizer* ROIResizer_;

  std::mutex mtx_;
  std::condition_variable cv_;
  Stream PDWaiting_;
  Stream PDProcessing_;
  Stream OFWaiting_;
  Stream OFProcessing_;
  Stream OFProcessed_;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
