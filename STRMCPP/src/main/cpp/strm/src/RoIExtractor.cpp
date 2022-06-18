#include "strm/RoIExtractor.hpp"

#include <set>

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"
#include "strm/Test.hpp"

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
  std::unique_lock<std::mutex> lock(mtx);
  mFramesForPD.push_back(frame);
  lock.unlock();
  cv.notify_one();
  std::stringstream ss;
  for (const auto& it: mFramesForOF) {
    ss << it.second.size() << ", ";
  }
  LOGD("RoIExtractor::enqueue  (%s, %4d)               // PD %lu | OF %s | Processed %lu",
       frame->shortKey.c_str(), frame->frameIndex, mFramesForPD.size(), ss.str().c_str(),
       mOFProcessingStartedFrames.size());
}

void RoIExtractor::notify() {
  cv.notify_all();
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

std::map<std::string, SortedFrames> RoIExtractor::getExtractedFrames() {
  std::unique_lock<std::mutex> lock(mtx);
  std::map<std::string, SortedFrames> extractedFrames = std::move(mOFProcessingStartedFrames);
  mOFProcessingStartedFrames.clear();
  // TODO: set useInferenceResultForOF = true only for lastFrames
  for (const auto& it : extractedFrames) {
    for (auto frame : it.second) {
      frame->useInferenceResultForOF = true;
    }
  }
  cv.wait(lock, [&extractedFrames]() {
    for (auto it : extractedFrames) {
      if (!it.second.empty() && (*it.second.rbegin())->roiExtractionStatus != OF_EXTRACTED) {
        return false;
      }
    }
    return true;
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
    if (mFramesForOF.empty()) {
      return false;
    }
    for (const auto& it : mFramesForOF) {
      if (!it.second.empty()) {
        return it.second.front()->readyForOFExtraction();
      }
    }
    return false;
  };

  while (true) {
    bool isOF;
    Frame* frame = nullptr;

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this, &isOF, &isOFJobReady, &isPDJobReady]() {
      isOF = isOFJobReady();
      return mbStop || isOF || isPDJobReady();
    });
    if (mbStop) {
      return;
    }

    if (isOF) {
      for (const auto& it : mFramesForOF) {
        if (!it.second.empty() && it.second.front()->readyForOFExtraction()) {
          frame = it.second.front();
        }
      }
      assert(frame != nullptr);
      frame->roiExtractionStatus = OF_EXTRACTING;
      mOFProcessingStartedFrames[frame->key].insert(frame);
      mFramesForOF[frame->key].pop_front();
    } else {
      frame = mFramesForPD.front();
      mFramesForPD.pop_front();
      assert(frame != nullptr);
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
      frame->filterPDRoIs(mConfig.MERGE_THRESHOLD);

      frame->resizeRoIStartTime = NowMicros();
      for (auto& roi : frame->childRoIs) {
        roi.targetSize = std::min(roi.maxEdgeLength, mResizeProfile->getTargetSize(roi.id, roi.features));
      }
      frame->resizeRoIEndTime = NowMicros();

      frame->mergeRoIStartTime = NowMicros();
      frame->parentRoIs = frame->childRoIs;
      mergeRoIs(frame->childRoIs, frame->parentRoIs, mMaxRoISize);
      frame->mergeRoIEndTime = NowMicros();

      LOGD("RoIExtractor::mergeRoIs(%s, %4d) took %4lu us  // %lu + %lu => %lu",
           frame->shortKey.c_str(),
           frame->frameIndex, frame->mergeRoIEndTime - frame->mergeRoIStartTime,
           std::count_if(frame->childRoIs.begin(), frame->childRoIs.end(),
                         [](const RoI& roi) { return roi.type == RoI::Type::PD; }),
           std::count_if(frame->childRoIs.begin(), frame->childRoIs.end(),
                         [](const RoI& roi) { return roi.type == RoI::Type::OF; }),
           frame->parentRoIs.size());

      testAssignedUniqueRoIID(frame->childRoIs);
      testParentChildrenIDsAndChildIDsSame(frame->childRoIs, frame->parentRoIs);

      frame->prevFrame->preProcessedMat.release();
      frame->roiExtractionStatus = OF_EXTRACTED;
    } else {
      lock.lock();
      mFramesForOF[frame->key].push_back(frame);
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
  testAssignedUniqueRoIID(pixelDiffRoIs);
  currFrame->pixelDiffRoIProcessEndTime = NowMicros();
  currFrame->childRoIs.insert(currFrame->childRoIs.end(), pixelDiffRoIs.begin(), pixelDiffRoIs.end());
  LOGD("RoIExtractor::processPD(%s, %4d) took %4lu us", currFrame->shortKey.c_str(),
       currFrame->frameIndex,
       currFrame->pixelDiffRoIProcessEndTime - currFrame->pixelDiffRoIProcessStartTime);
}

void RoIExtractor::processOF(Frame* currFrame) {
  assert(currFrame->roiExtractionStatus == OF_EXTRACTING);
  Frame* prevFrame = currFrame->prevFrame;
  std::vector<BoundingBox> reliablePrevBoxes;
  if (currFrame->prevFrame->useInferenceResultForOF) {
    testAssignedUniqueBoxID(currFrame->prevFrame->boxes);
    for (const std::unique_ptr<BoundingBox>& box : currFrame->prevFrame->boxes) {
      if (box->confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        reliablePrevBoxes.push_back(*box);
      }
    }
  } else {
    for (RoI& roi : currFrame->prevFrame->childRoIs) {
      BoundingBox reliableBox(roi.id, roi.location, 1, roi.label);
      reliableBox.srcRoI = &roi;
      reliablePrevBoxes.push_back(reliableBox);
    }
  }
  currFrame->opticalFlowRoIProcessStartTime = NowMicros();
  std::vector<RoI> opticalFlowRoIs = getOpticalFlowRoIs(prevFrame, currFrame, reliablePrevBoxes, mTargetSize);
  currFrame->opticalFlowRoIProcessEndTime = NowMicros();
  testAssignedUniqueRoIID(opticalFlowRoIs);
  currFrame->childRoIs.insert(currFrame->childRoIs.end(), opticalFlowRoIs.begin(),
                              opticalFlowRoIs.end());
  LOGD("RoIExtractor::processOF(%s, %4d) took %4lu us", currFrame->shortKey.c_str(),
       currFrame->frameIndex,
       currFrame->opticalFlowRoIProcessEndTime - currFrame->opticalFlowRoIProcessStartTime);
}

void RoIExtractor::mergeRoIs(std::vector<RoI>& childRoIs, std::vector<RoI>& parentRoIs, int maxSize) const {
  // Match roi <=> origRoI ID before merge
  for (RoI& pRoI : parentRoIs) {
    for (RoI& cRoI : childRoIs) {
      if (cRoI.id == pRoI.id) {
        pRoI.childRoIs.push_back(&cRoI);
        cRoI.parentRoI = &pRoI;
      }
    }
  }
  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < parentRoIs.size(); i++) {
      for (j = i + 1; j < parentRoIs.size(); j++) {
        const RoI& roi0 = parentRoIs[i];
        const RoI& roi1 = parentRoIs[j];
        int intersection = roi0.location.intersection(roi1.location);
        if ((float) intersection / (float) roi0.getArea() < mConfig.MERGE_THRESHOLD &&
            (float) intersection / (float) roi1.getArea() < mConfig.MERGE_THRESHOLD) {
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
    parentRoIs.push_back(RoI::mergeRoIs(parentRoIs[i], parentRoIs[j]));
    // Match child parent
    RoI* mergedRoI = &(*parentRoIs.rbegin());
    mergedRoI->childRoIs.insert(mergedRoI->childRoIs.end(),
                                parentRoIs[i].childRoIs.begin(), parentRoIs[i].childRoIs.end());
    mergedRoI->childRoIs.insert(mergedRoI->childRoIs.end(),
                                parentRoIs[j].childRoIs.begin(), parentRoIs[j].childRoIs.end());
    std::stringstream ss;
    for (RoI* cRoI : mergedRoI->childRoIs) {
      cRoI->parentRoI = mergedRoI;
      ss << cRoI->id << ", ";
    }
    LOGD("MergedRoI Info: targetSize = %d, childRoIs = %s", mergedRoI->targetSize, ss.str().c_str());
    assert(j > i);
    parentRoIs.erase(parentRoIs.begin() + j);
    parentRoIs.erase(parentRoIs.begin() + i);
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
      const BoundingBox& box = boundingBoxes[boxIndex];
      const Rect& loc = box.location;
      int newLeft = std::max(0, loc.left + shift.first);
      int newTop = std::max(0, loc.top + shift.second);
      int newRight = std::min(width, loc.right + shift.first);
      int newBottom = std::min(height, loc.bottom + shift.second);
      if (newLeft < newRight && newTop < newBottom) {
        opticalFlowRoIs.emplace_back(box.srcRoI, box.id, currFrame, Rect(newLeft, newTop, newRight, newBottom),
                                     RoI::Type::OF, box.label, shift, err, 0);
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
        nullptr,
        RoI::getNewIds(1).first,
        currFrame,
        boxAndFeature.first,
        RoI::PD,
        -1,
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
