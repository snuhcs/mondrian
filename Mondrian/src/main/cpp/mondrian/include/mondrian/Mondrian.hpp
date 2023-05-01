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
class PackedCanvas;
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

  void handleFullFrameResults(Frame* frame);

  void handlePackedCanvasResults(std::vector<PackedCanvas>& packedCanvas);

  void handleROIWiseResults(std::vector<PackedCanvas>& packedCanvas);

  void releaseFrames(const MultiStream& frames);

  void log(const Frame* frame);

  const MondrianConfig config_;
  const time_us scheduleInterval_;
  std::thread thread_;
  bool stop_;

  std::thread resultThread_;
  std::unique_ptr<Logger> resultLogger_;
  std::unique_ptr<Logger> executionLogger_;
  std::unique_ptr<Logger> ROILogger_;
  const cv::Size targetSize_;
  const std::vector<int> inputSizes_;

  std::unique_ptr<ROIExtractor> ROIExtractor_;
  std::unique_ptr<ROIResizer> ROIResizer_;
  std::unique_ptr<InferenceEngine> inferenceEngine_;
  std::unique_ptr<PatchReconstructor> patchReconstructor_;

  int prevEnqueuedVid_ = -1;
  std::mutex fairEnqueueMtx_;
  std::condition_variable fairCV_;

  int numStartedFrameBuffers_ = 0;
  std::mutex startMtx_;
  std::condition_variable startCV_;
  std::condition_variable enqueueCV_;
  bool startEnqueue_ = false;

  std::mutex frameBuffersMtx_;
  std::map<int, std::unique_ptr<FrameBuffer>> frameBuffers_;
  std::map<int, int> startIndices_;

  std::mutex resultsMtx_;
  std::condition_variable resultsCV_;
  std::map<int, std::map<int, FrameResult>> results_;
  std::map<int, int> resultIndices_;
};

} // namespace md

#endif // MONDRIAN_HPP_
