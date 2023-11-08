#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
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

  void workPostprocess();

  void processPD(Frame* currFrame) const;

  void processOF(Frame* currFrame) const;

  void postprocess(Frame* currFrame) const;

  bool stop_;
  bool isOFProcessing_;
  bool isPostprocessing_;
  bool collecting_;

  const ROIExtractorConfig config_;
  const ExecutionType executionType_;
  const int maxMergeSize_;
  const int roiSize_;

  ROIResizer* ROIResizer_;

  const std::string ROIExtractorPDTag_ = "ROIExtractorPD";
  std::thread PDThread_;
  std::mutex PDMtx_;
  std::condition_variable PDCv_;
  MultiStream PDWaiting_;

  const std::string ROIExtractorOFTag_ = "ROIExtractorOF";
  std::thread OFThread_;
  std::mutex OFMtx_;
  std::condition_variable OFCv_;
  MultiStream OFWaiting_;

  const std::string ROIExtractorPostprocessTag_ = "ROIExtractorPostprocess";
  std::thread PostprocessThread_;
  MultiStream PostprocessWaiting_;
  MultiStream Processed_;

  chrome_tracer::ChromeTracer* tracer_;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
