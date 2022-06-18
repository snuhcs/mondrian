#include "strm/RoIExtractor.hpp"

#include <set>

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"

namespace rm {

RoIExtractor::RoIExtractor(const RoIExtractorConfig& config, const ResizeProfile* resizeProfile,
                           int maxRoISize)
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
  std::lock_guard<std::mutex> lock(mtx);
  mFramesForPD.push_back(frame);
  cv.notify_one();
  LOGD("RoIExtractor::enqueue  (%s, %4d)               // PD %lu | OF %lu | Processed %lu",
       frame->shortKey.c_str(), frame->frameIndex, mFramesForPD.size(), mFramesForOF.size(),
       mOFProcessingStartedFrames.size());
}

void RoIExtractor::notify() {
  cv.notify_one();
}

void RoIExtractor::preprocess(Frame* frame) const {
  assert(frame != nullptr);
  // TODO: handle exceptional cases (!preProcessedMet.empty() == true)
  if (frame->preProcessedMat.empty()) {
    cv::resize(frame->mat, frame->preProcessedMat, mTargetSize);
    cv::cvtColor(frame->preProcessedMat, frame->preProcessedMat, cv::COLOR_BGR2GRAY);
  }
  assert(frame->preProcessedMat.size() == mTargetSize);
  assert(frame->preProcessedMat.channels() == 1);
}

