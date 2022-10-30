#ifndef INFERENCE_PLANNER_HPP_
#define INFERENCE_PLANNER_HPP_

#include <vector>
#include <map>

#include "strm/DataType.hpp"
#include "strm/Time.hpp"

namespace rm {

class InferencePlanner {
 public:
  static std::vector<InferenceInfo> getInferencePlan(
      const std::map<Device, std::map<int, time_us>>& latencyTable,
      time_us interval, bool roiWiseInference,
      const std::map<Device, time_us>& startTimes = {});

  static double weigh(const std::vector<time_us>& layout, std::map<long long, double> profile);

  static std::vector<time_us> search(time_us total,
                                     const std::vector<time_us>& bars,
                                     std::map<long long, double>& profile);
};

} // namespace rm

#endif // INFERENCE_PLANNER_HPP_
