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

  std::set<std::string> keys;
  for (Frame* frame : mFramesForPD) {
    keys.insert(frame->key);
  }

  for (const std::string& key : keys) {
    std::stringstream index, state, ready;
    state << "RoIExtractor State " << key << " : ";
    ready << "RoIExtractor Ready " << key << " : ";
    for (Frame* frame : mFramesForPD) {
      if (frame->key == key) {
        state << frame->roiExtractionStatus << " ";
        ready << (frame->prevFrame == nullptr ? "-" : std::to_string(frame->prevFrame->isOFReady)) << " ";
      }
    }
    LOGD("%s", state.str().c_str());
    LOGD("%s", index.str().c_str());
    LOGD("");
  }
  LOGD("RoIExtractor::enqueue(%d) end", frame->frameIndex);
}

std::vector<Frame*> RoIExtractor::getExtractedFrames() {
  std::unique_lock<std::mutex> lock(mtx);
  std::vector<Frame*> extractedFrames = std::move(mOFProcessingStartedFrames);
  mOFProcessingStartedFrames.clear();
  cv.wait(lock, [extractedFrames]() {
    return !extractedFrames.empty() && extractedFrames.back()->roiExtractionStatus == OF_EXTRACTED;
  });
  return extractedFrames;
}

void RoIExtractor::work() {
  LOGD("RoIExtractor::work()");
  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return mbStop || !mFramesForOF.empty() || !mFramesForPD.empty(); });
    if (mbStop) {
      return;
    }

    LOGD("RoIExtractor process start");
    time_us startTime = NowMicros();
    Frame* frameToProcess;
    if (!mFramesForOF.empty()) {
      frameToProcess = mFramesForOF.front();
      frameToProcess->roiExtractionStatus = OF_EXTRACTING;
      mOFProcessingStartedFrames.push_back(frameToProcess);
      mFramesForOF.pop_front();
    } else {
      frameToProcess = mFramesForPD.front();
      mFramesForPD.pop_front();
    }
    lock.unlock();

    process(frameToProcess);
    cv.notify_all();
  }
}

void RoIExtractor::process(Frame* currFrame) const {
  assert(currFrame->roiExtractionStatus == OF_WAITING || currFrame->roiExtractionStatus == OF_EXTRACTING);

  Frame* prevFrame = currFrame->prevFrame;
  LOGD("RoIExtractor::process((%s, %d), (%s, %d), %d)", prevFrame->key.c_str(),
       prevFrame->frameIndex, currFrame->key.c_str(), currFrame->frameIndex,
       currFrame->roiExtractionStatus);

  std::vector<RoI> rois;

  // Preprocess matrices
  if (prevFrame->preProcessedMat.empty()) {
    cv::Mat mat = prevFrame->mat;
    cv::resize(mat, mat, mTargetSize);
    cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
    prevFrame->preProcessedMat = mat;
  }
  if (currFrame->preProcessedMat.empty()) {
    cv::Mat mat = currFrame->mat;
    cv::resize(mat, mat, mTargetSize);
    cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
    currFrame->preProcessedMat = mat;
  }

  // PD RoI Extraction
  if (currFrame->roiExtractionStatus != OF_EXTRACTING) {
    currFrame->pixelDiffRoIProcessStartTime = NowMicros();
    std::vector<RoI> pixelDiffRoIs = getPixelDiffRoIs(prevFrame, currFrame,
                                                      mTargetSize, mConfig.MIN_ROI_AREA);
    currFrame->pixelDiffRoIProcessEndTime = NowMicros();
    currFrame->rois.insert(currFrame->rois.end(), pixelDiffRoIs.begin(), pixelDiffRoIs.end());

    currFrame->roiExtractionStatus = OF_WAITING;

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
    currFrame->updateBoxesToTrackWithOFRoIs(opticalFlowRoIs);
    currFrame->rois.insert(currFrame->rois.end(), opticalFlowRoIs.begin(), opticalFlowRoIs.end());

    currFrame->resizeRoIStartTime = NowMicros();
    for (auto& roi : currFrame->rois) {
      roi.targetSize = std::min(roi.maxEdgeLength, mResizeProfile->getTargetSize(roi.features));
    }
    currFrame->resizeRoIEndTime = NowMicros();

    currFrame->mergeRoIStartTime = NowMicros();
    currFrame->mergedRoIs = mergeRoIs(currFrame->rois, mConfig.MERGE_THRESHOLD, mMaxRoISize);
    currFrame->mergeRoIEndTime = NowMicros();
    LOGD("Merge: %lu => %lu", currFrame->rois.size(), currFrame->mergedRoIs.size());

    prevFrame->preProcessedMat.release();

    currFrame->roiExtractionStatus = OF_EXTRACTED;
  }
}

std::vector<RoI> RoIExtractor::mergeRoIs(const std::vector<RoI>& rois, const float mergeThreshold,
                                         const cv::Size& maxSize) {
  std::vector<RoI> mergedRoIs = rois;
  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < mergedRoIs.size(); i++) {
      for (j = i + 1; j < mergedRoIs.size(); j++) {
        const RoI& roi0 = mergedRoIs[i];
        const RoI& roi1 = mergedRoIs[j];
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
    const RoI& roi0 = mergedRoIs[i];
    const RoI& roi1 = mergedRoIs[j];
    std::string roiLabel = roi0.labelName.empty() || roi1.labelName.empty()
                           || roi0.labelName != roi1.labelName
                           ? "" : roi0.labelName;
    mergedRoIs.push_back(RoI::mergeRoIs(roi0, roi1));
    assert(j > i);
    mergedRoIs.erase(mergedRoIs.begin() + j);
    mergedRoIs.erase(mergedRoIs.begin() + i);
  }
  return mergedRoIs;
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
        opticalFlowRoIs.emplace_back(currFrame, Rect(newLeft, newTop, newRight, newBottom),
                                     RoI::Type::OF, box.labelName, shift, err, 0);
      }
    }
  }
  return opticalFlowRoIs;
}

std::vector<std::pair<std::pair<int, int>, float>> RoIExtractor::getShiftAndErrors(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->preProcessedMat.empty() && !currFrame->preProcessedMat.empty());

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
