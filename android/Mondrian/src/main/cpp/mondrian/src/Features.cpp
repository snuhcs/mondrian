#include "mondrian/Features.hpp"

namespace md {

OFFeatures::OFFeatures(const std::vector<Shift>& shifts,
                       const std::vector<float>& errs,
                       const std::vector<uchar>& statusVec) {
  assert(shifts.size() == errs.size() && errs.size() == statusVec.size());
  bool allInvalid = std::all_of(statusVec.begin(), statusVec.end(),
                                [](const uchar& status) { return status == 0; });
  if (allInvalid) {
    shiftAvg = {0, 0};
    shiftStd = {0, 0};
    shiftNcc = 100;
    avgErr = 100;
    return;
  }
  auto[validShifts, validErrs] = filterInvalid(shifts, errs, statusVec);
  auto[inlierShifts, inlierErrs] = filterOutlier(validShifts, validErrs);
  assert(!inlierShifts.empty());
  shiftAvg = avgOf(inlierShifts);
  shiftStd = stdOf(inlierShifts);
  shiftNcc = nccOf(inlierShifts);
  avgErr = avgOf(inlierErrs);
}

std::pair<std::vector<Shift>, std::vector<float>>
OFFeatures::filterInvalid(const std::vector<Shift>& shifts,
                          const std::vector<float>& errs,
                          const std::vector<uchar>& statusVec) {
  std::vector<Shift> validShifts;
  std::vector<float> validErrs;
  for (int i = 0; i < shifts.size(); i++) {
    assert(statusVec[i] == 0 || statusVec[i] == 1);
    if (statusVec[i] == 1) {
      validShifts.push_back(shifts[i]);
      validErrs.push_back(errs[i]);
    }
  }
  return {validShifts, validErrs};
}

std::pair<std::vector<Shift>, std::vector<float>>
OFFeatures::filterOutlier(const std::vector<Shift>& shifts,
                          const std::vector<float>& errs) {
  assert(!shifts.empty());
  std::vector<float> squareDistances;
  squareDistances.reserve(shifts.size());
  for (const auto&[x, y]: shifts) {
    squareDistances.push_back(x * x + y * y);
  }
  std::sort(squareDistances.begin(), squareDistances.end());
  float q1SquareDistance = squareDistances[squareDistances.size() / 4];

  std::vector<Shift> inlierShifts;
  std::vector<float> inlierErrs;
  for (int i = 0; i < shifts.size(); i++) {
    auto&[x, y] = shifts[i];
    if (x * x + y * y >= q1SquareDistance) {
      inlierShifts.emplace_back(x, y);
      inlierErrs.push_back(errs[i]);
    }
  }
  return {inlierShifts, inlierErrs};
}

Shift OFFeatures::avgOf(const std::vector<Shift>& shifts) {
  if (shifts.empty()) {
    return {0, 0};
  }
  Shift sum = {0, 0};
  for (const auto&[x, y]: shifts) {
    sum.first += x;
    sum.second += y;
  }
  return {sum.first / float(shifts.size()),
          sum.second / float(shifts.size())};
}

Shift OFFeatures::stdOf(const std::vector<Shift>& shifts) {
  if (shifts.empty()) {
    return {0, 0};
  }
  Shift var = {0, 0};
  auto[avgX, avgY] = avgOf(shifts);
  for (const auto&[x, y]: shifts) {
    var.first += (x - avgX) * (x - avgX);
    var.second += (y - avgY) * (y - avgY);
  }
  var.first /= float(shifts.size());
  var.second /= float(shifts.size());
  return {std::sqrt(var.first), std::sqrt(var.second)};
}

float OFFeatures::nccOf(const std::vector<Shift>& shifts) {
  if (shifts.size() <= 1) {
    return 0;
  }
  float sum = 0;
  for (int i = 0; i < shifts.size(); i++) {
    for (int j = i + 1; j < shifts.size(); j++) {
      auto&[Xi, Yi] = shifts[i];
      auto&[Xj, Yj] = shifts[j];
      float sizeI = Xi * Xi + Yi * Yi;
      float sizeJ = Xj * Xj + Yj * Yj;
      if (sizeI == 0 || sizeJ == 0) {
        continue;
      }
      sum += (Xi * Xj + Yi * Yj) / std::sqrt(sizeI * sizeJ);
    }
  }
  return sum / float(shifts.size() * (shifts.size() - 1) / 2);
}

float OFFeatures::avgOf(const std::vector<float>& errs) {
  if (errs.empty()) {
    return 0;
  }
  float sum = 0;
  for (const float& e: errs) {
    sum += e;
  }
  return sum /= float(errs.size());
}

} // namespace md
