#include "strm/RoIExtractor.hpp"

#include <numeric>
#include <set>

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"
#include "strm/Test.hpp"

namespace rm {

const cv::TermCriteria RoIExtractor::CRITERIA = cv::TermCriteria(
    cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);

RoIExtractor::RoIExtractor(const RoIExtractorConfig& config, bool run)
    : mConfig(config),
      mTargetSize(cv::Size(mConfig.EXTRACTION_RESIZE_WIDTH, mConfig.EXTRACTION_RESIZE_HEIGHT)),
      mbStop(false) {
  if (run) {
    mThreads.reserve(config.NUM_WORKERS);
    for (int i = 0; i < config.NUM_WORKERS; i++) {
      mThreads.emplace_back([this]() { work(); });
    }
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
  int numPDs = (int) mFramesForPD.size();
  int numOFs = std::accumulate(mFramesForOF.begin(), mFramesForOF.end(), 0,
                               [](int cnt, auto& it) { return cnt + it.second.size(); });
  int numProcessed = std::accumulate(mOFProcessingStartedFrames.begin(),
                                     mOFProcessingStartedFrames.end(),
                                     0, [](int cnt, auto& it) { return cnt + it.second.size(); });
  lock.unlock();
  cv.notify_all();
  LOGD("%-25s                for video %-5s frame %-4d // %4d PD | %4d OF | %4d Processed",
       "RoIExtractor::enqueue", frame->shortKey.c_str(), frame->frameIndex,
       numPDs, numOFs, numProcessed);
}

void RoIExtractor::notify() {
  cv.notify_all();
}

std::map<std::string, SortedFrames> RoIExtractor::getExtractedFrames() {
  std::unique_lock<std::mutex> lock(mtx);
  std::map<std::string, SortedFrames> extractedFrames = std::move(mOFProcessingStartedFrames);
  mOFProcessingStartedFrames.clear();
  // TODO: set useInferenceResultForOF = true only for lastFrames
  for (const auto&[key, aStreamFrames] : extractedFrames) {
    for (auto* frame : aStreamFrames) {
      frame->useInferenceResultForOF = true;
    }
  }
  cv.wait(lock, [&extractedFrames]() {
    return std::all_of(extractedFrames.begin(), extractedFrames.end(), [](auto& it) {
      return it.second.empty() || (*it.second.rbegin())->roiExtractionStatus == OF_EXTRACTED;
    });
  });
  lock.unlock();
  cv.notify_all();
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

  auto getPDJob = [this]() {
    if (mFramesForPD.empty()) {
      return (Frame*) nullptr;
    } else {
      return mFramesForPD.front();
    }
  };
  auto getOFJob = [this]() {
    for (auto&[key, aStreamFrames] : mFramesForOF) {
      if (!aStreamFrames.empty() && aStreamFrames.front()->readyForOFExtraction()) {
        return aStreamFrames.front();
      }
    }
    return (Frame*) nullptr;
  };

  while (true) {
    bool isOF;
    Frame* frame = nullptr;

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this, &isOF, &frame, &getPDJob, &getOFJob]() {
      if (mbStop) {
        return true;
      }
      frame = getOFJob();
      if (frame != nullptr) {
        isOF = true;
        return true;
      }
      frame = getPDJob();
      if (frame != nullptr) {
        isOF = false;
        return true;
      }
      return false;
    });

    if (mbStop) {
      lock.unlock();
      cv.notify_all();
      return;
    }

    if (isOF) {
      frame->roiExtractionStatus = OF_EXTRACTING;
      mOFProcessingStartedFrames[frame->key].insert(frame);
      mFramesForOF[frame->key].pop_front();
    } else {
      mFramesForPD.pop_front();
    }
    lock.unlock();

    if (isOF) {
      processOF(frame);
    } else {
      processPD(frame);
    }

    if (isOF) {
      frame->filterPDRoIs(mConfig.PD_FILTER_THRESHOLD);
      testAssignedUniqueRoIID(frame->childRoIs);
      frame->prevFrame->preProcessedMat.release();
      frame->roiExtractionStatus = OF_EXTRACTED;
      frame->isRoIsReady = true;
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
  getPixelDiffRoIs(prevFrame, currFrame, mTargetSize, mConfig.MIN_ROI_AREA, currFrame->childRoIs);
  currFrame->pixelDiffRoIProcessEndTime = NowMicros();
  LOGD("%-25s took %-6lld us for video %-5s frame %-4d // %4lu PD RoIs",
       "RoIExtractor::processPD",
       currFrame->pixelDiffRoIProcessEndTime - currFrame->pixelDiffRoIProcessStartTime,
       currFrame->shortKey.c_str(), currFrame->frameIndex, currFrame->childRoIs.size());
}

void RoIExtractor::processOF(Frame* currFrame) {
  assert(currFrame->roiExtractionStatus == OF_EXTRACTING);
  Frame* prevFrame = currFrame->prevFrame;
  std::vector<BoundingBox> reliablePrevBoxes;
  if (prevFrame->useInferenceResultForOF) {
    testAssignedUniqueBoxID(prevFrame->boxes);
    for (const std::unique_ptr<BoundingBox>& box : prevFrame->boxes) {
      if (box->confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        reliablePrevBoxes.emplace_back(box->id, Rect(
            std::max(0.0f, box->location.left),
            std::max(0.0f, box->location.top),
            std::min(float(currFrame->width), box->location.right),
            std::min(float(currFrame->height), box->location.bottom)),
                                       box->confidence, box->label, fromBB);
      }
    }
  } else {
    for (auto& cRoI : currFrame->prevFrame->childRoIs) {
      BoundingBox reliableBox(cRoI->id, cRoI->origLoc, 1, cRoI->label, cRoI->origin);
      reliableBox.srcRoI = cRoI.get();
      reliablePrevBoxes.push_back(reliableBox);
    }
  }
  currFrame->opticalFlowRoIProcessStartTime = NowMicros();
  getOpticalFlowRoIs(prevFrame, currFrame, reliablePrevBoxes, mTargetSize, currFrame->childRoIs);
  currFrame->opticalFlowRoIProcessEndTime = NowMicros();
  LOGD("%-25s took %-6lld us for video %-5s frame %-4d // %4lu OF RoIs", "RoIExtractor::processOF",
       currFrame->opticalFlowRoIProcessEndTime - currFrame->opticalFlowRoIProcessStartTime,
       currFrame->shortKey.c_str(), currFrame->frameIndex,
       std::count_if(currFrame->childRoIs.begin(), currFrame->childRoIs.end(),
                     [](auto& cRoI) { return cRoI->type == RoI::Type::OF; }));
}

void RoIExtractor::getOpticalFlowRoIs(const Frame* prevFrame, Frame* currFrame,
                                      const std::vector<BoundingBox>& boundingBoxes,
                                      const cv::Size& targetSize,
                                      std::vector<std::unique_ptr<RoI>>& outChildRoIs) const {
  float width = float(currFrame->mat.cols);
  float height = float(currFrame->mat.rows);

  std::vector<Rect> boundingRects;
  boundingRects.reserve(boundingBoxes.size());
  for (const auto& bbx : boundingBoxes) {
    boundingRects.emplace_back(bbx.location);
  }

  if (!boundingBoxes.empty()) {
    const std::vector<RoI::OFFeatures>& ofFeatures = opticalFlowTracking(
        prevFrame, currFrame, boundingRects, targetSize);
    assert(ofFeatures.size() == boundingBoxes.size());
    for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
      const BoundingBox& box = boundingBoxes[boxIndex];
      const Rect& loc = box.location;
      const RoI::OFFeatures& of = ofFeatures[boxIndex];
      float x = of.avgShift.first;
      float y = of.avgShift.second;
      float newLeft = std::max(0.0f, loc.left + x);
      float newTop = std::max(0.0f, loc.top + y);
      float newRight = std::min(float(width), loc.right + x);
      float newBottom = std::min(float(height), loc.bottom + y);
      if (newLeft < newRight && newTop < newBottom) {
        outChildRoIs.emplace_back(
            new RoI(box.srcRoI, box.id, currFrame, Rect(newLeft, newTop, newRight, newBottom),
                    RoI::Type::OF, box.origin, box.label, of, mConfig.ROI_PADDING, false));
      }
    }
  }
}

std::vector<RoI::OFFeatures> RoIExtractor::opticalFlowTracking(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->preProcessedMat.empty());
  assert(!currFrame->preProcessedMat.empty());
  assert(prevFrame->preProcessedMat.channels() == currFrame->preProcessedMat.channels());