FrameSet RoIExtractor::getExtractedFrames() {
  std::unique_lock<std::mutex> lock(mtx);
  FrameSet extractedFrames = std::move(mOFProcessingStartedFrames);
  mOFProcessingStartedFrames.clear();
  // TODO: set useInferenceResultForOF = true only for lastFrames
  for (Frame* frame : extractedFrames) {
    frame->useInferenceResultForOF = true;
  }
  cv.wait(lock, [&extractedFrames]() {
    return extractedFrames.empty() ||
           (*extractedFrames.rbegin())->roiExtractionStatus == OF_EXTRACTED;
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

  auto isPDJobReady = [this]() {
    return !mFramesForPD.empty();
  };
  auto isOFJobReady = [this]() {
    return !mFramesForOF.empty() && mFramesForOF.front()->readyForOFExtraction();
  };
  bool isOF;
  Frame* frame;

  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this, &isOF, &isOFJobReady, &isPDJobReady]() {
      isOF = isOFJobReady();
      return mbStop || isOF || isPDJobReady();
    });
    if (mbStop) {
      return;
    }

    if (isOF) {
      frame = mFramesForOF.front();
      frame->roiExtractionStatus = OF_EXTRACTING;
      mOFProcessingStartedFrames.insert(frame);
      mFramesForOF.pop_front();
    } else {
      frame = mFramesForPD.front();
      mFramesForPD.pop_front();
    }
    lock.unlock();

    // Preprocess matrices
    if (!isOF) {
      preprocess(frame);
    }

    if (isOF) {
      processOF(frame);
    } else {
      processPD(frame);
    }

    if (isOF) {
      frame->resizeRoIStartTime = NowMicros();
      for (auto& roi : frame->origRoIs) {
        roi.targetSize = std::min(roi.maxEdgeLength,
                                  mResizeProfile->getTargetSize(roi.id, roi.features));
      }
      frame->resizeRoIEndTime = NowMicros();

      frame->mergeRoIStartTime = NowMicros();
      frame->rois = frame->origRoIs;
      mergeRoIs(frame->origRoIs, frame->rois, mConfig.MERGE_THRESHOLD, mMaxRoISize);
      frame->mergeRoIEndTime = NowMicros();
      LOGD("RoIExtractor::mergeRoIs(%s, %4d) took %4lu us  // %lu + %lu => %lu",
           frame->shortKey.c_str(),
           frame->frameIndex, frame->mergeRoIEndTime - frame->mergeRoIStartTime,
           std::count_if(frame->origRoIs.begin(), frame->origRoIs.end(),
                         [](const RoI& roi) { return roi.type == RoI::Type::PD; }),
           std::count_if(frame->origRoIs.begin(), frame->origRoIs.end(),
                         [](const RoI& roi) { return roi.type == RoI::Type::OF; }),
           frame->rois.size());

      std::set<idType> origIDs;
      for (RoI& origRoI : frame->origRoIs) {
        assert(origIDs.find(origRoI.id) == origIDs.end());
        origIDs.insert(origRoI.id);
      }
      std::set<idType> childIDs;
      for (RoI& roi : frame->rois) {
        for (RoI* origRoI : roi.childrenRoIs) {
          assert(childIDs.find(origRoI->id) == childIDs.end());
          childIDs.insert(origRoI->id);
        }
      }
      assert(origIDs == childIDs);

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

void RoIExtractor::processPD(Frame* currFrame) {
  assert(currFrame->roiExtractionStatus == OF_WAITING);
  Frame* prevFrame = currFrame->prevFrame;
  currFrame->pixelDiffRoIProcessStartTime = NowMicros();
  std::vector<RoI> pixelDiffRoIs = getPixelDiffRoIs(prevFrame, currFrame, mTargetSize,
                                                    mConfig.MIN_ROI_AREA);
  currFrame->pixelDiffRoIProcessEndTime = NowMicros();
  currFrame->origRoIs.insert(currFrame->origRoIs.end(), pixelDiffRoIs.begin(), pixelDiffRoIs.end());
  LOGD("RoIExtractor::processPD(%s, %4d) took %4lu us", currFrame->shortKey.c_str(),
       currFrame->frameIndex,
       currFrame->pixelDiffRoIProcessEndTime - currFrame->pixelDiffRoIProcessStartTime);
}

void RoIExtractor::processOF(Frame* currFrame) {
  assert(currFrame->roiExtractionStatus == OF_EXTRACTING);
  Frame* prevFrame = currFrame->prevFrame;
  std::vector<BoundingBox> reliablePrevBoxes;
  if (currFrame->prevFrame->useInferenceResultForOF) {
    for (const BoundingBox& box : currFrame->prevFrame->boxes) {
      if (box.confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        reliablePrevBoxes.emplace_back(box.id, box.srcRoI, Rect(
            std::max(0, box.location.left - mConfig.ROI_PADDING),
            std::max(0, box.location.top - mConfig.ROI_PADDING),
            std::min(currFrame->width, box.location.right + mConfig.ROI_PADDING),
            std::min(currFrame->height, box.location.bottom + mConfig.ROI_PADDING)),
                                       box.confidence, box.labelName);
      }
    }
  } else {
    for (RoI& roi : currFrame->prevFrame->origRoIs) {
      reliablePrevBoxes.emplace_back(roi.id, &roi, roi.location, 1, roi.labelName);
    }
  }
  currFrame->opticalFlowRoIProcessStartTime = NowMicros();
  std::vector<RoI> opticalFlowRoIs = getOpticalFlowRoIs(prevFrame, currFrame, reliablePrevBoxes, mTargetSize);
  currFrame->opticalFlowRoIProcessEndTime = NowMicros();
  currFrame->origRoIs.insert(currFrame->origRoIs.end(), opticalFlowRoIs.begin(),
                             opticalFlowRoIs.end());
  LOGD("RoIExtractor::processOF(%s, %4d) took %4lu us", currFrame->shortKey.c_str(),
       currFrame->frameIndex,
       currFrame->opticalFlowRoIProcessEndTime - currFrame->opticalFlowRoIProcessStartTime);
}

void RoIExtractor::mergeRoIs(std::vector<RoI>& origRoIs, std::vector<RoI>& rois, const float mergeThreshold, int maxSize) {
  // Match roi <=> origRoI ID before merge
  for (RoI& roi : rois) {
    for (RoI& origRoI : origRoIs) {
      if (origRoI.id == roi.id) {
        roi.childrenRoIs.push_back(&origRoI);
      }
    }
  }
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
        if (newRight - newLeft > maxSize || newBottom - newTop > maxSize) {
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
    rois.push_back(RoI::mergeRoIs(rois[i], rois[j]));
    // Match child parent
    RoI* mergedRoI = &(*rois.rbegin());
    mergedRoI->childrenRoIs.insert(mergedRoI->childrenRoIs.end(),
                                   rois[i].childrenRoIs.begin(), rois[i].childrenRoIs.end());
    mergedRoI->childrenRoIs.insert(mergedRoI->childrenRoIs.end(),
                                   rois[j].childrenRoIs.begin(), rois[j].childrenRoIs.end());
    assert(j > i);
    rois.erase(rois.begin() + j);
    rois.erase(rois.begin() + i);
  }
}

std::vector<RoI> RoIExtractor::getOpticalFlowRoIs(
    const Frame* prevFrame, Frame* currFrame,
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
        opticalFlowRoIs.emplace_back(box.id, currFrame, box.srcRoI, Rect(newLeft, newTop, newRight, newBottom), RoI::Type::OF, box.labelName, shift, err, 0);
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
  assert(prevFrame->preProcessedMat.channels() == currFrame->preProcessedMat.channels());

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
    cv::Point bbxCentroidPoints(bbxCenterX * targetSize.width / currFrame->width,
                                bbxCenterY * targetSize.height / currFrame->height);
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
          (int) ((p1Arr[pointIdx].x - centroids.at(pointIdx).x) * currFrame->width /
                 targetSize.width),
          (int) ((p1Arr[pointIdx].y - centroids.at(pointIdx).y) * currFrame->height /
                 targetSize.height)),
                                  err[pointIdx]);
    } else {
      shiftAndErrors.emplace_back(std::make_pair(0, 0), 0);
    }
  }
  return shiftAndErrors;
}

std::vector<RoI> RoIExtractor::getPixelDiffRoIs(const Frame* prevFrame, Frame* currFrame,
                                                const cv::Size& targetSize, const int mixRoIArea) {
  assert(!prevFrame->preProcessedMat.empty());
  assert(!currFrame->preProcessedMat.empty());
  assert(prevFrame->preProcessedMat.channels() == currFrame->preProcessedMat.channels());
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
      float diffAreaRatio = (float) cv::contourArea(approxCurve) / (float) box.area();
      boxAndFeatures.emplace_back(originalBox, diffAreaRatio);
    }
  }

  std::vector<RoI> rois;
  rois.reserve(boxAndFeatures.size());
  for (const std::pair<Rect, float>& boxAndFeature : boxAndFeatures) {
    rois.emplace_back(
        RoI::getNewIds(1).first,
        currFrame,
        nullptr,
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
