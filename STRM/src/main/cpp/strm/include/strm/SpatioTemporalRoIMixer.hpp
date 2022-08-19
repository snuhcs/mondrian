#ifndef SPATIO_TEMPORAL_ROI_MIXER_HPP_
#define SPATIO_TEMPORAL_ROI_MIXER_HPP_

#include <jni.h>

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/RoIExtractor.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/PatchReconstructor.hpp"
#include "strm/Utils.hpp"
#include "strm/Logger.hpp"

namespace rm {

using FrameResult = std::tuple<time_us, cv::Mat, std::vector<BoundingBox>>;

class SpatioTemporalRoIMixer {
 public:
  SpatioTemporalRoIMixer(const STRMConfig& config,
                         InferenceEngine* inferenceEngine,
                         int numSourceVideos,
                         JavaVM* vm, JNIEnv* env, jobject strm, bool draw);

  ~SpatioTemporalRoIMixer();

  int enqueueImage(const std::string& key, const cv::Mat& mat);

 private:
  void waitForStart();

  void work();

  void outputWork();

  void preprocess(Frame* frame) const;

  void fullFrameInference(Frame* frame);

  void mixedInference(std::vector<MixedFrame>& mixedFrames);

  void roiWiseInference(std::vector<MixedFrame>& mixedFrames);

  void releaseFrames(const MultiStream& frames);

  void drawObjectDetectionResult(const cv::Mat& mat, const std::vector<BoundingBox>& boxes);

  static std::vector<InferenceInfo> getInferencePlan(
      time_us remainingTime, const std::map<Device, std::map<int, time_us>>& inferenceTimes);

  static void testNoInterpolationPacking(const MultiStream& frames, const Stream& droppedFrames,
                                         Frame* fullFrameTarget);

  const STRMConfig mConfig;
  const time_us mScheduleInterval;
  std::thread mThread;
  bool mbStop;

  std::thread mResultThread;
  std::unique_ptr<Logger> mResultLogger;
  std::unique_ptr<Logger> mLogger;
  std::unique_ptr<Logger> mRoILogger;
  InferenceEngine* mInferenceEngine;
  const cv::Size mTargetSize;
  const std::vector<int> mInputSizes;
  const int mInferenceFrameSize;

  std::unique_ptr<RoIExtractor> mRoIExtractor;
  std::unique_ptr<RoIResizer> mRoIResizer;
  std::unique_ptr<PatchMixer> mPatchMixer;
  std::unique_ptr<PatchReconstructor> mPatchReconstructor;

  int mNumSourceVideos;
  int mNumStartedFrameBuffers = 0;
  std::mutex mStartMtx;
  std::condition_variable mStartCv;
  std::condition_variable mEnqueueCv;
  bool mbStartEnqueue = false;

  std::mutex mFrameBuffersMtx;
  std::map<std::string, std::unique_ptr<FrameBuffer>> mFrameBuffers;

  std::mutex mResultsMtx;
  std::condition_variable mResultsCv;
  std::map<std::string, std::map<int, FrameResult>> mResults;
  std::map<std::string, int> mResultIndices;

  const bool draw;
  JavaVM* jvm;
  JNIEnv* env;
  jobject strm;
  jclass class_SpatioTemporalRoIMixer;
  jmethodID SpatioTemporalRoIMixer_drawObjectDetectionResult;
  jclass class_ArrayList;
  jmethodID ArrayList_init;
  jmethodID ArrayList_add;
  jclass class_BoundingBox;
  jmethodID BoundingBox_init;
};

} // namespace rm

#endif // SPATIO_TEMPORAL_ROI_MIXER_HPP_
