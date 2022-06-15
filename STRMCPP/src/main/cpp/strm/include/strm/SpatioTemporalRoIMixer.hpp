#ifndef SPATIO_TEMPORAL_ROI_MIXER_HPP_
#define SPATIO_TEMPORAL_ROI_MIXER_HPP_

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "strm/Config.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/ResizeProfile.hpp"
#include "strm/RoIExtractor.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/PatchReconstructor.hpp"
#include "strm/Utils.hpp"
#include "strm/Logger.hpp"

namespace rm {

class FrameBuffer {
 public:
  FrameBuffer(std::string key, int capacity, std::unique_ptr<Frame> firstFrame);

  int enqueue(const cv::Mat& mat);

  void pop();

  Frame* getFrame(int frameIndex);

 private:
  const std::string key;
  const int capacity;

  std::mutex mtx;
  std::condition_variable cv;
  std::vector<std::unique_ptr<Frame>> frames;
  int begin;
  int end;
};

class SpatioTemporalRoIMixer {
 public:
  SpatioTemporalRoIMixer(const STRMConfig& config,
                         const ResizeProfile* resizeProfile,
                         InferenceEngine* inferenceEngine);

  ~SpatioTemporalRoIMixer();

  int enqueueImage(const std::string& key, const cv::Mat& mat);

  std::vector<BoundingBox> getResults(const std::string& key, int frameIndex);

 private:
  void work();

  const STRMConfig mConfig;
  std::thread mThread;
  bool mbStop;

  const std::unique_ptr<Logger> mLogger;
  InferenceEngine* mInferenceEngine;
  const cv::Size mMixedFrameSize;

  std::unique_ptr<RoIExtractor> mRoIExtractor;
  std::unique_ptr<PatchReconstructor> mPatchReconstructor;

  std::mutex mFrameBuffersMtx;
  std::map<std::string, std::unique_ptr<FrameBuffer>> mFrameBuffers;

  std::mutex mResultsMtx;
  std::condition_variable mResultsCv;
  std::map<std::pair<std::string, int>, std::vector<BoundingBox>> mResults;
};

} // namespace rm

#endif // SPATIO_TEMPORAL_ROI_MIXER_HPP_