  const cv::Mat& prevImage = prevFrame->preProcessedMat;
  const cv::Mat& currImage = currFrame->preProcessedMat;

  std::vector<int> numPoints;
  std::vector<cv::Point2f> inputPoints;
  for (const Rect& bbx : boundingBoxes) {
    float xRatio = (float) targetSize.width / (float) prevFrame->width;
    float yRatio = (float) targetSize.height / (float) prevFrame->height;
    float x = std::min(bbx.left, bbx.right) * xRatio;
    float y = std::min(bbx.top, bbx.bottom) * yRatio;
    float w = std::abs(bbx.right - bbx.left) * xRatio;
    float h = std::abs(bbx.bottom - bbx.top) * yRatio;
    x = std::min(std::max(0.0f, x), float(prevImage.cols));
    y = std::min(std::max(0.0f, y), float(prevImage.rows));
    w = std::min(std::max(0.0f, w), float(prevImage.cols) - x);
    h = std::min(std::max(0.0f, h), float(prevImage.rows) - y);

    std::vector<cv::Point2f> points;
    cv::Rect2f roiBbx = cv::Rect2f(x, y, w, h);
    cv::goodFeaturesToTrack(prevImage(roiBbx), points, 100, 0.01, 5, cv::Mat(), 3, false, 0.03);
    for (cv::Point2f& p : points) {
      p.x += roiBbx.x;
      p.y += roiBbx.y;
    }
    if (points.empty()) {
      inputPoints.emplace_back(((float) bbx.left + (float) bbx.width() / 2) * xRatio,
                               ((float) bbx.top + (float) bbx.height() / 2) * yRatio);
      numPoints.push_back(1);
    } else {
      inputPoints.insert(inputPoints.end(), points.begin(), points.end());
      numPoints.push_back((int) points.size());
    }
  }

