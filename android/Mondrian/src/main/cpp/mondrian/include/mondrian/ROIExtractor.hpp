#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <set>
#include <thread>
#include <utility>

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
               ROIResizer* roiResizer);

  ~ROIExtractor();

  void enqueue(Frame* frame);

  MultiStream collectFrames(int currID);

  std::condition_variable& cv() {
    return cv_;
  }

 private:
  void work(int extractorId);

  void processPD(Frame* currFrame) const;

  void processOF(Frame* currFrame) const;

  std::vector<std::thread> threads_;
  bool stop_;

  const ROIExtractorConfig config_;
  const ExecutionType executionType_;
  const int maxMergeSize_;
  const int roiSize_;

  ROIResizer* ROIResizer_;

  std::mutex mtx_;
  std::condition_variable cv_;
  Stream PDWaiting_;
  Stream PDProcessing_;
  Stream OFWaiting_;
  Stream OFProcessing_;
  MultiStream OFProcessed_;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
