#include "strm/DecisionTree.hpp"

namespace rm {

float OFTree(float xy_ratio, float shift, float err) {
  if (xy_ratio <= 0.2022) {
    if (err <= 0.44536) {
      if (err <= -1.7882) {
        return 92.309;
      } else {  // if err > -1.7882
        if (err <= -0.096391) {
          return 53.843;
        } else {  // if err > -0.096391
          return 57.337;
        }
      }
    } else {  // if err > 0.44536
      return 61.336;
    }
  } else {  // if xy_ratio > 0.2022
    if (xy_ratio <= 3.4842) {
      if (err <= 0.52764) {
        if (err <= -1.6341) {
          return 59.359;
        } else {  // if err > -1.6341
          return 47.113;
        }
      } else {  // if err > 0.52764
        return 55.68;
      }
    } else {  // if xy_ratio > 3.4842
      return 63.715;
    }
  }
}

} // namespace rm