  std::vector<uchar> status;
  std::vector<float> errs;
  std::vector<cv::Point2f> outputPoints;
  cv::calcOpticalFlowPyrLK(prevImage, currImage, inputPoints, outputPoints, status, errs,
                           cv::Size(15, 15), 2, CRITERIA);
  assert(inputPoints.size() == outputPoints.size());
  assert(inputPoints.size() == status.size());
  assert(inputPoints.size() == errs.size());

  std::vector<RoI::OFFeatures> ofFeatures;
  int startIndex = 0;
  int endIndex;
  for (int numPoint : numPoints) {
    endIndex = startIndex + numPoint;
    std::vector<std::pair<float, float>> boxShifts;
    std::vector<float> boxErrs;
    for (int i = startIndex; i < endIndex; i++) {
      if (status[i] == 1) {
        float x = outputPoints[i].x - inputPoints[i].x;
        float y = outputPoints[i].y - inputPoints[i].y;
        boxShifts.emplace_back(
            x * (float) currFrame->width / (float) targetSize.width,
            y * (float) currFrame->height / (float) targetSize.height);
        boxErrs.push_back(errs[i]);
      } else {
        boxShifts.emplace_back(0, 0);
        boxErrs.push_back(0);
      }
    }
    ofFeatures.emplace_back(boxShifts, boxErrs);
    startIndex = endIndex;
  }
  return ofFeatures;
}

void RoIExtractor::getPixelDiffRoIs(const Frame* prevFrame, Frame* currFrame,
                                    const cv::Size& targetSize, const float mixRoIArea,
                                    std::vector<std::unique_ptr<RoI>>& outChildRoIs) const {
  assert(!prevFrame->preProcessedMat.empty());
  assert(!currFrame->preProcessedMat.empty());
  assert(prevFrame->preProcessedMat.channels() == currFrame->preProcessedMat.channels());
  cv::Mat mat = calculateDiffAndThreshold(prevFrame->preProcessedMat, currFrame->preProcessedMat);
  cannyEdgeDetection(mat);

  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;

  cv::findContours(mat, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // replaces get boxes from contours.
  std::vector<Rect> boxes;
  for (const std::vector<cv::Point>& contour : contours) {
    double approxDistance = cv::arcLength(contour, true) * 0.02;
    std::vector<cv::Point> approxCurve;
    cv::approxPolyDP(contour, approxCurve, approxDistance, true);
    cv::Rect2f box = cv::boundingRect(approxCurve);
    if (box.area() >= mixRoIArea) {
      boxes.emplace_back(box.x * float(currFrame->mat.cols) / float(targetSize.width),
                         box.y * float(currFrame->mat.rows) / float(targetSize.height),
                         (box.x + box.width) * float(currFrame->mat.cols) / float(targetSize.width),
                         (box.y + box.height) * float(currFrame->mat.rows) / float(targetSize.height));
    }
  }

  for (const Rect& box : boxes) {
    outChildRoIs.emplace_back(new RoI(
        nullptr,
        UNASSIGNED_ID,
        currFrame,
        box,
        RoI::PD,
        fromPD,
        -1,
        RoI::OFFeatures({}, {}),
        mConfig.ROI_PADDING,
        false));
  }
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
