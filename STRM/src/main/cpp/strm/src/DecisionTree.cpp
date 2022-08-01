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

float PDTree(float xy_ratio, float diff_area_ratio) {
  if (xy_ratio <= 0.19072) {
    if (diff_area_ratio <= 0.084096) {
      if (diff_area_ratio <= -0.31835) {
        if (xy_ratio <= -0.49562) {
          return 63.055;
        } else {  // if xy_ratio > -0.49562
          return 72.32;
        }
      } else {  // if diff_area_ratio > -0.31835
        return 62.093;
      }
    } else {  // if diff_area_ratio > 0.084096
      if (diff_area_ratio <= 0.58134) {
        if (xy_ratio <= -0.51303) {
          return 51.617;
        } else {  // if xy_ratio > -0.51303
          if (xy_ratio <= 0.061954) {
            return 59.744;
          } else {  // if xy_ratio > 0.061954
            return 75.008;
          }
        }
      } else {  // if diff_area_ratio > 0.58134
        return 47.331;
      }
    }
  } else {  // if xy_ratio > 0.19072
    if (xy_ratio <= 3.5173) {
      if (xy_ratio <= 0.40954) {
        if (diff_area_ratio <= 0.94344) {
          return 76.183;
        } else {  // if diff_area_ratio > 0.94344
          return 51.062;
        }
      } else {  // if xy_ratio > 0.40954
        if (diff_area_ratio <= -2.1985) {
          return 99.234;
        } else {  // if diff_area_ratio > -2.1985
          if (diff_area_ratio <= -1.1234) {
            if (xy_ratio <= 1.8547) {
              return 76.91;
            } else {  // if xy_ratio > 1.8547
              return 88.113;
            }
          } else {  // if diff_area_ratio > -1.1234
            if (xy_ratio <= 0.83821) {
              return 86.259;
            } else {  // if xy_ratio > 0.83821
              return 94.634;
            }
          }
        }
      }
    } else {  // if xy_ratio > 3.5173
      if (xy_ratio <= 5.0751) {
        return 103.98;
      } else {  // if xy_ratio > 5.0751
        return 123.08;
      }
    }
  }
}

} // namespace rm
