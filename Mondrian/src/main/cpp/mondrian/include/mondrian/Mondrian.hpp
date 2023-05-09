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

class Mondrian {
 public:
  Mondrian(const MondrianConfig& config, int numVideos, JNIEnv* env, jobject app);

  ~Mondrian();

  int enqueueImage(const int vid, const cv::Mat& yuvMat);

 private:
  void work();

  void outputWork();

  void preprocess(Frame* frame) const;

  void handleFullFrameResults(Frame* frame);

  void handlePackedCanvasesResults(std::vector<PackedCanvas>& packedCanvases);

  void handleROIWiseResults(std::vector<PackedCanvas>& packedCanvases);

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
  const std::set<int> inputSizes_;

  std::unique_ptr<ROIExtractor> ROIExtractor_;
  std::unique_ptr<ROIResizer> ROIResizer_;
  std::unique_ptr<InferenceEngine> inferenceEngine_;
  std::unique_ptr<PatchReconstructor> patchReconstructor_;

  std::mutex frameBuffersMtx_;
  std::map<int, std::unique_ptr<FrameBuffer>> frameBuffers_;

  int numVideos_;
  std::mutex startMtx_;
  std::condition_variable startCV_;

  std::mutex resultsMtx_;
  std::condition_variable resultsCV_;
  std::map<int, std::map<int, std::pair<time_us, std::vector<BoundingBox>>>> results_;
};

} // namespace md

#endif // MONDRIAN_HPP_
