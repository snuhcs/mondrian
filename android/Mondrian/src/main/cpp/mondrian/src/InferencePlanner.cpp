#include "mondrian/InferencePlanner.hpp"

#include <numeric>

namespace md {

double InferencePlanner::weigh(const std::vector<time_us>& layout,
                               std::map<time_us, double> profile) {
  double weight = 0;
  for (auto l : layout) {
    assert (profile.find(l) != profile.end());
    weight += double(l) * profile[l];
  }
  return weight;
}

std::vector<time_us> InferencePlanner::search(time_us total,
                                              const std::vector<time_us>& bars,
                                              std::map<long long, double>& profile) {
  std::vector<time_us> layout;
  time_us left = total;

  // greedy initialization
  for (auto l : bars) {
    time_us cnt = left / l;
    for (int i = 0; i < cnt; i++) {
      layout.push_back(l);
    }
    left -= l * cnt;
  }

  // if cannot fill any bar, return empty layout
  if (layout.empty()) {
    return layout;
  }

  // select type of bar to try removing
  time_us l_a = layout[0];
  assert (profile.find(l_a) != profile.end());
  double d_a = profile[l_a];
  long c_a = std::count(layout.begin(), layout.end(), l_a);
  double alpha = weigh(layout, profile) - double(l_a) * double(c_a) * d_a;
  auto l_a_it = std::find(bars.begin(), bars.end(), l_a);

  // if selected bar is the one with the smallest density, removing is no worth
  if (l_a_it == bars.end()) {
    return layout;
  }

  // figure maximum number of removal with potential benefit
  double d_b = profile[bars[std::distance(bars.begin(), l_a_it) + 1]];
  long k_max = std::min(c_a, long((double(left) * d_b - alpha) / (double(l_a) * (d_a - d_b))));
  if (k_max < 0) {
    return layout;
  }

  // try all possible number of removals, recursively calling this function for the other area
  std::vector<std::vector<time_us>> layouts;
  std::vector<time_us> subBars{bars.begin() + 1, bars.end()};
  for (int k = 0; k <= k_max; k++) {
    std::vector<time_us> layout_left(c_a - k, l_a);
    time_us sum = std::accumulate(layout_left.begin(), layout_left.end(), 0l);
    std::vector<time_us> layout_right = search(total - sum, subBars, profile);
    layout_left.insert(layout_left.end(), layout_right.begin(), layout_right.end());
    layouts.push_back(layout_left);
  }
  std::sort(layouts.begin(), layouts.end(),
            [profile](auto& l1, auto& l2) {
              return weigh(l1, profile) > weigh(l2, profile);
            });
  return layouts[0];
}

std::vector<InferenceInfo> InferencePlanner::getInferencePlan(
    const std::map<Device, std::map<std::pair<int, bool>, time_us>>& latencyTable,
    time_us interval, bool roiWiseInference,
    const std::map<Device, time_us>& startTimes) {
  std::vector<InferenceInfo> inferencePlan;

  std::map<Device, std::map<int, time_us>> latencyTableWoFullFrame;
  for (const auto& [device, size_forFullFrame_latency] : latencyTable) {
    for (const auto& [size_forFullFrame, latency] : size_forFullFrame_latency) {
      auto [size, forFullFrame] = size_forFullFrame;
      if (!forFullFrame) {
        latencyTableWoFullFrame[device][size] = latency;
      }
    }
  }

  for (const auto& [device, size_latency] : latencyTableWoFullFrame) {
    std::map<time_us, double> profile;
    std::vector<time_us> bars;
    std::map<time_us, int> latency_size;
    int min_size = std::min_element(
        size_latency.begin(), size_latency.end(),
        [](const auto& it0, const auto& it1) {
          return it0.first < it1.first;
        })->first;
    for (const auto& [size, latency] : size_latency) {
      if (roiWiseInference && size != min_size) {
        continue;
      }
      profile[latency] = double(size * size) / double(latency);
      bars.push_back(latency);
      latency_size[latency] = size;
    }
    std::sort(bars.begin(), bars.end(),
              [&profile](time_us b1, time_us b2) { return (profile[b1] > profile[b2]); });
    time_us startTime = startTimes.find(device) != startTimes.end()
                        ? startTimes.at(device)
                        : 0L;
    std::vector<time_us> layout = search(interval - startTime, bars, profile);
    inferencePlan.reserve(layout.size());
    for (auto& l : layout) {
      inferencePlan.push_back({device, latency_size[l], l, 0});
    }
  }
  for (const auto& deviceLatency : latencyTableWoFullFrame) {
    std::vector<InferenceInfo*> devicePlan;
    for (auto& info : inferencePlan) {
      if (info.device == deviceLatency.first) {
        devicePlan.push_back(&info);
      }
    }
    // Larger size first
    std::sort(devicePlan.begin(), devicePlan.end(),
              [](const InferenceInfo* l, const InferenceInfo* r) {
                return l->size > r->size;
              });
    time_us accumulatedLatency = 0;
    for (auto* info : devicePlan) {
      accumulatedLatency += info->latency;
      info->accumulatedLatency = accumulatedLatency;
    }
  }
  std::sort(inferencePlan.begin(), inferencePlan.end(),
            [](const InferenceInfo& l, const InferenceInfo& r) {
              return l.accumulatedLatency < r.accumulatedLatency;
            });
  return inferencePlan;
}

} // namespace md
