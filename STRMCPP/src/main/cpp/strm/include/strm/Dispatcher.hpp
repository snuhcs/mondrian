#ifndef DISPATCHER_HPP_
#define DISPATCHER_HPP_

#include <climits>
#include <fstream>
#include <map>
#include <utility>
#include <vector>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/ResizeProfile.hpp"
#include "strm/RoIPrioritizer.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/RoIExtractor.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/PatchReconstructorCallback.hpp"
#include "strm/Logger.hpp"

namespace rm {

class Dispatcher {
 public:
  Dispatcher(const DispatcherConfig& config,
             RoIExtractor* roIExtractor,
             PatchMixer* patchMixer,
             InferenceEngine* inferenceEngine,
             PatchReconstructor* patchReconstructor,
             Logger* logger);

  ~Dispatcher();

  int enqueue(const cv::Mat& mat);

  std::shared_ptr<Frame> getFrameToProcess();

  void process(const std::shared_ptr<Frame>& currFrame);

  void notifyResults();

  std::vector<BoundingBox> getResults(int frameIndex);

 private:
  std::vector<BoundingBox> getPrevBoxes(bool useInferenceResults);
};

} // namespace rm

#endif // DISPATCHER_HPP_
