#include "strm/tree/MTA.hpp"

namespace rm {

int MTA(float maxEdgeLength, float area, float HWRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr, float confidence) {
  if (HWRatio <= 1.012) {
    if (area <= 1796) {
      if (avgErr <= 9.0609) {
        if (maxEdgeLength <= 41.5) {
          if (shiftStd <= 0.00042214) {
            return 3;
          } else {  // if shiftStd > 0.00042214
            return 0;
          }
        } else {  // if maxEdgeLength > 41.5
          return 3;
        }
      } else {  // if avgErr > 9.0609
        return 0;
      }
    } else {  // if area > 1796
      if (shiftStd <= 0.030721) {
        if (HWRatio <= 0.86779) {
          if (avgErr <= 0.60495) {
            if (shiftAvg <= 1.1954e-05) {
              if (shiftNcc <= 0.50199) {
                if (shiftStd <= 8.1223e-05) {
                  if (avgErr <= 0.035893) {
                    if (avgErr <= 0.016695) {
                      return 4;
                    } else {  // if avgErr > 0.016695
                      return 3;
                    }
                  } else {  // if avgErr > 0.035893
                    if (maxEdgeLength <= 61.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 61.5
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 8.1223e-05
                  if (shiftStd <= 8.5008e-05) {
                    return 3;
                  } else {  // if shiftStd > 8.5008e-05
                    if (shiftAvg <= 1.9828e-06) {
                      return 4;
                    } else {  // if shiftAvg > 1.9828e-06
                      return 4;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.50199
                return 3;
              }
            } else {  // if shiftAvg > 1.1954e-05
              if (maxEdgeLength <= 59.5) {
                if (shiftStd <= 0.00063248) {
                  if (area <= 2580.5) {
                    if (shiftStd <= 0.00012039) {
                      return 4;
                    } else {  // if shiftStd > 0.00012039
                      return 0;
                    }
                  } else {  // if area > 2580.5
                    return 1;
                  }
                } else {  // if shiftStd > 0.00063248
                  return 4;
                }
              } else {  // if maxEdgeLength > 59.5
                if (avgErr <= 0.36806) {
                  if (avgErr <= 0.34522) {
                    if (shiftStd <= 0.00026455) {
                      return 2;
                    } else {  // if shiftStd > 0.00026455
                      return 4;
                    }
                  } else {  // if avgErr > 0.34522
                    if (shiftStd <= 0.0001497) {
                      return 4;
                    } else {  // if shiftStd > 0.0001497
                      return 3;
                    }
                  }
                } else {  // if avgErr > 0.36806
                  if (HWRatio <= 0.85934) {
                    if (shiftAvg <= 1.2501e-05) {
                      return 3;
                    } else {  // if shiftAvg > 1.2501e-05
                      return 4;
                    }
                  } else {  // if HWRatio > 0.85934
                    return 0;
                  }
                }
              }
            }
          } else {  // if avgErr > 0.60495
            if (avgErr <= 6.1489) {
              if (shiftNcc <= 0.45315) {
                if (shiftNcc <= -0.099429) {
                  return 2;
                } else {  // if shiftNcc > -0.099429
                  if (area <= 5637) {
                    if (avgErr <= 3.9828) {
                      return 4;
                    } else {  // if avgErr > 3.9828
                      return 4;
                    }
                  } else {  // if area > 5637
                    if (shiftNcc <= 0.0084518) {
                      return 2;
                    } else {  // if shiftNcc > 0.0084518
                      return 4;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.45315
                if (avgErr <= 0.86534) {
                  return 1;
                } else {  // if avgErr > 0.86534
                  if (shiftStd <= 0.0066754) {
                    return 4;
                  } else {  // if shiftStd > 0.0066754
                    if (HWRatio <= 0.82986) {
                      return 3;
                    } else {  // if HWRatio > 0.82986
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgErr > 6.1489
              if (area <= 7382) {
                if (area <= 2573.5) {
                  if (shiftStd <= 0.02119) {
                    return 4;
                  } else {  // if shiftStd > 0.02119
                    return 3;
                  }
                } else {  // if area > 2573.5
                  if (maxEdgeLength <= 77) {
                    return 1;
                  } else {  // if maxEdgeLength > 77
                    return 3;
                  }
                }
              } else {  // if area > 7382
                return 4;
              }
            }
          }
        } else {  // if HWRatio > 0.86779
          if (shiftAvg <= 0.0052777) {
            if (avgErr <= 0.58061) {
              if (area <= 3274.5) {
                if (area <= 2884.5) {
                  if (area <= 2753) {
                    if (maxEdgeLength <= 48.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 48.5
                      return 4;
                    }
                  } else {  // if area > 2753
                    if (shiftAvg <= 7.7647e-06) {
                      return 1;
                    } else {  // if shiftAvg > 7.7647e-06
                      return 3;
                    }
                  }
                } else {  // if area > 2884.5
                  if (avgErr <= 0.52059) {
                    if (shiftNcc <= -0.020359) {
                      return 4;
                    } else {  // if shiftNcc > -0.020359
                      return 4;
                    }
                  } else {  // if avgErr > 0.52059
                    return 3;
                  }
                }
              } else {  // if area > 3274.5
                if (maxEdgeLength <= 63.5) {
                  if (avgErr <= 0.26579) {
                    if (shiftStd <= 8.9827e-05) {
                      return 4;
                    } else {  // if shiftStd > 8.9827e-05
                      return 2;
                    }
                  } else {  // if avgErr > 0.26579
                    if (shiftNcc <= 0.12433) {
                      return 4;
                    } else {  // if shiftNcc > 0.12433
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 63.5
                  if (shiftStd <= 0.00083409) {
                    if (shiftAvg <= 1.1875e-05) {
                      return 4;
                    } else {  // if shiftAvg > 1.1875e-05
                      return 2;
                    }
                  } else {  // if shiftStd > 0.00083409
                    return 0;
                  }
                }
              }
            } else {  // if avgErr > 0.58061
              if (avgErr <= 1.7103) {
                if (area <= 2643) {
                  if (shiftAvg <= 5.2084e-05) {
                    return 4;
                  } else {  // if shiftAvg > 5.2084e-05
                    if (avgErr <= 0.75024) {
                      return 2;
                    } else {  // if avgErr > 0.75024
                      return 4;
                    }
                  }
                } else {  // if area > 2643
                  if (shiftAvg <= 5.5232e-06) {
                    if (avgErr <= 1.0695) {
                      return 4;
                    } else {  // if avgErr > 1.0695
                      return 0;
                    }
                  } else {  // if shiftAvg > 5.5232e-06
                    if (shiftAvg <= 0.0025129) {
                      return 4;
                    } else {  // if shiftAvg > 0.0025129
                      return 0;
                    }
                  }
                }
              } else {  // if avgErr > 1.7103
                if (area <= 2725.5) {
                  if (shiftStd <= 0.0057814) {
                    return 4;
                  } else {  // if shiftStd > 0.0057814
                    if (shiftStd <= 0.019682) {
                      return 1;
                    } else {  // if shiftStd > 0.019682
                      return 0;
                    }
                  }
                } else {  // if area > 2725.5
                  if (avgErr <= 1.7907) {
                    return 1;
                  } else {  // if avgErr > 1.7907
                    if (avgErr <= 3.3835) {
                      return 4;
                    } else {  // if avgErr > 3.3835
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.0052777
            if (avgErr <= 4.1636) {
              if (HWRatio <= 0.92146) {
                return 1;
              } else {  // if HWRatio > 0.92146
                if (shiftAvg <= 0.015896) {
                  if (shiftAvg <= 0.01484) {
                    return 2;
                  } else {  // if shiftAvg > 0.01484
                    if (area <= 3967) {
                      return 2;
                    } else {  // if area > 3967
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.015896
                  if (shiftAvg <= 0.019835) {
                    return 4;
                  } else {  // if shiftAvg > 0.019835
                    return 1;
                  }
                }
              }
            } else {  // if avgErr > 4.1636
              if (shiftAvg <= 0.022209) {
                if (maxEdgeLength <= 55.5) {
                  return 1;
                } else {  // if maxEdgeLength > 55.5
                  if (HWRatio <= 0.94025) {
                    return 4;
                  } else {  // if HWRatio > 0.94025
                    if (area <= 5124) {
                      return 3;
                    } else {  // if area > 5124
                      return 4;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.022209
                if (avgErr <= 6.5709) {
                  return 2;
                } else {  // if avgErr > 6.5709
                  if (shiftNcc <= 0.75356) {
                    return 0;
                  } else {  // if shiftNcc > 0.75356
                    return 4;
                  }
                }
              }
            }
          }
        }
      } else {  // if shiftStd > 0.030721
        if (maxEdgeLength <= 86.5) {
          if (area <= 4425) {
            if (area <= 3122) {
              if (HWRatio <= 0.65924) {
                if (HWRatio <= 0.62013) {
                  if (shiftNcc <= 0.93654) {
                    if (HWRatio <= 0.48889) {
                      return 4;
                    } else {  // if HWRatio > 0.48889
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.93654
                    if (shiftNcc <= 0.97082) {
                      return 0;
                    } else {  // if shiftNcc > 0.97082
                      return 4;
                    }
                  }
                } else {  // if HWRatio > 0.62013
                  if (HWRatio <= 0.63069) {
                    if (shiftStd <= 0.034202) {
                      return 4;
                    } else {  // if shiftStd > 0.034202
                      return 3;
                    }
                  } else {  // if HWRatio > 0.63069
                    if (area <= 2848) {
                      return 4;
                    } else {  // if area > 2848
                      return 0;
                    }
                  }
                }
              } else {  // if HWRatio > 0.65924
                if (area <= 2578) {
                  if (shiftAvg <= 0.27112) {
                    if (avgErr <= 5.1955) {
                      return 4;
                    } else {  // if avgErr > 5.1955
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.27112
                    return 4;
                  }
                } else {  // if area > 2578
                  if (avgErr <= 3.1624) {
                    return 4;
                  } else {  // if avgErr > 3.1624
                    if (shiftNcc <= 0.01084) {
                      return 1;
                    } else {  // if shiftNcc > 0.01084
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 3122
              if (maxEdgeLength <= 64.5) {
                if (area <= 3256.5) {
                  if (HWRatio <= 0.89915) {
                    if (shiftAvg <= 0.038489) {
                      return 2;
                    } else {  // if shiftAvg > 0.038489
                      return 1;
                    }
                  } else {  // if HWRatio > 0.89915
                    if (shiftStd <= 0.037607) {
                      return 2;
                    } else {  // if shiftStd > 0.037607
                      return 4;
                    }
                  }
                } else {  // if area > 3256.5
                  if (maxEdgeLength <= 59.5) {
                    if (avgErr <= 3.8317) {
                      return 3;
                    } else {  // if avgErr > 3.8317
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 59.5
                    if (shiftStd <= 0.25284) {
                      return 4;
                    } else {  // if shiftStd > 0.25284
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 64.5
                if (HWRatio <= 0.79438) {
                  if (avgErr <= 6.0529) {
                    if (avgErr <= 1.8493) {
                      return 4;
                    } else {  // if avgErr > 1.8493
                      return 4;
                    }
                  } else {  // if avgErr > 6.0529
                    if (avgErr <= 6.3548) {
                      return 0;
                    } else {  // if avgErr > 6.3548
                      return 4;
                    }
                  }
                } else {  // if HWRatio > 0.79438
                  if (shiftAvg <= 0.019191) {
                    if (shiftNcc <= 0.047866) {
                      return 4;
                    } else {  // if shiftNcc > 0.047866
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.019191
                    if (avgErr <= 6.2599) {
                      return 0;
                    } else {  // if avgErr > 6.2599
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if area > 4425
            if (area <= 5773.5) {
              if (maxEdgeLength <= 68.5) {
                if (shiftStd <= 0.071216) {
                  return 0;
                } else {  // if shiftStd > 0.071216
                  if (HWRatio <= 0.99265) {
                    return 3;
                  } else {  // if HWRatio > 0.99265
                    if (avgErr <= 10.497) {
                      return 0;
                    } else {  // if avgErr > 10.497
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 68.5
                if (shiftAvg <= 0.032623) {
                  if (avgErr <= 3.6424) {
                    if (maxEdgeLength <= 72.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 72.5
                      return 1;
                    }
                  } else {  // if avgErr > 3.6424
                    if (area <= 4929) {
                      return 3;
                    } else {  // if area > 4929
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.032623
                  if (maxEdgeLength <= 78.5) {
                    if (shiftAvg <= 25.226) {
                      return 1;
                    } else {  // if shiftAvg > 25.226
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 78.5
                    if (area <= 5482.5) {
                      return 3;
                    } else {  // if area > 5482.5
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 5773.5
              if (area <= 6927.5) {
                if (shiftStd <= 0.29667) {
                  if (area <= 6512.5) {
                    if (avgErr <= 12.642) {
                      return 2;
                    } else {  // if avgErr > 12.642
                      return 4;
                    }
                  } else {  // if area > 6512.5
                    if (area <= 6721.5) {
                      return 4;
                    } else {  // if area > 6721.5
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.29667
                  if (shiftStd <= 0.72689) {
                    if (shiftAvg <= 0.40646) {
                      return 2;
                    } else {  // if shiftAvg > 0.40646
                      return 2;
                    }
                  } else {  // if shiftStd > 0.72689
                    if (shiftStd <= 1.3396) {
                      return 3;
                    } else {  // if shiftStd > 1.3396
                      return 2;
                    }
                  }
                }
              } else {  // if area > 6927.5
                if (area <= 7055.5) {
                  if (area <= 6968) {
                    return 4;
                  } else {  // if area > 6968
                    if (maxEdgeLength <= 84.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 84.5
                      return 3;
                    }
                  }
                } else {  // if area > 7055.5
                  if (shiftStd <= 0.069675) {
                    return 4;
                  } else {  // if shiftStd > 0.069675
                    if (avgErr <= 12.292) {
                      return 3;
                    } else {  // if avgErr > 12.292
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 86.5
          if (avgErr <= 6.1733) {
            if (area <= 10212) {
              if (maxEdgeLength <= 98.5) {
                if (shiftStd <= 0.19296) {
                  if (shiftNcc <= 0.2213) {
                    return 4;
                  } else {  // if shiftNcc > 0.2213
                    if (shiftNcc <= 0.49212) {
                      return 3;
                    } else {  // if shiftNcc > 0.49212
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.19296
                  if (HWRatio <= 0.81894) {
                    return 2;
                  } else {  // if HWRatio > 0.81894
                    return 3;
                  }
                }
              } else {  // if maxEdgeLength > 98.5
                if (shiftStd <= 0.53671) {
                  if (shiftNcc <= 0.66509) {
                    if (HWRatio <= 0.64401) {
                      return 2;
                    } else {  // if HWRatio > 0.64401
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.66509
                    return 3;
                  }
                } else {  // if shiftStd > 0.53671
                  if (area <= 8934.5) {
                    return 4;
                  } else {  // if area > 8934.5
                    return 0;
                  }
                }
              }
            } else {  // if area > 10212
              if (area <= 12408) {
                if (shiftAvg <= 0.0711) {
                  return 4;
                } else {  // if shiftAvg > 0.0711
                  if (shiftNcc <= 0.85939) {
                    return 1;
                  } else {  // if shiftNcc > 0.85939
                    if (shiftNcc <= 0.91421) {
                      return 3;
                    } else {  // if shiftNcc > 0.91421
                      return 1;
                    }
                  }
                }
              } else {  // if area > 12408
                if (shiftNcc <= 0.46651) {
                  return 4;
                } else {  // if shiftNcc > 0.46651
                  return 2;
                }
              }
            }
          } else {  // if avgErr > 6.1733
            if (shiftAvg <= 0.56448) {
              if (maxEdgeLength <= 95.5) {
                if (shiftAvg <= 0.019301) {
                  return 3;
                } else {  // if shiftAvg > 0.019301
                  if (avgErr <= 11.253) {
                    if (avgErr <= 8.5508) {
                      return 4;
                    } else {  // if avgErr > 8.5508
                      return 3;
                    }
                  } else {  // if avgErr > 11.253
                    if (shiftStd <= 1.0164) {
                      return 4;
                    } else {  // if shiftStd > 1.0164
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 95.5
                if (HWRatio <= 1.0039) {
                  return 4;
                } else {  // if HWRatio > 1.0039
                  if (shiftNcc <= 0.57707) {
                    return 3;
                  } else {  // if shiftNcc > 0.57707
                    return 4;
                  }
                }
              }
            } else {  // if shiftAvg > 0.56448
              if (avgErr <= 13.298) {
                if (shiftNcc <= 0.799) {
                  if (HWRatio <= 0.97312) {
                    if (area <= 7778) {
                      return 3;
                    } else {  // if area > 7778
                      return 4;
                    }
                  } else {  // if HWRatio > 0.97312
                    return 2;
                  }
                } else {  // if shiftNcc > 0.799
                  return 3;
                }
              } else {  // if avgErr > 13.298
                if (maxEdgeLength <= 90.5) {
                  return 3;
                } else {  // if maxEdgeLength > 90.5
                  return 4;
                }
              }
            }
          }
        }
      }
    }
  } else {  // if HWRatio > 1.012
    if (area <= 5642.5) {
      if (area <= 4476.5) {
        if (maxEdgeLength <= 64.5) {
          if (area <= 3123.5) {
            if (area <= 2476.5) {
              if (avgErr <= 2.0096) {
                if (HWRatio <= 1.0421) {
                  if (maxEdgeLength <= 49) {
                    if (shiftNcc <= -0.023283) {
                      return 3;
                    } else {  // if shiftNcc > -0.023283
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 49
                    return 0;
                  }
                } else {  // if HWRatio > 1.0421
                  if (shiftAvg <= 0.021343) {
                    if (shiftNcc <= 0.42077) {
                      return 4;
                    } else {  // if shiftNcc > 0.42077
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.021343
                    return 3;
                  }
                }
              } else {  // if avgErr > 2.0096
                if (area <= 2026) {
                  if (area <= 1967) {
                    if (shiftNcc <= 0.71344) {
                      return 1;
                    } else {  // if shiftNcc > 0.71344
                      return 3;
                    }
                  } else {  // if area > 1967
                    if (shiftNcc <= 0.14406) {
                      return 3;
                    } else {  // if shiftNcc > 0.14406
                      return 2;
                    }
                  }
                } else {  // if area > 2026
                  if (maxEdgeLength <= 48.5) {
                    if (area <= 2069) {
                      return 3;
                    } else {  // if area > 2069
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 48.5
                    if (avgErr <= 9.1921) {
                      return 0;
                    } else {  // if avgErr > 9.1921
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 2476.5
              if (HWRatio <= 1.2474) {
                if (shiftStd <= 0.012454) {
                  if (shiftStd <= 0.00027896) {
                    if (shiftStd <= 0.00010811) {
                      return 4;
                    } else {  // if shiftStd > 0.00010811
                      return 1;
                    }
                  } else {  // if shiftStd > 0.00027896
                    if (avgErr <= 5.3665) {
                      return 4;
                    } else {  // if avgErr > 5.3665
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 0.012454
                  if (avgErr <= 10.305) {
                    if (HWRatio <= 1.0692) {
                      return 4;
                    } else {  // if HWRatio > 1.0692
                      return 1;
                    }
                  } else {  // if avgErr > 10.305
                    if (area <= 2936.5) {
                      return 4;
                    } else {  // if area > 2936.5
                      return 1;
                    }
                  }
                }
              } else {  // if HWRatio > 1.2474
                if (shiftAvg <= 0.27069) {
                  if (area <= 3048) {
                    if (shiftAvg <= 5.2259e-05) {
                      return 1;
                    } else {  // if shiftAvg > 5.2259e-05
                      return 1;
                    }
                  } else {  // if area > 3048
                    if (avgErr <= 5.3633) {
                      return 1;
                    } else {  // if avgErr > 5.3633
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.27069
                  if (shiftNcc <= 0.82584) {
                    return 4;
                  } else {  // if shiftNcc > 0.82584
                    if (HWRatio <= 1.3111) {
                      return 3;
                    } else {  // if HWRatio > 1.3111
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 3123.5
            if (shiftStd <= 0.016508) {
              if (avgErr <= 0.58995) {
                if (avgErr <= 0.083663) {
                  return 4;
                } else {  // if avgErr > 0.083663
                  if (shiftAvg <= 7.3916e-06) {
                    if (HWRatio <= 1.1078) {
                      return 2;
                    } else {  // if HWRatio > 1.1078
                      return 3;
                    }
                  } else {  // if shiftAvg > 7.3916e-06
                    return 2;
                  }
                }
              } else {  // if avgErr > 0.58995
                if (area <= 3240.5) {
                  if (shiftNcc <= 0.10534) {
                    if (HWRatio <= 1.0455) {
                      return 4;
                    } else {  // if HWRatio > 1.0455
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.10534
                    return 4;
                  }
                } else {  // if area > 3240.5
                  if (area <= 3969) {
                    if (shiftNcc <= 0.3095) {
                      return 4;
                    } else {  // if shiftNcc > 0.3095
                      return 2;
                    }
                  } else {  // if area > 3969
                    return 3;
                  }
                }
              }
            } else {  // if shiftStd > 0.016508
              if (avgErr <= 7.5411) {
                if (shiftAvg <= 0.04143) {
                  if (shiftAvg <= 0.0065349) {
                    if (area <= 3714.5) {
                      return 2;
                    } else {  // if area > 3714.5
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.0065349
                    if (shiftNcc <= 0.0032429) {
                      return 4;
                    } else {  // if shiftNcc > 0.0032429
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.04143
                  if (area <= 3937) {
                    if (shiftAvg <= 0.16185) {
                      return 2;
                    } else {  // if shiftAvg > 0.16185
                      return 1;
                    }
                  } else {  // if area > 3937
                    return 3;
                  }
                }
              } else {  // if avgErr > 7.5411
                if (maxEdgeLength <= 61.5) {
                  if (shiftAvg <= 0.055169) {
                    if (shiftNcc <= 0.072767) {
                      return 4;
                    } else {  // if shiftNcc > 0.072767
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.055169
                    return 4;
                  }
                } else {  // if maxEdgeLength > 61.5
                  if (shiftNcc <= 0.15877) {
                    if (shiftNcc <= 0.06604) {
                      return 2;
                    } else {  // if shiftNcc > 0.06604
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.15877
                    if (HWRatio <= 1.2575) {
                      return 2;
                    } else {  // if HWRatio > 1.2575
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 64.5
          if (HWRatio <= 2.1906) {
            if (area <= 4318) {
              if (shiftAvg <= 0.014743) {
                if (area <= 4106) {
                  if (area <= 3103.5) {
                    if (HWRatio <= 1.7125) {
                      return 1;
                    } else {  // if HWRatio > 1.7125
                      return 0;
                    }
                  } else {  // if area > 3103.5
                    if (shiftStd <= 4.7544e-05) {
                      return 2;
                    } else {  // if shiftStd > 4.7544e-05
                      return 0;
                    }
                  }
                } else {  // if area > 4106
                  if (HWRatio <= 1.2672) {
                    if (shiftStd <= 0.10785) {
                      return 0;
                    } else {  // if shiftStd > 0.10785
                      return 0;
                    }
                  } else {  // if HWRatio > 1.2672
                    if (shiftAvg <= 0.011967) {
                      return 3;
                    } else {  // if shiftAvg > 0.011967
                      return 0;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.014743
                if (avgErr <= 10.02) {
                  if (area <= 3673.5) {
                    if (area <= 3260) {
                      return 0;
                    } else {  // if area > 3260
                      return 0;
                    }
                  } else {  // if area > 3673.5
                    if (shiftAvg <= 0.044125) {
                      return 0;
                    } else {  // if shiftAvg > 0.044125
                      return 0;
                    }
                  }
                } else {  // if avgErr > 10.02
                  if (HWRatio <= 1.8738) {
                    if (shiftAvg <= 2.2275) {
                      return 0;
                    } else {  // if shiftAvg > 2.2275
                      return 4;
                    }
                  } else {  // if HWRatio > 1.8738
                    if (shiftNcc <= 0.65492) {
                      return 0;
                    } else {  // if shiftNcc > 0.65492
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 4318
              if (shiftNcc <= 0.012924) {
                if (avgErr <= 1.0307) {
                  if (avgErr <= 0.24771) {
                    return 0;
                  } else {  // if avgErr > 0.24771
                    return 4;
                  }
                } else {  // if avgErr > 1.0307
                  if (maxEdgeLength <= 78.5) {
                    if (avgErr <= 8.6431) {
                      return 3;
                    } else {  // if avgErr > 8.6431
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 78.5
                    if (area <= 4460) {
                      return 0;
                    } else {  // if area > 4460
                      return 3;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.012924
                if (shiftStd <= 1.1388) {
                  if (shiftAvg <= 0.0014327) {
                    if (maxEdgeLength <= 78) {
                      return 4;
                    } else {  // if maxEdgeLength > 78
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.0014327
                    if (area <= 4348.5) {
                      return 1;
                    } else {  // if area > 4348.5
                      return 0;
                    }
                  }
                } else {  // if shiftStd > 1.1388
                  if (shiftStd <= 29.632) {
                    if (shiftNcc <= 0.35108) {
                      return 3;
                    } else {  // if shiftNcc > 0.35108
                      return 4;
                    }
                  } else {  // if shiftStd > 29.632
                    return 0;
                  }
                }
              }
            }
          } else {  // if HWRatio > 2.1906
            if (shiftStd <= 0.079829) {
              if (avgErr <= 0.91558) {
                return 2;
              } else {  // if avgErr > 0.91558
                if (shiftNcc <= 0.52142) {
                  if (shiftAvg <= 0.00095408) {
                    if (HWRatio <= 2.303) {
                      return 3;
                    } else {  // if HWRatio > 2.303
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.00095408
                    if (maxEdgeLength <= 94.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 94.5
                      return 3;
                    }
                  }
                } else {  // if shiftNcc > 0.52142
                  return 0;
                }
              }
            } else {  // if shiftStd > 0.079829
              if (avgErr <= 17.808) {
                if (area <= 1822) {
                  return 3;
                } else {  // if area > 1822
                  if (area <= 3701) {
                    if (shiftAvg <= 1.5603) {
                      return 4;
                    } else {  // if shiftAvg > 1.5603
                      return 3;
                    }
                  } else {  // if area > 3701
                    if (area <= 3737.5) {
                      return 2;
                    } else {  // if area > 3737.5
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 17.808
                return 0;
              }
            }
          }
        }
      } else {  // if area > 4476.5
        if (maxEdgeLength <= 97.5) {
          if (area <= 5492) {
            if (shiftAvg <= 0.0040531) {
              if (avgErr <= 3.3842) {
                if (shiftNcc <= 0.018364) {
                  if (area <= 4634.5) {
                    if (shiftStd <= 0.004924) {
                      return 3;
                    } else {  // if shiftStd > 0.004924
                      return 4;
                    }
                  } else {  // if area > 4634.5
                    if (HWRatio <= 1.796) {
                      return 1;
                    } else {  // if HWRatio > 1.796
                      return 4;
                    }
                  }
                } else {  // if shiftNcc > 0.018364
                  if (maxEdgeLength <= 91.5) {
                    if (HWRatio <= 1.1194) {
                      return 4;
                    } else {  // if HWRatio > 1.1194
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 91.5
                    if (shiftNcc <= 0.30544) {
                      return 4;
                    } else {  // if shiftNcc > 0.30544
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 3.3842
                if (avgErr <= 4.9323) {
                  if (HWRatio <= 1.6697) {
                    if (maxEdgeLength <= 71.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 71.5
                      return 1;
                    }
                  } else {  // if HWRatio > 1.6697
                    if (avgErr <= 4.0623) {
                      return 1;
                    } else {  // if avgErr > 4.0623
                      return 3;
                    }
                  }
                } else {  // if avgErr > 4.9323
                  if (shiftNcc <= 0.079451) {
                    if (shiftNcc <= -0.0092146) {
                      return 1;
                    } else {  // if shiftNcc > -0.0092146
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.079451
                    if (avgErr <= 11.468) {
                      return 3;
                    } else {  // if avgErr > 11.468
                      return 1;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.0040531
              if (shiftNcc <= 0.15394) {
                if (area <= 4552) {
                  if (area <= 4513) {
                    if (HWRatio <= 1.2691) {
                      return 4;
                    } else {  // if HWRatio > 1.2691
                      return 1;
                    }
                  } else {  // if area > 4513
                    if (avgErr <= 5.6362) {
                      return 1;
                    } else {  // if avgErr > 5.6362
                      return 0;
                    }
                  }
                } else {  // if area > 4552
                  if (avgErr <= 13.896) {
                    if (shiftAvg <= 0.51098) {
                      return 1;
                    } else {  // if shiftAvg > 0.51098
                      return 4;
                    }
                  } else {  // if avgErr > 13.896
                    if (shiftAvg <= 0.043028) {
                      return 1;
                    } else {  // if shiftAvg > 0.043028
                      return 3;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.15394
                if (avgErr <= 8.3254) {
                  if (area <= 4547) {
                    if (area <= 4513) {
                      return 1;
                    } else {  // if area > 4513
                      return 0;
                    }
                  } else {  // if area > 4547
                    if (avgErr <= 5.1187) {
                      return 1;
                    } else {  // if avgErr > 5.1187
                      return 1;
                    }
                  }
                } else {  // if avgErr > 8.3254
                  if (shiftAvg <= 0.01552) {
                    if (shiftStd <= 0.098781) {
                      return 3;
                    } else {  // if shiftStd > 0.098781
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.01552
                    if (area <= 4585) {
                      return 1;
                    } else {  // if area > 4585
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 5492
            if (area <= 5625) {
              if (area <= 5589.5) {
                if (area <= 5527) {
                  if (HWRatio <= 1.2075) {
                    if (shiftAvg <= 0.38858) {
                      return 1;
                    } else {  // if shiftAvg > 0.38858
                      return 3;
                    }
                  } else {  // if HWRatio > 1.2075
                    if (area <= 5507) {
                      return 1;
                    } else {  // if area > 5507
                      return 2;
                    }
                  }
                } else {  // if area > 5527
                  if (HWRatio <= 1.2223) {
                    if (area <= 5582.5) {
                      return 2;
                    } else {  // if area > 5582.5
                      return 1;
                    }
                  } else {  // if HWRatio > 1.2223
                    if (area <= 5548.5) {
                      return 1;
                    } else {  // if area > 5548.5
                      return 1;
                    }
                  }
                }
              } else {  // if area > 5589.5
                if (maxEdgeLength <= 82.5) {
                  if (area <= 5622.5) {
                    if (maxEdgeLength <= 77.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 77.5
                      return 1;
                    }
                  } else {  // if area > 5622.5
                    if (shiftNcc <= 0.3469) {
                      return 4;
                    } else {  // if shiftNcc > 0.3469
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 82.5
                  if (avgErr <= 2.6841) {
                    if (shiftNcc <= 0.015103) {
                      return 2;
                    } else {  // if shiftNcc > 0.015103
                      return 3;
                    }
                  } else {  // if avgErr > 2.6841
                    if (shiftAvg <= 0.0049065) {
                      return 2;
                    } else {  // if shiftAvg > 0.0049065
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 5625
              if (shiftStd <= 0.00017835) {
                return 4;
              } else {  // if shiftStd > 0.00017835
                if (shiftStd <= 0.063537) {
                  if (shiftStd <= 0.057667) {
                    if (shiftNcc <= 0.25008) {
                      return 1;
                    } else {  // if shiftNcc > 0.25008
                      return 1;
                    }
                  } else {  // if shiftStd > 0.057667
                    return 4;
                  }
                } else {  // if shiftStd > 0.063537
                  return 1;
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 97.5
          if (HWRatio <= 2.2504) {
            if (avgErr <= 5.956) {
              if (shiftAvg <= 0.0048202) {
                if (avgErr <= 2.3209) {
                  if (shiftAvg <= 2.9524e-06) {
                    return 1;
                  } else {  // if shiftAvg > 2.9524e-06
                    if (HWRatio <= 1.916) {
                      return 4;
                    } else {  // if HWRatio > 1.916
                      return 0;
                    }
                  }
                } else {  // if avgErr > 2.3209
                  if (HWRatio <= 1.8319) {
                    return 0;
                  } else {  // if HWRatio > 1.8319
                    return 3;
                  }
                }
              } else {  // if shiftAvg > 0.0048202
                if (shiftAvg <= 0.48555) {
                  if (maxEdgeLength <= 100.5) {
                    if (avgErr <= 2.7251) {
                      return 1;
                    } else {  // if avgErr > 2.7251
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 100.5
                    if (HWRatio <= 2.1633) {
                      return 1;
                    } else {  // if HWRatio > 2.1633
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.48555
                  if (area <= 5050) {
                    return 3;
                  } else {  // if area > 5050
                    return 4;
                  }
                }
              }
            } else {  // if avgErr > 5.956
              if (shiftAvg <= 0.39593) {
                if (shiftAvg <= 0.083631) {
                  if (shiftAvg <= 0.062907) {
                    if (area <= 4949) {
                      return 3;
                    } else {  // if area > 4949
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.062907
                    if (maxEdgeLength <= 105.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 105.5
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.083631
                  if (shiftStd <= 0.23943) {
                    if (avgErr <= 16.417) {
                      return 0;
                    } else {  // if avgErr > 16.417
                      return 4;
                    }
                  } else {  // if shiftStd > 0.23943
                    if (shiftAvg <= 0.21164) {
                      return 0;
                    } else {  // if shiftAvg > 0.21164
                      return 1;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.39593
                if (HWRatio <= 1.9612) {
                  if (shiftNcc <= 0.70372) {
                    if (shiftNcc <= 0.38359) {
                      return 1;
                    } else {  // if shiftNcc > 0.38359
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.70372
                    return 1;
                  }
                } else {  // if HWRatio > 1.9612
                  if (avgErr <= 10.056) {
                    if (HWRatio <= 2.0288) {
                      return 0;
                    } else {  // if HWRatio > 2.0288
                      return 1;
                    }
                  } else {  // if avgErr > 10.056
                    if (shiftStd <= 1.3945) {
                      return 4;
                    } else {  // if shiftStd > 1.3945
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if HWRatio > 2.2504
            if (shiftStd <= 0.093283) {
              if (HWRatio <= 2.3011) {
                return 0;
              } else {  // if HWRatio > 2.3011
                if (HWRatio <= 2.4747) {
                  return 1;
                } else {  // if HWRatio > 2.4747
                  return 4;
                }
              }
            } else {  // if shiftStd > 0.093283
              if (area <= 4518.5) {
                if (maxEdgeLength <= 106) {
                  return 3;
                } else {  // if maxEdgeLength > 106
                  return 4;
                }
              } else {  // if area > 4518.5
                if (shiftAvg <= 0.047773) {
                  return 0;
                } else {  // if shiftAvg > 0.047773
                  if (shiftStd <= 0.7713) {
                    return 4;
                  } else {  // if shiftStd > 0.7713
                    if (shiftStd <= 0.96695) {
                      return 3;
                    } else {  // if shiftStd > 0.96695
                      return 4;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if area > 5642.5
      if (maxEdgeLength <= 97.5) {
        if (area <= 7025) {
          if (area <= 5782.5) {
            if (HWRatio <= 1.4642) {
              if (HWRatio <= 1.3283) {
                if (HWRatio <= 1.1643) {
                  if (HWRatio <= 1.1338) {
                    if (shiftNcc <= 0.031685) {
                      return 3;
                    } else {  // if shiftNcc > 0.031685
                      return 2;
                    }
                  } else {  // if HWRatio > 1.1338
                    if (avgErr <= 8.5941) {
                      return 1;
                    } else {  // if avgErr > 8.5941
                      return 1;
                    }
                  }
                } else {  // if HWRatio > 1.1643
                  if (shiftStd <= 1.4706) {
                    if (shiftStd <= 0.059919) {
                      return 2;
                    } else {  // if shiftStd > 0.059919
                      return 2;
                    }
                  } else {  // if shiftStd > 1.4706
                    return 3;
                  }
                }
              } else {  // if HWRatio > 1.3283
                if (HWRatio <= 1.3722) {
                  if (shiftNcc <= 0.035899) {
                    if (HWRatio <= 1.3462) {
                      return 1;
                    } else {  // if HWRatio > 1.3462
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.035899
                    if (avgErr <= 13.351) {
                      return 1;
                    } else {  // if avgErr > 13.351
                      return 4;
                    }
                  }
                } else {  // if HWRatio > 1.3722
                  if (maxEdgeLength <= 90.5) {
                    if (shiftStd <= 3.5042) {
                      return 2;
                    } else {  // if shiftStd > 3.5042
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 90.5
                    if (shiftStd <= 0.0088085) {
                      return 4;
                    } else {  // if shiftStd > 0.0088085
                      return 1;
                    }
                  }
                }
              }
            } else {  // if HWRatio > 1.4642
              if (shiftStd <= 0.061238) {
                if (shiftStd <= 0.056889) {
                  if (shiftStd <= 0.055547) {
                    if (shiftNcc <= 0.75389) {
                      return 2;
                    } else {  // if shiftNcc > 0.75389
                      return 4;
                    }
                  } else {  // if shiftStd > 0.055547
                    return 4;
                  }
                } else {  // if shiftStd > 0.056889
                  return 3;
                }
              } else {  // if shiftStd > 0.061238
                if (shiftStd <= 0.10432) {
                  if (shiftStd <= 0.10277) {
                    if (shiftAvg <= 0.36344) {
                      return 2;
                    } else {  // if shiftAvg > 0.36344
                      return 3;
                    }
                  } else {  // if shiftStd > 0.10277
                    return 4;
                  }
                } else {  // if shiftStd > 0.10432
                  if (area <= 5686.5) {
                    if (shiftStd <= 0.14103) {
                      return 2;
                    } else {  // if shiftStd > 0.14103
                      return 2;
                    }
                  } else {  // if area > 5686.5
                    return 2;
                  }
                }
              }
            }
          } else {  // if area > 5782.5
            if (shiftAvg <= 0.026119) {
              if (shiftAvg <= 0.0027769) {
                if (avgErr <= 0.2092) {
                  return 2;
                } else {  // if avgErr > 0.2092
                  if (shiftStd <= 0.00029842) {
                    if (shiftStd <= 0.00010241) {
                      return 4;
                    } else {  // if shiftStd > 0.00010241
                      return 3;
                    }
                  } else {  // if shiftStd > 0.00029842
                    if (shiftAvg <= 0.0027544) {
                      return 2;
                    } else {  // if shiftAvg > 0.0027544
                      return 4;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.0027769
                if (avgErr <= 2.1187) {
                  if (avgErr <= 1.0883) {
                    return 3;
                  } else {  // if avgErr > 1.0883
                    if (area <= 6377) {
                      return 2;
                    } else {  // if area > 6377
                      return 2;
                    }
                  }
                } else {  // if avgErr > 2.1187
                  if (HWRatio <= 1.1477) {
                    if (HWRatio <= 1.1379) {
                      return 2;
                    } else {  // if HWRatio > 1.1379
                      return 3;
                    }
                  } else {  // if HWRatio > 1.1477
                    if (shiftAvg <= 0.0099393) {
                      return 2;
                    } else {  // if shiftAvg > 0.0099393
                      return 2;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.026119
              if (avgErr <= 8.6945) {
                if (shiftNcc <= 0.089093) {
                  if (shiftStd <= 1.3488) {
                    if (avgErr <= 3.258) {
                      return 2;
                    } else {  // if avgErr > 3.258
                      return 2;
                    }
                  } else {  // if shiftStd > 1.3488
                    return 4;
                  }
                } else {  // if shiftNcc > 0.089093
                  if (shiftStd <= 0.041578) {
                    if (shiftStd <= 0.040716) {
                      return 2;
                    } else {  // if shiftStd > 0.040716
                      return 4;
                    }
                  } else {  // if shiftStd > 0.041578
                    if (HWRatio <= 1.1161) {
                      return 2;
                    } else {  // if HWRatio > 1.1161
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 8.6945
                if (shiftAvg <= 0.43638) {
                  if (avgErr <= 17.761) {
                    if (maxEdgeLength <= 77.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 77.5
                      return 2;
                    }
                  } else {  // if avgErr > 17.761
                    if (shiftNcc <= 0.3648) {
                      return 2;
                    } else {  // if shiftNcc > 0.3648
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.43638
                  if (shiftStd <= 0.43358) {
                    if (maxEdgeLength <= 89.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 89.5
                      return 4;
                    }
                  } else {  // if shiftStd > 0.43358
                    if (avgErr <= 9.5786) {
                      return 4;
                    } else {  // if avgErr > 9.5786
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 7025
          if (avgErr <= 12.101) {
            if (maxEdgeLength <= 91.5) {
              if (HWRatio <= 1.1313) {
                if (avgErr <= 5.5557) {
                  if (avgErr <= 1.8799) {
                    if (maxEdgeLength <= 88.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 88.5
                      return 4;
                    }
                  } else {  // if avgErr > 1.8799
                    if (area <= 7257) {
                      return 3;
                    } else {  // if area > 7257
                      return 3;
                    }
                  }
                } else {  // if avgErr > 5.5557
                  if (avgErr <= 6.9758) {
                    if (shiftNcc <= 0.81543) {
                      return 4;
                    } else {  // if shiftNcc > 0.81543
                      return 3;
                    }
                  } else {  // if avgErr > 6.9758
                    if (shiftAvg <= 0.31448) {
                      return 3;
                    } else {  // if shiftAvg > 0.31448
                      return 4;
                    }
                  }
                }
              } else {  // if HWRatio > 1.1313
                if (shiftAvg <= 0.012651) {
                  if (shiftNcc <= 0.052582) {
                    return 3;
                  } else {  // if shiftNcc > 0.052582
                    return 4;
                  }
                } else {  // if shiftAvg > 0.012651
                  if (shiftAvg <= 0.2125) {
                    return 2;
                  } else {  // if shiftAvg > 0.2125
                    return 4;
                  }
                }
              }
            } else {  // if maxEdgeLength > 91.5
              if (area <= 7082.5) {
                if (area <= 7040) {
                  return 3;
                } else {  // if area > 7040
                  if (shiftNcc <= 0.081051) {
                    return 3;
                  } else {  // if shiftNcc > 0.081051
                    if (shiftAvg <= 0.048437) {
                      return 2;
                    } else {  // if shiftAvg > 0.048437
                      return 2;
                    }
                  }
                }
              } else {  // if area > 7082.5
                if (HWRatio <= 1.0217) {
                  return 4;
                } else {  // if HWRatio > 1.0217
                  if (HWRatio <= 1.3041) {
                    if (shiftStd <= 0.00021498) {
                      return 4;
                    } else {  // if shiftStd > 0.00021498
                      return 3;
                    }
                  } else {  // if HWRatio > 1.3041
                    if (shiftAvg <= 0.14255) {
                      return 4;
                    } else {  // if shiftAvg > 0.14255
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if avgErr > 12.101
            if (area <= 7543) {
              if (area <= 7371.5) {
                if (HWRatio <= 1.132) {
                  if (HWRatio <= 1.0298) {
                    return 3;
                  } else {  // if HWRatio > 1.0298
                    if (avgErr <= 17.403) {
                      return 4;
                    } else {  // if avgErr > 17.403
                      return 3;
                    }
                  }
                } else {  // if HWRatio > 1.132
                  if (HWRatio <= 1.1519) {
                    if (avgErr <= 12.324) {
                      return 4;
                    } else {  // if avgErr > 12.324
                      return 2;
                    }
                  } else {  // if HWRatio > 1.1519
                    if (shiftStd <= 0.099282) {
                      return 3;
                    } else {  // if shiftStd > 0.099282
                      return 2;
                    }
                  }
                }
              } else {  // if area > 7371.5
                if (avgErr <= 17.575) {
                  if (shiftAvg <= 0.17257) {
                    if (shiftStd <= 1.3916) {
                      return 3;
                    } else {  // if shiftStd > 1.3916
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.17257
                    if (area <= 7471.5) {
                      return 4;
                    } else {  // if area > 7471.5
                      return 3;
                    }
                  }
                } else {  // if avgErr > 17.575
                  return 4;
                }
              }
            } else {  // if area > 7543
              if (shiftStd <= 1.1726) {
                if (HWRatio <= 1.1506) {
                  if (avgErr <= 16.404) {
                    if (shiftNcc <= 0.52807) {
                      return 3;
                    } else {  // if shiftNcc > 0.52807
                      return 4;
                    }
                  } else {  // if avgErr > 16.404
                    if (HWRatio <= 1.1059) {
                      return 3;
                    } else {  // if HWRatio > 1.1059
                      return 3;
                    }
                  }
                } else {  // if HWRatio > 1.1506
                  return 4;
                }
              } else {  // if shiftStd > 1.1726
                return 3;
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 97.5
        if (area <= 10044) {
          if (area <= 7005) {
            if (avgErr <= 4.0536) {
              if (shiftStd <= 0.00089701) {
                if (maxEdgeLength <= 107.5) {
                  if (shiftStd <= 0.00037829) {
                    if (maxEdgeLength <= 106.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 106.5
                      return 3;
                    }
                  } else {  // if shiftStd > 0.00037829
                    return 3;
                  }
                } else {  // if maxEdgeLength > 107.5
                  if (shiftNcc <= 0.018081) {
                    return 4;
                  } else {  // if shiftNcc > 0.018081
                    if (shiftNcc <= 0.034949) {
                      return 3;
                    } else {  // if shiftNcc > 0.034949
                      return 4;
                    }
                  }
                }
              } else {  // if shiftStd > 0.00089701
                if (HWRatio <= 2.1791) {
                  if (area <= 6750.5) {
                    if (area <= 6700.5) {
                      return 2;
                    } else {  // if area > 6700.5
                      return 3;
                    }
                  } else {  // if area > 6750.5
                    if (shiftStd <= 0.054601) {
                      return 2;
                    } else {  // if shiftStd > 0.054601
                      return 0;
                    }
                  }
                } else {  // if HWRatio > 2.1791
                  return 4;
                }
              }
            } else {  // if avgErr > 4.0536
              if (shiftNcc <= 0.48507) {
                if (avgErr <= 9.8586) {
                  if (area <= 6211) {
                    if (shiftStd <= 0.10173) {
                      return 2;
                    } else {  // if shiftStd > 0.10173
                      return 0;
                    }
                  } else {  // if area > 6211
                    if (area <= 6385) {
                      return 0;
                    } else {  // if area > 6385
                      return 0;
                    }
                  }
                } else {  // if avgErr > 9.8586
                  if (shiftAvg <= 0.14001) {
                    if (avgErr <= 11.193) {
                      return 0;
                    } else {  // if avgErr > 11.193
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.14001
                    if (shiftStd <= 1.3973) {
                      return 0;
                    } else {  // if shiftStd > 1.3973
                      return 4;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.48507
                if (avgErr <= 11.692) {
                  if (shiftAvg <= 0.092662) {
                    if (area <= 6595) {
                      return 2;
                    } else {  // if area > 6595
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.092662
                    if (shiftStd <= 2.9258) {
                      return 0;
                    } else {  // if shiftStd > 2.9258
                      return 2;
                    }
                  }
                } else {  // if avgErr > 11.692
                  if (shiftStd <= 0.16589) {
                    if (area <= 6303.5) {
                      return 0;
                    } else {  // if area > 6303.5
                      return 0;
                    }
                  } else {  // if shiftStd > 0.16589
                    if (shiftNcc <= 0.81804) {
                      return 0;
                    } else {  // if shiftNcc > 0.81804
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if area > 7005
            if (shiftAvg <= 0.075774) {
              if (avgErr <= 1.3285) {
                if (maxEdgeLength <= 100.5) {
                  if (HWRatio <= 1.375) {
                    if (HWRatio <= 1.3402) {
                      return 3;
                    } else {  // if HWRatio > 1.3402
                      return 2;
                    }
                  } else {  // if HWRatio > 1.375
                    return 4;
                  }
                } else {  // if maxEdgeLength > 100.5
                  if (HWRatio <= 1.2901) {
                    if (avgErr <= 0.19697) {
                      return 0;
                    } else {  // if avgErr > 0.19697
                      return 4;
                    }
                  } else {  // if HWRatio > 1.2901
                    if (HWRatio <= 1.4934) {
                      return 3;
                    } else {  // if HWRatio > 1.4934
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 1.3285
                if (maxEdgeLength <= 106.5) {
                  if (HWRatio <= 1.1647) {
                    if (shiftAvg <= 0.0069315) {
                      return 0;
                    } else {  // if shiftAvg > 0.0069315
                      return 4;
                    }
                  } else {  // if HWRatio > 1.1647
                    if (avgErr <= 12.9) {
                      return 0;
                    } else {  // if avgErr > 12.9
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 106.5
                  if (shiftAvg <= 0.020728) {
                    if (area <= 9917.5) {
                      return 0;
                    } else {  // if area > 9917.5
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.020728
                    if (avgErr <= 7.6305) {
                      return 0;
                    } else {  // if avgErr > 7.6305
                      return 0;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.075774
              if (HWRatio <= 1.2114) {
                if (avgErr <= 13.157) {
                  if (area <= 9561.5) {
                    if (shiftAvg <= 0.11333) {
                      return 3;
                    } else {  // if shiftAvg > 0.11333
                      return 0;
                    }
                  } else {  // if area > 9561.5
                    if (HWRatio <= 1.1131) {
                      return 4;
                    } else {  // if HWRatio > 1.1131
                      return 3;
                    }
                  }
                } else {  // if avgErr > 13.157
                  if (shiftStd <= 0.30491) {
                    if (shiftNcc <= 0.85371) {
                      return 4;
                    } else {  // if shiftNcc > 0.85371
                      return 4;
                    }
                  } else {  // if shiftStd > 0.30491
                    if (avgErr <= 15.086) {
                      return 0;
                    } else {  // if avgErr > 15.086
                      return 4;
                    }
                  }
                }
              } else {  // if HWRatio > 1.2114
                if (area <= 9829.5) {
                  if (maxEdgeLength <= 102.5) {
                    if (avgErr <= 11.141) {
                      return 0;
                    } else {  // if avgErr > 11.141
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 102.5
                    if (shiftAvg <= 2.9083) {
                      return 0;
                    } else {  // if shiftAvg > 2.9083
                      return 0;
                    }
                  }
                } else {  // if area > 9829.5
                  if (area <= 9938) {
                    if (maxEdgeLength <= 119.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 119.5
                      return 0;
                    }
                  } else {  // if area > 9938
                    if (shiftNcc <= 0.89825) {
                      return 0;
                    } else {  // if shiftNcc > 0.89825
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 10044
          if (area <= 12708) {
            if (HWRatio <= 1.2287) {
              if (avgErr <= 6.021) {
                if (HWRatio <= 1.2102) {
                  if (maxEdgeLength <= 103.5) {
                    return 3;
                  } else {  // if maxEdgeLength > 103.5
                    if (avgErr <= 0.94435) {
                      return 0;
                    } else {  // if avgErr > 0.94435
                      return 1;
                    }
                  }
                } else {  // if HWRatio > 1.2102
                  return 3;
                }
              } else {  // if avgErr > 6.021
                if (shiftAvg <= 0.46893) {
                  if (HWRatio <= 1.1391) {
                    return 4;
                  } else {  // if HWRatio > 1.1391
                    if (shiftAvg <= 0.013563) {
                      return 1;
                    } else {  // if shiftAvg > 0.013563
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.46893
                  if (area <= 12256) {
                    if (HWRatio <= 1.2094) {
                      return 1;
                    } else {  // if HWRatio > 1.2094
                      return 3;
                    }
                  } else {  // if area > 12256
                    if (shiftStd <= 2.3307) {
                      return 2;
                    } else {  // if shiftStd > 2.3307
                      return 3;
                    }
                  }
                }
              }
            } else {  // if HWRatio > 1.2287
              if (area <= 12415) {
                if (area <= 10307) {
                  if (shiftNcc <= 0.45409) {
                    if (shiftStd <= 0.56148) {
                      return 3;
                    } else {  // if shiftStd > 0.56148
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.45409
                    if (area <= 10296) {
                      return 1;
                    } else {  // if area > 10296
                      return 0;
                    }
                  }
                } else {  // if area > 10307
                  if (shiftAvg <= 0.097728) {
                    if (shiftStd <= 0.53872) {
                      return 1;
                    } else {  // if shiftStd > 0.53872
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.097728
                    if (shiftNcc <= 0.0073178) {
                      return 1;
                    } else {  // if shiftNcc > 0.0073178
                      return 1;
                    }
                  }
                }
              } else {  // if area > 12415
                if (area <= 12592) {
                  if (HWRatio <= 1.7905) {
                    if (HWRatio <= 1.4974) {
                      return 1;
                    } else {  // if HWRatio > 1.4974
                      return 2;
                    }
                  } else {  // if HWRatio > 1.7905
                    if (maxEdgeLength <= 167.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 167.5
                      return 2;
                    }
                  }
                } else {  // if area > 12592
                  if (shiftNcc <= 0.27854) {
                    if (area <= 12648) {
                      return 3;
                    } else {  // if area > 12648
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.27854
                    if (maxEdgeLength <= 152.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 152.5
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 12708
            if (area <= 16270) {
              if (area <= 15756) {
                if (HWRatio <= 1.1435) {
                  if (avgErr <= 6.5252) {
                    if (HWRatio <= 1.0343) {
                      return 4;
                    } else {  // if HWRatio > 1.0343
                      return 2;
                    }
                  } else {  // if avgErr > 6.5252
                    if (area <= 13966) {
                      return 4;
                    } else {  // if area > 13966
                      return 2;
                    }
                  }
                } else {  // if HWRatio > 1.1435
                  if (area <= 12942) {
                    if (shiftAvg <= 0.2834) {
                      return 3;
                    } else {  // if shiftAvg > 0.2834
                      return 2;
                    }
                  } else {  // if area > 12942
                    if (shiftNcc <= 0.097338) {
                      return 2;
                    } else {  // if shiftNcc > 0.097338
                      return 2;
                    }
                  }
                }
              } else {  // if area > 15756
                if (avgErr <= 12.747) {
                  if (HWRatio <= 1.3991) {
                    if (shiftStd <= 0.26643) {
                      return 2;
                    } else {  // if shiftStd > 0.26643
                      return 3;
                    }
                  } else {  // if HWRatio > 1.3991
                    if (area <= 15962) {
                      return 2;
                    } else {  // if area > 15962
                      return 2;
                    }
                  }
                } else {  // if avgErr > 12.747
                  if (shiftAvg <= 0.22369) {
                    return 2;
                  } else {  // if shiftAvg > 0.22369
                    if (shiftStd <= 2.1925) {
                      return 3;
                    } else {  // if shiftStd > 2.1925
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 16270
              if (maxEdgeLength <= 194.5) {
                if (area <= 16419) {
                  if (area <= 16408) {
                    if (maxEdgeLength <= 143) {
                      return 2;
                    } else {  // if maxEdgeLength > 143
                      return 3;
                    }
                  } else {  // if area > 16408
                    return 2;
                  }
                } else {  // if area > 16419
                  if (shiftAvg <= 36.588) {
                    if (HWRatio <= 1.0536) {
                      return 3;
                    } else {  // if HWRatio > 1.0536
                      return 3;
                    }
                  } else {  // if shiftAvg > 36.588
                    return 4;
                  }
                }
              } else {  // if maxEdgeLength > 194.5
                if (area <= 38940) {
                  if (avgErr <= 13.178) {
                    if (area <= 34427) {
                      return 0;
                    } else {  // if area > 34427
                      return 3;
                    }
                  } else {  // if avgErr > 13.178
                    if (shiftAvg <= 47.514) {
                      return 0;
                    } else {  // if shiftAvg > 47.514
                      return 0;
                    }
                  }
                } else {  // if area > 38940
                  if (area <= 50358) {
                    if (area <= 44121) {
                      return 1;
                    } else {  // if area > 44121
                      return 1;
                    }
                  } else {  // if area > 50358
                    if (area <= 65259) {
                      return 2;
                    } else {  // if area > 65259
                      return 3;
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
