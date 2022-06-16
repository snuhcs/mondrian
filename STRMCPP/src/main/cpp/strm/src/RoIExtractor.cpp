#include "strm/RoIExtractor.hpp"

#include <set>

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"

namespace rm {

RoIExtractor::RoIExtractor(const RoIExtractorConfig& config, const ResizeProfile* resizeProfile,
                           const cv::Size& maxRoISize)
    : mConfig(config),
      mTargetSize(cv::Size(mConfig.EXTRACTION_RESIZE_WIDTH, mConfig.EXTRACTION_RESIZE_HEIGHT)),
      mResizeProfile(resizeProfile), mMaxRoISize(maxRoISize), mbStop(false) {
  LOGD("RoIExtractor()");
  mThreads.reserve(config.NUM_WORKERS);
  for (int i = 0; i < config.NUM_WORKERS; i++) {
    mThreads.emplace_back([this]() { work(); });
  }
}

RoIExtractor::~RoIExtractor() {
  mbStop = true;
  cv.notify_all();
  for (auto& thread : mThreads) {
    thread.join();
  }
}

void RoIExtractor::enqueue(Frame* frame) {
  LOGD("RoIExtractor::enqueue(%d) start", frame->frameIndex);
  std::lock_guard<std::mutex> lock(mtx);
  mFramesForPD.push_back(frame);
  cv.notify_one();
  LOGD("RoIExtractor::enqueue(%d) end %lu %lu %lu", frame->frameIndex,
       mFramesForPD.size(), mFramesForOF.size(), mOFProcessingStartedFrames.size());
}

void RoIExtractor::notify() {
  cv.notify_all();
}

void RoIExtractor::preprocess(Frame* frame) const {
  cv::resize(frame->mat, frame->preProcessedMat, mTargetSize);
  cv::cvtColor(frame->preProcessedMat, frame->preProcessedMat, cv::COLOR_BGR2GRAY);
}

std::vector<Frame*> RoIExtractor::getExtractedFrames() {
  LOGD("RoIExtractor::getExtractedFrames()");
  std::unique_lock<std::mutex> lock(mtx);
  std::vector<Frame*> extractedFrames = std::move(mOFProcessingStartedFrames);
  mOFProcessingStartedFrames.clear();
  LOGD("RoIExtractor::getExtractedFrames() extractedFrames.back() == %d",
       extractedFrames.empty() ? -1 : extractedFrames.back()->frameIndex);
  cv.wait(lock, [extractedFrames]() {
    return extractedFrames.empty() || extractedFrames.back()->roiExtractionStatus == OF_EXTRACTED;
  });
  return extractedFrames;
}

void RoIExtractor::work() {
  /*
   *    Frame Status           Containing data structure    Frame Status
   * 1. Before PD extraction | mFramesForPD               | OF_WAITING
   * 2. Extracting PD        | -                          | OF_WAITING
   * 3. Before OF extraction | mFramesForOF               | OF_WAITING
   * 4. Extracting OF        | mOFProcessingStartedFrames | OF_EXTRACTING
   * 5. OF extraction ended  | mOFProcessingStartedFrames | OF_EXTRACTED
   */
  LOGD("RoIExtractor::work()");

  auto isPDJobReady = [this]() {
    return !mFramesForPD.empty();
  };
  auto isOFJobReady = [this]() {
    return !mFramesForOF.empty() && mFramesForOF.front()->readyForOFExtraction();
  };
  bool processOF;
  Frame* frame;

  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this, &processOF, &isOFJobReady, &isPDJobReady]() {
      processOF = isOFJobReady();
      return mbStop || processOF || isPDJobReady();
    });
    if (mbStop) {
      return;
    }

    if (processOF) {
      frame = mFramesForOF.front();
      frame->roiExtractionStatus = OF_EXTRACTING;
      mOFProcessingStartedFrames.push_back(frame);
      mFramesForOF.pop_front();
    } else {
      frame = mFramesForPD.front();
      mFramesForPD.pop_front();
    }
    lock.unlock();

    process(frame);

    if (processOF) {
      frame->resizeRoIStartTime = NowMicros();
      for (auto& roi : frame->rois) {
        roi.targetSize = std::min(roi.maxEdgeLength, mResizeProfile->getTargetSize(roi.features));
      }
      frame->resizeRoIEndTime = NowMicros();

      frame->mergeRoIStartTime = NowMicros();
      frame->rois = mergeRoIs(frame->origRoIs, mConfig.MERGE_THRESHOLD, mMaxRoISize);
      frame->mergeRoIEndTime = NowMicros();
      LOGD("Merge: %lu => %lu", frame->origRoIs.size(), frame->rois.size());

      frame->prevFrame->preProcessedMat.release();

      frame->roiExtractionStatus = OF_EXTRACTED;
    } else {
      lock.lock();
      mFramesForOF.push_back(frame);
      lock.unlock();
    }
    cv.notify_all();
  }
}

