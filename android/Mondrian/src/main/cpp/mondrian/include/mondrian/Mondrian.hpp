#ifndef MONDRIAN_HPP_
#define MONDRIAN_HPP_

#include <jni.h>

#include <thread>
#include <queue>

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

class Mondrian {
 public:
  Mondrian(const MondrianConfig& config, int numVideos, JNIEnv* env, jobject app);

  ~Mondrian();

  void enqueue(const int vid, const cv::Mat& yuvMat);

 private:
  void work();

  void workPreprocess();

  void workLog();

  void handleFullFrameResults(Frame* frame);

  void handlePackedCanvasesResults(std::vector<PackedCanvas>& packedCanvases);

  void handleROIWiseResults(std::vector<PackedCanvas>& packedCanvases);

  void releaseFrames(const MultiStream& frames);

  void log(const Frame* frame);

  const MondrianConfig config_;
  const time_us scheduleInterval_;
  std::thread thread_;
  bool stop_;

  int numVideos_;
  int numFirstFrameReadyVideos_;
  std::mutex startMtx_;
  std::condition_variable startCV_;

  std::thread preprocessThread_;
  std::mutex preprocessMtx_;
  std::condition_variable preprocessCV_;
  std::queue<Frame*> preprocessQueue_;

  std::thread resultThread_;
  std::unique_ptr<Logger> loggerBoxes_;
  std::unique_ptr<Logger> loggerFrame_;
  std::unique_ptr<Logger> loggerROI_;
  const cv::Size targetSize_;
  const std::set<int> inputSizes_;

  std::unique_ptr<ROIExtractor> ROIExtractor_;
  std::unique_ptr<ROIResizer> ROIResizer_;
  std::unique_ptr<InferenceEngine> inferenceEngine_;
  std::unique_ptr<PatchReconstructor> patchReconstructor_;

  std::mutex frameBuffersMtx_;
  std::map<int, std::unique_ptr<FrameBuffer>> frameBuffers_;

  std::mutex resultsMtx_;
  std::condition_variable resultsCV_;
  std::map<int, std::map<int, std::pair<time_us, std::vector<BoundingBox>>>> results_;
};

} // namespace md

#endif // MONDRIAN_HPP_
