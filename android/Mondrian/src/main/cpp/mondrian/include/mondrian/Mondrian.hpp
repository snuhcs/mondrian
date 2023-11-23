#ifndef MONDRIAN_HPP_
#define MONDRIAN_HPP_

#include <jni.h>

#include <thread>
#include <queue>

#include "chrome_tracer/tracer.h"
#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/DataType.hpp"

#include "mondrian/Frame.hpp"

namespace md {

class FrameBuffer;
class InferenceEngine;
class Logger;
class PackedCanvas;
class ROIExtractor;
class ROIPacker;
class ROIResizer;
class PatchReconstructor;

class Mondrian {
 public:
  Mondrian(const MondrianConfig& config, int numVideos, JNIEnv* env, jobject app);

  ~Mondrian();

  void enqueue(const VID vid, const cv::Mat& yuvMat);

  void dumpLogs() const;

 private:
  void enqueue(Frame* frame);

  void enqueueFrameWise(Frame* frame);

  void workSchedule();

  void waitForAllVideoReady();

  void waitForFirstInterval();

  void workPostprocess();

  void workLog();

  void handleFullFrameResults(Frame* frame, int currID);

  void handlePackedCanvasesResults(std::map<Device, std::vector<PackedCanvas>>& packedCanvasesTable, int currID);

  void handleROIWiseResults(std::map<Device, std::vector<PackedCanvas>>& packedCanvasesTable, int currID);

  void freeMats(const MultiStream& frames);

  void releaseFrames(const MultiStream& frames);

  void logFrame(const Frame* frame);

  void logFrames(const MultiStream& streams);

  const MondrianConfig config_;
  const time_us startTime_;

  // Frame Buffers
  std::mutex frameBuffersMtx_;
  std::map<int, std::unique_ptr<FrameBuffer>> frameBuffers_;

  // Components
  std::unique_ptr<ROIExtractor> ROIExtractor_;
  std::unique_ptr<ROIResizer> ROIResizer_;
  std::unique_ptr<ROIPacker> ROIPacker_;
  std::unique_ptr<InferenceEngine> inferenceEngine_;
  std::unique_ptr<PatchReconstructor> patchReconstructor_;

  // To support synchronized start
  int numVideos_;
  int numFirstFrameReadyVideos_;
  std::mutex startMtx_;
  std::condition_variable startCV_;

  // Thread: Scheduling
  const std::string scheduleThreadTag = "scheduleThread";
  std::thread scheduleThread_;
  time_us planningTime_;
  int numIntervals_;
  bool stop_;

  // Thread: Postprocessing
  const std::string postprocessThreadTag = "postprocessThread";
  std::thread postprocessThread_;
  std::mutex packingResultsMtx_;
  std::condition_variable packingResultsCV_;
  std::queue<PackingResult> packingResults_;

  // Thread : Logging
  const std::string logThreadTag = "logThread";
  std::thread logThread_;
  std::mutex logMtx_;
  std::condition_variable resultsCV_;
  std::list<MultiStream> results_;
  std::unique_ptr<Logger> loggerBoxes_;
  std::unique_ptr<Logger> loggerFrame_;
  std::unique_ptr<Logger> loggerROI_;

  // Tracer
  std::unique_ptr<chrome_tracer::ChromeTracer> tracer_;
};

} // namespace md

#endif // MONDRIAN_HPP_
