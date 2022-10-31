#ifndef SPATIO_TEMPORAL_ROI_MIXER_HPP_
#define SPATIO_TEMPORAL_ROI_MIXER_HPP_

#include <jni.h>

#include <thread>

#include "opencv2/core/mat.hpp"

#include "strm/Config.hpp"
#include "strm/Frame.hpp"

namespace rm {

class FrameBuffer;
class InferenceEngine;
class Logger;
class MixedFrame;
class RoIExtractor;
class RoIResizer;
class PatchMixer;
class PatchReconstructor;

using FrameResult = std::pair<time_us, std::vector<BoundingBox>>;

class SpatioTemporalRoIMixer {
 public:
  SpatioTemporalRoIMixer(const STRMConfig& config,
                         std::map<int, int> startIndices,
                         JavaVM* vm, JNIEnv* env, jobject strm);

  ~SpatioTemporalRoIMixer();

  int enqueueImage(const int vid, const cv::Mat& mat);

 private:
  void waitForStart();

  void work();

  void outputWork();

  void preprocess(Frame* frame) const;

  void fullFrameInference(Frame* frame);

  int getFullFrameSize(const std::map<Device, std::map<int, time_us>>& latencyTable);

  void mixedInference(std::vector<MixedFrame>& mixedFrames);

  void roiWiseInference(std::vector<MixedFrame>& mixedFrames);

  void releaseFrames(const MultiStream& frames);

  void log(const Frame* frame);

  static const int FULL_KEY_OFFSET;
  static const bool FAIR;

  const STRMConfig mConfig;
  const time_us mScheduleInterval;
  std::thread mThread;
  bool mbStop;

  std::thread mResultThread;
  std::unique_ptr<Logger> mResultLogger;
  std::unique_ptr<Logger> mExecutionLogger;
  std::unique_ptr<Logger> mRoILogger;
  const cv::Size mTargetSize;
  const std::vector<int> mInputSizes;

  std::unique_ptr<RoIExtractor> mRoIExtractor;
  std::unique_ptr<RoIResizer> mRoIResizer;
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

} // namespace rm

#endif // SPATIO_TEMPORAL_ROI_MIXER_HPP_