void RoIExtractor::process(Frame* currFrame) {
  assert(currFrame->roiExtractionStatus == OF_WAITING ||
         currFrame->roiExtractionStatus == OF_EXTRACTING);

  LOGD("RoIExtractor::process(%s, %d) %s", currFrame->shortKey().c_str(), currFrame->frameIndex,
       (currFrame->roiExtractionStatus == OF_WAITING ? "PD" : "OF"));

  std::vector<RoI> rois;
  Frame* prevFrame = currFrame->prevFrame;

  // Preprocess matrices
  preprocess(currFrame);

  // PD RoI Extraction
  if (currFrame->roiExtractionStatus != OF_EXTRACTING) {
    currFrame->pixelDiffRoIProcessStartTime = NowMicros();
    std::vector<RoI> pixelDiffRoIs = getPixelDiffRoIs(prevFrame, currFrame,
                                                      mTargetSize, mConfig.MIN_ROI_AREA);
    currFrame->pixelDiffRoIProcessEndTime = NowMicros();
    currFrame->origRoIs.insert(currFrame->origRoIs.end(), pixelDiffRoIs.begin(),
                               pixelDiffRoIs.end());

  } else { // OF RoI Extraction
    std::vector<BoundingBox> reliablePrevBoxes;
    for (const BoundingBox& bbx : currFrame->prevFrame->boxesToTrack) {
      if (bbx.confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        reliablePrevBoxes.push_back(bbx);
      }
    }
    currFrame->opticalFlowRoIProcessStartTime = NowMicros();
    std::vector<RoI> opticalFlowRoIs = getOpticalFlowRoIs(prevFrame, currFrame,
                                                          reliablePrevBoxes, mTargetSize);
    currFrame->opticalFlowRoIProcessEndTime = NowMicros();
    currFrame->origRoIs.insert(currFrame->origRoIs.end(), opticalFlowRoIs.begin(),
                               opticalFlowRoIs.end());
    currFrame->updateBoxesToTrackWithRoIs();
  }
}

