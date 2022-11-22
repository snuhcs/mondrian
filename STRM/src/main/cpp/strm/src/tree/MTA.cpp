#include "strm/tree/MTA.hpp"

namespace rm {

int MTA(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr, float confidence) {
  if (confidence <= 0.60725) {
    if (confidence <= 0.38758) {
      if (maxEdgeLength <= 20.294) {
        if (maxEdgeLength <= 17.054) {
          if (area <= 96.905) {
            if (confidence <= 0.3012) {
              if (avgErr <= 4.8987) {
                if (shiftAvg <= 0.081018) {
                  if (xyRatio <= 0.75885) {
                    if (area <= 95.807) {
                      return 4;
                    } else {  // if area > 95.807
                      return 4;
                    }
                  } else {  // if xyRatio > 0.75885
                    if (xyRatio <= 0.75937) {
                      return 3;
                    } else {  // if xyRatio > 0.75937
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.081018
                  if (xyRatio <= 0.44125) {
                    if (confidence <= 0.12847) {
                      return 4;
                    } else {  // if confidence > 0.12847
                      return 3;
                    }
                  } else {  // if xyRatio > 0.44125
                    if (confidence <= 0.26524) {
                      return 4;
                    } else {  // if confidence > 0.26524
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 4.8987
                if (confidence <= 0.21014) {
                  if (avgErr <= 4.9086) {
                    return 3;
                  } else {  // if avgErr > 4.9086
                    if (area <= 76.836) {
                      return 4;
                    } else {  // if area > 76.836
                      return 4;
                    }
                  }
                } else {  // if confidence > 0.21014
                  if (shiftAvg <= 20.372) {
                    if (confidence <= 0.21038) {
                      return 3;
                    } else {  // if confidence > 0.21038
                      return 4;
                    }
                  } else {  // if shiftAvg > 20.372
                    return 3;
                  }
                }
              }
            } else {  // if confidence > 0.3012
              if (avgErr <= 4.7533) {
                if (avgErr <= 0.00041667) {
                  if (confidence <= 0.32836) {
                    return 4;
                  } else {  // if confidence > 0.32836
                    if (confidence <= 0.37785) {
                      return 3;
                    } else {  // if confidence > 0.37785
                      return 4;
                    }
                  }
                } else {  // if avgErr > 0.00041667
                  if (maxEdgeLength <= 8.0613) {
                    if (confidence <= 0.34499) {
                      return 4;
                    } else {  // if confidence > 0.34499
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 8.0613
                    if (xyRatio <= 1.3493) {
                      return 4;
                    } else {  // if xyRatio > 1.3493
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 4.7533
                if (maxEdgeLength <= 15.296) {
                  if (shiftAvg <= 0.0022352) {
                    if (confidence <= 0.31992) {
                      return 4;
                    } else {  // if confidence > 0.31992
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.0022352
                    if (confidence <= 0.36862) {
                      return 4;
                    } else {  // if confidence > 0.36862
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 15.296
                  if (area <= 90.698) {
                    return 3;
                  } else {  // if area > 90.698
                    if (area <= 92.551) {
                      return 4;
                    } else {  // if area > 92.551
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 96.905
            if (confidence <= 0.25159) {
              if (area <= 146.85) {
                if (shiftAvg <= 0.00060511) {
                  if (xyRatio <= 0.64981) {
                    if (xyRatio <= 0.39546) {
                      return 4;
                    } else {  // if xyRatio > 0.39546
                      return 4;
                    }
                  } else {  // if xyRatio > 0.64981
                    if (maxEdgeLength <= 12.022) {
                      return 4;
                    } else {  // if maxEdgeLength > 12.022
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.00060511
                  if (confidence <= 0.19847) {
                    if (area <= 116.82) {
                      return 4;
                    } else {  // if area > 116.82
                      return 4;
                    }
                  } else {  // if confidence > 0.19847
                    if (shiftAvg <= 0.01784) {
                      return 4;
                    } else {  // if shiftAvg > 0.01784
                      return 4;
                    }
                  }
                }
              } else {  // if area > 146.85
                if (maxEdgeLength <= 13.313) {
                  if (shiftAvg <= 0.036027) {
                    if (xyRatio <= 0.89649) {
                      return 4;
                    } else {  // if xyRatio > 0.89649
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.036027
                    if (shiftStd <= 0.00062909) {
                      return 4;
                    } else {  // if shiftStd > 0.00062909
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 13.313
                  if (area <= 278.14) {
                    if (avgErr <= 0.043056) {
                      return 4;
                    } else {  // if avgErr > 0.043056
                      return 4;
                    }
                  } else {  // if area > 278.14
                    if (confidence <= 0.17171) {
                      return 3;
                    } else {  // if confidence > 0.17171
                      return 4;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.25159
              if (avgErr <= 2.0369) {
                if (maxEdgeLength <= 15.642) {
                  if (xyRatio <= 0.98388) {
                    if (xyRatio <= 0.60555) {
                      return 4;
                    } else {  // if xyRatio > 0.60555
                      return 4;
                    }
                  } else {  // if xyRatio > 0.98388
                    return 4;
                  }
                } else {  // if maxEdgeLength > 15.642
                  if (shiftAvg <= 0.0059952) {
                    if (xyRatio <= 0.40017) {
                      return 3;
                    } else {  // if xyRatio > 0.40017
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.0059952
                    if (xyRatio <= 0.55927) {
                      return 3;
                    } else {  // if xyRatio > 0.55927
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 2.0369
                if (shiftAvg <= 0.29504) {
                  if (confidence <= 0.3195) {
                    if (area <= 130.7) {
                      return 4;
                    } else {  // if area > 130.7
                      return 4;
                    }
                  } else {  // if confidence > 0.3195
                    if (xyRatio <= 0.98422) {
                      return 4;
                    } else {  // if xyRatio > 0.98422
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.29504
                  if (xyRatio <= 0.76875) {
                    if (shiftAvg <= 1.2524) {
                      return 4;
                    } else {  // if shiftAvg > 1.2524
                      return 4;
                    }
                  } else {  // if xyRatio > 0.76875
                    if (shiftStd <= 0.067994) {
                      return 4;
                    } else {  // if shiftStd > 0.067994
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 17.054
          if (xyRatio <= 0.97706) {
            if (confidence <= 0.27808) {
              if (confidence <= 0.21639) {
                if (xyRatio <= 0.95163) {
                  if (maxEdgeLength <= 17.444) {
                    if (area <= 238.44) {
                      return 4;
                    } else {  // if area > 238.44
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 17.444
                    if (shiftAvg <= 0.00090951) {
                      return 4;
                    } else {  // if shiftAvg > 0.00090951
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.95163
                  if (maxEdgeLength <= 17.617) {
                    if (avgErr <= 0.63411) {
                      return 4;
                    } else {  // if avgErr > 0.63411
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 17.617
                    if (shiftNcc <= -0.22133) {
                      return 3;
                    } else {  // if shiftNcc > -0.22133
                      return 4;
                    }
                  }
                }
              } else {  // if confidence > 0.21639
                if (maxEdgeLength <= 17.706) {
                  if (shiftAvg <= 0.018036) {
                    if (xyRatio <= 0.31757) {
                      return 3;
                    } else {  // if xyRatio > 0.31757
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.018036
                    if (maxEdgeLength <= 17.056) {
                      return 3;
                    } else {  // if maxEdgeLength > 17.056
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 17.706
                  if (shiftStd <= 0.35398) {
                    if (area <= 186.06) {
                      return 4;
                    } else {  // if area > 186.06
                      return 4;
                    }
                  } else {  // if shiftStd > 0.35398
                    if (shiftAvg <= 0.45685) {
                      return 3;
                    } else {  // if shiftAvg > 0.45685
                      return 4;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.27808
              if (avgErr <= 1.0979) {
                if (maxEdgeLength <= 19.33) {
                  if (maxEdgeLength <= 18.918) {
                    return 4;
                  } else {  // if maxEdgeLength > 18.918
                    if (shiftAvg <= 8.8925e-07) {
                      return 3;
                    } else {  // if shiftAvg > 8.8925e-07
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 19.33
                  if (area <= 195.96) {
                    if (area <= 145.11) {
                      return 4;
                    } else {  // if area > 145.11
                      return 3;
                    }
                  } else {  // if area > 195.96
                    if (shiftNcc <= 0.6763) {
                      return 4;
                    } else {  // if shiftNcc > 0.6763
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 1.0979
                if (xyRatio <= 0.55777) {
                  if (avgErr <= 9.6204) {
                    if (avgErr <= 5.0346) {
                      return 4;
                    } else {  // if avgErr > 5.0346
                      return 4;
                    }
                  } else {  // if avgErr > 9.6204
                    if (maxEdgeLength <= 19.294) {
                      return 4;
                    } else {  // if maxEdgeLength > 19.294
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.55777
                  if (xyRatio <= 0.78788) {
                    if (avgErr <= 14.702) {
                      return 4;
                    } else {  // if avgErr > 14.702
                      return 4;
                    }
                  } else {  // if xyRatio > 0.78788
                    if (confidence <= 0.29132) {
                      return 4;
                    } else {  // if confidence > 0.29132
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.97706
            if (avgErr <= 3.8084) {
              if (xyRatio <= 1.1115) {
                if (confidence <= 0.22029) {
                  if (area <= 334.84) {
                    if (maxEdgeLength <= 17.64) {
                      return 3;
                    } else {  // if maxEdgeLength > 17.64
                      return 3;
                    }
                  } else {  // if area > 334.84
                    if (area <= 390.92) {
                      return 4;
                    } else {  // if area > 390.92
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.22029
                  if (confidence <= 0.23901) {
                    if (maxEdgeLength <= 17.901) {
                      return 4;
                    } else {  // if maxEdgeLength > 17.901
                      return 3;
                    }
                  } else {  // if confidence > 0.23901
                    return 4;
                  }
                }
              } else {  // if xyRatio > 1.1115
                if (confidence <= 0.11335) {
                  if (shiftStd <= 7.2198e-05) {
                    if (confidence <= 0.11073) {
                      return 4;
                    } else {  // if confidence > 0.11073
                      return 3;
                    }
                  } else {  // if shiftStd > 7.2198e-05
                    if (shiftAvg <= 9.0571e-05) {
                      return 4;
                    } else {  // if shiftAvg > 9.0571e-05
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.11335
                  if (shiftStd <= 0.095115) {
                    if (maxEdgeLength <= 20.27) {
                      return 4;
                    } else {  // if maxEdgeLength > 20.27
                      return 3;
                    }
                  } else {  // if shiftStd > 0.095115
                    return 3;
                  }
                }
              }
            } else {  // if avgErr > 3.8084
              if (xyRatio <= 0.97836) {
                if (maxEdgeLength <= 18.997) {
                  if (area <= 311.53) {
                    if (shiftNcc <= 0.41071) {
                      return 4;
                    } else {  // if shiftNcc > 0.41071
                      return 3;
                    }
                  } else {  // if area > 311.53
                    return 4;
                  }
                } else {  // if maxEdgeLength > 18.997
                  return 3;
                }
              } else {  // if xyRatio > 0.97836
                if (confidence <= 0.36724) {
                  if (xyRatio <= 1.4847) {
                    if (shiftNcc <= 0.99996) {
                      return 4;
                    } else {  // if shiftNcc > 0.99996
                      return 3;
                    }
                  } else {  // if xyRatio > 1.4847
                    if (confidence <= 0.32496) {
                      return 4;
                    } else {  // if confidence > 0.32496
                      return 4;
                    }
                  }
                } else {  // if confidence > 0.36724
                  if (confidence <= 0.36828) {
                    return 3;
                  } else {  // if confidence > 0.36828
                    if (avgErr <= 18.11) {
                      return 4;
                    } else {  // if avgErr > 18.11
                      return 3;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 20.294
        if (maxEdgeLength <= 230.08) {
          if (confidence <= 0.25072) {
            if (shiftAvg <= 0.00011268) {
              if (maxEdgeLength <= 36.088) {
                if (area <= 477.54) {
                  if (xyRatio <= 0.66713) {
                    if (xyRatio <= 0.43377) {
                      return 4;
                    } else {  // if xyRatio > 0.43377
                      return 4;
                    }
                  } else {  // if xyRatio > 0.66713
                    if (avgErr <= 0.0029271) {
                      return 3;
                    } else {  // if avgErr > 0.0029271
                      return 4;
                    }
                  }
                } else {  // if area > 477.54
                  if (maxEdgeLength <= 29.841) {
                    if (avgErr <= 0.62147) {
                      return 3;
                    } else {  // if avgErr > 0.62147
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 29.841
                    if (xyRatio <= 0.8952) {
                      return 2;
                    } else {  // if xyRatio > 0.8952
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 36.088
                if (area <= 548.18) {
                  if (xyRatio <= 0.30017) {
                    if (xyRatio <= 0.27695) {
                      return 3;
                    } else {  // if xyRatio > 0.27695
                      return 4;
                    }
                  } else {  // if xyRatio > 0.30017
                    if (xyRatio <= 0.32059) {
                      return 4;
                    } else {  // if xyRatio > 0.32059
                      return 4;
                    }
                  }
                } else {  // if area > 548.18
                  if (avgErr <= 5.1109) {
                    if (avgErr <= 1.5432e-05) {
                      return 4;
                    } else {  // if avgErr > 1.5432e-05
                      return 4;
                    }
                  } else {  // if avgErr > 5.1109
                    if (shiftNcc <= -0.034085) {
                      return 2;
                    } else {  // if shiftNcc > -0.034085
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.00011268
              if (maxEdgeLength <= 28.773) {
                if (confidence <= 0.1722) {
                  if (maxEdgeLength <= 24.108) {
                    if (avgErr <= 1.4256) {
                      return 4;
                    } else {  // if avgErr > 1.4256
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 24.108
                    if (area <= 294.79) {
                      return 4;
                    } else {  // if area > 294.79
                      return 4;
                    }
                  }
                } else {  // if confidence > 0.1722
                  if (avgErr <= 9.8723) {
                    if (area <= 261.31) {
                      return 4;
                    } else {  // if area > 261.31
                      return 4;
                    }
                  } else {  // if avgErr > 9.8723
                    if (shiftAvg <= 0.51572) {
                      return 4;
                    } else {  // if shiftAvg > 0.51572
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 28.773
                if (shiftAvg <= 0.33175) {
                  if (xyRatio <= 1.1217) {
                    if (xyRatio <= 0.36264) {
                      return 4;
                    } else {  // if xyRatio > 0.36264
                      return 4;
                    }
                  } else {  // if xyRatio > 1.1217
                    if (area <= 1411.9) {
                      return 4;
                    } else {  // if area > 1411.9
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.33175
                  if (xyRatio <= 1.0861) {
                    if (xyRatio <= 0.3186) {
                      return 4;
                    } else {  // if xyRatio > 0.3186
                      return 4;
                    }
                  } else {  // if xyRatio > 1.0861
                    if (area <= 1263.2) {
                      return 4;
                    } else {  // if area > 1263.2
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.25072
            if (area <= 454.92) {
              if (xyRatio <= 0.70838) {
                if (avgErr <= 8.092) {
                  if (area <= 194.36) {
                    if (avgErr <= 2.0824) {
                      return 3;
                    } else {  // if avgErr > 2.0824
                      return 4;
                    }
                  } else {  // if area > 194.36
                    if (confidence <= 0.31345) {
                      return 4;
                    } else {  // if confidence > 0.31345
                      return 4;
                    }
                  }
                } else {  // if avgErr > 8.092
                  if (confidence <= 0.33111) {
                    if (area <= 265.85) {
                      return 4;
                    } else {  // if area > 265.85
                      return 4;
                    }
                  } else {  // if confidence > 0.33111
                    if (area <= 329.34) {
                      return 4;
                    } else {  // if area > 329.34
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 0.70838
                if (avgErr <= 3.6111) {
                  if (maxEdgeLength <= 20.936) {
                    if (avgErr <= 2.6656) {
                      return 4;
                    } else {  // if avgErr > 2.6656
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 20.936
                    if (maxEdgeLength <= 22.977) {
                      return 4;
                    } else {  // if maxEdgeLength > 22.977
                      return 4;
                    }
                  }
                } else {  // if avgErr > 3.6111
                  if (xyRatio <= 1.056) {
                    if (shiftNcc <= -0.1698) {
                      return 3;
                    } else {  // if shiftNcc > -0.1698
                      return 4;
                    }
                  } else {  // if xyRatio > 1.056
                    if (shiftAvg <= 5.1287) {
                      return 4;
                    } else {  // if shiftAvg > 5.1287
                      return 3;
                    }
                  }
                }
              }
            } else {  // if area > 454.92
              if (xyRatio <= 1.2339) {
                if (maxEdgeLength <= 46.526) {
                  if (xyRatio <= 0.6981) {
                    if (xyRatio <= 0.35936) {
                      return 4;
                    } else {  // if xyRatio > 0.35936
                      return 4;
                    }
                  } else {  // if xyRatio > 0.6981
                    if (avgErr <= 12.74) {
                      return 4;
                    } else {  // if avgErr > 12.74
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 46.526
                  if (avgErr <= 2.9858) {
                    if (shiftNcc <= 0.88287) {
                      return 4;
                    } else {  // if shiftNcc > 0.88287
                      return 2;
                    }
                  } else {  // if avgErr > 2.9858
                    if (shiftAvg <= 1.0309) {
                      return 4;
                    } else {  // if shiftAvg > 1.0309
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 1.2339
                if (shiftStd <= 0.0020816) {
                  return 4;
                } else {  // if shiftStd > 0.0020816
                  if (shiftNcc <= 0.99853) {
                    if (maxEdgeLength <= 137.29) {
                      return 4;
                    } else {  // if maxEdgeLength > 137.29
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.99853
                    if (shiftNcc <= 0.99918) {
                      return 3;
                    } else {  // if shiftNcc > 0.99918
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 230.08
          if (maxEdgeLength <= 256.73) {
            if (avgErr <= 0.65039) {
              if (xyRatio <= 2.8191) {
                if (confidence <= 0.14489) {
                  if (xyRatio <= 2.7945) {
                    return 3;
                  } else {  // if xyRatio > 2.7945
                    if (shiftAvg <= 5.2275e-07) {
                      return 2;
                    } else {  // if shiftAvg > 5.2275e-07
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.14489
                  if (xyRatio <= 2.7754) {
                    if (area <= 21522) {
                      return 2;
                    } else {  // if area > 21522
                      return 4;
                    }
                  } else {  // if xyRatio > 2.7754
                    if (xyRatio <= 2.809) {
                      return 2;
                    } else {  // if xyRatio > 2.809
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 2.8191
                if (confidence <= 0.22437) {
                  if (shiftNcc <= 0.10299) {
                    if (maxEdgeLength <= 254.64) {
                      return 2;
                    } else {  // if maxEdgeLength > 254.64
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.10299
                    if (area <= 22691) {
                      return 2;
                    } else {  // if area > 22691
                      return 4;
                    }
                  }
                } else {  // if confidence > 0.22437
                  if (area <= 23249) {
                    if (avgErr <= 0.088171) {
                      return 2;
                    } else {  // if avgErr > 0.088171
                      return 2;
                    }
                  } else {  // if area > 23249
                    if (confidence <= 0.24638) {
                      return 2;
                    } else {  // if confidence > 0.24638
                      return 4;
                    }
                  }
                }
              }
            } else {  // if avgErr > 0.65039
              if (shiftNcc <= 0.1996) {
                if (confidence <= 0.11628) {
                  return 3;
                } else {  // if confidence > 0.11628
                  if (xyRatio <= 2.7978) {
                    if (shiftAvg <= 0.0036599) {
                      return 3;
                    } else {  // if shiftAvg > 0.0036599
                      return 2;
                    }
                  } else {  // if xyRatio > 2.7978
                    if (avgErr <= 2.0038) {
                      return 2;
                    } else {  // if avgErr > 2.0038
                      return 3;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.1996
                if (maxEdgeLength <= 246.45) {
                  if (maxEdgeLength <= 242.79) {
                    return 4;
                  } else {  // if maxEdgeLength > 242.79
                    if (shiftNcc <= 0.60952) {
                      return 2;
                    } else {  // if shiftNcc > 0.60952
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 246.45
                  if (shiftAvg <= 1.2091) {
                    return 3;
                  } else {  // if shiftAvg > 1.2091
                    if (area <= 32037) {
                      return 2;
                    } else {  // if area > 32037
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 256.73
            if (confidence <= 0.15993) {
              if (maxEdgeLength <= 261.21) {
                if (xyRatio <= 2.8665) {
                  if (avgErr <= 0.62906) {
                    if (confidence <= 0.157) {
                      return 2;
                    } else {  // if confidence > 0.157
                      return 2;
                    }
                  } else {  // if avgErr > 0.62906
                    if (maxEdgeLength <= 260.03) {
                      return 3;
                    } else {  // if maxEdgeLength > 260.03
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 2.8665
                  if (confidence <= 0.14218) {
                    if (shiftStd <= 1.152e-05) {
                      return 2;
                    } else {  // if shiftStd > 1.152e-05
                      return 4;
                    }
                  } else {  // if confidence > 0.14218
                    return 4;
                  }
                }
              } else {  // if maxEdgeLength > 261.21
                if (confidence <= 0.14224) {
                  if (avgErr <= 0.30464) {
                    if (confidence <= 0.13884) {
                      return 2;
                    } else {  // if confidence > 0.13884
                      return 2;
                    }
                  } else {  // if avgErr > 0.30464
                    if (shiftNcc <= 0.22443) {
                      return 3;
                    } else {  // if shiftNcc > 0.22443
                      return 4;
                    }
                  }
                } else {  // if confidence > 0.14224
                  if (area <= 24059) {
                    if (confidence <= 0.15751) {
                      return 4;
                    } else {  // if confidence > 0.15751
                      return 2;
                    }
                  } else {  // if area > 24059
                    if (shiftNcc <= 0.022575) {
                      return 2;
                    } else {  // if shiftNcc > 0.022575
                      return 3;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.15993
              if (xyRatio <= 2.8525) {
                if (confidence <= 0.19537) {
                  if (shiftStd <= 8.7036e-06) {
                    if (shiftNcc <= -0.023541) {
                      return 4;
                    } else {  // if shiftNcc > -0.023541
                      return 2;
                    }
                  } else {  // if shiftStd > 8.7036e-06
                    if (xyRatio <= 2.8238) {
                      return 2;
                    } else {  // if xyRatio > 2.8238
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.19537
                  if (shiftAvg <= 4.0031e-06) {
                    if (maxEdgeLength <= 257.31) {
                      return 2;
                    } else {  // if maxEdgeLength > 257.31
                      return 4;
                    }
                  } else {  // if shiftAvg > 4.0031e-06
                    if (area <= 23633) {
                      return 2;
                    } else {  // if area > 23633
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 2.8525
                if (area <= 23659) {
                  if (confidence <= 0.16787) {
                    if (xyRatio <= 2.8622) {
                      return 2;
                    } else {  // if xyRatio > 2.8622
                      return 4;
                    }
                  } else {  // if confidence > 0.16787
                    if (confidence <= 0.17725) {
                      return 4;
                    } else {  // if confidence > 0.17725
                      return 4;
                    }
                  }
                } else {  // if area > 23659
                  if (maxEdgeLength <= 262.26) {
                    if (avgErr <= 0.43994) {
                      return 4;
                    } else {  // if avgErr > 0.43994
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 262.26
                    return 2;
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if confidence > 0.38758
      if (area <= 348.17) {
        if (confidence <= 0.49562) {
          if (maxEdgeLength <= 18.929) {
            if (avgErr <= 1.5066) {
              if (maxEdgeLength <= 13.463) {
                if (xyRatio <= 1.0281) {
                  if (shiftNcc <= 0.54925) {
                    if (confidence <= 0.38976) {
                      return 3;
                    } else {  // if confidence > 0.38976
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.54925
                    return 3;
                  }
                } else {  // if xyRatio > 1.0281
                  return 3;
                }
              } else {  // if maxEdgeLength > 13.463
                if (shiftAvg <= 0.0017674) {
                  if (avgErr <= 0.0092535) {
                    if (xyRatio <= 0.44297) {
                      return 3;
                    } else {  // if xyRatio > 0.44297
                      return 4;
                    }
                  } else {  // if avgErr > 0.0092535
                    if (avgErr <= 1.4175) {
                      return 4;
                    } else {  // if avgErr > 1.4175
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.0017674
                  if (shiftNcc <= 0.79438) {
                    if (confidence <= 0.43099) {
                      return 4;
                    } else {  // if confidence > 0.43099
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.79438
                    return 3;
                  }
                }
              }
            } else {  // if avgErr > 1.5066
              if (area <= 194.04) {
                if (area <= 91.365) {
                  if (xyRatio <= 0.7559) {
                    if (avgErr <= 6.8241) {
                      return 4;
                    } else {  // if avgErr > 6.8241
                      return 4;
                    }
                  } else {  // if xyRatio > 0.7559
                    if (xyRatio <= 1.2467) {
                      return 4;
                    } else {  // if xyRatio > 1.2467
                      return 3;
                    }
                  }
                } else {  // if area > 91.365
                  if (shiftAvg <= 0.10084) {
                    if (xyRatio <= 1.0152) {
                      return 4;
                    } else {  // if xyRatio > 1.0152
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.10084
                    if (maxEdgeLength <= 17.005) {
                      return 4;
                    } else {  // if maxEdgeLength > 17.005
                      return 4;
                    }
                  }
                }
              } else {  // if area > 194.04
                if (shiftAvg <= 0.37319) {
                  if (xyRatio <= 0.86743) {
                    if (confidence <= 0.43673) {
                      return 4;
                    } else {  // if confidence > 0.43673
                      return 4;
                    }
                  } else {  // if xyRatio > 0.86743
                    if (area <= 316) {
                      return 4;
                    } else {  // if area > 316
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.37319
                  if (area <= 205.71) {
                    if (shiftAvg <= 0.38788) {
                      return 3;
                    } else {  // if shiftAvg > 0.38788
                      return 4;
                    }
                  } else {  // if area > 205.71
                    if (shiftAvg <= 0.45065) {
                      return 4;
                    } else {  // if shiftAvg > 0.45065
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 18.929
            if (avgErr <= 11.692) {
              if (area <= 213.17) {
                if (avgErr <= 1.5826) {
                  if (xyRatio <= 0.43048) {
                    if (area <= 170.96) {
                      return 3;
                    } else {  // if area > 170.96
                      return 3;
                    }
                  } else {  // if xyRatio > 0.43048
                    if (confidence <= 0.41775) {
                      return 4;
                    } else {  // if confidence > 0.41775
                      return 3;
                    }
                  }
                } else {  // if avgErr > 1.5826
                  if (confidence <= 0.42735) {
                    if (shiftStd <= 0.0036819) {
                      return 3;
                    } else {  // if shiftStd > 0.0036819
                      return 4;
                    }
                  } else {  // if confidence > 0.42735
                    if (xyRatio <= 0.31671) {
                      return 4;
                    } else {  // if xyRatio > 0.31671
                      return 3;
                    }
                  }
                }
              } else {  // if area > 213.17
                if (xyRatio <= 0.61706) {
                  if (xyRatio <= 0.3581) {
                    if (shiftAvg <= 0.056266) {
                      return 4;
                    } else {  // if shiftAvg > 0.056266
                      return 3;
                    }
                  } else {  // if xyRatio > 0.3581
                    if (shiftAvg <= 1.3608) {
                      return 3;
                    } else {  // if shiftAvg > 1.3608
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.61706
                  if (shiftAvg <= 0.11769) {
                    if (xyRatio <= 0.87746) {
                      return 3;
                    } else {  // if xyRatio > 0.87746
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.11769
                    if (avgErr <= 3.9897) {
                      return 3;
                    } else {  // if avgErr > 3.9897
                      return 4;
                    }
                  }
                }
              }
            } else {  // if avgErr > 11.692
              if (confidence <= 0.42295) {
                if (avgErr <= 20.919) {
                  if (xyRatio <= 0.31695) {
                    if (area <= 273.32) {
                      return 4;
                    } else {  // if area > 273.32
                      return 4;
                    }
                  } else {  // if xyRatio > 0.31695
                    if (xyRatio <= 0.63869) {
                      return 4;
                    } else {  // if xyRatio > 0.63869
                      return 4;
                    }
                  }
                } else {  // if avgErr > 20.919
                  if (xyRatio <= 0.46244) {
                    if (xyRatio <= 0.25658) {
                      return 2;
                    } else {  // if xyRatio > 0.25658
                      return 4;
                    }
                  } else {  // if xyRatio > 0.46244
                    if (confidence <= 0.39988) {
                      return 3;
                    } else {  // if confidence > 0.39988
                      return 4;
                    }
                  }
                }
              } else {  // if confidence > 0.42295
                if (xyRatio <= 0.75281) {
                  if (xyRatio <= 0.39349) {
                    if (shiftAvg <= 0.57362) {
                      return 4;
                    } else {  // if shiftAvg > 0.57362
                      return 4;
                    }
                  } else {  // if xyRatio > 0.39349
                    if (avgErr <= 20.762) {
                      return 3;
                    } else {  // if avgErr > 20.762
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.75281
                  if (avgErr <= 15.072) {
                    if (shiftStd <= 0.0086598) {
                      return 4;
                    } else {  // if shiftStd > 0.0086598
                      return 4;
                    }
                  } else {  // if avgErr > 15.072
                    if (area <= 279.2) {
                      return 2;
                    } else {  // if area > 279.2
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.49562
          if (maxEdgeLength <= 19.087) {
            if (area <= 201.37) {
              if (avgErr <= 4.3674) {
                if (xyRatio <= 0.63345) {
                  if (maxEdgeLength <= 17.53) {
                    if (shiftAvg <= 0.00054772) {
                      return 4;
                    } else {  // if shiftAvg > 0.00054772
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 17.53
                    if (avgErr <= 2.3405) {
                      return 4;
                    } else {  // if avgErr > 2.3405
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.63345
                  if (confidence <= 0.54647) {
                    if (avgErr <= 1.9939) {
                      return 3;
                    } else {  // if avgErr > 1.9939
                      return 4;
                    }
                  } else {  // if confidence > 0.54647
                    if (area <= 100.75) {
                      return 4;
                    } else {  // if area > 100.75
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 4.3674
                if (shiftAvg <= 0.10118) {
                  if (xyRatio <= 0.76843) {
                    if (area <= 134.76) {
                      return 3;
                    } else {  // if area > 134.76
                      return 3;
                    }
                  } else {  // if xyRatio > 0.76843
                    if (maxEdgeLength <= 9.9731) {
                      return 4;
                    } else {  // if maxEdgeLength > 9.9731
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.10118
                  if (shiftStd <= 0.0012019) {
                    if (confidence <= 0.50987) {
                      return 4;
                    } else {  // if confidence > 0.50987
                      return 3;
                    }
                  } else {  // if shiftStd > 0.0012019
                    if (avgErr <= 9.519) {
                      return 3;
                    } else {  // if avgErr > 9.519
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 201.37
              if (xyRatio <= 0.73188) {
                if (confidence <= 0.50715) {
                  if (avgErr <= 16.143) {
                    if (maxEdgeLength <= 18.982) {
                      return 4;
                    } else {  // if maxEdgeLength > 18.982
                      return 2;
                    }
                  } else {  // if avgErr > 16.143
                    if (shiftNcc <= 0.82753) {
                      return 2;
                    } else {  // if shiftNcc > 0.82753
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.50715
                  if (shiftNcc <= 0.88261) {
                    if (shiftStd <= 0.018228) {
                      return 3;
                    } else {  // if shiftStd > 0.018228
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.88261
                    if (shiftNcc <= 0.97488) {
                      return 4;
                    } else {  // if shiftNcc > 0.97488
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.73188
                if (shiftAvg <= 0.21198) {
                  if (area <= 251) {
                    if (xyRatio <= 0.73906) {
                      return 4;
                    } else {  // if xyRatio > 0.73906
                      return 3;
                    }
                  } else {  // if area > 251
                    if (shiftNcc <= -0.28654) {
                      return 3;
                    } else {  // if shiftNcc > -0.28654
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.21198
                  if (shiftAvg <= 4.484) {
                    if (maxEdgeLength <= 18.483) {
                      return 4;
                    } else {  // if maxEdgeLength > 18.483
                      return 4;
                    }
                  } else {  // if shiftAvg > 4.484
                    if (avgErr <= 10.538) {
                      return 3;
                    } else {  // if avgErr > 10.538
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 19.087
            if (avgErr <= 12.227) {
              if (maxEdgeLength <= 22.095) {
                if (avgErr <= 1.914) {
                  if (xyRatio <= 0.51954) {
                    if (area <= 153.87) {
                      return 4;
                    } else {  // if area > 153.87
                      return 3;
                    }
                  } else {  // if xyRatio > 0.51954
                    if (shiftAvg <= 0.00025064) {
                      return 2;
                    } else {  // if shiftAvg > 0.00025064
                      return 3;
                    }
                  }
                } else {  // if avgErr > 1.914
                  if (shiftAvg <= 0.03667) {
                    if (area <= 164.91) {
                      return 3;
                    } else {  // if area > 164.91
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.03667
                    if (avgErr <= 4.9916) {
                      return 3;
                    } else {  // if avgErr > 4.9916
                      return 3;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 22.095
                if (xyRatio <= 0.37491) {
                  if (maxEdgeLength <= 28.883) {
                    if (xyRatio <= 0.33095) {
                      return 3;
                    } else {  // if xyRatio > 0.33095
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 28.883
                    if (shiftAvg <= 0.1287) {
                      return 4;
                    } else {  // if shiftAvg > 0.1287
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.37491
                  if (shiftAvg <= 0.11207) {
                    if (avgErr <= 5.7462) {
                      return 3;
                    } else {  // if avgErr > 5.7462
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.11207
                    if (xyRatio <= 0.49466) {
                      return 3;
                    } else {  // if xyRatio > 0.49466
                      return 3;
                    }
                  }
                }
              }
            } else {  // if avgErr > 12.227
              if (area <= 278.23) {
                if (xyRatio <= 0.48736) {
                  if (confidence <= 0.53848) {
                    if (avgErr <= 15.035) {
                      return 3;
                    } else {  // if avgErr > 15.035
                      return 4;
                    }
                  } else {  // if confidence > 0.53848
                    if (avgErr <= 14.853) {
                      return 3;
                    } else {  // if avgErr > 14.853
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.48736
                  if (maxEdgeLength <= 19.743) {
                    if (avgErr <= 13.545) {
                      return 3;
                    } else {  // if avgErr > 13.545
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 19.743
                    if (confidence <= 0.49996) {
                      return 3;
                    } else {  // if confidence > 0.49996
                      return 3;
                    }
                  }
                }
              } else {  // if area > 278.23
                if (avgErr <= 16.175) {
                  if (xyRatio <= 0.82044) {
                    if (shiftAvg <= 0.1492) {
                      return 3;
                    } else {  // if shiftAvg > 0.1492
                      return 3;
                    }
                  } else {  // if xyRatio > 0.82044
                    if (shiftNcc <= 0.92385) {
                      return 4;
                    } else {  // if shiftNcc > 0.92385
                      return 4;
                    }
                  }
                } else {  // if avgErr > 16.175
                  if (confidence <= 0.57262) {
                    if (xyRatio <= 0.33592) {
                      return 4;
                    } else {  // if xyRatio > 0.33592
                      return 3;
                    }
                  } else {  // if confidence > 0.57262
                    if (shiftAvg <= 0.64762) {
                      return 3;
                    } else {  // if shiftAvg > 0.64762
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 348.17
        if (confidence <= 0.51347) {
          if (area <= 492.25) {
            if (xyRatio <= 0.73182) {
              if (avgErr <= 1.5543) {
                if (area <= 415.37) {
                  if (avgErr <= 0.15188) {
                    if (area <= 411.09) {
                      return 3;
                    } else {  // if area > 411.09
                      return 4;
                    }
                  } else {  // if avgErr > 0.15188
                    if (shiftNcc <= 0.29245) {
                      return 4;
                    } else {  // if shiftNcc > 0.29245
                      return 4;
                    }
                  }
                } else {  // if area > 415.37
                  if (xyRatio <= 0.51881) {
                    if (xyRatio <= 0.27838) {
                      return 4;
                    } else {  // if xyRatio > 0.27838
                      return 3;
                    }
                  } else {  // if xyRatio > 0.51881
                    if (shiftAvg <= 2.8238e-05) {
                      return 3;
                    } else {  // if shiftAvg > 2.8238e-05
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 1.5543
                if (xyRatio <= 0.34724) {
                  if (shiftAvg <= 0.37547) {
                    if (avgErr <= 12.158) {
                      return 4;
                    } else {  // if avgErr > 12.158
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.37547
                    if (avgErr <= 5.8111) {
                      return 2;
                    } else {  // if avgErr > 5.8111
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.34724
                  if (confidence <= 0.45729) {
                    if (xyRatio <= 0.64072) {
                      return 4;
                    } else {  // if xyRatio > 0.64072
                      return 4;
                    }
                  } else {  // if confidence > 0.45729
                    if (shiftAvg <= 0.45939) {
                      return 3;
                    } else {  // if shiftAvg > 0.45939
                      return 2;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.73182
              if (area <= 442.13) {
                if (shiftStd <= 0.0020596) {
                  return 4;
                } else {  // if shiftStd > 0.0020596
                  if (avgErr <= 9.5376) {
                    if (maxEdgeLength <= 21.067) {
                      return 3;
                    } else {  // if maxEdgeLength > 21.067
                      return 4;
                    }
                  } else {  // if avgErr > 9.5376
                    if (xyRatio <= 0.82552) {
                      return 4;
                    } else {  // if xyRatio > 0.82552
                      return 4;
                    }
                  }
                }
              } else {  // if area > 442.13
                if (xyRatio <= 1.1351) {
                  if (xyRatio <= 1.0723) {
                    if (maxEdgeLength <= 23.043) {
                      return 4;
                    } else {  // if maxEdgeLength > 23.043
                      return 3;
                    }
                  } else {  // if xyRatio > 1.0723
                    return 3;
                  }
                } else {  // if xyRatio > 1.1351
                  if (shiftAvg <= 1.3368) {
                    if (avgErr <= 19.29) {
                      return 4;
                    } else {  // if avgErr > 19.29
                      return 3;
                    }
                  } else {  // if shiftAvg > 1.3368
                    if (shiftStd <= 0.042189) {
                      return 3;
                    } else {  // if shiftStd > 0.042189
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if area > 492.25
            if (avgErr <= 10.374) {
              if (shiftAvg <= 0.15316) {
                if (maxEdgeLength <= 48.618) {
                  if (area <= 615.7) {
                    if (avgErr <= 1.3548) {
                      return 3;
                    } else {  // if avgErr > 1.3548
                      return 2;
                    }
                  } else {  // if area > 615.7
                    if (xyRatio <= 0.78662) {
                      return 2;
                    } else {  // if xyRatio > 0.78662
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 48.618
                  if (avgErr <= 2.9873) {
                    if (shiftAvg <= 0.082525) {
                      return 4;
                    } else {  // if shiftAvg > 0.082525
                      return 2;
                    }
                  } else {  // if avgErr > 2.9873
                    if (area <= 3271.1) {
                      return 4;
                    } else {  // if area > 3271.1
                      return 2;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.15316
                if (confidence <= 0.4601) {
                  if (shiftAvg <= 0.71074) {
                    if (maxEdgeLength <= 48.134) {
                      return 2;
                    } else {  // if maxEdgeLength > 48.134
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.71074
                    if (maxEdgeLength <= 31.508) {
                      return 4;
                    } else {  // if maxEdgeLength > 31.508
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.4601
                  if (maxEdgeLength <= 28.369) {
                    if (xyRatio <= 0.77615) {
                      return 4;
                    } else {  // if xyRatio > 0.77615
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 28.369
                    if (shiftStd <= 5.9262) {
                      return 2;
                    } else {  // if shiftStd > 5.9262
                      return 4;
                    }
                  }
                }
              }
            } else {  // if avgErr > 10.374
              if (xyRatio <= 0.34916) {
                if (shiftAvg <= 0.55539) {
                  if (xyRatio <= 0.29239) {
                    if (shiftStd <= 0.5168) {
                      return 4;
                    } else {  // if shiftStd > 0.5168
                      return 3;
                    }
                  } else {  // if xyRatio > 0.29239
                    if (avgErr <= 12.446) {
                      return 3;
                    } else {  // if avgErr > 12.446
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.55539
                  if (shiftAvg <= 2.3969) {
                    if (confidence <= 0.41237) {
                      return 2;
                    } else {  // if confidence > 0.41237
                      return 3;
                    }
                  } else {  // if shiftAvg > 2.3969
                    if (shiftStd <= 3.2376) {
                      return 2;
                    } else {  // if shiftStd > 3.2376
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.34916
                if (area <= 2472.3) {
                  if (xyRatio <= 0.83125) {
                    if (confidence <= 0.44909) {
                      return 4;
                    } else {  // if confidence > 0.44909
                      return 2;
                    }
                  } else {  // if xyRatio > 0.83125
                    if (area <= 759.39) {
                      return 4;
                    } else {  // if area > 759.39
                      return 3;
                    }
                  }
                } else {  // if area > 2472.3
                  if (area <= 18192) {
                    if (confidence <= 0.50155) {
                      return 2;
                    } else {  // if confidence > 0.50155
                      return 1;
                    }
                  } else {  // if area > 18192
                    if (shiftAvg <= 70.48) {
                      return 1;
                    } else {  // if shiftAvg > 70.48
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.51347
          if (area <= 503.31) {
            if (avgErr <= 15.762) {
              if (shiftAvg <= 0.17453) {
                if (avgErr <= 6.3118) {
                  if (maxEdgeLength <= 24.117) {
                    if (maxEdgeLength <= 21.716) {
                      return 3;
                    } else {  // if maxEdgeLength > 21.716
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 24.117
                    if (maxEdgeLength <= 27.149) {
                      return 2;
                    } else {  // if maxEdgeLength > 27.149
                      return 3;
                    }
                  }
                } else {  // if avgErr > 6.3118
                  if (avgErr <= 9.7916) {
                    if (maxEdgeLength <= 36.568) {
                      return 2;
                    } else {  // if maxEdgeLength > 36.568
                      return 3;
                    }
                  } else {  // if avgErr > 9.7916
                    if (xyRatio <= 0.31608) {
                      return 4;
                    } else {  // if xyRatio > 0.31608
                      return 3;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.17453
                if (maxEdgeLength <= 27.303) {
                  if (xyRatio <= 0.77323) {
                    if (area <= 348.95) {
                      return 2;
                    } else {  // if area > 348.95
                      return 3;
                    }
                  } else {  // if xyRatio > 0.77323
                    if (confidence <= 0.58576) {
                      return 4;
                    } else {  // if confidence > 0.58576
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 27.303
                  if (avgErr <= 9.1644) {
                    if (shiftAvg <= 0.27787) {
                      return 2;
                    } else {  // if shiftAvg > 0.27787
                      return 2;
                    }
                  } else {  // if avgErr > 9.1644
                    if (maxEdgeLength <= 34.333) {
                      return 2;
                    } else {  // if maxEdgeLength > 34.333
                      return 3;
                    }
                  }
                }
              }
            } else {  // if avgErr > 15.762
              if (xyRatio <= 0.84975) {
                if (xyRatio <= 0.35911) {
                  if (confidence <= 0.57733) {
                    if (confidence <= 0.57362) {
                      return 4;
                    } else {  // if confidence > 0.57362
                      return 4;
                    }
                  } else {  // if confidence > 0.57733
                    if (confidence <= 0.58986) {
                      return 2;
                    } else {  // if confidence > 0.58986
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.35911
                  if (shiftStd <= 0.042235) {
                    if (xyRatio <= 0.64493) {
                      return 3;
                    } else {  // if xyRatio > 0.64493
                      return 4;
                    }
                  } else {  // if shiftStd > 0.042235
                    if (area <= 452.18) {
                      return 3;
                    } else {  // if area > 452.18
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.84975
                if (maxEdgeLength <= 21.357) {
                  if (shiftNcc <= 0.93629) {
                    return 4;
                  } else {  // if shiftNcc > 0.93629
                    if (shiftAvg <= 0.53405) {
                      return 3;
                    } else {  // if shiftAvg > 0.53405
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 21.357
                  if (confidence <= 0.52524) {
                    return 3;
                  } else {  // if confidence > 0.52524
                    if (confidence <= 0.6001) {
                      return 4;
                    } else {  // if confidence > 0.6001
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 503.31
            if (avgErr <= 8.2945) {
              if (maxEdgeLength <= 42.558) {
                if (xyRatio <= 0.58867) {
                  if (shiftAvg <= 0.036819) {
                    if (xyRatio <= 0.54318) {
                      return 2;
                    } else {  // if xyRatio > 0.54318
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.036819
                    if (shiftAvg <= 0.45094) {
                      return 2;
                    } else {  // if shiftAvg > 0.45094
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.58867
                  if (maxEdgeLength <= 40.98) {
                    if (maxEdgeLength <= 35.988) {
                      return 2;
                    } else {  // if maxEdgeLength > 35.988
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 40.98
                    if (shiftNcc <= 0.9752) {
                      return 2;
                    } else {  // if shiftNcc > 0.9752
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 42.558
                if (xyRatio <= 0.42037) {
                  if (shiftNcc <= 0.36328) {
                    if (shiftStd <= 0.0098241) {
                      return 4;
                    } else {  // if shiftStd > 0.0098241
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.36328
                    if (area <= 897.27) {
                      return 2;
                    } else {  // if area > 897.27
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.42037
                  if (xyRatio <= 0.9224) {
                    if (confidence <= 0.60524) {
                      return 2;
                    } else {  // if confidence > 0.60524
                      return 3;
                    }
                  } else {  // if xyRatio > 0.9224
                    if (shiftStd <= 0.15883) {
                      return 2;
                    } else {  // if shiftStd > 0.15883
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgErr > 8.2945
              if (area <= 1757.1) {
                if (xyRatio <= 0.32679) {
                  if (avgErr <= 11.179) {
                    if (maxEdgeLength <= 61.499) {
                      return 3;
                    } else {  // if maxEdgeLength > 61.499
                      return 2;
                    }
                  } else {  // if avgErr > 11.179
                    if (confidence <= 0.5552) {
                      return 4;
                    } else {  // if confidence > 0.5552
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.32679
                  if (maxEdgeLength <= 43.239) {
                    if (xyRatio <= 0.79339) {
                      return 2;
                    } else {  // if xyRatio > 0.79339
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 43.239
                    if (xyRatio <= 0.5134) {
                      return 2;
                    } else {  // if xyRatio > 0.5134
                      return 3;
                    }
                  }
                }
              } else {  // if area > 1757.1
                if (area <= 8379) {
                  if (shiftAvg <= 6.466) {
                    if (shiftStd <= 0.34321) {
                      return 2;
                    } else {  // if shiftStd > 0.34321
                      return 2;
                    }
                  } else {  // if shiftAvg > 6.466
                    if (shiftAvg <= 44.214) {
                      return 2;
                    } else {  // if shiftAvg > 44.214
                      return 4;
                    }
                  }
                } else {  // if area > 8379
                  if (shiftAvg <= 2.0352) {
                    if (confidence <= 0.53448) {
                      return 3;
                    } else {  // if confidence > 0.53448
                      return 1;
                    }
                  } else {  // if shiftAvg > 2.0352
                    if (confidence <= 0.59994) {
                      return 1;
                    } else {  // if confidence > 0.59994
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
  } else {  // if confidence > 0.60725
    if (confidence <= 0.86811) {
      if (maxEdgeLength <= 45.062) {
        if (confidence <= 0.72576) {
          if (maxEdgeLength <= 31.584) {
            if (area <= 167.03) {
              if (shiftAvg <= 0.031396) {
                if (xyRatio <= 0.67559) {
                  if (xyRatio <= 0.33047) {
                    if (area <= 145.55) {
                      return 3;
                    } else {  // if area > 145.55
                      return 3;
                    }
                  } else {  // if xyRatio > 0.33047
                    if (area <= 159.3) {
                      return 3;
                    } else {  // if area > 159.3
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.67559
                  if (area <= 143.16) {
                    if (avgErr <= 4.0167) {
                      return 4;
                    } else {  // if avgErr > 4.0167
                      return 3;
                    }
                  } else {  // if area > 143.16
                    if (avgErr <= 8.6584) {
                      return 3;
                    } else {  // if avgErr > 8.6584
                      return 2;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.031396
                if (area <= 139.4) {
                  if (avgErr <= 5.4458) {
                    if (maxEdgeLength <= 13.983) {
                      return 4;
                    } else {  // if maxEdgeLength > 13.983
                      return 3;
                    }
                  } else {  // if avgErr > 5.4458
                    if (maxEdgeLength <= 18.307) {
                      return 3;
                    } else {  // if maxEdgeLength > 18.307
                      return 3;
                    }
                  }
                } else {  // if area > 139.4
                  if (xyRatio <= 0.69838) {
                    if (shiftAvg <= 0.1992) {
                      return 3;
                    } else {  // if shiftAvg > 0.1992
                      return 2;
                    }
                  } else {  // if xyRatio > 0.69838
                    if (confidence <= 0.64974) {
                      return 3;
                    } else {  // if confidence > 0.64974
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 167.03
              if (confidence <= 0.66453) {
                if (xyRatio <= 0.35984) {
                  if (area <= 250.69) {
                    if (avgErr <= 11.746) {
                      return 3;
                    } else {  // if avgErr > 11.746
                      return 3;
                    }
                  } else {  // if area > 250.69
                    if (shiftAvg <= 0.42035) {
                      return 3;
                    } else {  // if shiftAvg > 0.42035
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.35984
                  if (maxEdgeLength <= 21.799) {
                    if (avgErr <= 4.2626) {
                      return 3;
                    } else {  // if avgErr > 4.2626
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 21.799
                    if (avgErr <= 11.104) {
                      return 2;
                    } else {  // if avgErr > 11.104
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.66453
                if (xyRatio <= 0.35765) {
                  if (area <= 282.98) {
                    if (avgErr <= 2.8461) {
                      return 2;
                    } else {  // if avgErr > 2.8461
                      return 3;
                    }
                  } else {  // if area > 282.98
                    if (avgErr <= 18.262) {
                      return 2;
                    } else {  // if avgErr > 18.262
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.35765
                  if (avgErr <= 10.864) {
                    if (avgErr <= 6.0213) {
                      return 2;
                    } else {  // if avgErr > 6.0213
                      return 2;
                    }
                  } else {  // if avgErr > 10.864
                    if (shiftAvg <= 0.42809) {
                      return 2;
                    } else {  // if shiftAvg > 0.42809
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 31.584
            if (avgErr <= 9.2539) {
              if (confidence <= 0.65247) {
                if (shiftAvg <= 0.24942) {
                  if (area <= 443.04) {
                    if (shiftAvg <= 0.034508) {
                      return 2;
                    } else {  // if shiftAvg > 0.034508
                      return 2;
                    }
                  } else {  // if area > 443.04
                    if (avgErr <= 5.8196) {
                      return 2;
                    } else {  // if avgErr > 5.8196
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.24942
                  if (avgErr <= 7.0968) {
                    if (shiftStd <= 0.80554) {
                      return 2;
                    } else {  // if shiftStd > 0.80554
                      return 3;
                    }
                  } else {  // if avgErr > 7.0968
                    if (shiftNcc <= 0.94443) {
                      return 2;
                    } else {  // if shiftNcc > 0.94443
                      return 2;
                    }
                  }
                }
              } else {  // if confidence > 0.65247
                if (shiftAvg <= 0.076455) {
                  if (maxEdgeLength <= 37.761) {
                    if (shiftStd <= 0.0038079) {
                      return 2;
                    } else {  // if shiftStd > 0.0038079
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 37.761
                    if (avgErr <= 2.2747) {
                      return 4;
                    } else {  // if avgErr > 2.2747
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.076455
                  if (maxEdgeLength <= 38.03) {
                    if (area <= 433.72) {
                      return 2;
                    } else {  // if area > 433.72
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 38.03
                    if (avgErr <= 8.0027) {
                      return 2;
                    } else {  // if avgErr > 8.0027
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgErr > 9.2539
              if (confidence <= 0.67574) {
                if (area <= 535.64) {
                  if (avgErr <= 15.339) {
                    if (maxEdgeLength <= 36.526) {
                      return 2;
                    } else {  // if maxEdgeLength > 36.526
                      return 3;
                    }
                  } else {  // if avgErr > 15.339
                    if (shiftAvg <= 0.59605) {
                      return 3;
                    } else {  // if shiftAvg > 0.59605
                      return 2;
                    }
                  }
                } else {  // if area > 535.64
                  if (area <= 827.49) {
                    if (xyRatio <= 0.36304) {
                      return 2;
                    } else {  // if xyRatio > 0.36304
                      return 2;
                    }
                  } else {  // if area > 827.49
                    if (shiftNcc <= 0.99865) {
                      return 2;
                    } else {  // if shiftNcc > 0.99865
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.67574
                if (area <= 834.99) {
                  if (xyRatio <= 0.37691) {
                    if (avgErr <= 17.637) {
                      return 2;
                    } else {  // if avgErr > 17.637
                      return 2;
                    }
                  } else {  // if xyRatio > 0.37691
                    if (shiftNcc <= 0.99187) {
                      return 2;
                    } else {  // if shiftNcc > 0.99187
                      return 2;
                    }
                  }
                } else {  // if area > 834.99
                  if (avgErr <= 15.97) {
                    if (shiftAvg <= 60.093) {
                      return 2;
                    } else {  // if shiftAvg > 60.093
                      return 2;
                    }
                  } else {  // if avgErr > 15.97
                    if (confidence <= 0.71599) {
                      return 2;
                    } else {  // if confidence > 0.71599
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.72576
          if (confidence <= 0.8033) {
            if (maxEdgeLength <= 31.923) {
              if (maxEdgeLength <= 28.8) {
                if (xyRatio <= 0.34471) {
                  if (shiftAvg <= 0.017725) {
                    if (area <= 227.39) {
                      return 3;
                    } else {  // if area > 227.39
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.017725
                    if (avgErr <= 2.7775) {
                      return 2;
                    } else {  // if avgErr > 2.7775
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.34471
                  if (shiftAvg <= 7.3394e-05) {
                    if (confidence <= 0.73948) {
                      return 2;
                    } else {  // if confidence > 0.73948
                      return 3;
                    }
                  } else {  // if shiftAvg > 7.3394e-05
                    if (xyRatio <= 0.70174) {
                      return 2;
                    } else {  // if xyRatio > 0.70174
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 28.8
                if (confidence <= 0.77219) {
                  if (shiftAvg <= 0.45815) {
                    if (avgErr <= 3.0018) {
                      return 2;
                    } else {  // if avgErr > 3.0018
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.45815
                    if (avgErr <= 10.824) {
                      return 2;
                    } else {  // if avgErr > 10.824
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.77219
                  if (shiftAvg <= 0.37106) {
                    if (avgErr <= 10.05) {
                      return 2;
                    } else {  // if avgErr > 10.05
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.37106
                    if (shiftNcc <= 0.98709) {
                      return 2;
                    } else {  // if shiftNcc > 0.98709
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 31.923
              if (avgErr <= 8.334) {
                if (maxEdgeLength <= 38.098) {
                  if (avgErr <= 5.7163) {
                    if (area <= 433.04) {
                      return 2;
                    } else {  // if area > 433.04
                      return 2;
                    }
                  } else {  // if avgErr > 5.7163
                    if (area <= 617.09) {
                      return 2;
                    } else {  // if area > 617.09
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 38.098
                  if (area <= 576.82) {
                    if (shiftAvg <= 1.6934) {
                      return 2;
                    } else {  // if shiftAvg > 1.6934
                      return 1;
                    }
                  } else {  // if area > 576.82
                    if (avgErr <= 3.79) {
                      return 2;
                    } else {  // if avgErr > 3.79
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 8.334
                if (area <= 826.82) {
                  if (confidence <= 0.76505) {
                    if (avgErr <= 18.418) {
                      return 2;
                    } else {  // if avgErr > 18.418
                      return 2;
                    }
                  } else {  // if confidence > 0.76505
                    if (avgErr <= 17.161) {
                      return 2;
                    } else {  // if avgErr > 17.161
                      return 2;
                    }
                  }
                } else {  // if area > 826.82
                  if (shiftAvg <= 5.4262) {
                    if (avgErr <= 10.32) {
                      return 2;
                    } else {  // if avgErr > 10.32
                      return 2;
                    }
                  } else {  // if shiftAvg > 5.4262
                    if (xyRatio <= 0.94413) {
                      return 1;
                    } else {  // if xyRatio > 0.94413
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.8033
            if (maxEdgeLength <= 36.7) {
              if (maxEdgeLength <= 33.71) {
                if (maxEdgeLength <= 22.376) {
                  if (confidence <= 0.80736) {
                    return 2;
                  } else {  // if confidence > 0.80736
                    return 3;
                  }
                } else {  // if maxEdgeLength > 22.376
                  if (area <= 432.28) {
                    if (shiftStd <= 0.019762) {
                      return 2;
                    } else {  // if shiftStd > 0.019762
                      return 2;
                    }
                  } else {  // if area > 432.28
                    if (avgErr <= 12.862) {
                      return 2;
                    } else {  // if avgErr > 12.862
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 33.71
                if (avgErr <= 6.3911) {
                  if (confidence <= 0.83137) {
                    if (shiftAvg <= 0.024243) {
                      return 2;
                    } else {  // if shiftAvg > 0.024243
                      return 2;
                    }
                  } else {  // if confidence > 0.83137
                    if (maxEdgeLength <= 35.428) {
                      return 2;
                    } else {  // if maxEdgeLength > 35.428
                      return 2;
                    }
                  }
                } else {  // if avgErr > 6.3911
                  if (avgErr <= 10.973) {
                    if (shiftAvg <= 0.27754) {
                      return 2;
                    } else {  // if shiftAvg > 0.27754
                      return 2;
                    }
                  } else {  // if avgErr > 10.973
                    if (area <= 834.16) {
                      return 2;
                    } else {  // if area > 834.16
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 36.7
              if (avgErr <= 9.7331) {
                if (avgErr <= 3.869) {
                  if (maxEdgeLength <= 42.464) {
                    if (confidence <= 0.84895) {
                      return 2;
                    } else {  // if confidence > 0.84895
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 42.464
                    if (xyRatio <= 0.50868) {
                      return 1;
                    } else {  // if xyRatio > 0.50868
                      return 2;
                    }
                  }
                } else {  // if avgErr > 3.869
                  if (confidence <= 0.82973) {
                    if (shiftNcc <= 0.95955) {
                      return 2;
                    } else {  // if shiftNcc > 0.95955
                      return 2;
                    }
                  } else {  // if confidence > 0.82973
                    if (shiftAvg <= 1.1449) {
                      return 2;
                    } else {  // if shiftAvg > 1.1449
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 9.7331
                if (confidence <= 0.84246) {
                  if (maxEdgeLength <= 40.373) {
                    if (shiftAvg <= 1.6432) {
                      return 2;
                    } else {  // if shiftAvg > 1.6432
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 40.373
                    if (area <= 919.19) {
                      return 2;
                    } else {  // if area > 919.19
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.84246
                  if (xyRatio <= 0.45577) {
                    if (area <= 609.92) {
                      return 2;
                    } else {  // if area > 609.92
                      return 2;
                    }
                  } else {  // if xyRatio > 0.45577
                    if (shiftAvg <= 0.027257) {
                      return 1;
                    } else {  // if shiftAvg > 0.027257
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 45.062
        if (confidence <= 0.81446) {
          if (area <= 2185.8) {
            if (confidence <= 0.73254) {
              if (xyRatio <= 0.33233) {
                if (shiftAvg <= 0.35914) {
                  if (confidence <= 0.69786) {
                    if (avgErr <= 10.602) {
                      return 2;
                    } else {  // if avgErr > 10.602
                      return 3;
                    }
                  } else {  // if confidence > 0.69786
                    if (area <= 549.64) {
                      return 3;
                    } else {  // if area > 549.64
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.35914
                  if (maxEdgeLength <= 49.871) {
                    if (xyRatio <= 0.257) {
                      return 3;
                    } else {  // if xyRatio > 0.257
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 49.871
                    if (shiftAvg <= 1.1612) {
                      return 2;
                    } else {  // if shiftAvg > 1.1612
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.33233
                if (avgErr <= 10.711) {
                  if (xyRatio <= 0.51038) {
                    if (avgErr <= 4.635) {
                      return 2;
                    } else {  // if avgErr > 4.635
                      return 2;
                    }
                  } else {  // if xyRatio > 0.51038
                    if (avgErr <= 7.0403) {
                      return 2;
                    } else {  // if avgErr > 7.0403
                      return 2;
                    }
                  }
                } else {  // if avgErr > 10.711
                  if (confidence <= 0.64911) {
                    if (shiftStd <= 0.11513) {
                      return 2;
                    } else {  // if shiftStd > 0.11513
                      return 2;
                    }
                  } else {  // if confidence > 0.64911
                    if (area <= 960.96) {
                      return 2;
                    } else {  // if area > 960.96
                      return 2;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.73254
              if (maxEdgeLength <= 49.921) {
                if (avgErr <= 4.1712) {
                  if (xyRatio <= 0.49448) {
                    if (shiftAvg <= 0.51988) {
                      return 1;
                    } else {  // if shiftAvg > 0.51988
                      return 2;
                    }
                  } else {  // if xyRatio > 0.49448
                    if (confidence <= 0.73548) {
                      return 1;
                    } else {  // if confidence > 0.73548
                      return 2;
                    }
                  }
                } else {  // if avgErr > 4.1712
                  if (maxEdgeLength <= 47.047) {
                    if (shiftAvg <= 0.2817) {
                      return 2;
                    } else {  // if shiftAvg > 0.2817
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 47.047
                    if (confidence <= 0.77891) {
                      return 2;
                    } else {  // if confidence > 0.77891
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 49.921
                if (xyRatio <= 0.2985) {
                  if (avgErr <= 12.027) {
                    if (shiftAvg <= 0.13834) {
                      return 2;
                    } else {  // if shiftAvg > 0.13834
                      return 2;
                    }
                  } else {  // if avgErr > 12.027
                    if (confidence <= 0.75947) {
                      return 2;
                    } else {  // if confidence > 0.75947
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.2985
                  if (avgErr <= 15.317) {
                    if (avgErr <= 3.0366) {
                      return 1;
                    } else {  // if avgErr > 3.0366
                      return 2;
                    }
                  } else {  // if avgErr > 15.317
                    if (xyRatio <= 0.36096) {
                      return 2;
                    } else {  // if xyRatio > 0.36096
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 2185.8
            if (area <= 5633.6) {
              if (confidence <= 0.74638) {
                if (avgErr <= 13.577) {
                  if (xyRatio <= 0.39629) {
                    if (shiftAvg <= 7.5487) {
                      return 2;
                    } else {  // if shiftAvg > 7.5487
                      return 1;
                    }
                  } else {  // if xyRatio > 0.39629
                    if (shiftAvg <= 8.2856) {
                      return 2;
                    } else {  // if shiftAvg > 8.2856
                      return 2;
                    }
                  }
                } else {  // if avgErr > 13.577
                  if (xyRatio <= 0.29882) {
                    if (confidence <= 0.72662) {
                      return 2;
                    } else {  // if confidence > 0.72662
                      return 2;
                    }
                  } else {  // if xyRatio > 0.29882
                    if (maxEdgeLength <= 64.315) {
                      return 2;
                    } else {  // if maxEdgeLength > 64.315
                      return 2;
                    }
                  }
                }
              } else {  // if confidence > 0.74638
                if (avgErr <= 8.4161) {
                  if (shiftNcc <= 0.91056) {
                    if (xyRatio <= 0.43819) {
                      return 1;
                    } else {  // if xyRatio > 0.43819
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.91056
                    if (confidence <= 0.77915) {
                      return 1;
                    } else {  // if confidence > 0.77915
                      return 1;
                    }
                  }
                } else {  // if avgErr > 8.4161
                  if (xyRatio <= 0.29212) {
                    if (xyRatio <= 0.25558) {
                      return 2;
                    } else {  // if xyRatio > 0.25558
                      return 2;
                    }
                  } else {  // if xyRatio > 0.29212
                    if (xyRatio <= 0.52619) {
                      return 1;
                    } else {  // if xyRatio > 0.52619
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 5633.6
              if (area <= 13354) {
                if (confidence <= 0.71942) {
                  if (shiftAvg <= 5.7026) {
                    if (maxEdgeLength <= 136.68) {
                      return 1;
                    } else {  // if maxEdgeLength > 136.68
                      return 2;
                    }
                  } else {  // if shiftAvg > 5.7026
                    if (shiftNcc <= 0.72893) {
                      return 2;
                    } else {  // if shiftNcc > 0.72893
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.71942
                  if (xyRatio <= 0.28673) {
                    if (confidence <= 0.77647) {
                      return 2;
                    } else {  // if confidence > 0.77647
                      return 1;
                    }
                  } else {  // if xyRatio > 0.28673
                    if (shiftAvg <= 12.657) {
                      return 1;
                    } else {  // if shiftAvg > 12.657
                      return 1;
                    }
                  }
                }
              } else {  // if area > 13354
                if (avgErr <= 6.6509) {
                  if (shiftStd <= 3.5935) {
                    if (avgErr <= 5.7947) {
                      return 3;
                    } else {  // if avgErr > 5.7947
                      return 0;
                    }
                  } else {  // if shiftStd > 3.5935
                    return 2;
                  }
                } else {  // if avgErr > 6.6509
                  if (xyRatio <= 0.55152) {
                    if (confidence <= 0.74289) {
                      return 1;
                    } else {  // if confidence > 0.74289
                      return 0;
                    }
                  } else {  // if xyRatio > 0.55152
                    if (avgErr <= 18.291) {
                      return 1;
                    } else {  // if avgErr > 18.291
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.81446
          if (area <= 2084.3) {
            if (maxEdgeLength <= 49.423) {
              if (avgErr <= 4.2488) {
                if (xyRatio <= 0.49788) {
                  if (shiftAvg <= 0.12836) {
                    if (area <= 930.45) {
                      return 1;
                    } else {  // if area > 930.45
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.12836
                    if (shiftNcc <= 0.62424) {
                      return 2;
                    } else {  // if shiftNcc > 0.62424
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.49788
                  if (confidence <= 0.86591) {
                    if (area <= 1195.1) {
                      return 2;
                    } else {  // if area > 1195.1
                      return 2;
                    }
                  } else {  // if confidence > 0.86591
                    if (xyRatio <= 0.6538) {
                      return 1;
                    } else {  // if xyRatio > 0.6538
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 4.2488
                if (confidence <= 0.84781) {
                  if (shiftStd <= 0.068181) {
                    if (avgErr <= 7.2063) {
                      return 2;
                    } else {  // if avgErr > 7.2063
                      return 2;
                    }
                  } else {  // if shiftStd > 0.068181
                    if (avgErr <= 8.9851) {
                      return 2;
                    } else {  // if avgErr > 8.9851
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.84781
                  if (xyRatio <= 0.46782) {
                    if (area <= 952.93) {
                      return 2;
                    } else {  // if area > 952.93
                      return 1;
                    }
                  } else {  // if xyRatio > 0.46782
                    if (avgErr <= 7.076) {
                      return 2;
                    } else {  // if avgErr > 7.076
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 49.423
              if (confidence <= 0.84986) {
                if (xyRatio <= 0.35014) {
                  if (shiftNcc <= 0.92683) {
                    if (avgErr <= 4.7871) {
                      return 1;
                    } else {  // if avgErr > 4.7871
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.92683
                    if (confidence <= 0.83242) {
                      return 2;
                    } else {  // if confidence > 0.83242
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.35014
                  if (xyRatio <= 0.46865) {
                    if (avgErr <= 6.6494) {
                      return 1;
                    } else {  // if avgErr > 6.6494
                      return 1;
                    }
                  } else {  // if xyRatio > 0.46865
                    if (shiftAvg <= 1.3107) {
                      return 2;
                    } else {  // if shiftAvg > 1.3107
                      return 1;
                    }
                  }
                }
              } else {  // if confidence > 0.84986
                if (avgErr <= 5.0076) {
                  if (xyRatio <= 0.55549) {
                    if (xyRatio <= 0.31188) {
                      return 2;
                    } else {  // if xyRatio > 0.31188
                      return 1;
                    }
                  } else {  // if xyRatio > 0.55549
                    if (shiftAvg <= 0.26581) {
                      return 1;
                    } else {  // if shiftAvg > 0.26581
                      return 2;
                    }
                  }
                } else {  // if avgErr > 5.0076
                  if (xyRatio <= 0.46554) {
                    if (shiftNcc <= 0.6347) {
                      return 1;
                    } else {  // if shiftNcc > 0.6347
                      return 1;
                    }
                  } else {  // if xyRatio > 0.46554
                    if (shiftAvg <= 1.3932) {
                      return 2;
                    } else {  // if shiftAvg > 1.3932
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 2084.3
            if (area <= 7427.6) {
              if (avgErr <= 8.1711) {
                if (area <= 5264.1) {
                  if (xyRatio <= 1.0405) {
                    if (area <= 2465) {
                      return 1;
                    } else {  // if area > 2465
                      return 1;
                    }
                  } else {  // if xyRatio > 1.0405
                    if (shiftAvg <= 0.94122) {
                      return 2;
                    } else {  // if shiftAvg > 0.94122
                      return 1;
                    }
                  }
                } else {  // if area > 5264.1
                  if (shiftNcc <= 0.41762) {
                    if (shiftStd <= 0.51734) {
                      return 0;
                    } else {  // if shiftStd > 0.51734
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.41762
                    if (shiftStd <= 1.9478) {
                      return 1;
                    } else {  // if shiftStd > 1.9478
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 8.1711
                if (maxEdgeLength <= 73.115) {
                  if (xyRatio <= 0.74278) {
                    if (xyRatio <= 0.51165) {
                      return 1;
                    } else {  // if xyRatio > 0.51165
                      return 2;
                    }
                  } else {  // if xyRatio > 0.74278
                    if (shiftStd <= 0.16082) {
                      return 1;
                    } else {  // if shiftStd > 0.16082
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 73.115
                  if (confidence <= 0.84604) {
                    if (shiftNcc <= 0.54882) {
                      return 1;
                    } else {  // if shiftNcc > 0.54882
                      return 1;
                    }
                  } else {  // if confidence > 0.84604
                    if (avgErr <= 17.349) {
                      return 1;
                    } else {  // if avgErr > 17.349
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 7427.6
              if (maxEdgeLength <= 184.78) {
                if (xyRatio <= 0.37078) {
                  if (confidence <= 0.86724) {
                    if (xyRatio <= 0.35234) {
                      return 1;
                    } else {  // if xyRatio > 0.35234
                      return 1;
                    }
                  } else {  // if confidence > 0.86724
                    return 0;
                  }
                } else {  // if xyRatio > 0.37078
                  if (shiftNcc <= 0.89979) {
                    if (confidence <= 0.82155) {
                      return 0;
                    } else {  // if confidence > 0.82155
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.89979
                    if (maxEdgeLength <= 97.817) {
                      return 2;
                    } else {  // if maxEdgeLength > 97.817
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 184.78
                if (area <= 9399.6) {
                  return 2;
                } else {  // if area > 9399.6
                  if (shiftAvg <= 37.71) {
                    if (area <= 15990) {
                      return 0;
                    } else {  // if area > 15990
                      return 0;
                    }
                  } else {  // if shiftAvg > 37.71
                    if (maxEdgeLength <= 315.3) {
                      return 0;
                    } else {  // if maxEdgeLength > 315.3
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if confidence > 0.86811
      if (confidence <= 0.9205) {
        if (maxEdgeLength <= 88.367) {
          if (confidence <= 0.88737) {
            if (maxEdgeLength <= 45.312) {
              if (xyRatio <= 0.46755) {
                if (shiftAvg <= 0.16643) {
                  if (avgErr <= 3.4273) {
                    if (avgErr <= 1.1616) {
                      return 2;
                    } else {  // if avgErr > 1.1616
                      return 1;
                    }
                  } else {  // if avgErr > 3.4273
                    if (confidence <= 0.87289) {
                      return 2;
                    } else {  // if confidence > 0.87289
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.16643
                  if (avgErr <= 3.8733) {
                    if (xyRatio <= 0.39487) {
                      return 1;
                    } else {  // if xyRatio > 0.39487
                      return 1;
                    }
                  } else {  // if avgErr > 3.8733
                    if (shiftAvg <= 1.5226) {
                      return 2;
                    } else {  // if shiftAvg > 1.5226
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.46755
                if (avgErr <= 11.417) {
                  if (shiftAvg <= 1.3689) {
                    if (shiftAvg <= 0.15068) {
                      return 1;
                    } else {  // if shiftAvg > 0.15068
                      return 2;
                    }
                  } else {  // if shiftAvg > 1.3689
                    if (maxEdgeLength <= 43.712) {
                      return 1;
                    } else {  // if maxEdgeLength > 43.712
                      return 1;
                    }
                  }
                } else {  // if avgErr > 11.417
                  if (shiftAvg <= 1.6173) {
                    if (shiftNcc <= 0.97553) {
                      return 2;
                    } else {  // if shiftNcc > 0.97553
                      return 2;
                    }
                  } else {  // if shiftAvg > 1.6173
                    if (maxEdgeLength <= 43.186) {
                      return 2;
                    } else {  // if maxEdgeLength > 43.186
                      return 1;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 45.312
              if (maxEdgeLength <= 70.385) {
                if (xyRatio <= 0.46323) {
                  if (avgErr <= 4.5812) {
                    if (maxEdgeLength <= 46.742) {
                      return 1;
                    } else {  // if maxEdgeLength > 46.742
                      return 1;
                    }
                  } else {  // if avgErr > 4.5812
                    if (confidence <= 0.87273) {
                      return 1;
                    } else {  // if confidence > 0.87273
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.46323
                  if (shiftAvg <= 1.2188) {
                    if (shiftAvg <= 0.10082) {
                      return 1;
                    } else {  // if shiftAvg > 0.10082
                      return 1;
                    }
                  } else {  // if shiftAvg > 1.2188
                    if (avgErr <= 5.6877) {
                      return 2;
                    } else {  // if avgErr > 5.6877
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 70.385
                if (area <= 2133.9) {
                  if (confidence <= 0.87555) {
                    if (xyRatio <= 0.28774) {
                      return 1;
                    } else {  // if xyRatio > 0.28774
                      return 1;
                    }
                  } else {  // if confidence > 0.87555
                    if (shiftNcc <= 0.47206) {
                      return 1;
                    } else {  // if shiftNcc > 0.47206
                      return 1;
                    }
                  }
                } else {  // if area > 2133.9
                  if (shiftAvg <= 23.692) {
                    if (xyRatio <= 0.50789) {
                      return 1;
                    } else {  // if xyRatio > 0.50789
                      return 1;
                    }
                  } else {  // if shiftAvg > 23.692
                    if (avgErr <= 13.814) {
                      return 0;
                    } else {  // if avgErr > 13.814
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.88737
            if (maxEdgeLength <= 72.898) {
              if (xyRatio <= 0.5076) {
                if (confidence <= 0.89767) {
                  if (shiftAvg <= 0.82231) {
                    if (avgErr <= 4.4343) {
                      return 1;
                    } else {  // if avgErr > 4.4343
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.82231
                    if (maxEdgeLength <= 47.733) {
                      return 2;
                    } else {  // if maxEdgeLength > 47.733
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.89767
                  if (area <= 1838) {
                    if (area <= 738.25) {
                      return 2;
                    } else {  // if area > 738.25
                      return 1;
                    }
                  } else {  // if area > 1838
                    if (shiftAvg <= 0.86542) {
                      return 1;
                    } else {  // if shiftAvg > 0.86542
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.5076
                if (maxEdgeLength <= 45.54) {
                  if (shiftAvg <= 1.5525) {
                    if (shiftAvg <= 0.12762) {
                      return 1;
                    } else {  // if shiftAvg > 0.12762
                      return 2;
                    }
                  } else {  // if shiftAvg > 1.5525
                    if (maxEdgeLength <= 40.784) {
                      return 1;
                    } else {  // if maxEdgeLength > 40.784
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 45.54
                  if (maxEdgeLength <= 61.863) {
                    if (confidence <= 0.89925) {
                      return 1;
                    } else {  // if confidence > 0.89925
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 61.863
                    if (confidence <= 0.91034) {
                      return 1;
                    } else {  // if confidence > 0.91034
                      return 1;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 72.898
              if (confidence <= 0.90926) {
                if (xyRatio <= 0.3558) {
                  if (avgErr <= 12.545) {
                    if (shiftNcc <= 0.89165) {
                      return 1;
                    } else {  // if shiftNcc > 0.89165
                      return 1;
                    }
                  } else {  // if avgErr > 12.545
                    if (confidence <= 0.90047) {
                      return 1;
                    } else {  // if confidence > 0.90047
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.3558
                  if (shiftAvg <= 2.3673) {
                    if (avgErr <= 7.1614) {
                      return 1;
                    } else {  // if avgErr > 7.1614
                      return 1;
                    }
                  } else {  // if shiftAvg > 2.3673
                    if (shiftAvg <= 17.195) {
                      return 1;
                    } else {  // if shiftAvg > 17.195
                      return 0;
                    }
                  }
                }
              } else {  // if confidence > 0.90926
                if (avgErr <= 7.2551) {
                  if (xyRatio <= 0.48918) {
                    if (shiftAvg <= 0.52167) {
                      return 1;
                    } else {  // if shiftAvg > 0.52167
                      return 1;
                    }
                  } else {  // if xyRatio > 0.48918
                    if (xyRatio <= 0.85637) {
                      return 1;
                    } else {  // if xyRatio > 0.85637
                      return 0;
                    }
                  }
                } else {  // if avgErr > 7.2551
                  if (area <= 1913.9) {
                    if (maxEdgeLength <= 76.263) {
                      return 1;
                    } else {  // if maxEdgeLength > 76.263
                      return 1;
                    }
                  } else {  // if area > 1913.9
                    if (shiftAvg <= 2.6442) {
                      return 1;
                    } else {  // if shiftAvg > 2.6442
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 88.367
          if (shiftAvg <= 6.5119) {
            if (confidence <= 0.90519) {
              if (area <= 7676.5) {
                if (avgErr <= 9.7336) {
                  if (maxEdgeLength <= 106.15) {
                    if (xyRatio <= 0.41099) {
                      return 1;
                    } else {  // if xyRatio > 0.41099
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 106.15
                    if (avgErr <= 6.0493) {
                      return 1;
                    } else {  // if avgErr > 6.0493
                      return 1;
                    }
                  }
                } else {  // if avgErr > 9.7336
                  if (xyRatio <= 0.33331) {
                    if (shiftNcc <= 0.9747) {
                      return 1;
                    } else {  // if shiftNcc > 0.9747
                      return 0;
                    }
                  } else {  // if xyRatio > 0.33331
                    if (confidence <= 0.89584) {
                      return 1;
                    } else {  // if confidence > 0.89584
                      return 1;
                    }
                  }
                }
              } else {  // if area > 7676.5
                if (maxEdgeLength <= 185.3) {
                  if (shiftNcc <= 0.91827) {
                    if (xyRatio <= 0.44066) {
                      return 1;
                    } else {  // if xyRatio > 0.44066
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.91827
                    if (shiftStd <= 1.1715) {
                      return 0;
                    } else {  // if shiftStd > 1.1715
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 185.3
                  if (confidence <= 0.90454) {
                    if (area <= 15746) {
                      return 0;
                    } else {  // if area > 15746
                      return 0;
                    }
                  } else {  // if confidence > 0.90454
                    return 1;
                  }
                }
              }
            } else {  // if confidence > 0.90519
              if (avgErr <= 7.9605) {
                if (area <= 5890.9) {
                  if (avgErr <= 6.5793) {
                    if (maxEdgeLength <= 110.64) {
                      return 1;
                    } else {  // if maxEdgeLength > 110.64
                      return 1;
                    }
                  } else {  // if avgErr > 6.5793
                    if (shiftStd <= 0.22598) {
                      return 1;
                    } else {  // if shiftStd > 0.22598
                      return 1;
                    }
                  }
                } else {  // if area > 5890.9
                  if (maxEdgeLength <= 143.86) {
                    if (shiftNcc <= 0.91914) {
                      return 1;
                    } else {  // if shiftNcc > 0.91914
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 143.86
                    if (xyRatio <= 0.49844) {
                      return 0;
                    } else {  // if xyRatio > 0.49844
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 7.9605
                if (area <= 15062) {
                  if (confidence <= 0.91556) {
                    if (shiftAvg <= 2.3346) {
                      return 1;
                    } else {  // if shiftAvg > 2.3346
                      return 1;
                    }
                  } else {  // if confidence > 0.91556
                    if (maxEdgeLength <= 103.6) {
                      return 1;
                    } else {  // if maxEdgeLength > 103.6
                      return 0;
                    }
                  }
                } else {  // if area > 15062
                  if (avgErr <= 14.913) {
                    if (xyRatio <= 0.59577) {
                      return 0;
                    } else {  // if xyRatio > 0.59577
                      return 2;
                    }
                  } else {  // if avgErr > 14.913
                    if (confidence <= 0.91798) {
                      return 0;
                    } else {  // if confidence > 0.91798
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 6.5119
            if (maxEdgeLength <= 181.19) {
              if (confidence <= 0.90321) {
                if (maxEdgeLength <= 111.28) {
                  if (shiftNcc <= 0.81612) {
                    if (maxEdgeLength <= 100.28) {
                      return 1;
                    } else {  // if maxEdgeLength > 100.28
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.81612
                    if (avgErr <= 10.383) {
                      return 1;
                    } else {  // if avgErr > 10.383
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 111.28
                  if (xyRatio <= 0.3889) {
                    if (shiftAvg <= 9.3481) {
                      return 1;
                    } else {  // if shiftAvg > 9.3481
                      return 1;
                    }
                  } else {  // if xyRatio > 0.3889
                    if (xyRatio <= 0.75243) {
                      return 0;
                    } else {  // if xyRatio > 0.75243
                      return 1;
                    }
                  }
                }
              } else {  // if confidence > 0.90321
                if (shiftNcc <= 0.74121) {
                  if (xyRatio <= 0.75493) {
                    if (confidence <= 0.9139) {
                      return 1;
                    } else {  // if confidence > 0.9139
                      return 0;
                    }
                  } else {  // if xyRatio > 0.75493
                    if (shiftNcc <= 0.60404) {
                      return 1;
                    } else {  // if shiftNcc > 0.60404
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.74121
                  if (avgErr <= 11.745) {
                    if (area <= 6435.9) {
                      return 1;
                    } else {  // if area > 6435.9
                      return 0;
                    }
                  } else {  // if avgErr > 11.745
                    if (shiftNcc <= 0.96392) {
                      return 0;
                    } else {  // if shiftNcc > 0.96392
                      return 0;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 181.19
              if (area <= 16010) {
                if (shiftNcc <= 0.91759) {
                  if (maxEdgeLength <= 218.21) {
                    if (confidence <= 0.90093) {
                      return 0;
                    } else {  // if confidence > 0.90093
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 218.21
                    if (shiftStd <= 96.384) {
                      return 0;
                    } else {  // if shiftStd > 96.384
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.91759
                  if (xyRatio <= 0.24421) {
                    return 1;
                  } else {  // if xyRatio > 0.24421
                    if (avgErr <= 23.748) {
                      return 0;
                    } else {  // if avgErr > 23.748
                      return 1;
                    }
                  }
                }
              } else {  // if area > 16010
                if (avgErr <= 15.509) {
                  if (xyRatio <= 1.5086) {
                    if (maxEdgeLength <= 189.54) {
                      return 0;
                    } else {  // if maxEdgeLength > 189.54
                      return 0;
                    }
                  } else {  // if xyRatio > 1.5086
                    return 2;
                  }
                } else {  // if avgErr > 15.509
                  if (maxEdgeLength <= 224.58) {
                    if (maxEdgeLength <= 188.68) {
                      return 0;
                    } else {  // if maxEdgeLength > 188.68
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 224.58
                    if (maxEdgeLength <= 308.72) {
                      return 0;
                    } else {  // if maxEdgeLength > 308.72
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if confidence > 0.9205
        if (maxEdgeLength <= 89.953) {
          if (maxEdgeLength <= 74.717) {
            if (shiftAvg <= 0.6982) {
              if (area <= 2286.2) {
                if (maxEdgeLength <= 60.399) {
                  if (xyRatio <= 1.1813) {
                    if (area <= 2244.4) {
                      return 1;
                    } else {  // if area > 2244.4
                      return 2;
                    }
                  } else {  // if xyRatio > 1.1813
                    if (shiftNcc <= 0.85517) {
                      return 1;
                    } else {  // if shiftNcc > 0.85517
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 60.399
                  if (confidence <= 0.93594) {
                    if (xyRatio <= 0.52354) {
                      return 1;
                    } else {  // if xyRatio > 0.52354
                      return 0;
                    }
                  } else {  // if confidence > 0.93594
                    if (avgErr <= 8.7433) {
                      return 1;
                    } else {  // if avgErr > 8.7433
                      return 1;
                    }
                  }
                }
              } else {  // if area > 2286.2
                if (xyRatio <= 0.64555) {
                  if (xyRatio <= 0.46951) {
                    if (avgErr <= 9.8141) {
                      return 1;
                    } else {  // if avgErr > 9.8141
                      return 1;
                    }
                  } else {  // if xyRatio > 0.46951
                    if (avgErr <= 6.816) {
                      return 0;
                    } else {  // if avgErr > 6.816
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.64555
                  if (shiftStd <= 0.98228) {
                    if (shiftNcc <= 0.79428) {
                      return 1;
                    } else {  // if shiftNcc > 0.79428
                      return 1;
                    }
                  } else {  // if shiftStd > 0.98228
                    if (shiftNcc <= 0.3802) {
                      return 1;
                    } else {  // if shiftNcc > 0.3802
                      return 2;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.6982
              if (area <= 3402.4) {
                if (shiftNcc <= 0.99103) {
                  if (maxEdgeLength <= 72.886) {
                    if (xyRatio <= 1.5023) {
                      return 1;
                    } else {  // if xyRatio > 1.5023
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 72.886
                    if (confidence <= 0.92344) {
                      return 1;
                    } else {  // if confidence > 0.92344
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.99103
                  if (shiftStd <= 0.098531) {
                    if (xyRatio <= 1.2899) {
                      return 1;
                    } else {  // if xyRatio > 1.2899
                      return 2;
                    }
                  } else {  // if shiftStd > 0.098531
                    if (area <= 2772.1) {
                      return 2;
                    } else {  // if area > 2772.1
                      return 0;
                    }
                  }
                }
              } else {  // if area > 3402.4
                if (shiftAvg <= 1.4074) {
                  if (xyRatio <= 0.6392) {
                    return 1;
                  } else {  // if xyRatio > 0.6392
                    return 0;
                  }
                } else {  // if shiftAvg > 1.4074
                  if (shiftAvg <= 7.3672) {
                    if (xyRatio <= 0.64195) {
                      return 0;
                    } else {  // if xyRatio > 0.64195
                      return 1;
                    }
                  } else {  // if shiftAvg > 7.3672
                    if (avgErr <= 8.2058) {
                      return 1;
                    } else {  // if avgErr > 8.2058
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 74.717
            if (avgErr <= 7.1182) {
              if (xyRatio <= 0.47556) {
                if (avgErr <= 6.4828) {
                  if (shiftAvg <= 0.29276) {
                    if (area <= 2695.2) {
                      return 0;
                    } else {  // if area > 2695.2
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.29276
                    if (shiftNcc <= 0.96991) {
                      return 1;
                    } else {  // if shiftNcc > 0.96991
                      return 1;
                    }
                  }
                } else {  // if avgErr > 6.4828
                  if (xyRatio <= 0.40261) {
                    if (shiftAvg <= 0.38886) {
                      return 0;
                    } else {  // if shiftAvg > 0.38886
                      return 1;
                    }
                  } else {  // if xyRatio > 0.40261
                    if (area <= 3471.3) {
                      return 1;
                    } else {  // if area > 3471.3
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.47556
                if (confidence <= 0.93556) {
                  if (shiftStd <= 0.18173) {
                    if (xyRatio <= 0.47979) {
                      return 0;
                    } else {  // if xyRatio > 0.47979
                      return 0;
                    }
                  } else {  // if shiftStd > 0.18173
                    if (shiftAvg <= 1.3508) {
                      return 1;
                    } else {  // if shiftAvg > 1.3508
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.93556
                  return 1;
                }
              }
            } else {  // if avgErr > 7.1182
              if (maxEdgeLength <= 84.931) {
                if (avgErr <= 14.451) {
                  if (area <= 2354.9) {
                    if (avgErr <= 14.154) {
                      return 1;
                    } else {  // if avgErr > 14.154
                      return 0;
                    }
                  } else {  // if area > 2354.9
                    if (shiftAvg <= 2.2518) {
                      return 1;
                    } else {  // if shiftAvg > 2.2518
                      return 1;
                    }
                  }
                } else {  // if avgErr > 14.451
                  if (xyRatio <= 0.40812) {
                    if (shiftAvg <= 1.4073) {
                      return 1;
                    } else {  // if shiftAvg > 1.4073
                      return 0;
                    }
                  } else {  // if xyRatio > 0.40812
                    if (shiftAvg <= 9.9516) {
                      return 1;
                    } else {  // if shiftAvg > 9.9516
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 84.931
                if (shiftAvg <= 2.6633) {
                  if (confidence <= 0.92223) {
                    if (shiftStd <= 0.1097) {
                      return 0;
                    } else {  // if shiftStd > 0.1097
                      return 1;
                    }
                  } else {  // if confidence > 0.92223
                    if (avgErr <= 15.856) {
                      return 0;
                    } else {  // if avgErr > 15.856
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 2.6633
                  if (maxEdgeLength <= 86.767) {
                    if (xyRatio <= 0.61961) {
                      return 1;
                    } else {  // if xyRatio > 0.61961
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 86.767
                    if (xyRatio <= 0.43059) {
                      return 0;
                    } else {  // if xyRatio > 0.43059
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 89.953
          if (shiftAvg <= 4.6594) {
            if (avgErr <= 7.477) {
              if (maxEdgeLength <= 121.35) {
                if (avgErr <= 6.2144) {
                  if (maxEdgeLength <= 112.87) {
                    if (xyRatio <= 0.54351) {
                      return 1;
                    } else {  // if xyRatio > 0.54351
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 112.87
                    if (xyRatio <= 0.46767) {
                      return 0;
                    } else {  // if xyRatio > 0.46767
                      return 1;
                    }
                  }
                } else {  // if avgErr > 6.2144
                  if (maxEdgeLength <= 97.27) {
                    if (shiftAvg <= 0.21249) {
                      return 0;
                    } else {  // if shiftAvg > 0.21249
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 97.27
                    if (area <= 4774.6) {
                      return 1;
                    } else {  // if area > 4774.6
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 121.35
                if (maxEdgeLength <= 133.29) {
                  if (confidence <= 0.9296) {
                    if (avgErr <= 4.7671) {
                      return 0;
                    } else {  // if avgErr > 4.7671
                      return 1;
                    }
                  } else {  // if confidence > 0.9296
                    if (shiftAvg <= 3.4969) {
                      return 0;
                    } else {  // if shiftAvg > 3.4969
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 133.29
                  if (area <= 12309) {
                    if (maxEdgeLength <= 163.47) {
                      return 0;
                    } else {  // if maxEdgeLength > 163.47
                      return 1;
                    }
                  } else {  // if area > 12309
                    if (shiftAvg <= 4.4446) {
                      return 0;
                    } else {  // if shiftAvg > 4.4446
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 7.477
              if (confidence <= 0.92855) {
                if (avgErr <= 13.905) {
                  if (area <= 14212) {
                    if (shiftNcc <= 0.17153) {
                      return 1;
                    } else {  // if shiftNcc > 0.17153
                      return 0;
                    }
                  } else {  // if area > 14212
                    if (avgErr <= 13.003) {
                      return 0;
                    } else {  // if avgErr > 13.003
                      return 1;
                    }
                  }
                } else {  // if avgErr > 13.905
                  if (shiftNcc <= 0.67682) {
                    if (area <= 14429) {
                      return 1;
                    } else {  // if area > 14429
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.67682
                    if (shiftAvg <= 3.1883) {
                      return 0;
                    } else {  // if shiftAvg > 3.1883
                      return 0;
                    }
                  }
                }
              } else {  // if confidence > 0.92855
                if (confidence <= 0.94823) {
                  if (shiftNcc <= 0.84364) {
                    if (avgErr <= 14.539) {
                      return 0;
                    } else {  // if avgErr > 14.539
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.84364
                    if (xyRatio <= 0.47411) {
                      return 0;
                    } else {  // if xyRatio > 0.47411
                      return 0;
                    }
                  }
                } else {  // if confidence > 0.94823
                  if (shiftAvg <= 3.6394) {
                    if (maxEdgeLength <= 147.63) {
                      return 0;
                    } else {  // if maxEdgeLength > 147.63
                      return 0;
                    }
                  } else {  // if shiftAvg > 3.6394
                    if (area <= 3563.6) {
                      return 1;
                    } else {  // if area > 3563.6
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 4.6594
            if (confidence <= 0.9395) {
              if (maxEdgeLength <= 192.54) {
                if (shiftNcc <= 0.81643) {
                  if (xyRatio <= 0.75667) {
                    if (shiftStd <= 37.419) {
                      return 0;
                    } else {  // if shiftStd > 37.419
                      return 0;
                    }
                  } else {  // if xyRatio > 0.75667
                    if (shiftAvg <= 11.274) {
                      return 0;
                    } else {  // if shiftAvg > 11.274
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.81643
                  if (avgErr <= 9.0102) {
                    if (maxEdgeLength <= 126.93) {
                      return 1;
                    } else {  // if maxEdgeLength > 126.93
                      return 0;
                    }
                  } else {  // if avgErr > 9.0102
                    if (maxEdgeLength <= 99.197) {
                      return 0;
                    } else {  // if maxEdgeLength > 99.197
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 192.54
                if (avgErr <= 17.618) {
                  if (shiftAvg <= 261.33) {
                    if (area <= 14843) {
                      return 0;
                    } else {  // if area > 14843
                      return 0;
                    }
                  } else {  // if shiftAvg > 261.33
                    if (shiftAvg <= 513.5) {
                      return 1;
                    } else {  // if shiftAvg > 513.5
                      return 2;
                    }
                  }
                } else {  // if avgErr > 17.618
                  if (xyRatio <= 0.60607) {
                    if (confidence <= 0.92713) {
                      return 0;
                    } else {  // if confidence > 0.92713
                      return 0;
                    }
                  } else {  // if xyRatio > 0.60607
                    if (area <= 47514) {
                      return 0;
                    } else {  // if area > 47514
                      return 0;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.9395
              if (avgErr <= 7.5103) {
                if (area <= 16561) {
                  if (maxEdgeLength <= 128.1) {
                    if (confidence <= 0.95594) {
                      return 1;
                    } else {  // if confidence > 0.95594
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 128.1
                    if (area <= 9548.4) {
                      return 0;
                    } else {  // if area > 9548.4
                      return 0;
                    }
                  }
                } else {  // if area > 16561
                  if (avgErr <= 6.1695) {
                    if (shiftAvg <= 27.657) {
                      return 0;
                    } else {  // if shiftAvg > 27.657
                      return 1;
                    }
                  } else {  // if avgErr > 6.1695
                    if (avgErr <= 7.5031) {
                      return 0;
                    } else {  // if avgErr > 7.5031
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 7.5103
                if (maxEdgeLength <= 206.6) {
                  if (confidence <= 0.95134) {
                    if (shiftNcc <= 0.72744) {
                      return 0;
                    } else {  // if shiftNcc > 0.72744
                      return 0;
                    }
                  } else {  // if confidence > 0.95134
                    if (avgErr <= 9.8752) {
                      return 0;
                    } else {  // if avgErr > 9.8752
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 206.6
                  if (xyRatio <= 0.63448) {
                    if (shiftNcc <= 0.058965) {
                      return 1;
                    } else {  // if shiftNcc > 0.058965
                      return 0;
                    }
                  } else {  // if xyRatio > 0.63448
                    if (avgErr <= 13.408) {
                      return 0;
                    } else {  // if avgErr > 13.408
                      return 0;
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
