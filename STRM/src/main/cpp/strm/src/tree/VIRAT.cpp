#include "strm/tree/VIRAT.hpp"

namespace rm {

int VIRAT(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
          float shiftNcc, float avgErr, float confidence) {
  // TODO : remove confidence from parameter
  if (area <= 4309.5) {
    if (area <= 2861) {
      if (area <= 1469) {
        return 0;
      }
      else {  // if area > 1469
        if (shiftNcc <= -0.099429) {
          if (shiftAvg <= 0.00060585) {
            return 1;
          }
          else {  // if shiftAvg > 0.00060585
            return 0;
          }
        }
        else {  // if shiftNcc > -0.099429
          if (xyRatio <= 1.3258) {
            if (shiftStd <= 0.042864) {
              return 1;
            }
            else {  // if shiftStd > 0.042864
              if (shiftStd <= 0.043091) {
                return 0;
              }
              else {  // if shiftStd > 0.043091
                if (area <= 2838.5) {
                  if (maxEdgeLength <= 53.5) {
                    if (xyRatio <= 0.78302) {
                      return 1;
                    }
                    else {  // if xyRatio > 0.78302
                      return 1;
                    }
                  }
                  else {  // if maxEdgeLength > 53.5
                    return 1;
                  }
                }
                else {  // if area > 2838.5
                  if (shiftAvg <= 0.019939) {
                    if (shiftStd <= 0.15208) {
                      return 1;
                    }
                    else {  // if shiftStd > 0.15208
                      return 0;
                    }
                  }
                  else {  // if shiftAvg > 0.019939
                    return 1;
                  }
                }
              }
            }
          }
          else {  // if xyRatio > 1.3258
            if (shiftStd <= 0.028159) {
              if (avgErr <= 4.4337) {
                if (maxEdgeLength <= 53.5) {
                  return 0;
                }
                else {  // if maxEdgeLength > 53.5
                  return 1;
                }
              }
              else {  // if avgErr > 4.4337
                if (avgErr <= 4.8928) {
                  return 0;
                }
                else {  // if avgErr > 4.8928
                  if (shiftStd <= 0.026248) {
                    return 1;
                  }
                  else {  // if shiftStd > 0.026248
                    return 0;
                  }
                }
              }
            }
            else {  // if shiftStd > 0.028159
              if (shiftAvg <= 0.22597) {
                return 1;
              }
              else {  // if shiftAvg > 0.22597
                if (shiftAvg <= 0.24534) {
                  return 0;
                }
                else {  // if shiftAvg > 0.24534
                  return 1;
                }
              }
            }
          }
        }
      }
    }
    else {  // if area > 2861
      if (shiftAvg <= 0.0097556) {
        if (shiftStd <= 0.3164) {
          if (shiftNcc <= -0.073445) {
            return 1;
          }
          else {  // if shiftNcc > -0.073445
            if (xyRatio <= 1.2451) {
              if (shiftAvg <= 0.0041288) {
                if (avgErr <= 0.35938) {
                  if (avgErr <= 0.35204) {
                    if (area <= 4285) {
                      return 2;
                    }
                    else {  // if area > 4285
                      return 1;
                    }
                  }
                  else {  // if avgErr > 0.35204
                    return 1;
                  }
                }
                else {  // if avgErr > 0.35938
                  if (shiftStd <= 0.021585) {
                    return 2;
                  }
                  else {  // if shiftStd > 0.021585
                    if (shiftStd <= 0.022303) {
                      return 1;
                    }
                    else {  // if shiftStd > 0.022303
                      return 2;
                    }
                  }
                }
              }
              else {  // if shiftAvg > 0.0041288
                if (shiftStd <= 0.009628) {
                  return 1;
                }
                else {  // if shiftStd > 0.009628
                  if (shiftAvg <= 0.0041519) {
                    return 1;
                  }
                  else {  // if shiftAvg > 0.0041519
                    if (shiftNcc <= 0.07443) {
                      return 2;
                    }
                    else {  // if shiftNcc > 0.07443
                      return 2;
                    }
                  }
                }
              }
            }
            else {  // if xyRatio > 1.2451
              if (area <= 2880.5) {
                if (shiftNcc <= 0.023397) {
                  return 2;
                }
                else {  // if shiftNcc > 0.023397
                  return 1;
                }
              }
              else {  // if area > 2880.5
                if (xyRatio <= 1.2454) {
                  return 1;
                }
                else {  // if xyRatio > 1.2454
                  if (avgErr <= 3.9049) {
                    if (avgErr <= 0.13906) {
                      return 2;
                    }
                    else {  // if avgErr > 0.13906
                      return 2;
                    }
                  }
                  else {  // if avgErr > 3.9049
                    if (area <= 4007) {
                      return 2;
                    }
                    else {  // if area > 4007
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
        else {  // if shiftStd > 0.3164
          if (avgErr <= 9.8806) {
            return 1;
          }
          else {  // if avgErr > 9.8806
            return 2;
          }
        }
      }
      else {  // if shiftAvg > 0.0097556
        if (avgErr <= 8.7074) {
          if (xyRatio <= 1.2971) {
            if (shiftNcc <= 0.93117) {
              if (shiftStd <= 0.017712) {
                if (xyRatio <= 1.0377) {
                  return 1;
                }
                else {  // if xyRatio > 1.0377
                  return 0;
                }
              }
              else {  // if shiftStd > 0.017712
                if (shiftAvg <= 0.054117) {
                  if (shiftAvg <= 0.0098358) {
                    if (maxEdgeLength <= 65) {
                      return 2;
                    }
                    else {  // if maxEdgeLength > 65
                      return 1;
                    }
                  }
                  else {  // if shiftAvg > 0.0098358
                    if (maxEdgeLength <= 54.5) {
                      return 0;
                    }
                    else {  // if maxEdgeLength > 54.5
                      return 2;
                    }
                  }
                }
                else {  // if shiftAvg > 0.054117
                  if (area <= 3962.5) {
                    if (avgErr <= 6.4937) {
                      return 2;
                    }
                    else {  // if avgErr > 6.4937
                      return 2;
                    }
                  }
                  else {  // if area > 3962.5
                    if (maxEdgeLength <= 66.5) {
                      return 0;
                    }
                    else {  // if maxEdgeLength > 66.5
                      return 2;
                    }
                  }
                }
              }
            }
            else {  // if shiftNcc > 0.93117
              if (area <= 4136) {
                return 0;
              }
              else {  // if area > 4136
                return 1;
              }
            }
          }
          else {  // if xyRatio > 1.2971
            if (shiftNcc <= 0.46292) {
              if (area <= 4135) {
                if (area <= 4102) {
                  if (shiftStd <= 0.034502) {
                    if (shiftStd <= 0.032578) {
                      return 2;
                    }
                    else {  // if shiftStd > 0.032578
                      return 0;
                    }
                  }
                  else {  // if shiftStd > 0.034502
                    if (shiftNcc <= -0.06084) {
                      return 1;
                    }
                    else {  // if shiftNcc > -0.06084
                      return 2;
                    }
                  }
                }
                else {  // if area > 4102
                  if (shiftAvg <= 0.015722) {
                    return 2;
                  }
                  else {  // if shiftAvg > 0.015722
                    if (shiftStd <= 0.12649) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.12649
                      return 2;
                    }
                  }
                }
              }
              else {  // if area > 4135
                if (shiftAvg <= 0.063815) {
                  if (area <= 4138) {
                    return 1;
                  }
                  else {  // if area > 4138
                    if (shiftStd <= 0.15158) {
                      return 2;
                    }
                    else {  // if shiftStd > 0.15158
                      return 0;
                    }
                  }
                }
                else {  // if shiftAvg > 0.063815
                  if (area <= 4267) {
                    if (shiftNcc <= 0.089444) {
                      return 2;
                    }
                    else {  // if shiftNcc > 0.089444
                      return 1;
                    }
                  }
                  else {  // if area > 4267
                    return 2;
                  }
                }
              }
            }
            else {  // if shiftNcc > 0.46292
              if (xyRatio <= 1.5102) {
                if (shiftAvg <= 0.082449) {
                  if (area <= 3810) {
                    if (area <= 3636) {
                      return 2;
                    }
                    else {  // if area > 3636
                      return 0;
                    }
                  }
                  else {  // if area > 3810
                    return 2;
                  }
                }
                else {  // if shiftAvg > 0.082449
                  if (xyRatio <= 1.3395) {
                    if (shiftAvg <= 0.10859) {
                      return 2;
                    }
                    else {  // if shiftAvg > 0.10859
                      return 1;
                    }
                  }
                  else {  // if xyRatio > 1.3395
                    if (area <= 3288) {
                      return 2;
                    }
                    else {  // if area > 3288
                      return 0;
                    }
                  }
                }
              }
              else {  // if xyRatio > 1.5102
                if (shiftNcc <= 0.49627) {
                  if (area <= 3588) {
                    return 1;
                  }
                  else {  // if area > 3588
                    if (shiftNcc <= 0.48436) {
                      return 2;
                    }
                    else {  // if shiftNcc > 0.48436
                      return 0;
                    }
                  }
                }
                else {  // if shiftNcc > 0.49627
                  if (shiftStd <= 0.21392) {
                    if (xyRatio <= 1.7842) {
                      return 2;
                    }
                    else {  // if xyRatio > 1.7842
                      return 2;
                    }
                  }
                  else {  // if shiftStd > 0.21392
                    return 2;
                  }
                }
              }
            }
          }
        }
        else {  // if avgErr > 8.7074
          if (xyRatio <= 1.2368) {
            if (avgErr <= 18.144) {
              if (avgErr <= 9.2544) {
                if (avgErr <= 9.2278) {
                  if (shiftNcc <= 0.18346) {
                    if (xyRatio <= 0.86268) {
                      return 2;
                    }
                    else {  // if xyRatio > 0.86268
                      return 1;
                    }
                  }
                  else {  // if shiftNcc > 0.18346
                    return 2;
                  }
                }
                else {  // if avgErr > 9.2278
                  return 1;
                }
              }
              else {  // if avgErr > 9.2544
                if (area <= 4156) {
                  if (shiftAvg <= 0.012067) {
                    if (shiftAvg <= 0.011864) {
                      return 2;
                    }
                    else {  // if shiftAvg > 0.011864
                      return 0;
                    }
                  }
                  else {  // if shiftAvg > 0.012067
                    return 2;
                  }
                }
                else {  // if area > 4156
                  if (area <= 4169) {
                    return 0;
                  }
                  else {  // if area > 4169
                    if (shiftNcc <= 0.12161) {
                      return 2;
                    }
                    else {  // if shiftNcc > 0.12161
                      return 2;
                    }
                  }
                }
              }
            }
            else {  // if avgErr > 18.144
              if (shiftAvg <= 4.8648) {
                return 0;
              }
              else {  // if shiftAvg > 4.8648
                return 2;
              }
            }
          }
          else {  // if xyRatio > 1.2368
            if (shiftAvg <= 0.010471) {
              return 0;
            }
            else {  // if shiftAvg > 0.010471
              if (shiftNcc <= 0.42867) {
                if (area <= 4300) {
                  if (maxEdgeLength <= 89.5) {
                    if (xyRatio <= 1.2936) {
                      return 2;
                    }
                    else {  // if xyRatio > 1.2936
                      return 2;
                    }
                  }
                  else {  // if maxEdgeLength > 89.5
                    if (xyRatio <= 2.0909) {
                      return 0;
                    }
                    else {  // if xyRatio > 2.0909
                      return 2;
                    }
                  }
                }
                else {  // if area > 4300
                  if (shiftNcc <= 0.21283) {
                    return 1;
                  }
                  else {  // if shiftNcc > 0.21283
                    return 2;
                  }
                }
              }
              else {  // if shiftNcc > 0.42867
                if (shiftNcc <= 0.43415) {
                  return 0;
                }
                else {  // if shiftNcc > 0.43415
                  if (xyRatio <= 1.7582) {
                    if (shiftAvg <= 0.1925) {
                      return 2;
                    }
                    else {  // if shiftAvg > 0.1925
                      return 2;
                    }
                  }
                  else {  // if xyRatio > 1.7582
                    if (shiftAvg <= 0.095168) {
                      return 2;
                    }
                    else {  // if shiftAvg > 0.095168
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else {  // if area > 4309.5
    if (area <= 5875) {
      if (shiftAvg <= 0.055593) {
        if (xyRatio <= 1.3614) {
          if (shiftNcc <= 0.22972) {
            if (xyRatio <= 1.1102) {
              if (shiftAvg <= 0.003192) {
                if (shiftStd <= 0.03723) {
                  return 3;
                }
                else {  // if shiftStd > 0.03723
                  if (shiftStd <= 0.043206) {
                    return 2;
                  }
                  else {  // if shiftStd > 0.043206
                    return 3;
                  }
                }
              }
              else {  // if shiftAvg > 0.003192
                if (shiftAvg <= 0.0032525) {
                  return 0;
                }
                else {  // if shiftAvg > 0.0032525
                  if (avgErr <= 1.9569) {
                    if (shiftAvg <= 0.025289) {
                      return 3;
                    }
                    else {  // if shiftAvg > 0.025289
                      return 1;
                    }
                  }
                  else {  // if avgErr > 1.9569
                    if (shiftStd <= 0.013024) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.013024
                      return 3;
                    }
                  }
                }
              }
            }
            else {  // if xyRatio > 1.1102
              if (avgErr <= 3.3967) {
                if (shiftAvg <= 0.023917) {
                  if (avgErr <= 1.3432) {
                    if (avgErr <= 1.3399) {
                      return 3;
                    }
                    else {  // if avgErr > 1.3399
                      return 0;
                    }
                  }
                  else {  // if avgErr > 1.3432
                    if (xyRatio <= 1.3543) {
                      return 3;
                    }
                    else {  // if xyRatio > 1.3543
                      return 3;
                    }
                  }
                }
                else {  // if shiftAvg > 0.023917
                  if (xyRatio <= 1.1203) {
                    if (shiftAvg <= 0.029831) {
                      return 0;
                    }
                    else {  // if shiftAvg > 0.029831
                      return 1;
                    }
                  }
                  else {  // if xyRatio > 1.1203
                    if (avgErr <= 1.4832) {
                      return 1;
                    }
                    else {  // if avgErr > 1.4832
                      return 3;
                    }
                  }
                }
              }
              else {  // if avgErr > 3.3967
                if (shiftStd <= 0.061556) {
                  if (shiftStd <= 0.046521) {
                    if (avgErr <= 4.2201) {
                      return 3;
                    }
                    else {  // if avgErr > 4.2201
                      return 3;
                    }
                  }
                  else {  // if shiftStd > 0.046521
                    if (shiftStd <= 0.050584) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.050584
                      return 3;
                    }
                  }
                }
                else {  // if shiftStd > 0.061556
                  if (shiftNcc <= -0.047708) {
                    return 0;
                  }
                  else {  // if shiftNcc > -0.047708
                    if (area <= 4358) {
                      return 1;
                    }
                    else {  // if area > 4358
                      return 3;
                    }
                  }
                }
              }
            }
          }
          else {  // if shiftNcc > 0.22972
            if (shiftNcc <= 0.24021) {
              if (avgErr <= 6.3344) {
                if (area <= 4730.5) {
                  if (avgErr <= 2.1724) {
                    return 3;
                  }
                  else {  // if avgErr > 2.1724
                    return 2;
                  }
                }
                else {  // if area > 4730.5
                  if (shiftAvg <= 0.035648) {
                    if (shiftStd <= 0.10297) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.10297
                      return 1;
                    }
                  }
                  else {  // if shiftAvg > 0.035648
                    if (shiftStd <= 0.094256) {
                      return 1;
                    }
                    else {  // if shiftStd > 0.094256
                      return 3;
                    }
                  }
                }
              }
              else {  // if avgErr > 6.3344
                return 3;
              }
            }
            else {  // if shiftNcc > 0.24021
              if (shiftNcc <= 0.691) {
                if (avgErr <= 7.4845) {
                  if (avgErr <= 7.4498) {
                    if (xyRatio <= 1.2823) {
                      return 3;
                    }
                    else {  // if xyRatio > 1.2823
                      return 3;
                    }
                  }
                  else {  // if avgErr > 7.4498
                    return 0;
                  }
                }
                else {  // if avgErr > 7.4845
                  if (shiftStd <= 0.22063) {
                    if (area <= 4913) {
                      return 3;
                    }
                    else {  // if area > 4913
                      return 3;
                    }
                  }
                  else {  // if shiftStd > 0.22063
                    if (area <= 4864) {
                      return 0;
                    }
                    else {  // if area > 4864
                      return 3;
                    }
                  }
                }
              }
              else {  // if shiftNcc > 0.691
                return 1;
              }
            }
          }
        }
        else {  // if xyRatio > 1.3614
          if (shiftStd <= 0.0074091) {
            if (avgErr <= 0.20645) {
              if (shiftAvg <= 3.8008e-05) {
                return 3;
              }
              else {  // if shiftAvg > 3.8008e-05
                return 1;
              }
            }
            else {  // if avgErr > 0.20645
              return 3;
            }
          }
          else {  // if shiftStd > 0.0074091
            if (shiftNcc <= 0.084326) {
              if (shiftAvg <= 0.014205) {
                if (avgErr <= 4.591) {
                  if (maxEdgeLength <= 81.5) {
                    if (area <= 4439.5) {
                      return 3;
                    }
                    else {  // if area > 4439.5
                      return 2;
                    }
                  }
                  else {  // if maxEdgeLength > 81.5
                    if (shiftNcc <= -0.03304) {
                      return 1;
                    }
                    else {  // if shiftNcc > -0.03304
                      return 3;
                    }
                  }
                }
                else {  // if avgErr > 4.591
                  if (avgErr <= 4.9726) {
                    if (area <= 5490) {
                      return 0;
                    }
                    else {  // if area > 5490
                      return 1;
                    }
                  }
                  else {  // if avgErr > 4.9726
                    if (shiftNcc <= -0.0032291) {
                      return 3;
                    }
                    else {  // if shiftNcc > -0.0032291
                      return 3;
                    }
                  }
                }
              }
              else {  // if shiftAvg > 0.014205
                if (avgErr <= 1.2599) {
                  if (shiftNcc <= 0.04623) {
                    return 2;
                  }
                  else {  // if shiftNcc > 0.04623
                    return 1;
                  }
                }
                else {  // if avgErr > 1.2599
                  if (shiftNcc <= 0.019018) {
                    if (avgErr <= 14.551) {
                      return 3;
                    }
                    else {  // if avgErr > 14.551
                      return 1;
                    }
                  }
                  else {  // if shiftNcc > 0.019018
                    if (avgErr <= 1.6561) {
                      return 3;
                    }
                    else {  // if avgErr > 1.6561
                      return 3;
                    }
                  }
                }
              }
            }
            else {  // if shiftNcc > 0.084326
              if (avgErr <= 7.6987) {
                if (shiftAvg <= 0.0044541) {
                  if (area <= 5458) {
                    if (avgErr <= 5.4313) {
                      return 3;
                    }
                    else {  // if avgErr > 5.4313
                      return 0;
                    }
                  }
                  else {  // if area > 5458
                    if (shiftStd <= 0.092603) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.092603
                      return 1;
                    }
                  }
                }
                else {  // if shiftAvg > 0.0044541
                  if (shiftStd <= 0.2084) {
                    if (xyRatio <= 1.3688) {
                      return 0;
                    }
                    else {  // if xyRatio > 1.3688
                      return 3;
                    }
                  }
                  else {  // if shiftStd > 0.2084
                    if (avgErr <= 4.278) {
                      return 2;
                    }
                    else {  // if avgErr > 4.278
                      return 0;
                    }
                  }
                }
              }
              else {  // if avgErr > 7.6987
                if (shiftNcc <= 0.089216) {
                  if (shiftAvg <= 0.019897) {
                    return 0;
                  }
                  else {  // if shiftAvg > 0.019897
                    return 1;
                  }
                }
                else {  // if shiftNcc > 0.089216
                  if (shiftAvg <= 0.05482) {
                    if (xyRatio <= 1.857) {
                      return 3;
                    }
                    else {  // if xyRatio > 1.857
                      return 3;
                    }
                  }
                  else {  // if shiftAvg > 0.05482
                    return 1;
                  }
                }
              }
            }
          }
        }
      }
      else {  // if shiftAvg > 0.055593
        if (avgErr <= 11.598) {
          if (shiftNcc <= 0.5499) {
            if (area <= 5183.5) {
              if (avgErr <= 7.1118) {
                if (avgErr <= 5.9891) {
                  if (xyRatio <= 0.85714) {
                    return 0;
                  }
                  else {  // if xyRatio > 0.85714
                    if (shiftStd <= 0.15323) {
                      return 3;
                    }
                    else {  // if shiftStd > 0.15323
                      return 3;
                    }
                  }
                }
                else {  // if avgErr > 5.9891
                  if (xyRatio <= 1.3596) {
                    if (avgErr <= 7.0358) {
                      return 3;
                    }
                    else {  // if avgErr > 7.0358
                      return 2;
                    }
                  }
                  else {  // if xyRatio > 1.3596
                    if (avgErr <= 6.0278) {
                      return 1;
                    }
                    else {  // if avgErr > 6.0278
                      return 0;
                    }
                  }
                }
              }
              else {  // if avgErr > 7.1118
                if (shiftStd <= 0.36103) {
                  if (maxEdgeLength <= 82.5) {
                    if (maxEdgeLength <= 71.5) {
                      return 3;
                    }
                    else {  // if maxEdgeLength > 71.5
                      return 3;
                    }
                  }
                  else {  // if maxEdgeLength > 82.5
                    if (shiftNcc <= 0.47902) {
                      return 3;
                    }
                    else {  // if shiftNcc > 0.47902
                      return 0;
                    }
                  }
                }
                else {  // if shiftStd > 0.36103
                  if (area <= 4700.5) {
                    if (shiftAvg <= 0.40242) {
                      return 3;
                    }
                    else {  // if shiftAvg > 0.40242
                      return 0;
                    }
                  }
                  else {  // if area > 4700.5
                    if (shiftStd <= 0.38151) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.38151
                      return 3;
                    }
                  }
                }
              }
            }
            else {  // if area > 5183.5
              if (area <= 5224.5) {
                if (shiftNcc <= 0.40148) {
                  if (shiftStd <= 0.082087) {
                    return 2;
                  }
                  else {  // if shiftStd > 0.082087
                    if (shiftAvg <= 0.4132) {
                      return 1;
                    }
                    else {  // if shiftAvg > 0.4132
                      return 3;
                    }
                  }
                }
                else {  // if shiftNcc > 0.40148
                  if (avgErr <= 8.5055) {
                    return 0;
                  }
                  else {  // if avgErr > 8.5055
                    return 3;
                  }
                }
              }
              else {  // if area > 5224.5
                if (avgErr <= 11.508) {
                  if (avgErr <= 9.0786) {
                    if (shiftNcc <= 0.38495) {
                      return 3;
                    }
                    else {  // if shiftNcc > 0.38495
                      return 3;
                    }
                  }
                  else {  // if avgErr > 9.0786
                    if (shiftNcc <= -0.0014168) {
                      return 1;
                    }
                    else {  // if shiftNcc > -0.0014168
                      return 3;
                    }
                  }
                }
                else {  // if avgErr > 11.508
                  if (area <= 5747) {
                    return 1;
                  }
                  else {  // if area > 5747
                    return 0;
                  }
                }
              }
            }
          }
          else {  // if shiftNcc > 0.5499
            if (shiftAvg <= 0.18363) {
              if (avgErr <= 7.7226) {
                if (shiftStd <= 0.067457) {
                  if (shiftNcc <= 0.90521) {
                    if (avgErr <= 4.7433) {
                      return 3;
                    }
                    else {  // if avgErr > 4.7433
                      return 3;
                    }
                  }
                  else {  // if shiftNcc > 0.90521
                    if (maxEdgeLength <= 86.5) {
                      return 1;
                    }
                    else {  // if maxEdgeLength > 86.5
                      return 3;
                    }
                  }
                }
                else {  // if shiftStd > 0.067457
                  if (avgErr <= 6.7683) {
                    if (xyRatio <= 1.3143) {
                      return 3;
                    }
                    else {  // if xyRatio > 1.3143
                      return 0;
                    }
                  }
                  else {  // if avgErr > 6.7683
                    if (area <= 4969) {
                      return 0;
                    }
                    else {  // if area > 4969
                      return 0;
                    }
                  }
                }
              }
              else {  // if avgErr > 7.7226
                if (area <= 4386) {
                  return 0;
                }
                else {  // if area > 4386
                  if (maxEdgeLength <= 75.5) {
                    return 3;
                  }
                  else {  // if maxEdgeLength > 75.5
                    if (maxEdgeLength <= 76.5) {
                      return 1;
                    }
                    else {  // if maxEdgeLength > 76.5
                      return 3;
                    }
                  }
                }
              }
            }
            else {  // if shiftAvg > 0.18363
              if (avgErr <= 5.6279) {
                if (maxEdgeLength <= 96.5) {
                  if (maxEdgeLength <= 95.5) {
                    if (xyRatio <= 0.93215) {
                      return 0;
                    }
                    else {  // if xyRatio > 0.93215
                      return 3;
                    }
                  }
                  else {  // if maxEdgeLength > 95.5
                    return 0;
                  }
                }
                else {  // if maxEdgeLength > 96.5
                  if (shiftStd <= 0.078091) {
                    return 0;
                  }
                  else {  // if shiftStd > 0.078091
                    if (avgErr <= 2.5003) {
                      return 1;
                    }
                    else {  // if avgErr > 2.5003
                      return 3;
                    }
                  }
                }
              }
              else {  // if avgErr > 5.6279
                if (shiftStd <= 0.29785) {
                  if (area <= 5365.5) {
                    if (area <= 5332.5) {
                      return 0;
                    }
                    else {  // if area > 5332.5
                      return 3;
                    }
                  }
                  else {  // if area > 5365.5
                    if (shiftAvg <= 0.19015) {
                      return 1;
                    }
                    else {  // if shiftAvg > 0.19015
                      return 0;
                    }
                  }
                }
                else {  // if shiftStd > 0.29785
                  if (avgErr <= 6.6665) {
                    return 1;
                  }
                  else {  // if avgErr > 6.6665
                    if (shiftAvg <= 0.5305) {
                      return 3;
                    }
                    else {  // if shiftAvg > 0.5305
                      return 3;
                    }
                  }
                }
              }
            }
          }
        }
        else {  // if avgErr > 11.598
          if (shiftNcc <= 0.71515) {
            if (area <= 5790.5) {
              if (shiftNcc <= 0.71413) {
                if (area <= 4518) {
                  if (shiftStd <= 0.10674) {
                    if (avgErr <= 13.308) {
                      return 0;
                    }
                    else {  // if avgErr > 13.308
                      return 2;
                    }
                  }
                  else {  // if shiftStd > 0.10674
                    if (area <= 4350.5) {
                      return 1;
                    }
                    else {  // if area > 4350.5
                      return 3;
                    }
                  }
                }
                else {  // if area > 4518
                  if (shiftStd <= 0.07764) {
                    if (area <= 5775.5) {
                      return 3;
                    }
                    else {  // if area > 5775.5
                      return 1;
                    }
                  }
                  else {  // if shiftStd > 0.07764
                    if (shiftStd <= 0.083384) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.083384
                      return 3;
                    }
                  }
                }
              }
              else {  // if shiftNcc > 0.71413
                if (shiftAvg <= 0.11864) {
                  return 0;
                }
                else {  // if shiftAvg > 0.11864
                  return 1;
                }
              }
            }
            else {  // if area > 5790.5
              if (shiftAvg <= 0.078917) {
                return 3;
              }
              else {  // if shiftAvg > 0.078917
                if (shiftStd <= 0.18986) {
                  if (shiftNcc <= 0.64325) {
                    return 2;
                  }
                  else {  // if shiftNcc > 0.64325
                    return 1;
                  }
                }
                else {  // if shiftStd > 0.18986
                  if (shiftStd <= 4.2352) {
                    return 3;
                  }
                  else {  // if shiftStd > 4.2352
                    return 2;
                  }
                }
              }
            }
          }
          else {  // if shiftNcc > 0.71515
            if (shiftStd <= 18.167) {
              if (avgErr <= 18.644) {
                if (avgErr <= 13.38) {
                  if (avgErr <= 13.313) {
                    if (area <= 5829.5) {
                      return 3;
                    }
                    else {  // if area > 5829.5
                      return 0;
                    }
                  }
                  else {  // if avgErr > 13.313
                    return 0;
                  }
                }
                else {  // if avgErr > 13.38
                  if (shiftStd <= 0.35924) {
                    if (maxEdgeLength <= 75.5) {
                      return 3;
                    }
                    else {  // if maxEdgeLength > 75.5
                      return 3;
                    }
                  }
                  else {  // if shiftStd > 0.35924
                    if (avgErr <= 16.688) {
                      return 3;
                    }
                    else {  // if avgErr > 16.688
                      return 2;
                    }
                  }
                }
              }
              else {  // if avgErr > 18.644
                return 1;
              }
            }
            else {  // if shiftStd > 18.167
              return 1;
            }
          }
        }
      }
    }
    else {  // if area > 5875
      if (maxEdgeLength <= 112.5) {
        if (avgErr <= 11.785) {
          if (shiftNcc <= 0.33333) {
            if (shiftStd <= 0.016232) {
              if (avgErr <= 3.2596) {
                if (maxEdgeLength <= 81.5) {
                  return 2;
                }
                else {  // if maxEdgeLength > 81.5
                  return 4;
                }
              }
              else {  // if avgErr > 3.2596
                if (maxEdgeLength <= 90) {
                  return 4;
                }
                else {  // if maxEdgeLength > 90
                  if (maxEdgeLength <= 92.5) {
                    return 0;
                  }
                  else {  // if maxEdgeLength > 92.5
                    if (shiftAvg <= 0.00069009) {
                      return 1;
                    }
                    else {  // if shiftAvg > 0.00069009
                      return 2;
                    }
                  }
                }
              }
            }
            else {  // if shiftStd > 0.016232
              if (xyRatio <= 1.1384) {
                if (avgErr <= 2.157) {
                  if (shiftAvg <= 0.01647) {
                    return 4;
                  }
                  else {  // if shiftAvg > 0.01647
                    if (shiftAvg <= 0.024067) {
                      return 2;
                    }
                    else {  // if shiftAvg > 0.024067
                      return 1;
                    }
                  }
                }
                else {  // if avgErr > 2.157
                  if (shiftNcc <= 0.21449) {
                    if (avgErr <= 11.761) {
                      return 4;
                    }
                    else {  // if avgErr > 11.761
                      return 0;
                    }
                  }
                  else {  // if shiftNcc > 0.21449
                    if (avgErr <= 6.8009) {
                      return 4;
                    }
                    else {  // if avgErr > 6.8009
                      return 4;
                    }
                  }
                }
              }
              else {  // if xyRatio > 1.1384
                if (maxEdgeLength <= 102.5) {
                  if (shiftNcc <= 0.17041) {
                    if (avgErr <= 1.9339) {
                      return 4;
                    }
                    else {  // if avgErr > 1.9339
                      return 4;
                    }
                  }
                  else {  // if shiftNcc > 0.17041
                    if (shiftNcc <= 0.18336) {
                      return 1;
                    }
                    else {  // if shiftNcc > 0.18336
                      return 4;
                    }
                  }
                }
                else {  // if maxEdgeLength > 102.5
                  if (shiftStd <= 0.34201) {
                    if (shiftNcc <= 0.034732) {
                      return 4;
                    }
                    else {  // if shiftNcc > 0.034732
                      return 4;
                    }
                  }
                  else {  // if shiftStd > 0.34201
                    if (area <= 9210) {
                      return 0;
                    }
                    else {  // if area > 9210
                      return 4;
                    }
                  }
                }
              }
            }
          }
          else {  // if shiftNcc > 0.33333
            if (xyRatio <= 1.22) {
              if (area <= 8188.5) {
                if (shiftNcc <= 0.62861) {
                  if (shiftStd <= 0.092666) {
                    if (shiftStd <= 0.021308) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.021308
                      return 4;
                    }
                  }
                  else {  // if shiftStd > 0.092666
                    if (maxEdgeLength <= 79) {
                      return 0;
                    }
                    else {  // if maxEdgeLength > 79
                      return 4;
                    }
                  }
                }
                else {  // if shiftNcc > 0.62861
                  if (xyRatio <= 1.0119) {
                    if (avgErr <= 4.7781) {
                      return 4;
                    }
                    else {  // if avgErr > 4.7781
                      return 4;
                    }
                  }
                  else {  // if xyRatio > 1.0119
                    if (area <= 5988) {
                      return 0;
                    }
                    else {  // if area > 5988
                      return 4;
                    }
                  }
                }
              }
              else {  // if area > 8188.5
                if (avgErr <= 4.4844) {
                  return 1;
                }
                else {  // if avgErr > 4.4844
                  if (shiftNcc <= 0.8129) {
                    if (xyRatio <= 1.1492) {
                      return 4;
                    }
                    else {  // if xyRatio > 1.1492
                      return 0;
                    }
                  }
                  else {  // if shiftNcc > 0.8129
                    if (shiftAvg <= 0.6673) {
                      return 0;
                    }
                    else {  // if shiftAvg > 0.6673
                      return 1;
                    }
                  }
                }
              }
            }
            else {  // if xyRatio > 1.22
              if (avgErr <= 4.9249) {
                if (shiftStd <= 0.10532) {
                  if (area <= 6575.5) {
                    if (avgErr <= 1.3482) {
                      return 2;
                    }
                    else {  // if avgErr > 1.3482
                      return 4;
                    }
                  }
                  else {  // if area > 6575.5
                    if (xyRatio <= 1.2603) {
                      return 0;
                    }
                    else {  // if xyRatio > 1.2603
                      return 4;
                    }
                  }
                }
                else {  // if shiftStd > 0.10532
                  if (area <= 6177.5) {
                    if (shiftNcc <= 0.8252) {
                      return 4;
                    }
                    else {  // if shiftNcc > 0.8252
                      return 0;
                    }
                  }
                  else {  // if area > 6177.5
                    if (shiftNcc <= 0.87173) {
                      return 0;
                    }
                    else {  // if shiftNcc > 0.87173
                      return 4;
                    }
                  }
                }
              }
              else {  // if avgErr > 4.9249
                if (shiftAvg <= 1.0245) {
                  if (shiftNcc <= 0.65275) {
                    if (shiftStd <= 0.096057) {
                      return 4;
                    }
                    else {  // if shiftStd > 0.096057
                      return 0;
                    }
                  }
                  else {  // if shiftNcc > 0.65275
                    if (xyRatio <= 1.3007) {
                      return 0;
                    }
                    else {  // if xyRatio > 1.3007
                      return 0;
                    }
                  }
                }
                else {  // if shiftAvg > 1.0245
                  if (avgErr <= 7.3755) {
                    if (shiftAvg <= 1.556) {
                      return 2;
                    }
                    else {  // if shiftAvg > 1.556
                      return 4;
                    }
                  }
                  else {  // if avgErr > 7.3755
                    return 4;
                  }
                }
              }
            }
          }
        }
        else {  // if avgErr > 11.785
          if (avgErr <= 13.95) {
            if (xyRatio <= 1.4191) {
              if (shiftAvg <= 0.001093) {
                if (area <= 6643.5) {
                  return 1;
                }
                else {  // if area > 6643.5
                  return 0;
                }
              }
              else {  // if shiftAvg > 0.001093
                if (area <= 9791) {
                  if (avgErr <= 13.909) {
                    if (shiftStd <= 0.035932) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.035932
                      return 4;
                    }
                  }
                  else {  // if avgErr > 13.909
                    if (shiftStd <= 0.091827) {
                      return 2;
                    }
                    else {  // if shiftStd > 0.091827
                      return 0;
                    }
                  }
                }
                else {  // if area > 9791
                  if (shiftStd <= 0.34797) {
                    if (maxEdgeLength <= 104) {
                      return 2;
                    }
                    else {  // if maxEdgeLength > 104
                      return 4;
                    }
                  }
                  else {  // if shiftStd > 0.34797
                    if (avgErr <= 12.54) {
                      return 0;
                    }
                    else {  // if avgErr > 12.54
                      return 1;
                    }
                  }
                }
              }
            }
            else {  // if xyRatio > 1.4191
              if (avgErr <= 12.726) {
                if (shiftNcc <= 0.66253) {
                  if (maxEdgeLength <= 96.5) {
                    return 1;
                  }
                  else {  // if maxEdgeLength > 96.5
                    if (avgErr <= 12.347) {
                      return 4;
                    }
                    else {  // if avgErr > 12.347
                      return 0;
                    }
                  }
                }
                else {  // if shiftNcc > 0.66253
                  if (shiftStd <= 0.41844) {
                    if (avgErr <= 12.247) {
                      return 0;
                    }
                    else {  // if avgErr > 12.247
                      return 0;
                    }
                  }
                  else {  // if shiftStd > 0.41844
                    return 2;
                  }
                }
              }
              else {  // if avgErr > 12.726
                if (xyRatio <= 1.4445) {
                  if (shiftNcc <= 0.72155) {
                    if (xyRatio <= 1.4239) {
                      return 4;
                    }
                    else {  // if xyRatio > 1.4239
                      return 0;
                    }
                  }
                  else {  // if shiftNcc > 0.72155
                    if (avgErr <= 13.426) {
                      return 4;
                    }
                    else {  // if avgErr > 13.426
                      return 1;
                    }
                  }
                }
                else {  // if xyRatio > 1.4445
                  if (shiftNcc <= 0.29215) {
                    if (shiftNcc <= 0.075718) {
                      return 4;
                    }
                    else {  // if shiftNcc > 0.075718
                      return 0;
                    }
                  }
                  else {  // if shiftNcc > 0.29215
                    return 4;
                  }
                }
              }
            }
          }
          else {  // if avgErr > 13.95
            if (area <= 6495) {
              if (shiftNcc <= -0.017086) {
                if (shiftAvg <= 0.049216) {
                  return 0;
                }
                else {  // if shiftAvg > 0.049216
                  return 2;
                }
              }
              else {  // if shiftNcc > -0.017086
                if (xyRatio <= 1.4135) {
                  if (shiftStd <= 2.8667) {
                    if (area <= 6479) {
                      return 4;
                    }
                    else {  // if area > 6479
                      return 1;
                    }
                  }
                  else {  // if shiftStd > 2.8667
                    return 1;
                  }
                }
                else {  // if xyRatio > 1.4135
                  if (shiftAvg <= 0.050634) {
                    return 1;
                  }
                  else {  // if shiftAvg > 0.050634
                    if (shiftStd <= 0.03403) {
                      return 2;
                    }
                    else {  // if shiftStd > 0.03403
                      return 4;
                    }
                  }
                }
              }
            }
            else {  // if area > 6495
              if (xyRatio <= 1.2063) {
                if (shiftStd <= 0.037009) {
                  if (shiftStd <= 0.036166) {
                    return 4;
                  }
                  else {  // if shiftStd > 0.036166
                    return 2;
                  }
                }
                else {  // if shiftStd > 0.037009
                  return 4;
                }
              }
              else {  // if xyRatio > 1.2063
                if (xyRatio <= 1.2092) {
                  return 0;
                }
                else {  // if xyRatio > 1.2092
                  if (xyRatio <= 1.2148) {
                    if (maxEdgeLength <= 91.5) {
                      return 1;
                    }
                    else {  // if maxEdgeLength > 91.5
                      return 4;
                    }
                  }
                  else {  // if xyRatio > 1.2148
                    if (area <= 7147) {
                      return 4;
                    }
                    else {  // if area > 7147
                      return 4;
                    }
                  }
                }
              }
            }
          }
        }
      }
      else {  // if maxEdgeLength > 112.5
        if (area <= 16260) {
          if (shiftNcc <= 0.3518) {
            if (maxEdgeLength <= 139.5) {
              if (xyRatio <= 1.1968) {
                if (shiftAvg <= 0.46118) {
                  if (shiftStd <= 0.21449) {
                    if (area <= 10538) {
                      return 4;
                    }
                    else {  // if area > 10538
                      return 0;
                    }
                  }
                  else {  // if shiftStd > 0.21449
                    return 4;
                  }
                }
                else {  // if shiftAvg > 0.46118
                  if (avgErr <= 7.2697) {
                    return 4;
                  }
                  else {  // if avgErr > 7.2697
                    return 2;
                  }
                }
              }
              else {  // if xyRatio > 1.1968
                if (avgErr <= 3.5999) {
                  if (area <= 6124) {
                    return 2;
                  }
                  else {  // if area > 6124
                    return 4;
                  }
                }
                else {  // if avgErr > 3.5999
                  if (area <= 9210) {
                    if (shiftStd <= 0.39392) {
                      return 4;
                    }
                    else {  // if shiftStd > 0.39392
                      return 0;
                    }
                  }
                  else {  // if area > 9210
                    if (shiftStd <= 0.54185) {
                      return 4;
                    }
                    else {  // if shiftStd > 0.54185
                      return 4;
                    }
                  }
                }
              }
            }
            else {  // if maxEdgeLength > 139.5
              if (shiftNcc <= 0.11456) {
                if (avgErr <= 10.468) {
                  if (xyRatio <= 1.7742) {
                    if (area <= 13632) {
                      return 0;
                    }
                    else {  // if area > 13632
                      return 4;
                    }
                  }
                  else {  // if xyRatio > 1.7742
                    return 4;
                  }
                }
                else {  // if avgErr > 10.468
                  if (maxEdgeLength <= 161.5) {
                    if (shiftStd <= 7.2246) {
                      return 0;
                    }
                    else {  // if shiftStd > 7.2246
                      return 1;
                    }
                  }
                  else {  // if maxEdgeLength > 161.5
                    if (avgErr <= 15.477) {
                      return 1;
                    }
                    else {  // if avgErr > 15.477
                      return 4;
                    }
                  }
                }
              }
              else {  // if shiftNcc > 0.11456
                if (avgErr <= 5.302) {
                  if (area <= 11693) {
                    return 4;
                  }
                  else {  // if area > 11693
                    return 1;
                  }
                }
                else {  // if avgErr > 5.302
                  if (xyRatio <= 1.8606) {
                    if (shiftStd <= 0.46034) {
                      return 0;
                    }
                    else {  // if shiftStd > 0.46034
                      return 0;
                    }
                  }
                  else {  // if xyRatio > 1.8606
                    if (avgErr <= 22.148) {
                      return 0;
                    }
                    else {  // if avgErr > 22.148
                      return 1;
                    }
                  }
                }
              }
            }
          }
          else {  // if shiftNcc > 0.3518
            if (xyRatio <= 1.3037) {
              if (shiftAvg <= 0.2449) {
                if (xyRatio <= 1.2472) {
                  if (avgErr <= 5.4622) {
                    if (area <= 12761) {
                      return 1;
                    }
                    else {  // if area > 12761
                      return 0;
                    }
                  }
                  else {  // if avgErr > 5.4622
                    if (area <= 16165) {
                      return 4;
                    }
                    else {  // if area > 16165
                      return 0;
                    }
                  }
                }
                else {  // if xyRatio > 1.2472
                  if (shiftAvg <= 0.11974) {
                    if (shiftAvg <= 0.053843) {
                      return 4;
                    }
                    else {  // if shiftAvg > 0.053843
                      return 1;
                    }
                  }
                  else {  // if shiftAvg > 0.11974
                    return 0;
                  }
                }
              }
              else {  // if shiftAvg > 0.2449
                if (xyRatio <= 1.1225) {
                  if (avgErr <= 6.3498) {
                    if (xyRatio <= 1.0343) {
                      return 2;
                    }
                    else {  // if xyRatio > 1.0343
                      return 1;
                    }
                  }
                  else {  // if avgErr > 6.3498
                    if (shiftAvg <= 0.50788) {
                      return 4;
                    }
                    else {  // if shiftAvg > 0.50788
                      return 1;
                    }
                  }
                }
                else {  // if xyRatio > 1.1225
                  if (shiftStd <= 1.5038) {
                    if (xyRatio <= 1.3005) {
                      return 0;
                    }
                    else {  // if xyRatio > 1.3005
                      return 4;
                    }
                  }
                  else {  // if shiftStd > 1.5038
                    if (avgErr <= 9.5357) {
                      return 3;
                    }
                    else {  // if avgErr > 9.5357
                      return 1;
                    }
                  }
                }
              }
            }
            else {  // if xyRatio > 1.3037
              if (avgErr <= 5.0807) {
                if (area <= 10900) {
                  if (shiftAvg <= 0.093884) {
                    if (avgErr <= 4.9874) {
                      return 0;
                    }
                    else {  // if avgErr > 4.9874
                      return 4;
                    }
                  }
                  else {  // if shiftAvg > 0.093884
                    if (maxEdgeLength <= 119.5) {
                      return 4;
                    }
                    else {  // if maxEdgeLength > 119.5
                      return 4;
                    }
                  }
                }
                else {  // if area > 10900
                  if (shiftStd <= 0.14501) {
                    if (shiftStd <= 0.10355) {
                      return 1;
                    }
                    else {  // if shiftStd > 0.10355
                      return 2;
                    }
                  }
                  else {  // if shiftStd > 0.14501
                    if (shiftStd <= 0.8942) {
                      return 4;
                    }
                    else {  // if shiftStd > 0.8942
                      return 0;
                    }
                  }
                }
              }
              else {  // if avgErr > 5.0807
                if (area <= 9029) {
                  if (avgErr <= 15.073) {
                    if (shiftAvg <= 1.8732) {
                      return 0;
                    }
                    else {  // if shiftAvg > 1.8732
                      return 4;
                    }
                  }
                  else {  // if avgErr > 15.073
                    if (shiftAvg <= 2.0067) {
                      return 4;
                    }
                    else {  // if shiftAvg > 2.0067
                      return 0;
                    }
                  }
                }
                else {  // if area > 9029
                  if (area <= 10410) {
                    if (shiftNcc <= 0.81954) {
                      return 0;
                    }
                    else {  // if shiftNcc > 0.81954
                      return 1;
                    }
                  }
                  else {  // if area > 10410
                    if (avgErr <= 6.3345) {
                      return 0;
                    }
                    else {  // if avgErr > 6.3345
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
        else {  // if area > 16260
          if (area <= 39052) {
            if (area <= 25766) {
              if (xyRatio <= 1.0536) {
                return 4;
              }
              else {  // if xyRatio > 1.0536
                if (area <= 17636) {
                  if (shiftNcc <= 0.08972) {
                    if (maxEdgeLength <= 153.5) {
                      return 4;
                    }
                    else {  // if maxEdgeLength > 153.5
                      return 1;
                    }
                  }
                  else {  // if shiftNcc > 0.08972
                    if (avgErr <= 14.14) {
                      return 1;
                    }
                    else {  // if avgErr > 14.14
                      return 1;
                    }
                  }
                }
                else {  // if area > 17636
                  if (xyRatio <= 1.4526) {
                    if (area <= 23050) {
                      return 1;
                    }
                    else {  // if area > 23050
                      return 4;
                    }
                  }
                  else {  // if xyRatio > 1.4526
                    if (avgErr <= 8.0188) {
                      return 0;
                    }
                    else {  // if avgErr > 8.0188
                      return 1;
                    }
                  }
                }
              }
            }
            else {  // if area > 25766
              if (area <= 36869) {
                if (xyRatio <= 1.1932) {
                  if (shiftNcc <= 0.96729) {
                    return 4;
                  }
                  else {  // if shiftNcc > 0.96729
                    if (maxEdgeLength <= 181) {
                      return 0;
                    }
                    else {  // if maxEdgeLength > 181
                      return 4;
                    }
                  }
                }
                else {  // if xyRatio > 1.1932
                  if (area <= 29986) {
                    if (avgErr <= 16.853) {
                      return 0;
                    }
                    else {  // if avgErr > 16.853
                      return 0;
                    }
                  }
                  else {  // if area > 29986
                    if (xyRatio <= 1.3716) {
                      return 4;
                    }
                    else {  // if xyRatio > 1.3716
                      return 0;
                    }
                  }
                }
              }
              else {  // if area > 36869
                if (area <= 38065) {
                  if (shiftNcc <= 0.90522) {
                    if (area <= 36985) {
                      return 1;
                    }
                    else {  // if area > 36985
                      return 4;
                    }
                  }
                  else {  // if shiftNcc > 0.90522
                    if (avgErr <= 10.786) {
                      return 4;
                    }
                    else {  // if avgErr > 10.786
                      return 2;
                    }
                  }
                }
                else {  // if area > 38065
                  if (shiftNcc <= 0.94118) {
                    if (xyRatio <= 1.5651) {
                      return 1;
                    }
                    else {  // if xyRatio > 1.5651
                      return 1;
                    }
                  }
                  else {  // if shiftNcc > 0.94118
                    if (shiftAvg <= 10.147) {
                      return 1;
                    }
                    else {  // if shiftAvg > 10.147
                      return 2;
                    }
                  }
                }
              }
            }
          }
          else {  // if area > 39052
            if (area <= 47844) {
              if (maxEdgeLength <= 245.5) {
                if (shiftNcc <= 0.16935) {
                  return 2;
                }
                else {  // if shiftNcc > 0.16935
                  if (area <= 42562) {
                    if (area <= 41448) {
                      return 4;
                    }
                    else {  // if area > 41448
                      return 1;
                    }
                  }
                  else {  // if area > 42562
                    if (avgErr <= 19.275) {
                      return 4;
                    }
                    else {  // if avgErr > 19.275
                      return 0;
                    }
                  }
                }
              }
              else {  // if maxEdgeLength > 245.5
                if (maxEdgeLength <= 319) {
                  if (shiftAvg <= 58.49) {
                    if (shiftNcc <= 0.93659) {
                      return 4;
                    }
                    else {  // if shiftNcc > 0.93659
                      return 1;
                    }
                  }
                  else {  // if shiftAvg > 58.49
                    return 1;
                  }
                }
                else {  // if maxEdgeLength > 319
                  if (area <= 44000) {
                    return 1;
                  }
                  else {  // if area > 44000
                    return 2;
                  }
                }
              }
            }
            else {  // if area > 47844
              if (xyRatio <= 1.7286) {
                if (avgErr <= 17.195) {
                  if (shiftAvg <= 90.588) {
                    if (shiftStd <= 27.714) {
                      return 4;
                    }
                    else {  // if shiftStd > 27.714
                      return 1;
                    }
                  }
                  else {  // if shiftAvg > 90.588
                    return 1;
                  }
                }
                else {  // if avgErr > 17.195
                  return 3;
                }
              }
              else {  // if xyRatio > 1.7286
                if (xyRatio <= 1.7431) {
                  return 1;
                }
                else {  // if xyRatio > 1.7431
                  if (avgErr <= 7.992) {
                    return 1;
                  }
                  else {  // if avgErr > 7.992
                    if (shiftAvg <= 65.109) {
                      return 4;
                    }
                    else {  // if shiftAvg > 65.109
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

} // namespace rm
