#ifndef MONDRIAN_HPP_
#define MONDRIAN_HPP_

#include <jni.h>

#include <thread>

#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/Frame.hpp"

namespace md {

class FrameBuffer;
class InferenceEngine;
class Logger;
class MixedFrame;
class ROIExtractor;
class ROIResizer;
class PatchReconstructor;

using FrameResult = std::pair<time_us, std::vector<BoundingBox>>;

class Mondrian {
 public:
  Mondrian(const MondrianConfig& config, std::map<int, int> startIndices,
           JNIEnv* env, jobject app);

  ~Mondrian();

  int enqueueImage(const int vid, const cv::Mat& yuvMat);

 private:
  void waitForStart();

  void work();

  void outputWork();

  void preprocess(Frame* frame) const;

  void handleFullFrameInferenceResults(Frame* frame);

  void handleMixedFrameInferenceResults(std::vector<MixedFrame>& mixedFrames);

  void handleROIWiseInferenceResults(std::vector<MixedFrame>& mixedFrames);

  void releaseFrames(const MultiStream& frames);

  void log(const Frame* frame);

  static const bool FAIR;

  const MondrianConfig mConfig;
  const time_us mScheduleInterval;
  std::thread mThread;
  bool mbStop;

  std::thread mResultThread;
  std::unique_ptr<Logger> mResultLogger;
  std::unique_ptr<Logger> mExecutionLogger;
  std::unique_ptr<Logger> mROILogger;
  const cv::Size mTargetSize;
  const std::vector<int> mInputSizes;

  std::unique_ptr<ROIExtractor> mROIExtractor;
  std::unique_ptr<ROIResizer> mROIResizer;
  std::unique_ptr<InferenceEngine> mInferenceEngine;
  std::unique_ptr<PatchReconstructor> mPatchReconstructor;

  int mPrevEnqueuedVid = -1;
  std::mutex mFairEnqueueMtx;
  std::condition_variable mFairCv;

  int mNumStartedFrameBuffers = 0;
  std::mutex mStartMtx;
  std::condition_variable mStartCv;
  std::condition_variable mEnqueueCv;
  bool mbStartEnqueue = false;

  std::mutex mFrameBuffersMtx;
  std::map<int, std::unique_ptr<FrameBuffer>> mFrameBuffers;
  std::map<int, int> mStartIndices;

  std::mutex mResultsMtx;
  std::condition_variable mResultsCv;
  std::map<int, std::map<int, FrameResult>> mResults;
  std::map<int, int> mResultIndices;
};

} // namespace md

#endif // MONDRIAN_HPP_
