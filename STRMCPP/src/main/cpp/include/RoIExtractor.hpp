#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <utility>

#include <opencv2/opencv.hpp>

#include "Config.hpp"
#include "DataType.hpp"

namespace rm {

class RoIExtractor {
public:
    RoIExtractor(RoIExtractorConfig config)
    : mConfig(config),
      mTargetSize(cv::Size(mConfig.EXTRACTION_RESIZE_WIDTH, mConfig.EXTRACTION_RESIZE_HEIGHT)) {};
    bool useOpticalFlowRoIs();
    void process(const std::pair<std::pair<Frame, Frame>,std::vector<BoundingBox>>& item);

private:
    static void mergeSingleFrameRoIs(
            const Frame& frame, std::vector<RoI> rois, float mergeThreshold);
    static std::vector<RoI> getOpticalFlowRoIs(
            const Frame& prevFrame, const Frame& currFrame,
            const std::vector<BoundingBox>& boundingBoxes, const cv::Size& targetSize);
    static std::vector<std::vector<int>> getBoundingBoxShifts(
            const cv::Mat* prevImage, const cv::Mat* currImage,
            const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize);
    static std::vector<RoI> getPixelDiffRoIs(
            const Frame& prevFrame, const Frame& currFrame, const cv::Size& targetSize);
    static void calculateDiffAndThreshold(
            const cv::Mat& frame0, const cv::Mat& frame1, cv::Mat diff);
    static void cannyEdgeDetection(cv::Mat mat);

    RoIExtractorConfig mConfig;
    cv::Size mTargetSize;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