std::vector<RoI> RoIExtractor::mergeRoIs(std::vector<RoI>& origRois, const float mergeThreshold,
                                         const cv::Size& maxSize) {
  std::vector<RoI> rois = origRois;
  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < rois.size(); i++) {
      for (j = i + 1; j < rois.size(); j++) {
        const RoI& roi0 = rois[i];
        const RoI& roi1 = rois[j];
        int intersection = roi0.location.intersection(roi1.location);
        if ((float) intersection / (float) roi0.getArea() < mergeThreshold &&
            (float) intersection / (float) roi1.getArea() < mergeThreshold) {
          continue;
        }
        int newLeft = std::min(roi0.location.left, roi1.location.left);
        int newTop = std::min(roi0.location.top, roi1.location.top);
        int newRight = std::max(roi0.location.right, roi1.location.right);
        int newBottom = std::max(roi0.location.bottom, roi1.location.bottom);
        if (newRight - newLeft > maxSize.width || newBottom - newTop > maxSize.height) {
          continue;
        }
        int newArea = (newRight - newLeft) * (newBottom - newLeft);
        if (roi0.targetSize * roi1.maxEdgeLength > roi1.targetSize * roi0.maxEdgeLength) {
          // If roi0 resizes conservatively than roi1
          newArea = newArea * roi0.targetSize * roi0.targetSize
                    / roi0.maxEdgeLength / roi0.maxEdgeLength;
        } else {
          // If roi1 resizes conservatively than roi0
          newArea = newArea * roi1.targetSize * roi1.targetSize
                    / roi1.maxEdgeLength / roi1.maxEdgeLength;
        }
        int originalArea = roi0.getResizedArea() + roi1.getResizedArea();
        if (newArea >= originalArea) {
          continue;
        }
        updated = true;
        break;
      }
      if (updated) {
        break;
      }
    }
    if (!updated) {
      break;
    }
    const RoI& roi0 = rois[i];
    const RoI& roi1 = rois[j];
    std::string roiLabel = roi0.labelName.empty() || roi1.labelName.empty()
                           || roi0.labelName != roi1.labelName
                           ? "" : roi0.labelName;
    rois.push_back(RoI::mergeRoIs(roi0, roi1));
    assert(j > i);
    rois.erase(rois.begin() + j);
    rois.erase(rois.begin() + i);
  }
  return rois;
}

std::vector<RoI> RoIExtractor::getOpticalFlowRoIs(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<BoundingBox>& boundingBoxes, const cv::Size& targetSize) {
  int width = currFrame->mat.cols;
  int height = currFrame->mat.rows;

  std::vector<Rect> boundingRects;
  boundingRects.reserve(boundingBoxes.size());
  for (const auto& bbx : boundingBoxes) {
    boundingRects.emplace_back(bbx.location);
  }

  std::vector<RoI> opticalFlowRoIs;
  if (!boundingBoxes.empty()) {
    const std::vector<std::pair<std::pair<int, int>, float>>& shiftAndErrors = getShiftAndErrors(
        prevFrame, currFrame, boundingRects, targetSize);
    for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
      const std::pair<int, int>& shift = shiftAndErrors.at(boxIndex).first;
      const float err = shiftAndErrors.at(boxIndex).second;
      const BoundingBox& box = boundingBoxes.at(boxIndex);
      const Rect& loc = box.location;
      int newLeft = std::max(0, loc.left + shift.first);
      int newTop = std::max(0, loc.top + shift.second);
      int newRight = std::min(width, loc.right + shift.first);
      int newBottom = std::min(height, loc.bottom + shift.second);
      if (newLeft < newRight && newTop < newBottom) {
        opticalFlowRoIs.emplace_back(box.id, currFrame, Rect(newLeft, newTop, newRight, newBottom),
                                     RoI::Type::OF, box.labelName, shift, err, 0);
      }
    }
  }
  return opticalFlowRoIs;
}

