#include "strm/RoIExtractor.hpp"

#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"
#include "strm/Test.hpp"

namespace rm {

const cv::TermCriteria RoIExtractor::CRITERIA = cv::TermCriteria(
    cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);

RoIExtractor::RoIExtractor(const RoIExtractorConfig& config, bool run, bool allowInterpolation,
                           bool roiWiseInference, const PatchMixer* patchMixer,
                           RoIResizer* roiResizer,
                           std::vector<InferenceInfo> inferencePlan)
    : mConfig(config), mPatchMixer(patchMixer), mRoIResizer(roiResizer),
      mInferencePlan(std::move(inferencePlan)),
      mRoICount(mRoIWiseInference ? int(inferencePlan.size()) : 0),
      mRoIWiseInference(roiWiseInference), mTargetSize(
        cv::Size(int(mConfig.EXTRACTION_RESIZE_WIDTH), int(mConfig.EXTRACTION_RESIZE_HEIGHT))),
      mAllowInterpolation(allowInterpolation), mbStop(false), isFullyPacked(false) {
  if (run) {
    if (!mAllowInterpolation) {
      std::lock_guard<std::mutex> queueLock(mtx);
      resetPack();
    }
    mThreads.reserve(config.NUM_WORKERS);
    for (int extractorId = 0; extractorId < config.NUM_WORKERS; extractorId++) {
      mThreads.push_back(std::thread([this, extractorId]() { work(extractorId); }));
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
  std::unique_lock<std::mutex> queueLock(mtx);
  cv.wait(queueLock, [this]() { return mPDWaiting.size() < mConfig.MAX_QUEUE_SIZE; });
  mPDWaiting.insert(frame);
  LOGD("%-25s                 for video %-5d frame %-4d // %4lu PD | %4lu OF | %4lu Processed",
       "RoIExtractor::enqueue", frame->vid, frame->frameIndex,
       mPDWaiting.size(), mOFWaiting.size(), mExtractionFinished.size());
  queueLock.unlock();
  cv.notify_all();
}

void RoIExtractor::notify() {
  cv.notify_all();
}

MultiStream RoIExtractor::getExtractedFrames(std::vector<InferenceInfo>& inferencePlan) {
  mInferencePlan = inferencePlan;
  MultiStream extractedFrames;
  std::unique_lock<std::mutex> queueLock(mtx);
  cv.wait(queueLock, [this]() { return mAllowInterpolation || isFullyPacked; });
  std::for_each(mExtractionFinished.begin(), mExtractionFinished.end(),
                [](Frame* frame) { frame->useInferenceResultForOF = true; });
  std::for_each(mOFProcessing.begin(), mOFProcessing.end(),
                [](Frame* frame) { frame->extractOFAgain = true; });
  for (Frame* frame : mExtractionFinished) {
    extractedFrames[frame->vid].insert(frame);
  }
  mExtractionFinished.clear();
  resetPack();
  queueLock.unlock();
  cv.notify_all();
  return extractedFrames;
}

void RoIExtractor::reEnqueueFrames(const Stream& droppedFrames) {
  assert(mExtractionFinished.empty());
  std::unique_lock<std::mutex> queueLock(mtx);
  size_t prevNumOFJobs = mOFWaiting.size();
  std::for_each(droppedFrames.begin(), droppedFrames.end(),
                [](Frame* frame) { resetOFRoIExtraction(frame); });
  mOFWaiting.insert(droppedFrames.begin(), droppedFrames.end());
  LOGD("%-25s                                        // %4lu PD | %4lu => %4lu OF | %4lu Processed",
       "RoIExtractor::reEnqueueFrames", mPDWaiting.size(), prevNumOFJobs, mOFWaiting.size(),
       mExtractionFinished.size());
  queueLock.unlock();
  cv.notify_all();
}

void RoIExtractor::resetPack() {
  isFullyPacked = false;
  std::lock_guard<std::mutex> packLock(packMtx);
  if (mRoIWiseInference) {
    mRoICount = int(mInferencePlan.size());
  } else {
    for (auto& info : mInferencePlan) {
      mFreeRectsList.push_back(
          {info.device, info.size, {Rect(0, 0, float(info.size), float(info.size))}});
    }
  }
}

void RoIExtractor::resetOFRoIExtraction(Frame* frame) {
  frame->childRoIs.erase(std::remove_if(
      frame->childRoIs.begin(), frame->childRoIs.end(),
      [](const auto& cRoI) { return cRoI->type == OF; }), frame->childRoIs.end());
  std::for_each(frame->childRoIs.begin(), frame->childRoIs.end(), [](auto& cRoI) {
    if (cRoI->type == PD) { cRoI->id = UNASSIGNED_ID; }
  });
  frame->useInferenceResultForOF = false;
  frame->extractOFAgain = false;
  frame->isRoIsReady = false;
}

void RoIExtractor::work(int extractorId) {
  /*
   *    Frame Status           Containing data structure   extractOFAgain
   * 1. Before PD extraction | mPDWaiting                | false
   * 2. Extracting PD        | -                         | false
   * 3. Before OF extraction | mOFWaiting                | false
   * 4. Extracting OF        | mOFProcessing             | false
   * 5. OF extraction ended  | mExtractionFinished       | true
   */

  auto getPDJob = [this]() {
    if (!mPDWaiting.empty() && mOFWaiting.size() < mConfig.MAX_QUEUE_SIZE) {
      return *mPDWaiting.begin();
    } else {
      return (Frame*) nullptr;
    }
  };
  auto getOFJob = [this]() {
    if (!isFullyPacked && !mOFWaiting.empty()
        && (*mOFWaiting.begin())->readyForOFExtraction()) {
      return *mOFWaiting.begin();
    } else {
      return (Frame*) nullptr;
    }
  };

  while (true) {
    bool isOF = false;
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
      frame->OFExtractorID = extractorId;
      mOFWaiting.erase(mOFWaiting.begin());
      mOFProcessing.insert(frame);
    } else {
      frame->PDExtractorID = extractorId;
      mPDWaiting.erase(mPDWaiting.begin());
    }
    lock.unlock();
    cv.notify_all();

    if (isOF) {
      processOF(frame);
    } else {
      processPD(frame);
    }

    if (isOF) {
      frame->filterPDRoIs(mConfig.PD_FILTER_THRESHOLD);
      testAssignedUniqueRoIID(frame->childRoIs);
      frame->resizeStartTime = NowMicros();
      frame->resizeRoIs(mRoIResizer);
      frame->resizeEndTime = NowMicros();
      frame->mergeRoIStartTime = NowMicros();
      if (mConfig.MERGE) {
        frame->resetParentRoIs();
        frame->mergeRoIs(mConfig.MERGE_THRESHOLD, float(std::min_element(
            mInferencePlan.begin(), mInferencePlan.end(),
            [](const InferenceInfo& l, const InferenceInfo& r) {
              return l.size < r.size;
            })->size));
        testAssignedUniqueRoIID(frame->childRoIs);
        testParentChildrenIDsAndChildIDsSame(frame->childRoIs, frame->parentRoIs);
        testChildRoIsFrameRelation(frame->childRoIs);
        testParentRoIsFrameRelation(frame->parentRoIs);
      }
      frame->mergeRoIEndTime = NowMicros();
      bool isAllPacked = true;
      if (!mAllowInterpolation) {
        std::lock_guard<std::mutex> packLock(packMtx);
        if (mRoIWiseInference) {
          mRoICount -= int(frame->parentRoIs.size());
          isAllPacked = mRoICount <= 0;
        } else {
          auto& config = mPatchMixer->mConfig;
          for (auto& pRoI : frame->parentRoIs) {
            isAllPacked = mPatchMixer->tryPackRoI(pRoI->getResizedWidthHeight(), mFreeRectsList,
                                                  config.EMULATED_BATCH);
            if (!isAllPacked) {
              break;
            }
          }
        }
      }
      lock.lock();
      mOFProcessing.erase(frame);
      if (frame->extractOFAgain) {
        resetOFRoIExtraction(frame);
        mOFWaiting.insert(frame);
      } else {
        frame->isRoIsReady = true;
        mExtractionFinished.insert(frame);
      }
      if (!isAllPacked && mExtractionFinished.size() >= 2) {
        isFullyPacked = true;
      }
      if (!mAllowInterpolation &&
          mPDWaiting.empty() &&
          mOFWaiting.empty() &&
          mOFProcessing.empty()) {
        isFullyPacked = true;
      }
      lock.unlock();
    } else {
      lock.lock();
      mOFWaiting.insert(frame);
      lock.unlock();
    }
    cv.notify_all();
  }
}

void RoIExtractor::processPD(Frame* currFrame) {
  currFrame->pixelDiffRoIProcessStartTime = NowMicros();
  getPixelDiffRoIs(currFrame, mTargetSize, mConfig.MIN_ROI_AREA, currFrame->childRoIs);
  currFrame->pixelDiffRoIProcessEndTime = NowMicros();
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu PD RoIs",
       "RoIExtractor::processPD",
       currFrame->pixelDiffRoIProcessEndTime - currFrame->pixelDiffRoIProcessStartTime,
       currFrame->vid, currFrame->frameIndex, currFrame->childRoIs.size());
}

void RoIExtractor::processOF(Frame* currFrame) {
  assert(!currFrame->extractOFAgain);
  Frame* prevFrame = currFrame->prevFrame;
  std::vector<BoundingBox> reliablePrevBoxes;
  if (prevFrame->useInferenceResultForOF) {
    testAssignedUniqueBoxID(prevFrame->boxes);
    for (const std::unique_ptr<BoundingBox>& box : prevFrame->boxes) {
      if (box->confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        BoundingBox reliableBox(box->id, Rect(
            std::max(0.0f, box->location.left),
            std::max(0.0f, box->location.top),
            std::min(float(currFrame->width), box->location.right),
            std::min(float(currFrame->height), box->location.bottom)),
                                box->confidence, box->label, origin_BB);
        reliableBox.srcRoI = box->srcRoI;
        reliablePrevBoxes.push_back(reliableBox);
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
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu OF RoIs", "RoIExtractor::processOF",
       currFrame->opticalFlowRoIProcessEndTime - currFrame->opticalFlowRoIProcessStartTime,
       currFrame->vid, currFrame->frameIndex,
       std::count_if(currFrame->childRoIs.begin(), currFrame->childRoIs.end(),
                     [](auto& cRoI) { return cRoI->type == OF; }));
}

void RoIExtractor::getOpticalFlowRoIs(const Frame* prevFrame, Frame* currFrame,
                                      const std::vector<BoundingBox>& boundingBoxes,
                                      const cv::Size& targetSize,
                                      std::vector<std::unique_ptr<RoI>>& outChildRoIs) const {
  auto width = float(currFrame->mat.cols);
  auto height = float(currFrame->mat.rows);

  std::vector<Rect> boundingRects;
  boundingRects.reserve(boundingBoxes.size());
  for (const auto& bbx : boundingBoxes) {
    boundingRects.push_back(Rect(bbx.location));
  }

  if (!boundingBoxes.empty()) {
    const std::vector<OFFeatures>& ofFeatures = opticalFlowTracking(
        prevFrame, currFrame, boundingRects, targetSize);
    assert(ofFeatures.size() == boundingBoxes.size());
    for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
      const BoundingBox& box = boundingBoxes[boxIndex];
      const Rect& loc = box.location;
      const OFFeatures& of = ofFeatures[boxIndex];
      float x = of.shiftAvg.first;
      float y = of.shiftAvg.second;
      float newLeft = std::max(0.0f, loc.left + x);
      float newTop = std::max(0.0f, loc.top + y);
      float newRight = std::min(float(width), loc.right + x);
      float newBottom = std::min(float(height), loc.bottom + y);
      if (newLeft < newRight && newTop < newBottom) {
        outChildRoIs.push_back(std::make_unique<RoI>(
            box.srcRoI, box.id, currFrame, Rect(newLeft, newTop, newRight, newBottom),
            OF, box.origin, box.label, of, box.confidence, mConfig.ROI_PADDING, false));
      }
    }
  }
}

std::vector<OFFeatures> RoIExtractor::opticalFlowTracking(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->preProcessedMat.empty());
  assert(!currFrame->preProcessedMat.empty());
  assert(prevFrame->preProcessedMat.channels() == currFrame->preProcessedMat.channels());

  const cv::Mat& prevImage = prevFrame->preProcessedMat;
  const cv::Mat& currImage = currFrame->preProcessedMat;

  std::vector<int> startEndIndices = {0};
  std::vector<cv::Point2f> inputPoints;
  for (const Rect& bbx: boundingBoxes) {
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
    cv::Rect roiBbx = cv::Rect(int(x), int(y), int(w), int(h));
    cv::goodFeaturesToTrack(prevImage(roiBbx), points, 50, 0.01, 5, cv::Mat(), 3, false, 0.03);
    for (cv::Point2f& p: points) {
      p.x += x;
      p.y += y;
    }
    if (points.empty()) {
      startEndIndices.push_back(startEndIndices.back() + 1);
      inputPoints.push_back(cv::Point2f(
          ((float) bbx.left + (float) bbx.width() / 2) * xRatio,
          ((float) bbx.top + (float) bbx.height() / 2) * yRatio));
    } else {
      startEndIndices.push_back(startEndIndices.back() + int(points.size()));
      inputPoints.insert(inputPoints.end(), points.begin(), points.end());
    }
  }
  assert(startEndIndices.back() == inputPoints.size());

  std::vector<uchar> status;
  std::vector<float> errs;
  std::vector<cv::Point2f> outputPoints;
  cv::calcOpticalFlowPyrLK(prevImage, currImage, inputPoints, outputPoints, status, errs,
                           cv::Size(15, 15), 2, CRITERIA);
  assert(inputPoints.size() == outputPoints.size());
  assert(inputPoints.size() == status.size());
  assert(inputPoints.size() == errs.size());

  std::vector<OFFeatures> ofFeatures;
  for (int i = 0; i < startEndIndices.size() - 1; i++) {
    int startIndex = startEndIndices[i];
    int endIndex = startEndIndices[i + 1];
    std::vector<std::pair<float, float>> boxShifts;
    std::vector<float> boxErrs;
    std::vector<uchar> boxStatusVec;
    for (int j = startIndex; j < endIndex; j++) {
      float x = outputPoints[j].x - inputPoints[j].x;
      float y = outputPoints[j].y - inputPoints[j].y;
      boxShifts.push_back({x * (float) currFrame->width / (float) targetSize.width,
                           y * (float) currFrame->height / (float) targetSize.height});
      boxErrs.push_back(errs[j]);
      boxStatusVec.push_back(status[j]);
    }
    ofFeatures.push_back(OFFeatures(boxShifts, boxErrs, boxStatusVec));
  }
  return ofFeatures;
}

void RoIExtractor::getPixelDiffRoIs(Frame* currFrame, const cv::Size& targetSize,
                                    const float minRoIArea,
                                    std::vector<std::unique_ptr<RoI>>& outChildRoIs) const {

  // Find {PD_INTERVAL}th previous frame. If not available, use farthest frame.
  Frame *prevFrame = currFrame;
  for (int i=0; i<mConfig.PD_INTERVAL; ++i) {
    assert(prevFrame!= nullptr);
    if (prevFrame->prevFrame == nullptr) {
      break;
    }
    prevFrame = prevFrame->prevFrame;
  }

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
    if (box.area() >= minRoIArea) {
      boxes.push_back(Rect(
          box.x * float(currFrame->mat.cols) / float(targetSize.width),
          box.y * float(currFrame->mat.rows) / float(targetSize.height),
          (box.x + box.width) * float(currFrame->mat.cols) / float(targetSize.width),
          (box.y + box.height) * float(currFrame->mat.rows) / float(targetSize.height)));
    }
  }

  for (const Rect& box : boxes) {
    outChildRoIs.push_back(std::make_unique<RoI>(
        nullptr,
        UNASSIGNED_ID,
        currFrame,
        box,
        PD,
        origin_PD,
        -1,
        OFFeatures({}, {}, {}),
        RoI::INVALID_CONF,
        mConfig.ROI_PADDING,
        false));
  }
}

cv::Mat RoIExtractor::calculateDiffAndThreshold(
    const cv::Mat& prevMat, const cv::Mat& currMat) {
  cv::Mat diff;
  cv::absdiff(prevMat, currMat, diff);
  cv::dilate(diff, diff,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(0, 0),
             2);
  cv::threshold(diff, diff, 35, 255, cv::THRESH_BINARY);
  return diff;
}

void RoIExtractor::cannyEdgeDetection(cv::Mat mat) {
  cv::Canny(mat, mat, 120, 255, 3, false);
  cv::dilate(mat, mat,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(0, 0),
             1);
}

} // namespace rm
