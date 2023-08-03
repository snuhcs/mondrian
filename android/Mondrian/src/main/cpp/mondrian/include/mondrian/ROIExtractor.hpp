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
  ROIExtractor(const ROIExtractorConfig& config, ROIResizer* roiResizer);

  ~ROIExtractor();

  void enqueue(Frame* frame);

  Stream collectFrames(int scheduleID);

  std::condition_variable& cv() {
    return cv_;
  }

 private:
  void work(int extractorId);

  void processPD(Frame* currFrame) const;

  void processOF(Frame* currFrame);

  bool stop_;
  std::vector<std::thread> threads_;

  const ROIExtractorConfig config_;

  ROIResizer* ROIResizer_;

  std::mutex mtx_;
  std::condition_variable cv_;
  Stream PDWaiting_;
  Stream PDProcessing_;
  Stream OFWaiting_;
  Stream OFProcessing_;
  Stream OFProcessed_;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
