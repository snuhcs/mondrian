#include "strm/tree/VIRAT.hpp"

namespace rm {

//float VIRAT(float width, float height, float maxEdgeLength, float type, float origin, float xyRatio,
//            float avgShiftX, float avgShiftY, float avgShift, float stdShiftX, float stdShiftY,
//            float stdShift, float avgErr, float ncc) {
//  if (maxEdgeLength <= 228.81) {
//    if (height <= 173.06) {
//      if (width <= 184.89) {
//        if (maxEdgeLength <= 60.373) {
//          if (height <= 51.304) {
//            if (height <= 48.04) {
//              return 2;
//            } else {  // if height > 48.04
//              if (height <= 49.28) {
//                if (stdShiftX <= 0.0047056) {
//                  return 1;
//                } else {  // if stdShiftX > 0.0047056
//                  if (stdShift <= 0.00071701) {
//                    if (maxEdgeLength <= 59.894) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 59.894
//                      return 1;
//                    }
//                  } else {  // if stdShift > 0.00071701
//                    if (avgShift <= 0.0216) {
//                      return 1;
//                    } else {  // if avgShift > 0.0216
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if height > 49.28
//                if (stdShiftY <= 0.0014801) {
//                  if (avgShift <= 0.0035311) {
//                    return 1;
//                  } else {  // if avgShift > 0.0035311
//                    if (xyRatio <= 0.69315) {
//                      return 1;
//                    } else {  // if xyRatio > 0.69315
//                      return 2;
//                    }
//                  }
//                } else {  // if stdShiftY > 0.0014801
//                  if (xyRatio <= 0.96686) {
//                    if (avgShift <= 0.00015879) {
//                      return 1;
//                    } else {  // if avgShift > 0.00015879
//                      return 2;
//                    }
//                  } else {  // if xyRatio > 0.96686
//                    if (maxEdgeLength <= 55.427) {
//                      return 1;
//                    } else {  // if maxEdgeLength > 55.427
//                      return 2;
//                    }
//                  }
//                }
//              }
//            }
//          } else {  // if height > 51.304
//            if (width <= 55.992) {
//              if (width <= 42.197) {
//                if (avgShiftX <= 0.50133) {
//                  return 2;
//                } else {  // if avgShiftX > 0.50133
//                  if (avgShiftX <= 0.68172) {
//                    return 1;
//                  } else {  // if avgShiftX > 0.68172
//                    return 2;
//                  }
//                }
//              } else {  // if width > 42.197
//                if (avgErr <= 2.2967) {
//                  if (width <= 55.152) {
//                    if (width <= 50.007) {
//                      return 1;
//                    } else {  // if width > 50.007
//                      return 1;
//                    }
//                  } else {  // if width > 55.152
//                    if (height <= 56.011) {
//                      return 1;
//                    } else {  // if height > 56.011
//                      return 2;
//                    }
//                  }
//                } else {  // if avgErr > 2.2967
//                  if (width <= 52.391) {
//                    if (stdShift <= 0.00039675) {
//                      return 1;
//                    } else {  // if stdShift > 0.00039675
//                      return 2;
//                    }
//                  } else {  // if width > 52.391
//                    return 1;
//                  }
//                }
//              }
//            } else {  // if width > 55.992
//              if (width <= 56.132) {
//                if (maxEdgeLength <= 58.598) {
//                  return 2;
//                } else {  // if maxEdgeLength > 58.598
//                  return 1;
//                }
//              } else {  // if width > 56.132
//                if (maxEdgeLength <= 57.225) {
//                  if (maxEdgeLength <= 56.538) {
//                    if (maxEdgeLength <= 56.368) {
//                      return 1;
//                    } else {  // if maxEdgeLength > 56.368
//                      return 2;
//                    }
//                  } else {  // if maxEdgeLength > 56.538
//                    if (height <= 56.669) {
//                      return 1;
//                    } else {  // if height > 56.669
//                      return 2;
//                    }
//                  }
//                } else {  // if maxEdgeLength > 57.225
//                  if (stdShiftY <= 0.01456) {
//                    if (stdShiftX <= 0.0053188) {
//                      return 2;
//                    } else {  // if stdShiftX > 0.0053188
//                      return 1;
//                    }
//                  } else {  // if stdShiftY > 0.01456
//                    if (avgShiftX <= 0.023739) {
//                      return 2;
//                    } else {  // if avgShiftX > 0.023739
//                      return 1;
//                    }
//                  }
//                }
//              }
//            }
//          }
//        } else {  // if maxEdgeLength > 60.373
//          if (height <= 70) {
//            if (height <= 43.736) {
//              if (xyRatio <= 4.0173) {
//                if (maxEdgeLength <= 95.111) {
//                  if (avgShiftY <= -0.062034) {
//                    if (avgShiftY <= -0.069602) {
//                      return 2;
//                    } else {  // if avgShiftY > -0.069602
//                      return 1;
//                    }
//                  } else {  // if avgShiftY > -0.062034
//                    if (xyRatio <= 3.2131) {
//                      return 2;
//                    } else {  // if xyRatio > 3.2131
//                      return 2;
//                    }
//                  }
//                } else {  // if maxEdgeLength > 95.111
//                  if (maxEdgeLength <= 100.9) {
//                    if (avgShiftY <= 0.079214) {
//                      return 1;
//                    } else {  // if avgShiftY > 0.079214
//                      return 0;
//                    }
//                  } else {  // if maxEdgeLength > 100.9
//                    if (width <= 125.04) {
//                      return 2;
//                    } else {  // if width > 125.04
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if xyRatio > 4.0173
//                if (width <= 135.36) {
//                  if (width <= 125.13) {
//                    if (height <= 41.306) {
//                      return 2;
//                    } else {  // if height > 41.306
//                      return 1;
//                    }
//                  } else {  // if width > 125.13
//                    if (maxEdgeLength <= 133.04) {
//                      return 0;
//                    } else {  // if maxEdgeLength > 133.04
//                      return 1;
//                    }
//                  }
//                } else {  // if width > 135.36
//                  if (avgShift <= 23.705) {
//                    if (xyRatio <= 4.5018) {
//                      return 2;
//                    } else {  // if xyRatio > 4.5018
//                      return 2;
//                    }
//                  } else {  // if avgShift > 23.705
//                    return 1;
//                  }
//                }
//              }
//            } else {  // if height > 43.736
//              if (width <= 95.659) {
//                if (height <= 62.099) {
//                  if (maxEdgeLength <= 60.826) {
//                    if (avgShiftY <= 0.024745) {
//                      return 2;
//                    } else {  // if avgShiftY > 0.024745
//                      return 1;
//                    }
//                  } else {  // if maxEdgeLength > 60.826
//                    if (avgErr <= 2.4444) {
//                      return 2;
//                    } else {  // if avgErr > 2.4444
//                      return 2;
//                    }
//                  }
//                } else {  // if height > 62.099
//                  if (xyRatio <= 1.3058) {
//                    if (xyRatio <= 0.7874) {
//                      return 2;
//                    } else {  // if xyRatio > 0.7874
//                      return 2;
//                    }
//                  } else {  // if xyRatio > 1.3058
//                    if (height <= 62.76) {
//                      return 2;
//                    } else {  // if height > 62.76
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if width > 95.659
//                if (xyRatio <= 1.8807) {
//                  if (avgShift <= 0.010639) {
//                    if (avgErr <= 0.47748) {
//                      return 0;
//                    } else {  // if avgErr > 0.47748
//                      return 2;
//                    }
//                  } else {  // if avgShift > 0.010639
//                    if (xyRatio <= 1.66) {
//                      return 2;
//                    } else {  // if xyRatio > 1.66
//                      return 0;
//                    }
//                  }
//                } else {  // if xyRatio > 1.8807
//                  if (maxEdgeLength <= 133.42) {
//                    if (height <= 57.241) {
//                      return 2;
//                    } else {  // if height > 57.241
//                      return 2;
//                    }
//                  } else {  // if maxEdgeLength > 133.42
//                    if (maxEdgeLength <= 184.38) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 184.38
//                      return 2;
//                    }
//                  }
//                }
//              }
//            }
//          } else {  // if height > 70
//            if (xyRatio <= 0.556) {
//              if (avgErr <= 0.46262) {
//                if (height <= 139.31) {
//                  if (xyRatio <= 0.37332) {
//                    if (width <= 62.062) {
//                      return 1;
//                    } else {  // if width > 62.062
//                      return 2;
//                    }
//                  } else {  // if xyRatio > 0.37332
//                    if (ncc <= 0.61681) {
//                      return 2;
//                    } else {  // if ncc > 0.61681
//                      return 1;
//                    }
//                  }
//                } else {  // if height > 139.31
//                  if (width <= 67.118) {
//                    if (stdShift <= 0.00036716) {
//                      return 0;
//                    } else {  // if stdShift > 0.00036716
//                      return 2;
//                    }
//                  } else {  // if width > 67.118
//                    if (maxEdgeLength <= 155.7) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 155.7
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if avgErr > 0.46262
//                if (height <= 155.84) {
//                  if (height <= 72.507) {
//                    if (xyRatio <= 0.5281) {
//                      return 1;
//                    } else {  // if xyRatio > 0.5281
//                      return 2;
//                    }
//                  } else {  // if height > 72.507
//                    if (height <= 136.52) {
//                      return 2;
//                    } else {  // if height > 136.52
//                      return 2;
//                    }
//                  }
//                } else {  // if height > 155.84
//                  if (xyRatio <= 0.27864) {
//                    if (avgShiftX <= 2.1345) {
//                      return 2;
//                    } else {  // if avgShiftX > 2.1345
//                      return 0;
//                    }
//                  } else {  // if xyRatio > 0.27864
//                    if (width <= 83.613) {
//                      return 2;
//                    } else {  // if width > 83.613
//                      return 1;
//                    }
//                  }
//                }
//              }
//            } else {  // if xyRatio > 0.556
//              if (avgShift <= 0.07948) {
//                if (xyRatio <= 1.8412) {
//                  if (width <= 60.716) {
//                    if (stdShift <= 5.318e-05) {
//                      return 1;
//                    } else {  // if stdShift > 5.318e-05
//                      return 2;
//                    }
//                  } else {  // if width > 60.716
//                    if (maxEdgeLength <= 92) {
//                      return 0;
//                    } else {  // if maxEdgeLength > 92
//                      return 2;
//                    }
//                  }
//                } else {  // if xyRatio > 1.8412
//                  if (width <= 136.73) {
//                    if (height <= 76.838) {
//                      return 2;
//                    } else {  // if height > 76.838
//                      return 1;
//                    }
//                  } else {  // if width > 136.73
//                    if (height <= 84.491) {
//                      return 2;
//                    } else {  // if height > 84.491
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if avgShift > 0.07948
//                if (xyRatio <= 0.85731) {
//                  if (xyRatio <= 0.55663) {
//                    return 0;
//                  } else {  // if xyRatio > 0.55663
//                    if (width <= 105.04) {
//                      return 2;
//                    } else {  // if width > 105.04
//                      return 2;
//                    }
//                  }
//                } else {  // if xyRatio > 0.85731
//                  if (xyRatio <= 1.3472) {
//                    if (xyRatio <= 1.1944) {
//                      return 0;
//                    } else {  // if xyRatio > 1.1944
//                      return 1;
//                    }
//                  } else {  // if xyRatio > 1.3472
//                    if (width <= 104) {
//                      return 0;
//                    } else {  // if width > 104
//                      return 2;
//                    }
//                  }
//                }
//              }
//            }
//          }
//        }
//      } else {  // if width > 184.89
//        if (height <= 66.377) {
//          if (maxEdgeLength <= 193.97) {
//            if (avgShiftY <= 0.35004) {
//              if (height <= 62.816) {
//                if (xyRatio <= 5.375) {
//                  if (avgShiftX <= 0.19257) {
//                    if (avgShiftY <= 0.14609) {
//                      return 1;
//                    } else {  // if avgShiftY > 0.14609
//                      return 1;
//                    }
//                  } else {  // if avgShiftX > 0.19257
//                    if (avgShiftX <= 2.597) {
//                      return 2;
//                    } else {  // if avgShiftX > 2.597
//                      return 1;
//                    }
//                  }
//                } else {  // if xyRatio > 5.375
//                  if (avgShiftX <= 0.02839) {
//                    return 2;
//                  } else {  // if avgShiftX > 0.02839
//                    return 1;
//                  }
//                }
//              } else {  // if height > 62.816
//                return 2;
//              }
//            } else {  // if avgShiftY > 0.35004
//              if (stdShift <= 0.13835) {
//                if (stdShiftY <= 0.17067) {
//                  if (avgShiftY <= 0.81481) {
//                    return 2;
//                  } else {  // if avgShiftY > 0.81481
//                    return 1;
//                  }
//                } else {  // if stdShiftY > 0.17067
//                  return 0;
//                }
//              } else {  // if stdShift > 0.13835
//                if (avgErr <= 6.7209) {
//                  return 2;
//                } else {  // if avgErr > 6.7209
//                  return 1;
//                }
//              }
//            }
//          } else {  // if maxEdgeLength > 193.97
//            if (stdShiftY <= 0.04207) {
//              if (width <= 223.64) {
//                if (ncc <= -0.051296) {
//                  return 1;
//                } else {  // if ncc > -0.051296
//                  if (avgShiftX <= 0.018191) {
//                    if (maxEdgeLength <= 197.07) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 197.07
//                      return 2;
//                    }
//                  } else {  // if avgShiftX > 0.018191
//                    if (avgShiftY <= -0.0014914) {
//                      return 1;
//                    } else {  // if avgShiftY > -0.0014914
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if width > 223.64
//                if (avgErr <= 0.31705) {
//                  if (width <= 225.21) {
//                    return 1;
//                  } else {  // if width > 225.21
//                    return 2;
//                  }
//                } else {  // if avgErr > 0.31705
//                  return 1;
//                }
//              }
//            } else {  // if stdShiftY > 0.04207
//              if (avgShiftY <= 0.52254) {
//                if (xyRatio <= 7.7602) {
//                  if (xyRatio <= 3.9321) {
//                    if (ncc <= 0.61407) {
//                      return 2;
//                    } else {  // if ncc > 0.61407
//                      return 1;
//                    }
//                  } else {  // if xyRatio > 3.9321
//                    if (height <= 61.855) {
//                      return 1;
//                    } else {  // if height > 61.855
//                      return 1;
//                    }
//                  }
//                } else {  // if xyRatio > 7.7602
//                  return 2;
//                }
//              } else {  // if avgShiftY > 0.52254
//                if (avgShiftX <= -3.1492) {
//                  if (stdShiftX <= 0.32992) {
//                    return 0;
//                  } else {  // if stdShiftX > 0.32992
//                    if (height <= 40.736) {
//                      return 1;
//                    } else {  // if height > 40.736
//                      return 2;
//                    }
//                  }
//                } else {  // if avgShiftX > -3.1492
//                  if (stdShiftY <= 0.20679) {
//                    if (stdShift <= 0.18792) {
//                      return 0;
//                    } else {  // if stdShift > 0.18792
//                      return 0;
//                    }
//                  } else {  // if stdShiftY > 0.20679
//                    if (avgShiftY <= 0.59976) {
//                      return 0;
//                    } else {  // if avgShiftY > 0.59976
//                      return 1;
//                    }
//                  }
//                }
//              }
//            }
//          }
//        } else {  // if height > 66.377
//          if (stdShiftY <= 0.073601) {
//            if (xyRatio <= 3.5192) {
//              if (xyRatio <= 2.965) {
//                if (xyRatio <= 2.6194) {
//                  if (height <= 93.13) {
//                    if (avgErr <= 0.20783) {
//                      return 0;
//                    } else {  // if avgErr > 0.20783
//                      return 2;
//                    }
//                  } else {  // if height > 93.13
//                    if (width <= 214.05) {
//                      return 1;
//                    } else {  // if width > 214.05
//                      return 2;
//                    }
//                  }
//                } else {  // if xyRatio > 2.6194
//                  if (stdShift <= 0.012035) {
//                    if (ncc <= -0.012995) {
//                      return 2;
//                    } else {  // if ncc > -0.012995
//                      return 2;
//                    }
//                  } else {  // if stdShift > 0.012035
//                    if (stdShift <= 0.013055) {
//                      return 1;
//                    } else {  // if stdShift > 0.013055
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if xyRatio > 2.965
//                if (height <= 73.829) {
//                  if (maxEdgeLength <= 193.3) {
//                    if (xyRatio <= 3.5008) {
//                      return 2;
//                    } else {  // if xyRatio > 3.5008
//                      return 1;
//                    }
//                  } else {  // if maxEdgeLength > 193.3
//                    if (height <= 71.326) {
//                      return 1;
//                    } else {  // if height > 71.326
//                      return 2;
//                    }
//                  }
//                } else {  // if height > 73.829
//                  if (maxEdgeLength <= 206.76) {
//                    if (avgShiftY <= -0.050608) {
//                      return 0;
//                    } else {  // if avgShiftY > -0.050608
//                      return 1;
//                    }
//                  } else {  // if maxEdgeLength > 206.76
//                    if (stdShiftY <= 0.057819) {
//                      return 2;
//                    } else {  // if stdShiftY > 0.057819
//                      return 1;
//                    }
//                  }
//                }
//              }
//            } else {  // if xyRatio > 3.5192
//              if (maxEdgeLength <= 223.95) {
//                if (height <= 74.011) {
//                  if (maxEdgeLength <= 197.15) {
//                    if (stdShiftY <= 0.038946) {
//                      return 2;
//                    } else {  // if stdShiftY > 0.038946
//                      return 1;
//                    }
//                  } else {  // if maxEdgeLength > 197.15
//                    if (avgShiftY <= -0.010198) {
//                      return 2;
//                    } else {  // if avgShiftY > -0.010198
//                      return 2;
//                    }
//                  }
//                } else {  // if height > 74.011
//                  return 1;
//                }
//              } else {  // if maxEdgeLength > 223.95
//                if (stdShiftY <= 0.040365) {
//                  if (avgShiftX <= 0.0064315) {
//                    return 2;
//                  } else {  // if avgShiftX > 0.0064315
//                    return 1;
//                  }
//                } else {  // if stdShiftY > 0.040365
//                  return 1;
//                }
//              }
//            }
//          } else {  // if stdShiftY > 0.073601
//            if (xyRatio <= 2.8331) {
//              if (xyRatio <= 1.7642) {
//                if (xyRatio <= 1.2609) {
//                  if (stdShiftY <= 0.15007) {
//                    if (avgShiftY <= 1.9751) {
//                      return 0;
//                    } else {  // if avgShiftY > 1.9751
//                      return 2;
//                    }
//                  } else {  // if stdShiftY > 0.15007
//                    if (avgErr <= 4.5577) {
//                      return 1;
//                    } else {  // if avgErr > 4.5577
//                      return 0;
//                    }
//                  }
//                } else {  // if xyRatio > 1.2609
//                  if (avgShiftX <= -2.6175) {
//                    return 2;
//                  } else {  // if avgShiftX > -2.6175
//                    if (width <= 218.94) {
//                      return 1;
//                    } else {  // if width > 218.94
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if xyRatio > 1.7642
//                if (xyRatio <= 2.4834) {
//                  return 2;
//                } else {  // if xyRatio > 2.4834
//                  if (avgErr <= 1.4616) {
//                    if (ncc <= 0.50575) {
//                      return 2;
//                    } else {  // if ncc > 0.50575
//                      return 1;
//                    }
//                  } else {  // if avgErr > 1.4616
//                    return 1;
//                  }
//                }
//              }
//            } else {  // if xyRatio > 2.8331
//              if (ncc <= 0.29724) {
//                if (stdShiftY <= 0.075702) {
//                  if (width <= 216.24) {
//                    if (ncc <= 0.073314) {
//                      return 2;
//                    } else {  // if ncc > 0.073314
//                      return 1;
//                    }
//                  } else {  // if width > 216.24
//                    return 0;
//                  }
//                } else {  // if stdShiftY > 0.075702
//                  if (avgShiftX <= -0.033025) {
//                    if (avgShift <= 0.0087706) {
//                      return 2;
//                    } else {  // if avgShift > 0.0087706
//                      return 1;
//                    }
//                  } else {  // if avgShiftX > -0.033025
//                    if (height <= 83.31) {
//                      return 1;
//                    } else {  // if height > 83.31
//                      return 0;
//                    }
//                  }
//                }
//              } else {  // if ncc > 0.29724
//                if (height <= 74.528) {
//                  if (maxEdgeLength <= 224.31) {
//                    if (avgShiftX <= -1.8133) {
//                      return 1;
//                    } else {  // if avgShiftX > -1.8133
//                      return 2;
//                    }
//                  } else {  // if maxEdgeLength > 224.31
//                    return 1;
//                  }
//                } else {  // if height > 74.528
//                  if (avgErr <= 0.45341) {
//                    if (xyRatio <= 3.3064) {
//                      return 2;
//                    } else {  // if xyRatio > 3.3064
//                      return 0;
//                    }
//                  } else {  // if avgErr > 0.45341
//                    return 1;
//                  }
//                }
//              }
//            }
//          }
//        }
//      }
//    } else {  // if height > 173.06
//      if (xyRatio <= 0.82918) {
//        if (stdShiftX <= 0.11827) {
//          if (width <= 87.95) {
//            if (width <= 84.382) {
//              if (maxEdgeLength <= 180.95) {
//                if (ncc <= 0.036429) {
//                  return 2;
//                } else {  // if ncc > 0.036429
//                  if (xyRatio <= 0.26501) {
//                    if (maxEdgeLength <= 175.01) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 175.01
//                      return 1;
//                    }
//                  } else {  // if xyRatio > 0.26501
//                    if (maxEdgeLength <= 177.48) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 177.48
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if maxEdgeLength > 180.95
//                if (stdShiftY <= 0.1021) {
//                  if (stdShiftY <= 0.044514) {
//                    if (avgShiftX <= -0.0065066) {
//                      return 2;
//                    } else {  // if avgShiftX > -0.0065066
//                      return 0;
//                    }
//                  } else {  // if stdShiftY > 0.044514
//                    return 0;
//                  }
//                } else {  // if stdShiftY > 0.1021
//                  if (avgShiftX <= -0.14855) {
//                    if (width <= 78.668) {
//                      return 1;
//                    } else {  // if width > 78.668
//                      return 0;
//                    }
//                  } else {  // if avgShiftX > -0.14855
//                    if (avgShift <= 0.15081) {
//                      return 2;
//                    } else {  // if avgShift > 0.15081
//                      return 0;
//                    }
//                  }
//                }
//              }
//            } else {  // if width > 84.382
//              if (height <= 175.62) {
//                if (stdShiftY <= 0.12828) {
//                  if (stdShiftX <= 0.03432) {
//                    if (avgErr <= 0.78286) {
//                      return 0;
//                    } else {  // if avgErr > 0.78286
//                      return 2;
//                    }
//                  } else {  // if stdShiftX > 0.03432
//                    return 2;
//                  }
//                } else {  // if stdShiftY > 0.12828
//                  if (ncc <= 0.14402) {
//                    return 0;
//                  } else {  // if ncc > 0.14402
//                    return 1;
//                  }
//                }
//              } else {  // if height > 175.62
//                if (avgShiftY <= -0.19098) {
//                  if (ncc <= 0.7831) {
//                    return 1;
//                  } else {  // if ncc > 0.7831
//                    return 0;
//                  }
//                } else {  // if avgShiftY > -0.19098
//                  if (avgShift <= 0.0031595) {
//                    if (avgShift <= 0.0029368) {
//                      return 0;
//                    } else {  // if avgShift > 0.0029368
//                      return 2;
//                    }
//                  } else {  // if avgShift > 0.0031595
//                    if (avgShiftY <= 0.13625) {
//                      return 0;
//                    } else {  // if avgShiftY > 0.13625
//                      return 0;
//                    }
//                  }
//                }
//              }
//            }
//          } else {  // if width > 87.95
//            if (xyRatio <= 0.44713) {
//              return 1;
//            } else {  // if xyRatio > 0.44713
//              if (avgErr <= 1.7256) {
//                if (avgShiftY <= 0.099304) {
//                  if (ncc <= 0.087584) {
//                    if (width <= 100.45) {
//                      return 2;
//                    } else {  // if width > 100.45
//                      return 1;
//                    }
//                  } else {  // if ncc > 0.087584
//                    if (avgShiftX <= 0.14364) {
//                      return 2;
//                    } else {  // if avgShiftX > 0.14364
//                      return 1;
//                    }
//                  }
//                } else {  // if avgShiftY > 0.099304
//                  return 0;
//                }
//              } else {  // if avgErr > 1.7256
//                if (xyRatio <= 0.46095) {
//                  return 2;
//                } else {  // if xyRatio > 0.46095
//                  if (xyRatio <= 0.68409) {
//                    return 1;
//                  } else {  // if xyRatio > 0.68409
//                    return 0;
//                  }
//                }
//              }
//            }
//          }
//        } else {  // if stdShiftX > 0.11827
//          if (width <= 78.153) {
//            if (avgShiftY <= 0.34534) {
//              if (stdShiftX <= 0.21815) {
//                if (avgShiftX <= -0.13021) {
//                  if (stdShift <= 0.13591) {
//                    if (maxEdgeLength <= 189.71) {
//                      return 1;
//                    } else {  // if maxEdgeLength > 189.71
//                      return 0;
//                    }
//                  } else {  // if stdShift > 0.13591
//                    if (width <= 72.79) {
//                      return 2;
//                    } else {  // if width > 72.79
//                      return 1;
//                    }
//                  }
//                } else {  // if avgShiftX > -0.13021
//                  if (stdShift <= 0.024578) {
//                    return 1;
//                  } else {  // if stdShift > 0.024578
//                    if (avgErr <= 3.7776) {
//                      return 2;
//                    } else {  // if avgErr > 3.7776
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if stdShiftX > 0.21815
//                if (xyRatio <= 0.26388) {
//                  return 0;
//                } else {  // if xyRatio > 0.26388
//                  if (avgShiftX <= 0.56843) {
//                    if (avgShiftX <= -0.85263) {
//                      return 0;
//                    } else {  // if avgShiftX > -0.85263
//                      return 2;
//                    }
//                  } else {  // if avgShiftX > 0.56843
//                    if (avgShiftY <= -0.11052) {
//                      return 2;
//                    } else {  // if avgShiftY > -0.11052
//                      return 0;
//                    }
//                  }
//                }
//              }
//            } else {  // if avgShiftY > 0.34534
//              if (avgErr <= 6.2342) {
//                if (stdShiftY <= 1.0251) {
//                  if (avgErr <= 4.4321) {
//                    if (width <= 70.856) {
//                      return 1;
//                    } else {  // if width > 70.856
//                      return 2;
//                    }
//                  } else {  // if avgErr > 4.4321
//                    if (ncc <= 0.6818) {
//                      return 1;
//                    } else {  // if ncc > 0.6818
//                      return 1;
//                    }
//                  }
//                } else {  // if stdShiftY > 1.0251
//                  return 0;
//                }
//              } else {  // if avgErr > 6.2342
//                if (avgErr <= 9.0505) {
//                  if (ncc <= 0.62122) {
//                    return 1;
//                  } else {  // if ncc > 0.62122
//                    if (avgShift <= 3.4429) {
//                      return 2;
//                    } else {  // if avgShift > 3.4429
//                      return 1;
//                    }
//                  }
//                } else {  // if avgErr > 9.0505
//                  return 1;
//                }
//              }
//            }
//          } else {  // if width > 78.153
//            if (xyRatio <= 0.60296) {
//              if (width <= 88.774) {
//                if (stdShiftX <= 0.29755) {
//                  if (xyRatio <= 0.34337) {
//                    if (ncc <= 0.4758) {
//                      return 1;
//                    } else {  // if ncc > 0.4758
//                      return 2;
//                    }
//                  } else {  // if xyRatio > 0.34337
//                    if (height <= 177.02) {
//                      return 1;
//                    } else {  // if height > 177.02
//                      return 0;
//                    }
//                  }
//                } else {  // if stdShiftX > 0.29755
//                  if (avgErr <= 6.7465) {
//                    if (avgShiftY <= 1.246) {
//                      return 1;
//                    } else {  // if avgShiftY > 1.246
//                      return 0;
//                    }
//                  } else {  // if avgErr > 6.7465
//                    if (avgShiftY <= -1.2463) {
//                      return 1;
//                    } else {  // if avgShiftY > -1.2463
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if width > 88.774
//                if (avgShiftY <= -0.05321) {
//                  if (avgShiftX <= 1.6367) {
//                    if (maxEdgeLength <= 218) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 218
//                      return 1;
//                    }
//                  } else {  // if avgShiftX > 1.6367
//                    return 1;
//                  }
//                } else {  // if avgShiftY > -0.05321
//                  if (avgShiftY <= 0.57431) {
//                    if (maxEdgeLength <= 175.51) {
//                      return 2;
//                    } else {  // if maxEdgeLength > 175.51
//                      return 1;
//                    }
//                  } else {  // if avgShiftY > 0.57431
//                    if (avgErr <= 6.3997) {
//                      return 1;
//                    } else {  // if avgErr > 6.3997
//                      return 2;
//                    }
//                  }
//                }
//              }
//            } else {  // if xyRatio > 0.60296
//              if (avgShiftY <= 0.4648) {
//                if (xyRatio <= 0.61036) {
//                  return 0;
//                } else {  // if xyRatio > 0.61036
//                  if (width <= 132.22) {
//                    if (stdShiftX <= 0.56431) {
//                      return 2;
//                    } else {  // if stdShiftX > 0.56431
//                      return 0;
//                    }
//                  } else {  // if width > 132.22
//                    if (avgShift <= 8.988) {
//                      return 1;
//                    } else {  // if avgShift > 8.988
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if avgShiftY > 0.4648
//                if (height <= 189) {
//                  return 2;
//                } else {  // if height > 189
//                  if (avgShiftX <= -2.9382) {
//                    if (avgShiftX <= -3.2297) {
//                      return 0;
//                    } else {  // if avgShiftX > -3.2297
//                      return 1;
//                    }
//                  } else {  // if avgShiftX > -2.9382
//                    if (avgShift <= 6.2836) {
//                      return 0;
//                    } else {  // if avgShift > 6.2836
//                      return 2;
//                    }
//                  }
//                }
//              }
//            }
//          }
//        }
//      } else {  // if xyRatio > 0.82918
//        if (stdShiftY <= 0.53201) {
//          if (xyRatio <= 0.92394) {
//            if (width <= 169.3) {
//              if (avgShiftX <= 0.0026839) {
//                if (xyRatio <= 0.91919) {
//                  if (height <= 182.37) {
//                    if (stdShiftY <= 0.015858) {
//                      return 0;
//                    } else {  // if stdShiftY > 0.015858
//                      return 2;
//                    }
//                  } else {  // if height > 182.37
//                    if (avgErr <= 1.1103) {
//                      return 0;
//                    } else {  // if avgErr > 1.1103
//                      return 2;
//                    }
//                  }
//                } else {  // if xyRatio > 0.91919
//                  return 0;
//                }
//              } else {  // if avgShiftX > 0.0026839
//                if (stdShiftY <= 0.013459) {
//                  return 0;
//                } else {  // if stdShiftY > 0.013459
//                  if (xyRatio <= 0.89422) {
//                    return 0;
//                  } else {  // if xyRatio > 0.89422
//                    if (avgShiftY <= 0.019098) {
//                      return 2;
//                    } else {  // if avgShiftY > 0.019098
//                      return 2;
//                    }
//                  }
//                }
//              }
//            } else {  // if width > 169.3
//              if (avgErr <= 0.63197) {
//                if (avgShiftX <= 0.0070322) {
//                  return 0;
//                } else {  // if avgShiftX > 0.0070322
//                  return 2;
//                }
//              } else {  // if avgErr > 0.63197
//                return 0;
//              }
//            }
//          } else {  // if xyRatio > 0.92394
//            if (xyRatio <= 1.0628) {
//              if (avgShiftX <= 0.13454) {
//                if (xyRatio <= 0.92755) {
//                  if (xyRatio <= 0.92712) {
//                    if (width <= 168.36) {
//                      return 0;
//                    } else {  // if width > 168.36
//                      return 0;
//                    }
//                  } else {  // if xyRatio > 0.92712
//                    return 2;
//                  }
//                } else {  // if xyRatio > 0.92755
//                  if (width <= 164.44) {
//                    if (ncc <= 0.33299) {
//                      return 2;
//                    } else {  // if ncc > 0.33299
//                      return 0;
//                    }
//                  } else {  // if width > 164.44
//                    return 0;
//                  }
//                }
//              } else {  // if avgShiftX > 0.13454
//                if (avgShiftX <= 0.57937) {
//                  if (stdShiftX <= 0.29478) {
//                    return 0;
//                  } else {  // if stdShiftX > 0.29478
//                    if (avgErr <= 4.7838) {
//                      return 2;
//                    } else {  // if avgErr > 4.7838
//                      return 0;
//                    }
//                  }
//                } else {  // if avgShiftX > 0.57937
//                  if (avgShiftY <= -2.5893) {
//                    return 1;
//                  } else {  // if avgShiftY > -2.5893
//                    if (stdShiftX <= 0.19354) {
//                      return 1;
//                    } else {  // if stdShiftX > 0.19354
//                      return 0;
//                    }
//                  }
//                }
//              }
//            } else {  // if xyRatio > 1.0628
//              if (xyRatio <= 1.1906) {
//                if (stdShiftX <= 0.16289) {
//                  return 1;
//                } else {  // if stdShiftX > 0.16289
//                  if (stdShift <= 0.07725) {
//                    if (stdShiftY <= 0.17447) {
//                      return 0;
//                    } else {  // if stdShiftY > 0.17447
//                      return 2;
//                    }
//                  } else {  // if stdShift > 0.07725
//                    if (avgErr <= 4.7881) {
//                      return 0;
//                    } else {  // if avgErr > 4.7881
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if xyRatio > 1.1906
//                return 0;
//              }
//            }
//          }
//        } else {  // if stdShiftY > 0.53201
//          if (avgShiftY <= -0.58529) {
//            return 2;
//          } else {  // if avgShiftY > -0.58529
//            if (avgShiftX <= 0.77516) {
//              if (ncc <= 0.83386) {
//                if (ncc <= 0.62405) {
//                  if (avgShiftY <= 0.14937) {
//                    return 2;
//                  } else {  // if avgShiftY > 0.14937
//                    return 1;
//                  }
//                } else {  // if ncc > 0.62405
//                  return 2;
//                }
//              } else {  // if ncc > 0.83386
//                return 0;
//              }
//            } else {  // if avgShiftX > 0.77516
//              return 0;
//            }
//          }
//        }
//      }
//    }
//  } else {  // if maxEdgeLength > 228.81
//    if (height <= 145) {
//      if (xyRatio <= 5.712) {
//        if (height <= 123.51) {
//          if (maxEdgeLength <= 283.66) {
//            if (xyRatio <= 3.0299) {
//              if (maxEdgeLength <= 237.73) {
//                if (avgShift <= 4.4509e-05) {
//                  return 1;
//                } else {  // if avgShift > 4.4509e-05
//                  if (xyRatio <= 2.2716) {
//                    return 0;
//                  } else {  // if xyRatio > 2.2716
//                    if (xyRatio <= 2.8249) {
//                      return 2;
//                    } else {  // if xyRatio > 2.8249
//                      return 2;
//                    }
//                  }
//                }
//              } else {  // if maxEdgeLength > 237.73
//                if (avgErr <= 1.4667) {
//                  if (avgShiftY <= -0.11754) {
//                    return 0;
//                  } else {  // if avgShiftY > -0.11754
//                    if (xyRatio <= 2.3553) {
//                      return 0;
//                    } else {  // if xyRatio > 2.3553
//                      return 1;
//                    }
//                  }
//                } else {  // if avgErr > 1.4667
//                  if (avgShift <= 4.6203) {
//                    if (stdShiftX <= 0.12819) {
//                      return 1;
//                    } else {  // if stdShiftX > 0.12819
//                      return 0;
//                    }
//                  } else {  // if avgShift > 4.6203
//                    return 1;
//                  }
//                }
//              }
//            } else {  // if xyRatio > 3.0299
//              if (width <= 261.01) {
//                if (height <= 62.654) {
//                  if (stdShiftY <= 0.046878) {
//                    return 1;
//                  } else {  // if stdShiftY > 0.046878
//                    if (width <= 236.25) {
//                      return 2;
//                    } else {  // if width > 236.25
//                      return 1;
//                    }
//                  }
//                } else {  // if height > 62.654
//                  if (stdShiftX <= 3.3104) {
//                    if (height <= 77.545) {
//                      return 1;
//                    } else {  // if height > 77.545
//                      return 1;
//                    }
//                  } else {  // if stdShiftX > 3.3104
//                    return 2;
//                  }
//                }
//              } else {  // if width > 261.01
//                if (height <= 74.31) {
//                  if (xyRatio <= 4.7252) {
//                    if (xyRatio <= 4.6013) {
//                      return 1;
//                    } else {  // if xyRatio > 4.6013
//                      return 1;
//                    }
//                  } else {  // if xyRatio > 4.7252
//                    if (stdShiftY <= 0.019416) {
//                      return 0;
//                    } else {  // if stdShiftY > 0.019416
//                      return 1;
//                    }
//                  }
//                } else {  // if height > 74.31
//                  if (maxEdgeLength <= 267.53) {
//                    if (avgShiftY <= 0.0065374) {
//                      return 0;
//                    } else {  // if avgShiftY > 0.0065374
//                      return 1;
//                    }
//                  } else {  // if maxEdgeLength > 267.53
//                    if (ncc <= -0.05852) {
//                      return 0;
//                    } else {  // if ncc > -0.05852
//                      return 1;
//                    }
//                  }
//                }
//              }
//            }
//          } else {  // if maxEdgeLength > 283.66
//            if (stdShift <= 0.030041) {
//              if (height <= 115.76) {
//                if (avgShift <= 0.00039048) {
//                  if (height <= 113.88) {
//                    if (stdShiftX <= 0.032792) {
//                      return 0;
//                    } else {  // if stdShiftX > 0.032792
//                      return 1;
//                    }
//                  } else {  // if height > 113.88
//                    if (avgShift <= 5.6328e-05) {
//                      return 1;
//                    } else {  // if avgShift > 5.6328e-05
//                      return 0;
//                    }
//                  }
//                } else {  // if avgShift > 0.00039048
//                  return 0;
//                }
//              } else {  // if height > 115.76
//                if (maxEdgeLength <= 343.05) {
//                  if (ncc <= 0.030482) {
//                    if (avgErr <= 0.49441) {
//                      return 1;
//                    } else {  // if avgErr > 0.49441
//                      return 0;
//                    }
//                  } else {  // if ncc > 0.030482
//                    if (stdShiftY <= 0.061462) {
//                      return 1;
//                    } else {  // if stdShiftY > 0.061462
//                      return 1;
//                    }
//                  }
//                } else {  // if maxEdgeLength > 343.05
//                  if (avgShiftY <= 0.035738) {
//                    return 0;
//                  } else {  // if avgShiftY > 0.035738
//                    return 1;
//                  }
//                }
//              }
//            } else {  // if stdShift > 0.030041
//              if (stdShiftY <= 0.035007) {
//                return 1;
//              } else {  // if stdShiftY > 0.035007
//                if (avgShiftX <= 0.24173) {
//                  return 0;
//                } else {  // if avgShiftX > 0.24173
//                  if (avgErr <= 1.4691) {
//                    return 1;
//                  } else {  // if avgErr > 1.4691
//                    if (avgShiftX <= 0.28846) {
//                      return 0;
//                    } else {  // if avgShiftX > 0.28846
//                      return 0;
//                    }
//                  }
//                }
//              }
//            }
//          }
//        } else {  // if height > 123.51
//          if (height <= 144.01) {
//            if (stdShiftX <= 0.015455) {
//              if (avgErr <= 0.16666) {
//                return 1;
//              } else {  // if avgErr > 0.16666
//                return 0;
//              }
//            } else {  // if stdShiftX > 0.015455
//              if (avgErr <= 0.67373) {
//                if (stdShift <= 0.0017585) {
//                  if (stdShift <= 0.0017405) {
//                    return 1;
//                  } else {  // if stdShift > 0.0017405
//                    return 0;
//                  }
//                } else {  // if stdShift > 0.0017585
//                  return 1;
//                }
//              } else {  // if avgErr > 0.67373
//                if (avgErr <= 0.67421) {
//                  return 0;
//                } else {  // if avgErr > 0.67421
//                  if (maxEdgeLength <= 271.92) {
//                    if (width <= 258.55) {
//                      return 1;
//                    } else {  // if width > 258.55
//                      return 0;
//                    }
//                  } else {  // if maxEdgeLength > 271.92
//                    return 1;
//                  }
//                }
//              }
//            }
//          } else {  // if height > 144.01
//            if (maxEdgeLength <= 305.15) {
//              if (width <= 301.38) {
//                return 1;
//              } else {  // if width > 301.38
//                if (avgShiftX <= 0.0034176) {
//                  if (ncc <= 0.044393) {
//                    return 1;
//                  } else {  // if ncc > 0.044393
//                    return 0;
//                  }
//                } else {  // if avgShiftX > 0.0034176
//                  if (stdShiftY <= 0.51605) {
//                    if (xyRatio <= 2.211) {
//                      return 0;
//                    } else {  // if xyRatio > 2.211
//                      return 1;
//                    }
//                  } else {  // if stdShiftY > 0.51605
//                    return 0;
//                  }
//                }
//              }
//            } else {  // if maxEdgeLength > 305.15
//              return 1;
//            }
//          }
//        }
//      } else {  // if xyRatio > 5.712
//        if (xyRatio <= 7.4644) {
//          return 0;
//        } else {  // if xyRatio > 7.4644
//          return 2;
//        }
//      }
//    } else {  // if height > 145
//      if (width <= 94.94) {
//        if (height <= 236.9) {
//          if (avgShift <= 5.1718) {
//            if (avgShift <= 4.696) {
//              if (ncc <= 0.54416) {
//                if (avgErr <= 1.7365) {
//                  if (stdShiftX <= 0.15539) {
//                    return 1;
//                  } else {  // if stdShiftX > 0.15539
//                    if (stdShiftY <= 0.17232) {
//                      return 1;
//                    } else {  // if stdShiftY > 0.17232
//                      return 2;
//                    }
//                  }
//                } else {  // if avgErr > 1.7365
//                  return 2;
//                }
//              } else {  // if ncc > 0.54416
//                if (avgShiftY <= -0.69384) {
//                  if (stdShift <= 1.2423) {
//                    return 2;
//                  } else {  // if stdShift > 1.2423
//                    return 1;
//                  }
//                } else {  // if avgShiftY > -0.69384
//                  return 1;
//                }
//              }
//            } else {  // if avgShift > 4.696
//              return 2;
//            }
//          } else {  // if avgShift > 5.1718
//            return 1;
//          }
//        } else {  // if height > 236.9
//          if (stdShiftY <= 1.6634) {
//            if (avgShiftX <= 1.5313) {
//              if (maxEdgeLength <= 240.33) {
//                if (avgShiftX <= 0.15243) {
//                  return 1;
//                } else {  // if avgShiftX > 0.15243
//                  if (stdShift <= 0.12712) {
//                    return 0;
//                  } else {  // if stdShift > 0.12712
//                    return 1;
//                  }
//                }
//              } else {  // if maxEdgeLength > 240.33
//                if (width <= 79.503) {
//                  if (ncc <= 0.91008) {
//                    if (stdShiftX <= 0.6208) {
//                      return 1;
//                    } else {  // if stdShiftX > 0.6208
//                      return 1;
//                    }
//                  } else {  // if ncc > 0.91008
//                    if (avgShiftX <= 0.5456) {
//                      return 1;
//                    } else {  // if avgShiftX > 0.5456
//                      return 0;
//                    }
//                  }
//                } else {  // if width > 79.503
//                  if (avgErr <= 4.4426) {
//                    if (avgShiftX <= 0.037843) {
//                      return 1;
//                    } else {  // if avgShiftX > 0.037843
//                      return 1;
//                    }
//                  } else {  // if avgErr > 4.4426
//                    if (xyRatio <= 0.24584) {
//                      return 0;
//                    } else {  // if xyRatio > 0.24584
//                      return 1;
//                    }
//                  }
//                }
//              }
//            } else {  // if avgShiftX > 1.5313
//              return 0;
//            }
//          } else {  // if stdShiftY > 1.6634
//            return 0;
//          }
//        }
//      } else {  // if width > 94.94
//        if (xyRatio <= 1.701) {
//          if (width <= 311.22) {
//            if (avgShift <= 7.2452) {
//              if (stdShift <= 0.085777) {
//                if (height <= 199.46) {
//                  if (width <= 264.95) {
//                    if (xyRatio <= 1.4215) {
//                      return 1;
//                    } else {  // if xyRatio > 1.4215
//                      return 0;
//                    }
//                  } else {  // if width > 264.95
//                    if (height <= 184.84) {
//                      return 1;
//                    } else {  // if height > 184.84
//                      return 1;
//                    }
//                  }
//                } else {  // if height > 199.46
//                  if (maxEdgeLength <= 304.79) {
//                    if (xyRatio <= 0.32518) {
//                      return 0;
//                    } else {  // if xyRatio > 0.32518
//                      return 0;
//                    }
//                  } else {  // if maxEdgeLength > 304.79
//                    if (ncc <= 0.54198) {
//                      return 1;
//                    } else {  // if ncc > 0.54198
//                      return 0;
//                    }
//                  }
//                }
//              } else {  // if stdShift > 0.085777
//                if (xyRatio <= 1.5948) {
//                  if (width <= 149) {
//                    if (avgShiftX <= -1.6348) {
//                      return 0;
//                    } else {  // if avgShiftX > -1.6348
//                      return 1;
//                    }
//                  } else {  // if width > 149
//                    if (maxEdgeLength <= 235) {
//                      return 0;
//                    } else {  // if maxEdgeLength > 235
//                      return 1;
//                    }
//                  }
//                } else {  // if xyRatio > 1.5948
//                  if (stdShiftX <= 0.24857) {
//                    return 0;
//                  } else {  // if stdShiftX > 0.24857
//                    if (xyRatio <= 1.6386) {
//                      return 1;
//                    } else {  // if xyRatio > 1.6386
//                      return 0;
//                    }
//                  }
//                }
//              }
//            } else {  // if avgShift > 7.2452
//              if (stdShift <= 0.13558) {
//                return 1;
//              } else {  // if stdShift > 0.13558
//                if (avgErr <= 6.1907) {
//                  if (stdShift <= 0.78592) {
//                    return 0;
//                  } else {  // if stdShift > 0.78592
//                    if (width <= 294.35) {
//                      return 0;
//                    } else {  // if width > 294.35
//                      return 1;
//                    }
//                  }
//                } else {  // if avgErr > 6.1907
//                  return 1;
//                }
//              }
//            }
//          } else {  // if width > 311.22
//            if (xyRatio <= 1.6433) {
//              if (xyRatio <= 1.6427) {
//                if (height <= 199.51) {
//                  return 1;
//                } else {  // if height > 199.51
//                  if (avgShiftX <= 3.7961) {
//                    if (ncc <= 0.063548) {
//                      return 0;
//                    } else {  // if ncc > 0.063548
//                      return 0;
//                    }
//                  } else {  // if avgShiftX > 3.7961
//                    return 1;
//                  }
//                }
//              } else {  // if xyRatio > 1.6427
//                return 1;
//              }
//            } else {  // if xyRatio > 1.6433
//              return 0;
//            }
//          }
//        } else {  // if xyRatio > 1.701
//          if (maxEdgeLength <= 316.98) {
//            if (xyRatio <= 1.9761) {
//              if (height <= 178.5) {
//                if (xyRatio <= 1.8705) {
//                  if (maxEdgeLength <= 312.63) {
//                    if (height <= 175.55) {
//                      return 1;
//                    } else {  // if height > 175.55
//                      return 1;
//                    }
//                  } else {  // if maxEdgeLength > 312.63
//                    if (height <= 177.51) {
//                      return 0;
//                    } else {  // if height > 177.51
//                      return 1;
//                    }
//                  }
//                } else {  // if xyRatio > 1.8705
//                  if (avgShift <= 0.0058396) {
//                    if (stdShiftX <= 0.016959) {
//                      return 0;
//                    } else {  // if stdShiftX > 0.016959
//                      return 1;
//                    }
//                  } else {  // if avgShift > 0.0058396
//                    if (stdShiftX <= 0.15617) {
//                      return 1;
//                    } else {  // if stdShiftX > 0.15617
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if height > 178.5
//                if (avgErr <= 1.0103) {
//                  return 1;
//                } else {  // if avgErr > 1.0103
//                  if (height <= 189.54) {
//                    if (avgShiftX <= 1.8683) {
//                      return 1;
//                    } else {  // if avgShiftX > 1.8683
//                      return 0;
//                    }
//                  } else {  // if height > 189.54
//                    if (avgShiftY <= 1.5239) {
//                      return 0;
//                    } else {  // if avgShiftY > 1.5239
//                      return 1;
//                    }
//                  }
//                }
//              }
//            } else {  // if xyRatio > 1.9761
//              if (height <= 163.64) {
//                if (avgShiftY <= -0.012383) {
//                  if (avgErr <= 0.98693) {
//                    if (maxEdgeLength <= 294.07) {
//                      return 1;
//                    } else {  // if maxEdgeLength > 294.07
//                      return 0;
//                    }
//                  } else {  // if avgErr > 0.98693
//                    return 1;
//                  }
//                } else {  // if avgShiftY > -0.012383
//                  if (stdShift <= 0.0050562) {
//                    if (stdShiftX <= 0.042665) {
//                      return 1;
//                    } else {  // if stdShiftX > 0.042665
//                      return 0;
//                    }
//                  } else {  // if stdShift > 0.0050562
//                    if (xyRatio <= 2.1659) {
//                      return 1;
//                    } else {  // if xyRatio > 2.1659
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if height > 163.64
//                return 0;
//              }
//            }
//          } else {  // if maxEdgeLength > 316.98
//            if (maxEdgeLength <= 330.09) {
//              if (height <= 174.53) {
//                if (xyRatio <= 1.9262) {
//                  return 0;
//                } else {  // if xyRatio > 1.9262
//                  if (maxEdgeLength <= 323.26) {
//                    if (stdShiftX <= 0.048678) {
//                      return 1;
//                    } else {  // if stdShiftX > 0.048678
//                      return 1;
//                    }
//                  } else {  // if maxEdgeLength > 323.26
//                    if (xyRatio <= 2.0637) {
//                      return 0;
//                    } else {  // if xyRatio > 2.0637
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if height > 174.53
//                if (height <= 191.89) {
//                  if (xyRatio <= 2.0048) {
//                    if (stdShiftY <= 0.025571) {
//                      return 1;
//                    } else {  // if stdShiftY > 0.025571
//                      return 1;
//                    }
//                  } else {  // if xyRatio > 2.0048
//                    if (height <= 174.64) {
//                      return 1;
//                    } else {  // if height > 174.64
//                      return 0;
//                    }
//                  }
//                } else {  // if height > 191.89
//                  if (width <= 320.68) {
//                    return 0;
//                  } else {  // if width > 320.68
//                    return 1;
//                  }
//                }
//              }
//            } else {  // if maxEdgeLength > 330.09
//              if (maxEdgeLength <= 985.96) {
//                if (avgShift <= 3.3987e-06) {
//                  return 0;
//                } else {  // if avgShift > 3.3987e-06
//                  if (stdShiftY <= 0.013964) {
//                    if (xyRatio <= 2.0335) {
//                      return 0;
//                    } else {  // if xyRatio > 2.0335
//                      return 1;
//                    }
//                  } else {  // if stdShiftY > 0.013964
//                    if (width <= 330.54) {
//                      return 1;
//                    } else {  // if width > 330.54
//                      return 1;
//                    }
//                  }
//                }
//              } else {  // if maxEdgeLength > 985.96
//                return 0;
//              }
//            }
//          }
//        }
//      }
//    }
//  }
//}

} // namespace rm