std::vector<std::pair<std::pair<int, int>, float>> RoIExtractor::getShiftAndErrors(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->preProcessedMat.empty());
  assert(!currFrame->preProcessedMat.empty());

  const cv::Mat& prevImage = prevFrame->preProcessedMat;
  const cv::Mat& currImage = currFrame->preProcessedMat;

  std::vector<cv::Point2f> p0;
  std::vector<cv::Point2f> p1;

  std::vector<uchar> status;
  std::vector<float> err;

  std::vector<cv::Point> centroids;
  for (const Rect& bbx : boundingBoxes) {
    int bbxCenterX = bbx.left + bbx.width() / 2;
    int bbxCenterY = bbx.top + bbx.height() / 2;
    cv::Point bbxCentroidPoints(bbxCenterX * targetSize.width / currFrame->mat.cols,
                                bbxCenterY * targetSize.height / currFrame->mat.rows);
    centroids.push_back(bbxCentroidPoints);
    // might not work... replaces p0.fromList(centroids); p0 is Point2f,
    p0.push_back(bbxCentroidPoints);
  }
  cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);
  cv::calcOpticalFlowPyrLK(prevImage, currImage, p0, p1, status, err, cv::Size(15, 15), 2,
                           criteria);

  uchar StatusArr[status.size()];
  std::copy(status.begin(), status.end(), StatusArr);

  cv::Point p1Arr[p1.size()];
  std::copy(p1.begin(), p1.end(), p1Arr);

  std::vector<std::pair<std::pair<int, int>, float>> shiftAndErrors;
  for (int pointIdx = 0; pointIdx < centroids.size(); pointIdx++) {
    if (StatusArr[pointIdx] == 1) {
      shiftAndErrors.emplace_back(std::make_pair(
          (int) ((p1Arr[pointIdx].x - centroids.at(pointIdx).x) * currFrame->mat.cols /
                 targetSize.width),
          (int) ((p1Arr[pointIdx].y - centroids.at(pointIdx).y) * currFrame->mat.rows /
                 targetSize.height)),
                                  err[pointIdx]);
    } else {
      shiftAndErrors.emplace_back(std::make_pair(0, 0), 0);
    }
  }
  return shiftAndErrors;
}

std::vector<RoI> RoIExtractor::getPixelDiffRoIs(const Frame* prevFrame, const Frame* currFrame,
                                                const cv::Size& targetSize, const int mixRoIArea) {
  cv::Mat mat = calculateDiffAndThreshold(prevFrame->preProcessedMat, currFrame->preProcessedMat);
  cannyEdgeDetection(mat);

  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;

  cv::findContours(mat, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // replaces get boxes from contours.
  std::vector<std::pair<Rect, float>> boxAndFeatures;
  for (const std::vector<cv::Point>& contour : contours) {
    double approxDistance = cv::arcLength(contour, true) * 0.02;
    std::vector<cv::Point> approxCurve;
    cv::approxPolyDP(contour, approxCurve, approxDistance, true);
    cv::Rect box = cv::boundingRect(approxCurve);
    if (box.area() >= mixRoIArea) {
      Rect originalBox = Rect(box.x * currFrame->mat.cols / targetSize.width,
                              box.y * currFrame->mat.rows / targetSize.height,
                              (box.x + box.width) * currFrame->mat.cols / targetSize.width,
                              (box.y + box.height) * currFrame->mat.rows / targetSize.height);
      float diffAreaRatio = cv::contourArea(approxCurve) / box.area();
      boxAndFeatures.emplace_back(originalBox, diffAreaRatio);
    }
  }

  std::vector<RoI> rois;
  rois.reserve(boxAndFeatures.size());
  for (const std::pair<Rect, float>& boxAndFeature : boxAndFeatures) {
    rois.emplace_back(
        RoI::getNewIds(1).first,
        currFrame,
        boxAndFeature.first,
        RoI::PD,
        "",
        std::make_pair(0, 0),
        0,
        boxAndFeature.second);
  }
  return rois;
}

cv::Mat RoIExtractor::calculateDiffAndThreshold(
    const cv::Mat& prevMat, const cv::Mat& currMat) {
  cv::Mat diff;
  cv::absdiff(prevMat, currMat, diff);
  for (int i = 0; i < 3; i++) {
    cv::dilate(diff, diff, cv::getStructuringElement(
        cv::MORPH_RECT, cv::Size(3, 3)), cv::Point(0, 0), i + 1);
  }
  cv::threshold(diff, diff, 30, 255, cv::THRESH_BINARY);
  return diff;
}

void RoIExtractor::cannyEdgeDetection(cv::Mat mat) {
  cv::GaussianBlur(mat, mat, cv::Size(3, 3), 0);
  cv::Canny(mat, mat, 120, 255, 3, true);
  cv::dilate(mat, mat, cv::getStructuringElement(
      cv::MORPH_RECT, cv::Size(5, 5)), cv::Point(0, 0), 1);
}

} // namespace rm
