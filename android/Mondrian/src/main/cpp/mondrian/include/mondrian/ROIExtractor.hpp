#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <set>
#include <thread>
#include <utility>

#include "chrome_tracer/tracer.h"
#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/Frame.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

class ROIExtractor {
 public:
  ROIExtractor(const ROIExtractorConfig& config,
               const ExecutionType executionType,
               const int maxMergeSize,
               const int roiSize,
               ROIResizer* roiResizer,
               chrome_tracer::ChromeTracer* tracer);

  ~ROIExtractor();

  void enqueue(Frame* frame);

  void notify();

  MultiStream collectFrames(int currID);

 private:
  void workPD();

  void workOF();

  void processPD(Frame* currFrame) const;

  void processOF(Frame* currFrame) const;

  bool stop_;

  const ROIExtractorConfig config_;
  const ExecutionType executionType_;
  const int maxMergeSize_;
  const int roiSize_;

  ROIResizer* ROIResizer_;

  std::thread PDThread_;
  std::mutex PDMtx_;
  std::condition_variable PDCv_;
  Stream PDWaiting_;

  std::thread OFThread_;
  std::mutex OFMtx_;
  std::condition_variable OFCv_;
  Stream OFWaiting_;
  Stream OFProcessing_;
  MultiStream OFProcessed_;

  chrome_tracer::ChromeTracer* tracer_;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
