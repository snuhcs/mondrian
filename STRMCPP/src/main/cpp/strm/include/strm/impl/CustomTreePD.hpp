#ifndef IMPL_CUSTOM_TREE_PD_HPP_
#define IMPL_CUSTOM_TREE_PD_HPP_

namespace rm {

float PDTree(float xy_ratio, float diff_area_ratio) {
  if (xy_ratio <= 0.19072) {
    if (diff_area_ratio <= 0.084096) {
      if (diff_area_ratio <= -0.31835) {
        if (xy_ratio <= -0.49562) {
          return 63.055;
        }
        else {  // if xy_ratio > -0.49562
          return 72.32;
        }
      }
      else {  // if diff_area_ratio > -0.31835
        return 62.093;
      }
    }
    else {  // if diff_area_ratio > 0.084096
      if (diff_area_ratio <= 0.58134) {
        if (xy_ratio <= -0.51303) {
          return 51.617;
        }
        else {  // if xy_ratio > -0.51303
          if (xy_ratio <= 0.061954) {
            return 59.744;
          }
          else {  // if xy_ratio > 0.061954
            return 75.008;
          }
        }
      }
      else {  // if diff_area_ratio > 0.58134
        return 47.331;
      }
    }
  }
  else {  // if xy_ratio > 0.19072
    if (xy_ratio <= 3.5173) {
      if (xy_ratio <= 0.40954) {
        if (diff_area_ratio <= 0.94344) {
          return 76.183;
        }
        else {  // if diff_area_ratio > 0.94344
          return 51.062;
        }
      }
      else {  // if xy_ratio > 0.40954
        if (diff_area_ratio <= -2.1985) {
          return 99.234;
        }
        else {  // if diff_area_ratio > -2.1985
          if (diff_area_ratio <= -1.1234) {
            if (xy_ratio <= 1.8547) {
              return 76.91;
            }
            else {  // if xy_ratio > 1.8547
              return 88.113;
            }
          }
          else {  // if diff_area_ratio > -1.1234
            if (xy_ratio <= 0.83821) {
              return 86.259;
            }
            else {  // if xy_ratio > 0.83821
              return 94.634;
            }
          }
        }
      }
    }
    else {  // if xy_ratio > 3.5173
      if (xy_ratio <= 5.0751) {
        return 103.98;
      }
      else {  // if xy_ratio > 5.0751
        return 123.08;
      }
    }
  }
}

} // namespace rm

#endif // IMPL_CUSTOM_TREE_PD_HPP_
