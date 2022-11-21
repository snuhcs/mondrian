#include "strm/tree/MTA.hpp"

namespace rm {

int MTA(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr, float confidence) {
  if (confidence <= 0.78267) {
    if (confidence <= 0.49327) {
      if (confidence <= 0.28656) {
        if (maxEdgeLength <= 28.367) {
          if (area <= 156.88) {
            if (maxEdgeLength <= 16.521) {
              if (shiftAvg <= 7.3218) {
                if (area <= 110.07) {
                  if (confidence <= 0.28553) {
                    if (avgErr <= 4.2572) {
                      return 4;
                    } else {  // if avgErr > 4.2572
                      return 4;
                    }
                  } else {  // if confidence > 0.28553
                    if (maxEdgeLength <= 14.844) {
                      return 4;
                    } else {  // if maxEdgeLength > 14.844
                      return 3;
                    }
                  }
                } else {  // if area > 110.07
                  if (area <= 110.1) {
                    return 3;
                  } else {  // if area > 110.1
                    if (shiftAvg <= 0.48596) {
                      return 4;
                    } else {  // if shiftAvg > 0.48596
                      return 4;
                    }
                  }
                }
              } else {  // if shiftAvg > 7.3218
                if (xyRatio <= 0.67337) {
                  return 4;
                } else {  // if xyRatio > 0.67337
                  if (maxEdgeLength <= 11.968) {
                    if (xyRatio <= 0.97369) {
                      return 4;
                    } else {  // if xyRatio > 0.97369
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 11.968
                    return 3;
                  }
                }
              }
            } else {  // if maxEdgeLength > 16.521
              if (avgErr <= 0.87063) {
                if (area <= 121.63) {
                  if (area <= 111.11) {
                    return 4;
                  } else {  // if area > 111.11
                    if (xyRatio <= 0.41966) {
                      return 3;
                    } else {  // if xyRatio > 0.41966
                      return 4;
                    }
                  }
                } else {  // if area > 121.63
                  return 4;
                }
              } else {  // if avgErr > 0.87063
                if (area <= 147.43) {
                  if (avgErr <= 1.1287) {
                    if (area <= 119.54) {
                      return 3;
                    } else {  // if area > 119.54
                      return 4;
                    }
                  } else {  // if avgErr > 1.1287
                    if (confidence <= 0.28258) {
                      return 4;
                    } else {  // if confidence > 0.28258
                      return 4;
                    }
                  }
                } else {  // if area > 147.43
                  if (area <= 147.44) {
                    return 3;
                  } else {  // if area > 147.44
                    if (shiftAvg <= 3.625) {
                      return 4;
                    } else {  // if shiftAvg > 3.625
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 156.88
            if (avgErr <= 0.52481) {
              if (maxEdgeLength <= 23.188) {
                if (area <= 253.97) {
                  if (confidence <= 0.1955) {
                    if (maxEdgeLength <= 14.235) {
                      return 3;
                    } else {  // if maxEdgeLength > 14.235
                      return 4;
                    }
                  } else {  // if confidence > 0.1955
                    if (maxEdgeLength <= 14.405) {
                      return 3;
                    } else {  // if maxEdgeLength > 14.405
                      return 4;
                    }
                  }
                } else {  // if area > 253.97
                  if (xyRatio <= 0.75923) {
                    if (maxEdgeLength <= 20.846) {
                      return 3;
                    } else {  // if maxEdgeLength > 20.846
                      return 3;
                    }
                  } else {  // if xyRatio > 0.75923
                    return 4;
                  }
                }
              } else {  // if maxEdgeLength > 23.188
                if (maxEdgeLength <= 25.985) {
                  if (confidence <= 0.24902) {
                    if (avgErr <= 0.1324) {
                      return 4;
                    } else {  // if avgErr > 0.1324
                      return 4;
                    }
                  } else {  // if confidence > 0.24902
                    if (confidence <= 0.27469) {
                      return 4;
                    } else {  // if confidence > 0.27469
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 25.985
                  if (area <= 348.15) {
                    if (shiftNcc <= -0.1794) {
                      return 4;
                    } else {  // if shiftNcc > -0.1794
                      return 4;
                    }
                  } else {  // if area > 348.15
                    if (xyRatio <= 0.62007) {
                      return 2;
                    } else {  // if xyRatio > 0.62007
                      return 4;
                    }
                  }
                }
              }
            } else {  // if avgErr > 0.52481
              if (confidence <= 0.17299) {
                if (maxEdgeLength <= 22.745) {
                  if (avgErr <= 1.6363) {
                    if (area <= 262.76) {
                      return 4;
                    } else {  // if area > 262.76
                      return 4;
                    }
                  } else {  // if avgErr > 1.6363
                    if (confidence <= 0.13441) {
                      return 4;
                    } else {  // if confidence > 0.13441
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 22.745
                  if (xyRatio <= 0.31824) {
                    if (area <= 168.46) {
                      return 4;
                    } else {  // if area > 168.46
                      return 4;
                    }
                  } else {  // if xyRatio > 0.31824
                    if (shiftAvg <= 0.022657) {
                      return 4;
                    } else {  // if shiftAvg > 0.022657
                      return 4;
                    }
                  }
                }
              } else {  // if confidence > 0.17299
                if (maxEdgeLength <= 20.661) {
                  if (avgErr <= 1.959) {
                    if (maxEdgeLength <= 14.467) {
                      return 3;
                    } else {  // if maxEdgeLength > 14.467
                      return 4;
                    }
                  } else {  // if avgErr > 1.959
                    if (maxEdgeLength <= 19.603) {
                      return 4;
                    } else {  // if maxEdgeLength > 19.603
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 20.661
                  if (avgErr <= 7.9226) {
                    if (shiftAvg <= 0.020974) {
                      return 4;
                    } else {  // if shiftAvg > 0.020974
                      return 4;
                    }
                  } else {  // if avgErr > 7.9226
                    if (confidence <= 0.26479) {
                      return 4;
                    } else {  // if confidence > 0.26479
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 28.367
          if (shiftAvg <= 0.31792) {
            if (xyRatio <= 0.41678) {
              if (avgErr <= 2.8583) {
                if (shiftStd <= 0.049037) {
                  if (confidence <= 0.2401) {
                    if (area <= 416.15) {
                      return 4;
                    } else {  // if area > 416.15
                      return 4;
                    }
                  } else {  // if confidence > 0.2401
                    if (confidence <= 0.24057) {
                      return 2;
                    } else {  // if confidence > 0.24057
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.049037
                  if (confidence <= 0.20124) {
                    if (maxEdgeLength <= 41.219) {
                      return 4;
                    } else {  // if maxEdgeLength > 41.219
                      return 2;
                    }
                  } else {  // if confidence > 0.20124
                    return 3;
                  }
                }
              } else {  // if avgErr > 2.8583
                if (area <= 394.5) {
                  if (confidence <= 0.23026) {
                    if (shiftAvg <= 0.020155) {
                      return 4;
                    } else {  // if shiftAvg > 0.020155
                      return 4;
                    }
                  } else {  // if confidence > 0.23026
                    if (area <= 313.46) {
                      return 4;
                    } else {  // if area > 313.46
                      return 4;
                    }
                  }
                } else {  // if area > 394.5
                  if (maxEdgeLength <= 58.662) {
                    if (confidence <= 0.21915) {
                      return 4;
                    } else {  // if confidence > 0.21915
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 58.662
                    if (shiftNcc <= 0.96341) {
                      return 4;
                    } else {  // if shiftNcc > 0.96341
                      return 3;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.41678
              if (shiftStd <= 0.00050328) {
                if (area <= 614.05) {
                  if (xyRatio <= 0.68595) {
                    if (xyRatio <= 0.46814) {
                      return 4;
                    } else {  // if xyRatio > 0.46814
                      return 2;
                    }
                  } else {  // if xyRatio > 0.68595
                    if (area <= 575.62) {
                      return 4;
                    } else {  // if area > 575.62
                      return 2;
                    }
                  }
                } else {  // if area > 614.05
                  if (maxEdgeLength <= 34.257) {
                    if (avgErr <= 1.0071) {
                      return 4;
                    } else {  // if avgErr > 1.0071
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 34.257
                    if (confidence <= 0.18398) {
                      return 4;
                    } else {  // if confidence > 0.18398
                      return 4;
                    }
                  }
                }
              } else {  // if shiftStd > 0.00050328
                if (xyRatio <= 0.70804) {
                  if (area <= 568.97) {
                    if (shiftStd <= 0.00069394) {
                      return 2;
                    } else {  // if shiftStd > 0.00069394
                      return 4;
                    }
                  } else {  // if area > 568.97
                    if (confidence <= 0.21226) {
                      return 4;
                    } else {  // if confidence > 0.21226
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.70804
                  if (avgErr <= 5.039) {
                    if (avgErr <= 4.951) {
                      return 4;
                    } else {  // if avgErr > 4.951
                      return 3;
                    }
                  } else {  // if avgErr > 5.039
                    if (confidence <= 0.16643) {
                      return 4;
                    } else {  // if confidence > 0.16643
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.31792
            if (area <= 1339.1) {
              if (xyRatio <= 0.77518) {
                if (avgErr <= 10.681) {
                  if (confidence <= 0.18706) {
                    if (maxEdgeLength <= 32.885) {
                      return 4;
                    } else {  // if maxEdgeLength > 32.885
                      return 4;
                    }
                  } else {  // if confidence > 0.18706
                    if (xyRatio <= 0.691) {
                      return 4;
                    } else {  // if xyRatio > 0.691
                      return 4;
                    }
                  }
                } else {  // if avgErr > 10.681
                  if (confidence <= 0.25754) {
                    if (area <= 968.35) {
                      return 4;
                    } else {  // if area > 968.35
                      return 4;
                    }
                  } else {  // if confidence > 0.25754
                    if (maxEdgeLength <= 29.885) {
                      return 4;
                    } else {  // if maxEdgeLength > 29.885
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 0.77518
                if (area <= 1116.2) {
                  if (shiftNcc <= -0.016818) {
                    return 3;
                  } else {  // if shiftNcc > -0.016818
                    if (shiftAvg <= 0.35408) {
                      return 4;
                    } else {  // if shiftAvg > 0.35408
                      return 4;
                    }
                  }
                } else {  // if area > 1116.2
                  if (avgErr <= 6.6786) {
                    return 3;
                  } else {  // if avgErr > 6.6786
                    if (shiftNcc <= 0.95629) {
                      return 4;
                    } else {  // if shiftNcc > 0.95629
                      return 3;
                    }
                  }
                }
              }
            } else {  // if area > 1339.1
              if (maxEdgeLength <= 144.65) {
                if (area <= 3399.8) {
                  if (confidence <= 0.1454) {
                    if (area <= 1374.3) {
                      return 2;
                    } else {  // if area > 1374.3
                      return 4;
                    }
                  } else {  // if confidence > 0.1454
                    if (avgErr <= 9.7898) {
                      return 4;
                    } else {  // if avgErr > 9.7898
                      return 3;
                    }
                  }
                } else {  // if area > 3399.8
                  if (xyRatio <= 1.3992) {
                    if (shiftStd <= 1.2176) {
                      return 4;
                    } else {  // if shiftStd > 1.2176
                      return 1;
                    }
                  } else {  // if xyRatio > 1.3992
                    return 0;
                  }
                }
              } else {  // if maxEdgeLength > 144.65
                if (avgErr <= 8.9805) {
                  if (area <= 11231) {
                    if (xyRatio <= 0.30503) {
                      return 4;
                    } else {  // if xyRatio > 0.30503
                      return 3;
                    }
                  } else {  // if area > 11231
                    if (confidence <= 0.24897) {
                      return 4;
                    } else {  // if confidence > 0.24897
                      return 2;
                    }
                  }
                } else {  // if avgErr > 8.9805
                  if (maxEdgeLength <= 165.6) {
                    if (shiftAvg <= 5.1802) {
                      return 1;
                    } else {  // if shiftAvg > 5.1802
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 165.6
                    return 4;
                  }
                }
              }
            }
          }
        }
      } else {  // if confidence > 0.28656
        if (maxEdgeLength <= 28.588) {
          if (avgErr <= 0.69962) {
            if (maxEdgeLength <= 24.512) {
              if (area <= 166.67) {
                if (area <= 117.61) {
                  return 4;
                } else {  // if area > 117.61
                  if (shiftAvg <= 5.553e-06) {
                    return 3;
                  } else {  // if shiftAvg > 5.553e-06
                    if (area <= 119.27) {
                      return 3;
                    } else {  // if area > 119.27
                      return 4;
                    }
                  }
                }
              } else {  // if area > 166.67
                if (xyRatio <= 0.4016) {
                  if (area <= 221.29) {
                    if (maxEdgeLength <= 23.187) {
                      return 3;
                    } else {  // if maxEdgeLength > 23.187
                      return 4;
                    }
                  } else {  // if area > 221.29
                    if (avgErr <= 0.50274) {
                      return 3;
                    } else {  // if avgErr > 0.50274
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.4016
                  if (area <= 279.84) {
                    if (maxEdgeLength <= 20.956) {
                      return 3;
                    } else {  // if maxEdgeLength > 20.956
                      return 3;
                    }
                  } else {  // if area > 279.84
                    if (maxEdgeLength <= 21.717) {
                      return 2;
                    } else {  // if maxEdgeLength > 21.717
                      return 4;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 24.512
              if (avgErr <= 0.66127) {
                if (maxEdgeLength <= 28) {
                  if (avgErr <= 0.41491) {
                    if (confidence <= 0.47311) {
                      return 4;
                    } else {  // if confidence > 0.47311
                      return 3;
                    }
                  } else {  // if avgErr > 0.41491
                    if (area <= 255.12) {
                      return 4;
                    } else {  // if area > 255.12
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 28
                  if (area <= 532.47) {
                    if (xyRatio <= 0.41012) {
                      return 4;
                    } else {  // if xyRatio > 0.41012
                      return 2;
                    }
                  } else {  // if area > 532.47
                    if (shiftNcc <= -0.15253) {
                      return 2;
                    } else {  // if shiftNcc > -0.15253
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 0.66127
                if (area <= 272.55) {
                  return 4;
                } else {  // if area > 272.55
                  if (maxEdgeLength <= 28.538) {
                    return 3;
                  } else {  // if maxEdgeLength > 28.538
                    return 2;
                  }
                }
              }
            }
          } else {  // if avgErr > 0.69962
            if (maxEdgeLength <= 20.184) {
              if (confidence <= 0.43859) {
                if (area <= 139.01) {
                  if (shiftAvg <= 2.2786) {
                    if (area <= 101.36) {
                      return 4;
                    } else {  // if area > 101.36
                      return 4;
                    }
                  } else {  // if shiftAvg > 2.2786
                    return 3;
                  }
                } else {  // if area > 139.01
                  if (xyRatio <= 0.91569) {
                    if (xyRatio <= 0.61963) {
                      return 4;
                    } else {  // if xyRatio > 0.61963
                      return 4;
                    }
                  } else {  // if xyRatio > 0.91569
                    if (shiftAvg <= 13.72) {
                      return 4;
                    } else {  // if shiftAvg > 13.72
                      return 1;
                    }
                  }
                }
              } else {  // if confidence > 0.43859
                if (xyRatio <= 0.62615) {
                  if (maxEdgeLength <= 19.978) {
                    if (confidence <= 0.47138) {
                      return 4;
                    } else {  // if confidence > 0.47138
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 19.978
                    if (shiftStd <= 0.0013297) {
                      return 4;
                    } else {  // if shiftStd > 0.0013297
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.62615
                  if (shiftAvg <= 0.24505) {
                    if (shiftStd <= 0.0012689) {
                      return 4;
                    } else {  // if shiftStd > 0.0012689
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.24505
                    if (avgErr <= 11.545) {
                      return 4;
                    } else {  // if avgErr > 11.545
                      return 4;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 20.184
              if (xyRatio <= 0.6981) {
                if (xyRatio <= 0.32883) {
                  if (shiftAvg <= 0.12843) {
                    if (xyRatio <= 0.3152) {
                      return 4;
                    } else {  // if xyRatio > 0.3152
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.12843
                    if (avgErr <= 8.6499) {
                      return 4;
                    } else {  // if avgErr > 8.6499
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.32883
                  if (confidence <= 0.43559) {
                    if (xyRatio <= 0.45863) {
                      return 4;
                    } else {  // if xyRatio > 0.45863
                      return 4;
                    }
                  } else {  // if confidence > 0.43559
                    if (shiftAvg <= 0.34972) {
                      return 4;
                    } else {  // if shiftAvg > 0.34972
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 0.6981
                if (area <= 643.64) {
                  if (shiftStd <= 0.002405) {
                    if (maxEdgeLength <= 28.529) {
                      return 4;
                    } else {  // if maxEdgeLength > 28.529
                      return 2;
                    }
                  } else {  // if shiftStd > 0.002405
                    if (shiftAvg <= 0.089819) {
                      return 4;
                    } else {  // if shiftAvg > 0.089819
                      return 4;
                    }
                  }
                } else {  // if area > 643.64
                  if (xyRatio <= 1.1759) {
                    if (area <= 653.3) {
                      return 3;
                    } else {  // if area > 653.3
                      return 4;
                    }
                  } else {  // if xyRatio > 1.1759
                    if (shiftStd <= 0.00061628) {
                      return 3;
                    } else {  // if shiftStd > 0.00061628
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 28.588
          if (shiftStd <= 0.00062071) {
            if (maxEdgeLength <= 30.766) {
              if (xyRatio <= 0.43832) {
                if (shiftAvg <= 0.0048888) {
                  return 4;
                } else {  // if shiftAvg > 0.0048888
                  if (area <= 297.81) {
                    if (maxEdgeLength <= 29.393) {
                      return 4;
                    } else {  // if maxEdgeLength > 29.393
                      return 3;
                    }
                  } else {  // if area > 297.81
                    return 3;
                  }
                }
              } else {  // if xyRatio > 0.43832
                if (xyRatio <= 0.68771) {
                  if (xyRatio <= 0.50964) {
                    if (maxEdgeLength <= 29.623) {
                      return 2;
                    } else {  // if maxEdgeLength > 29.623
                      return 4;
                    }
                  } else {  // if xyRatio > 0.50964
                    if (maxEdgeLength <= 28.677) {
                      return 2;
                    } else {  // if maxEdgeLength > 28.677
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.68771
                  if (shiftStd <= 0.00045138) {
                    return 4;
                  } else {  // if shiftStd > 0.00045138
                    return 3;
                  }
                }
              }
            } else {  // if maxEdgeLength > 30.766
              if (shiftAvg <= 0.01659) {
                if (maxEdgeLength <= 34.211) {
                  if (shiftStd <= 2.7164e-06) {
                    if (confidence <= 0.3702) {
                      return 4;
                    } else {  // if confidence > 0.3702
                      return 3;
                    }
                  } else {  // if shiftStd > 2.7164e-06
                    if (avgErr <= 0.5447) {
                      return 2;
                    } else {  // if avgErr > 0.5447
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 34.211
                  if (area <= 1210.4) {
                    if (xyRatio <= 0.33533) {
                      return 3;
                    } else {  // if xyRatio > 0.33533
                      return 4;
                    }
                  } else {  // if area > 1210.4
                    if (confidence <= 0.33903) {
                      return 2;
                    } else {  // if confidence > 0.33903
                      return 4;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.01659
                if (area <= 280.3) {
                  return 4;
                } else {  // if area > 280.3
                  return 3;
                }
              }
            }
          } else {  // if shiftStd > 0.00062071
            if (shiftAvg <= 0.47472) {
              if (confidence <= 0.40406) {
                if (area <= 313.79) {
                  if (shiftAvg <= 0.078776) {
                    if (area <= 303.76) {
                      return 4;
                    } else {  // if area > 303.76
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.078776
                    if (maxEdgeLength <= 33.726) {
                      return 4;
                    } else {  // if maxEdgeLength > 33.726
                      return 3;
                    }
                  }
                } else {  // if area > 313.79
                  if (xyRatio <= 1.0744) {
                    if (area <= 1041) {
                      return 4;
                    } else {  // if area > 1041
                      return 4;
                    }
                  } else {  // if xyRatio > 1.0744
                    if (confidence <= 0.33643) {
                      return 4;
                    } else {  // if confidence > 0.33643
                      return 4;
                    }
                  }
                }
              } else {  // if confidence > 0.40406
                if (maxEdgeLength <= 32.987) {
                  if (avgErr <= 2.375) {
                    if (shiftAvg <= 4.8094e-05) {
                      return 3;
                    } else {  // if shiftAvg > 4.8094e-05
                      return 2;
                    }
                  } else {  // if avgErr > 2.375
                    if (area <= 300.83) {
                      return 4;
                    } else {  // if area > 300.83
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 32.987
                  if (maxEdgeLength <= 37.264) {
                    if (avgErr <= 7.6135) {
                      return 3;
                    } else {  // if avgErr > 7.6135
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 37.264
                    if (shiftStd <= 0.066857) {
                      return 3;
                    } else {  // if shiftStd > 0.066857
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.47472
              if (avgErr <= 9.8054) {
                if (avgErr <= 6.5943) {
                  if (xyRatio <= 0.26725) {
                    if (shiftNcc <= 0.93687) {
                      return 4;
                    } else {  // if shiftNcc > 0.93687
                      return 3;
                    }
                  } else {  // if xyRatio > 0.26725
                    if (confidence <= 0.45384) {
                      return 3;
                    } else {  // if confidence > 0.45384
                      return 3;
                    }
                  }
                } else {  // if avgErr > 6.5943
                  if (maxEdgeLength <= 51.945) {
                    if (confidence <= 0.44193) {
                      return 3;
                    } else {  // if confidence > 0.44193
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 51.945
                    if (confidence <= 0.48465) {
                      return 4;
                    } else {  // if confidence > 0.48465
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 9.8054
                if (maxEdgeLength <= 32.027) {
                  if (area <= 620.77) {
                    if (shiftAvg <= 0.8007) {
                      return 4;
                    } else {  // if shiftAvg > 0.8007
                      return 4;
                    }
                  } else {  // if area > 620.77
                    if (shiftStd <= 0.026944) {
                      return 2;
                    } else {  // if shiftStd > 0.026944
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 32.027
                  if (shiftAvg <= 0.49434) {
                    if (confidence <= 0.4575) {
                      return 3;
                    } else {  // if confidence > 0.4575
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.49434
                    if (shiftNcc <= 0.95443) {
                      return 4;
                    } else {  // if shiftNcc > 0.95443
                      return 4;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if confidence > 0.49327
      if (maxEdgeLength <= 34.605) {
        if (confidence <= 0.64203) {
          if (avgErr <= 0.5828) {
            if (area <= 406.5) {
              if (xyRatio <= 0.41824) {
                if (maxEdgeLength <= 22.043) {
                  if (area <= 200.66) {
                    if (shiftAvg <= 3.65e-06) {
                      return 3;
                    } else {  // if shiftAvg > 3.65e-06
                      return 3;
                    }
                  } else {  // if area > 200.66
                    if (maxEdgeLength <= 21.981) {
                      return 4;
                    } else {  // if maxEdgeLength > 21.981
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 22.043
                  if (avgErr <= 0.11372) {
                    if (area <= 198.62) {
                      return 3;
                    } else {  // if area > 198.62
                      return 3;
                    }
                  } else {  // if avgErr > 0.11372
                    if (maxEdgeLength <= 23.058) {
                      return 3;
                    } else {  // if maxEdgeLength > 23.058
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 0.41824
                if (maxEdgeLength <= 21.179) {
                  if (confidence <= 0.60702) {
                    if (xyRatio <= 0.52672) {
                      return 4;
                    } else {  // if xyRatio > 0.52672
                      return 3;
                    }
                  } else {  // if confidence > 0.60702
                    if (xyRatio <= 0.49115) {
                      return 4;
                    } else {  // if xyRatio > 0.49115
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 21.179
                  if (xyRatio <= 0.61038) {
                    if (area <= 209.75) {
                      return 3;
                    } else {  // if area > 209.75
                      return 3;
                    }
                  } else {  // if xyRatio > 0.61038
                    return 4;
                  }
                }
              }
            } else {  // if area > 406.5
              if (maxEdgeLength <= 28.655) {
                return 4;
              } else {  // if maxEdgeLength > 28.655
                if (shiftStd <= 8.3297e-08) {
                  if (area <= 506.81) {
                    return 4;
                  } else {  // if area > 506.81
                    return 3;
                  }
                } else {  // if shiftStd > 8.3297e-08
                  if (maxEdgeLength <= 30.15) {
                    if (area <= 411.99) {
                      return 2;
                    } else {  // if area > 411.99
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 30.15
                    if (confidence <= 0.58759) {
                      return 2;
                    } else {  // if confidence > 0.58759
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if avgErr > 0.5828
            if (maxEdgeLength <= 27.928) {
              if (confidence <= 0.55074) {
                if (area <= 117.34) {
                  if (confidence <= 0.54787) {
                    if (xyRatio <= 0.87144) {
                      return 4;
                    } else {  // if xyRatio > 0.87144
                      return 3;
                    }
                  } else {  // if confidence > 0.54787
                    return 3;
                  }
                } else {  // if area > 117.34
                  if (avgErr <= 8.3752) {
                    if (avgErr <= 3.1664) {
                      return 4;
                    } else {  // if avgErr > 3.1664
                      return 4;
                    }
                  } else {  // if avgErr > 8.3752
                    if (xyRatio <= 0.39484) {
                      return 4;
                    } else {  // if xyRatio > 0.39484
                      return 4;
                    }
                  }
                }
              } else {  // if confidence > 0.55074
                if (xyRatio <= 0.54944) {
                  if (area <= 220.39) {
                    if (area <= 124.08) {
                      return 4;
                    } else {  // if area > 124.08
                      return 4;
                    }
                  } else {  // if area > 220.39
                    if (shiftAvg <= 0.17186) {
                      return 4;
                    } else {  // if shiftAvg > 0.17186
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.54944
                  if (shiftAvg <= 0.27303) {
                    if (avgErr <= 8.1087) {
                      return 3;
                    } else {  // if avgErr > 8.1087
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.27303
                    if (xyRatio <= 0.6936) {
                      return 3;
                    } else {  // if xyRatio > 0.6936
                      return 4;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 27.928
              if (avgErr <= 1.9525) {
                if (maxEdgeLength <= 29.438) {
                  if (xyRatio <= 0.63698) {
                    if (confidence <= 0.58784) {
                      return 3;
                    } else {  // if confidence > 0.58784
                      return 3;
                    }
                  } else {  // if xyRatio > 0.63698
                    if (shiftNcc <= 0.60207) {
                      return 2;
                    } else {  // if shiftNcc > 0.60207
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 29.438
                  if (shiftAvg <= 0.0076062) {
                    if (xyRatio <= 0.65791) {
                      return 2;
                    } else {  // if xyRatio > 0.65791
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.0076062
                    if (shiftNcc <= 0.9495) {
                      return 4;
                    } else {  // if shiftNcc > 0.9495
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 1.9525
                if (shiftAvg <= 0.36188) {
                  if (avgErr <= 8.3166) {
                    if (shiftAvg <= 0.049603) {
                      return 3;
                    } else {  // if shiftAvg > 0.049603
                      return 3;
                    }
                  } else {  // if avgErr > 8.3166
                    if (area <= 366.64) {
                      return 4;
                    } else {  // if area > 366.64
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.36188
                  if (avgErr <= 9.988) {
                    if (avgErr <= 5.5774) {
                      return 3;
                    } else {  // if avgErr > 5.5774
                      return 3;
                    }
                  } else {  // if avgErr > 9.988
                    if (xyRatio <= 0.56937) {
                      return 3;
                    } else {  // if xyRatio > 0.56937
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.64203
          if (shiftAvg <= 0.15574) {
            if (confidence <= 0.73192) {
              if (area <= 365.9) {
                if (avgErr <= 8.114) {
                  if (xyRatio <= 0.59139) {
                    if (area <= 121.09) {
                      return 4;
                    } else {  // if area > 121.09
                      return 3;
                    }
                  } else {  // if xyRatio > 0.59139
                    if (shiftStd <= 0.046234) {
                      return 3;
                    } else {  // if shiftStd > 0.046234
                      return 4;
                    }
                  }
                } else {  // if avgErr > 8.114
                  if (area <= 248.88) {
                    if (shiftAvg <= 0.07026) {
                      return 4;
                    } else {  // if shiftAvg > 0.07026
                      return 3;
                    }
                  } else {  // if area > 248.88
                    if (xyRatio <= 0.51429) {
                      return 3;
                    } else {  // if xyRatio > 0.51429
                      return 3;
                    }
                  }
                }
              } else {  // if area > 365.9
                if (shiftStd <= 0.092837) {
                  if (xyRatio <= 0.61678) {
                    if (confidence <= 0.68504) {
                      return 3;
                    } else {  // if confidence > 0.68504
                      return 3;
                    }
                  } else {  // if xyRatio > 0.61678
                    if (shiftStd <= 0.00038249) {
                      return 2;
                    } else {  // if shiftStd > 0.00038249
                      return 3;
                    }
                  }
                } else {  // if shiftStd > 0.092837
                  if (shiftNcc <= -0.043956) {
                    if (maxEdgeLength <= 31.09) {
                      return 4;
                    } else {  // if maxEdgeLength > 31.09
                      return 3;
                    }
                  } else {  // if shiftNcc > -0.043956
                    if (shiftAvg <= 0.1093) {
                      return 3;
                    } else {  // if shiftAvg > 0.1093
                      return 3;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.73192
              if (xyRatio <= 0.46093) {
                if (xyRatio <= 0.35523) {
                  if (area <= 212.36) {
                    if (xyRatio <= 0.34248) {
                      return 4;
                    } else {  // if xyRatio > 0.34248
                      return 3;
                    }
                  } else {  // if area > 212.36
                    if (avgErr <= 6.7603) {
                      return 2;
                    } else {  // if avgErr > 6.7603
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.35523
                  if (avgErr <= 8.758) {
                    if (shiftAvg <= 0.12722) {
                      return 3;
                    } else {  // if shiftAvg > 0.12722
                      return 3;
                    }
                  } else {  // if avgErr > 8.758
                    if (maxEdgeLength <= 32.261) {
                      return 3;
                    } else {  // if maxEdgeLength > 32.261
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.46093
                if (maxEdgeLength <= 25.887) {
                  if (maxEdgeLength <= 22.832) {
                    if (area <= 218.98) {
                      return 3;
                    } else {  // if area > 218.98
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 22.832
                    if (xyRatio <= 0.52693) {
                      return 2;
                    } else {  // if xyRatio > 0.52693
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 25.887
                  if (shiftAvg <= 0.03407) {
                    if (area <= 632.32) {
                      return 3;
                    } else {  // if area > 632.32
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.03407
                    if (area <= 635.99) {
                      return 3;
                    } else {  // if area > 635.99
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.15574
            if (area <= 225.51) {
              if (xyRatio <= 0.4173) {
                if (maxEdgeLength <= 24.544) {
                  if (shiftStd <= 0.02908) {
                    if (shiftStd <= 0.01123) {
                      return 4;
                    } else {  // if shiftStd > 0.01123
                      return 4;
                    }
                  } else {  // if shiftStd > 0.02908
                    if (maxEdgeLength <= 24.334) {
                      return 3;
                    } else {  // if maxEdgeLength > 24.334
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 24.544
                  if (xyRatio <= 0.28471) {
                    if (area <= 214.16) {
                      return 4;
                    } else {  // if area > 214.16
                      return 2;
                    }
                  } else {  // if xyRatio > 0.28471
                    if (confidence <= 0.65862) {
                      return 4;
                    } else {  // if confidence > 0.65862
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.4173
                if (confidence <= 0.66918) {
                  if (shiftNcc <= 0.99987) {
                    if (confidence <= 0.66865) {
                      return 3;
                    } else {  // if confidence > 0.66865
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.99987
                    return 4;
                  }
                } else {  // if confidence > 0.66918
                  if (shiftStd <= 0.058347) {
                    if (confidence <= 0.77708) {
                      return 3;
                    } else {  // if confidence > 0.77708
                      return 2;
                    }
                  } else {  // if shiftStd > 0.058347
                    if (maxEdgeLength <= 17.825) {
                      return 4;
                    } else {  // if maxEdgeLength > 17.825
                      return 3;
                    }
                  }
                }
              }
            } else {  // if area > 225.51
              if (confidence <= 0.73453) {
                if (shiftAvg <= 0.28376) {
                  if (area <= 383.38) {
                    if (avgErr <= 8.9355) {
                      return 3;
                    } else {  // if avgErr > 8.9355
                      return 3;
                    }
                  } else {  // if area > 383.38
                    if (avgErr <= 12.101) {
                      return 3;
                    } else {  // if avgErr > 12.101
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.28376
                  if (maxEdgeLength <= 24.35) {
                    if (shiftAvg <= 0.87391) {
                      return 3;
                    } else {  // if shiftAvg > 0.87391
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 24.35
                    if (maxEdgeLength <= 28.385) {
                      return 3;
                    } else {  // if maxEdgeLength > 28.385
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.73453
                if (xyRatio <= 0.52507) {
                  if (maxEdgeLength <= 28.5) {
                    if (avgErr <= 8.8364) {
                      return 2;
                    } else {  // if avgErr > 8.8364
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 28.5
                    if (avgErr <= 10.817) {
                      return 3;
                    } else {  // if avgErr > 10.817
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.52507
                  if (maxEdgeLength <= 25.828) {
                    if (avgErr <= 3.1406) {
                      return 4;
                    } else {  // if avgErr > 3.1406
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 25.828
                    if (shiftStd <= 0.0062761) {
                      return 3;
                    } else {  // if shiftStd > 0.0062761
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 34.605
        if (shiftAvg <= 0.74742) {
          if (confidence <= 0.672) {
            if (confidence <= 0.58234) {
              if (shiftStd <= 0.038774) {
                if (shiftStd <= 0.0010226) {
                  if (area <= 402.09) {
                    return 3;
                  } else {  // if area > 402.09
                    if (xyRatio <= 0.54078) {
                      return 4;
                    } else {  // if xyRatio > 0.54078
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.0010226
                  if (avgErr <= 9.4801) {
                    if (area <= 696.99) {
                      return 3;
                    } else {  // if area > 696.99
                      return 3;
                    }
                  } else {  // if avgErr > 9.4801
                    if (xyRatio <= 0.33368) {
                      return 4;
                    } else {  // if xyRatio > 0.33368
                      return 3;
                    }
                  }
                }
              } else {  // if shiftStd > 0.038774
                if (area <= 2025.8) {
                  if (shiftAvg <= 0.083144) {
                    if (maxEdgeLength <= 38.4) {
                      return 3;
                    } else {  // if maxEdgeLength > 38.4
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.083144
                    if (avgErr <= 6.1449) {
                      return 3;
                    } else {  // if avgErr > 6.1449
                      return 3;
                    }
                  }
                } else {  // if area > 2025.8
                  if (shiftNcc <= 0.43636) {
                    if (confidence <= 0.56204) {
                      return 4;
                    } else {  // if confidence > 0.56204
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.43636
                    if (shiftStd <= 0.26256) {
                      return 3;
                    } else {  // if shiftStd > 0.26256
                      return 2;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.58234
              if (maxEdgeLength <= 40.8) {
                if (avgErr <= 6.048) {
                  if (xyRatio <= 0.27934) {
                    if (shiftNcc <= 0.93513) {
                      return 4;
                    } else {  // if shiftNcc > 0.93513
                      return 3;
                    }
                  } else {  // if xyRatio > 0.27934
                    if (xyRatio <= 0.77636) {
                      return 3;
                    } else {  // if xyRatio > 0.77636
                      return 2;
                    }
                  }
                } else {  // if avgErr > 6.048
                  if (shiftAvg <= 0.11261) {
                    if (area <= 452.03) {
                      return 3;
                    } else {  // if area > 452.03
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.11261
                    if (avgErr <= 8.5015) {
                      return 3;
                    } else {  // if avgErr > 8.5015
                      return 3;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 40.8
                if (avgErr <= 5.2349) {
                  if (shiftNcc <= 0.26529) {
                    if (shiftNcc <= 0.16077) {
                      return 3;
                    } else {  // if shiftNcc > 0.16077
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.26529
                    if (area <= 906.19) {
                      return 3;
                    } else {  // if area > 906.19
                      return 3;
                    }
                  }
                } else {  // if avgErr > 5.2349
                  if (area <= 1126.1) {
                    if (xyRatio <= 0.30571) {
                      return 4;
                    } else {  // if xyRatio > 0.30571
                      return 3;
                    }
                  } else {  // if area > 1126.1
                    if (shiftAvg <= 0.29206) {
                      return 3;
                    } else {  // if shiftAvg > 0.29206
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.672
            if (maxEdgeLength <= 45.689) {
              if (avgErr <= 4.7891) {
                if (shiftAvg <= 0.52999) {
                  if (area <= 475.94) {
                    if (xyRatio <= 0.35932) {
                      return 2;
                    } else {  // if xyRatio > 0.35932
                      return 3;
                    }
                  } else {  // if area > 475.94
                    if (xyRatio <= 0.56933) {
                      return 3;
                    } else {  // if xyRatio > 0.56933
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.52999
                  if (maxEdgeLength <= 35.685) {
                    if (area <= 567.41) {
                      return 2;
                    } else {  // if area > 567.41
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 35.685
                    if (maxEdgeLength <= 37.184) {
                      return 2;
                    } else {  // if maxEdgeLength > 37.184
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 4.7891
                if (maxEdgeLength <= 37.924) {
                  if (avgErr <= 12.342) {
                    if (avgErr <= 8.2082) {
                      return 3;
                    } else {  // if avgErr > 8.2082
                      return 3;
                    }
                  } else {  // if avgErr > 12.342
                    if (shiftStd <= 0.042363) {
                      return 2;
                    } else {  // if shiftStd > 0.042363
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 37.924
                  if (xyRatio <= 0.4719) {
                    if (xyRatio <= 0.34078) {
                      return 3;
                    } else {  // if xyRatio > 0.34078
                      return 2;
                    }
                  } else {  // if xyRatio > 0.4719
                    if (avgErr <= 7.0556) {
                      return 3;
                    } else {  // if avgErr > 7.0556
                      return 3;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 45.689
              if (maxEdgeLength <= 48.819) {
                if (avgErr <= 5.3466) {
                  if (confidence <= 0.77849) {
                    if (xyRatio <= 0.5027) {
                      return 3;
                    } else {  // if xyRatio > 0.5027
                      return 2;
                    }
                  } else {  // if confidence > 0.77849
                    if (maxEdgeLength <= 46.276) {
                      return 3;
                    } else {  // if maxEdgeLength > 46.276
                      return 2;
                    }
                  }
                } else {  // if avgErr > 5.3466
                  if (shiftNcc <= 0.82958) {
                    if (maxEdgeLength <= 48.321) {
                      return 3;
                    } else {  // if maxEdgeLength > 48.321
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.82958
                    if (shiftAvg <= 0.29564) {
                      return 3;
                    } else {  // if shiftAvg > 0.29564
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 48.819
                if (maxEdgeLength <= 56.084) {
                  if (shiftNcc <= 0.74894) {
                    if (confidence <= 0.76194) {
                      return 3;
                    } else {  // if confidence > 0.76194
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.74894
                    if (xyRatio <= 0.35971) {
                      return 2;
                    } else {  // if xyRatio > 0.35971
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 56.084
                  if (area <= 1385.3) {
                    if (maxEdgeLength <= 56.641) {
                      return 1;
                    } else {  // if maxEdgeLength > 56.641
                      return 3;
                    }
                  } else {  // if area > 1385.3
                    if (confidence <= 0.73382) {
                      return 3;
                    } else {  // if confidence > 0.73382
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if shiftAvg > 0.74742
          if (area <= 2123.7) {
            if (avgErr <= 5.5894) {
              if (area <= 718.4) {
                if (shiftStd <= 0.011045) {
                  if (shiftAvg <= 1.0153) {
                    return 4;
                  } else {  // if shiftAvg > 1.0153
                    return 1;
                  }
                } else {  // if shiftStd > 0.011045
                  if (shiftStd <= 0.95351) {
                    if (area <= 470.08) {
                      return 2;
                    } else {  // if area > 470.08
                      return 3;
                    }
                  } else {  // if shiftStd > 0.95351
                    if (shiftNcc <= 0.80531) {
                      return 4;
                    } else {  // if shiftNcc > 0.80531
                      return 3;
                    }
                  }
                }
              } else {  // if area > 718.4
                if (area <= 1928.1) {
                  if (maxEdgeLength <= 82.336) {
                    if (confidence <= 0.66354) {
                      return 3;
                    } else {  // if confidence > 0.66354
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 82.336
                    return 4;
                  }
                } else {  // if area > 1928.1
                  if (shiftNcc <= 0.71867) {
                    return 3;
                  } else {  // if shiftNcc > 0.71867
                    if (shiftStd <= 0.11076) {
                      return 1;
                    } else {  // if shiftStd > 0.11076
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 5.5894
              if (avgErr <= 11.171) {
                if (xyRatio <= 0.33719) {
                  if (maxEdgeLength <= 64.045) {
                    if (maxEdgeLength <= 45.134) {
                      return 2;
                    } else {  // if maxEdgeLength > 45.134
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 64.045
                    if (confidence <= 0.7524) {
                      return 3;
                    } else {  // if confidence > 0.7524
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.33719
                  if (shiftAvg <= 1.1155) {
                    if (shiftNcc <= 0.98514) {
                      return 2;
                    } else {  // if shiftNcc > 0.98514
                      return 3;
                    }
                  } else {  // if shiftAvg > 1.1155
                    if (xyRatio <= 0.34672) {
                      return 1;
                    } else {  // if xyRatio > 0.34672
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 11.171
                if (confidence <= 0.68359) {
                  if (area <= 658.19) {
                    if (shiftAvg <= 1.3908) {
                      return 3;
                    } else {  // if shiftAvg > 1.3908
                      return 2;
                    }
                  } else {  // if area > 658.19
                    if (xyRatio <= 0.37935) {
                      return 3;
                    } else {  // if xyRatio > 0.37935
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.68359
                  if (shiftNcc <= 0.96136) {
                    if (maxEdgeLength <= 51.987) {
                      return 2;
                    } else {  // if maxEdgeLength > 51.987
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.96136
                    if (xyRatio <= 0.78686) {
                      return 3;
                    } else {  // if xyRatio > 0.78686
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 2123.7
            if (area <= 4831.4) {
              if (shiftStd <= 1.5018) {
                if (confidence <= 0.69352) {
                  if (avgErr <= 8.0108) {
                    if (confidence <= 0.57737) {
                      return 2;
                    } else {  // if confidence > 0.57737
                      return 1;
                    }
                  } else {  // if avgErr > 8.0108
                    if (confidence <= 0.65399) {
                      return 1;
                    } else {  // if confidence > 0.65399
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.69352
                  if (avgErr <= 4.5828) {
                    if (shiftStd <= 0.07678) {
                      return 3;
                    } else {  // if shiftStd > 0.07678
                      return 2;
                    }
                  } else {  // if avgErr > 4.5828
                    if (maxEdgeLength <= 58.79) {
                      return 1;
                    } else {  // if maxEdgeLength > 58.79
                      return 1;
                    }
                  }
                }
              } else {  // if shiftStd > 1.5018
                if (confidence <= 0.65714) {
                  if (avgErr <= 16.983) {
                    if (xyRatio <= 1.439) {
                      return 3;
                    } else {  // if xyRatio > 1.439
                      return 1;
                    }
                  } else {  // if avgErr > 16.983
                    if (xyRatio <= 0.29449) {
                      return 2;
                    } else {  // if xyRatio > 0.29449
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.65714
                  if (maxEdgeLength <= 73.313) {
                    if (xyRatio <= 0.49067) {
                      return 1;
                    } else {  // if xyRatio > 0.49067
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 73.313
                    if (shiftStd <= 15.958) {
                      return 1;
                    } else {  // if shiftStd > 15.958
                      return 3;
                    }
                  }
                }
              }
            } else {  // if area > 4831.4
              if (shiftNcc <= 0.74123) {
                if (avgErr <= 8.9034) {
                  if (shiftAvg <= 4.8924) {
                    if (avgErr <= 7.3816) {
                      return 3;
                    } else {  // if avgErr > 7.3816
                      return 3;
                    }
                  } else {  // if shiftAvg > 4.8924
                    if (area <= 5687.8) {
                      return 4;
                    } else {  // if area > 5687.8
                      return 2;
                    }
                  }
                } else {  // if avgErr > 8.9034
                  if (avgErr <= 12.371) {
                    if (shiftAvg <= 4.9948) {
                      return 1;
                    } else {  // if shiftAvg > 4.9948
                      return 3;
                    }
                  } else {  // if avgErr > 12.371
                    if (shiftNcc <= 0.45814) {
                      return 1;
                    } else {  // if shiftNcc > 0.45814
                      return 0;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.74123
                if (shiftNcc <= 0.96942) {
                  if (confidence <= 0.77389) {
                    if (area <= 8317.8) {
                      return 1;
                    } else {  // if area > 8317.8
                      return 2;
                    }
                  } else {  // if confidence > 0.77389
                    if (confidence <= 0.78128) {
                      return 0;
                    } else {  // if confidence > 0.78128
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.96942
                  if (xyRatio <= 0.72824) {
                    if (shiftAvg <= 11.192) {
                      return 0;
                    } else {  // if shiftAvg > 11.192
                      return 0;
                    }
                  } else {  // if xyRatio > 0.72824
                    if (confidence <= 0.58055) {
                      return 1;
                    } else {  // if confidence > 0.58055
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
  } else {  // if confidence > 0.78267
    if (maxEdgeLength <= 86.765) {
      if (confidence <= 0.87237) {
        if (maxEdgeLength <= 48.662) {
          if (maxEdgeLength <= 37.365) {
            if (shiftAvg <= 0.089518) {
              if (maxEdgeLength <= 34.112) {
                if (xyRatio <= 0.47712) {
                  if (shiftNcc <= -0.34985) {
                    return 4;
                  } else {  // if shiftNcc > -0.34985
                    if (xyRatio <= 0.34404) {
                      return 2;
                    } else {  // if xyRatio > 0.34404
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.47712
                  if (avgErr <= 9.2709) {
                    if (maxEdgeLength <= 23.445) {
                      return 3;
                    } else {  // if maxEdgeLength > 23.445
                      return 3;
                    }
                  } else {  // if avgErr > 9.2709
                    if (shiftNcc <= 0.035621) {
                      return 2;
                    } else {  // if shiftNcc > 0.035621
                      return 3;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 34.112
                if (xyRatio <= 0.46517) {
                  if (area <= 483.31) {
                    if (shiftStd <= 0.017252) {
                      return 2;
                    } else {  // if shiftStd > 0.017252
                      return 2;
                    }
                  } else {  // if area > 483.31
                    if (maxEdgeLength <= 35.624) {
                      return 3;
                    } else {  // if maxEdgeLength > 35.624
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.46517
                  if (avgErr <= 7.3052) {
                    if (confidence <= 0.84898) {
                      return 3;
                    } else {  // if confidence > 0.84898
                      return 2;
                    }
                  } else {  // if avgErr > 7.3052
                    if (confidence <= 0.79702) {
                      return 3;
                    } else {  // if confidence > 0.79702
                      return 2;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.089518
              if (avgErr <= 7.2062) {
                if (maxEdgeLength <= 29.094) {
                  if (area <= 219.38) {
                    return 3;
                  } else {  // if area > 219.38
                    if (shiftAvg <= 0.56816) {
                      return 2;
                    } else {  // if shiftAvg > 0.56816
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 29.094
                  if (avgErr <= 4.7207) {
                    if (shiftAvg <= 0.31623) {
                      return 3;
                    } else {  // if shiftAvg > 0.31623
                      return 2;
                    }
                  } else {  // if avgErr > 4.7207
                    if (maxEdgeLength <= 31.547) {
                      return 2;
                    } else {  // if maxEdgeLength > 31.547
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 7.2062
                if (avgErr <= 11.075) {
                  if (confidence <= 0.83639) {
                    if (shiftAvg <= 0.96783) {
                      return 3;
                    } else {  // if shiftAvg > 0.96783
                      return 2;
                    }
                  } else {  // if confidence > 0.83639
                    if (maxEdgeLength <= 36.13) {
                      return 2;
                    } else {  // if maxEdgeLength > 36.13
                      return 2;
                    }
                  }
                } else {  // if avgErr > 11.075
                  if (maxEdgeLength <= 31.173) {
                    if (shiftStd <= 0.0058561) {
                      return 2;
                    } else {  // if shiftStd > 0.0058561
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 31.173
                    if (area <= 511) {
                      return 2;
                    } else {  // if area > 511
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 37.365
            if (confidence <= 0.82997) {
              if (avgErr <= 4.9828) {
                if (avgErr <= 4.071) {
                  if (maxEdgeLength <= 45.067) {
                    if (maxEdgeLength <= 37.785) {
                      return 2;
                    } else {  // if maxEdgeLength > 37.785
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 45.067
                    if (maxEdgeLength <= 46.954) {
                      return 2;
                    } else {  // if maxEdgeLength > 46.954
                      return 3;
                    }
                  }
                } else {  // if avgErr > 4.071
                  if (shiftAvg <= 0.056918) {
                    if (xyRatio <= 0.48481) {
                      return 2;
                    } else {  // if xyRatio > 0.48481
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.056918
                    if (avgErr <= 4.0874) {
                      return 2;
                    } else {  // if avgErr > 4.0874
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 4.9828
                if (shiftAvg <= 1.3214) {
                  if (xyRatio <= 0.4821) {
                    if (area <= 659.67) {
                      return 2;
                    } else {  // if area > 659.67
                      return 2;
                    }
                  } else {  // if xyRatio > 0.4821
                    if (maxEdgeLength <= 44.002) {
                      return 3;
                    } else {  // if maxEdgeLength > 44.002
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 1.3214
                  if (shiftAvg <= 3.8966) {
                    if (shiftNcc <= 0.98701) {
                      return 2;
                    } else {  // if shiftNcc > 0.98701
                      return 2;
                    }
                  } else {  // if shiftAvg > 3.8966
                    if (xyRatio <= 1.1341) {
                      return 1;
                    } else {  // if xyRatio > 1.1341
                      return 3;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.82997
              if (avgErr <= 7.7651) {
                if (avgErr <= 5.213) {
                  if (maxEdgeLength <= 45.831) {
                    if (shiftNcc <= 0.58371) {
                      return 1;
                    } else {  // if shiftNcc > 0.58371
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 45.831
                    if (shiftAvg <= 0.056501) {
                      return 2;
                    } else {  // if shiftAvg > 0.056501
                      return 2;
                    }
                  }
                } else {  // if avgErr > 5.213
                  if (xyRatio <= 0.36092) {
                    if (shiftStd <= 0.10891) {
                      return 2;
                    } else {  // if shiftStd > 0.10891
                      return 2;
                    }
                  } else {  // if xyRatio > 0.36092
                    if (area <= 833.22) {
                      return 1;
                    } else {  // if area > 833.22
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 7.7651
                if (shiftAvg <= 1.532) {
                  if (confidence <= 0.84763) {
                    if (area <= 567.07) {
                      return 3;
                    } else {  // if area > 567.07
                      return 2;
                    }
                  } else {  // if confidence > 0.84763
                    if (shiftAvg <= 0.32552) {
                      return 2;
                    } else {  // if shiftAvg > 0.32552
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 1.532
                  if (maxEdgeLength <= 44.537) {
                    if (maxEdgeLength <= 38.916) {
                      return 2;
                    } else {  // if maxEdgeLength > 38.916
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 44.537
                    if (shiftNcc <= 0.93829) {
                      return 2;
                    } else {  // if shiftNcc > 0.93829
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 48.662
          if (area <= 2026) {
            if (shiftAvg <= 1.3377) {
              if (xyRatio <= 0.325) {
                if (area <= 1000.3) {
                  if (confidence <= 0.85659) {
                    if (shiftNcc <= 0.15589) {
                      return 3;
                    } else {  // if shiftNcc > 0.15589
                      return 2;
                    }
                  } else {  // if confidence > 0.85659
                    if (avgErr <= 9.1451) {
                      return 1;
                    } else {  // if avgErr > 9.1451
                      return 2;
                    }
                  }
                } else {  // if area > 1000.3
                  if (confidence <= 0.85963) {
                    if (avgErr <= 8.2204) {
                      return 3;
                    } else {  // if avgErr > 8.2204
                      return 2;
                    }
                  } else {  // if confidence > 0.85963
                    if (avgErr <= 6.7914) {
                      return 1;
                    } else {  // if avgErr > 6.7914
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.325
                if (maxEdgeLength <= 56.14) {
                  if (confidence <= 0.83022) {
                    if (shiftAvg <= 0.62372) {
                      return 2;
                    } else {  // if shiftAvg > 0.62372
                      return 2;
                    }
                  } else {  // if confidence > 0.83022
                    if (avgErr <= 4.8992) {
                      return 2;
                    } else {  // if avgErr > 4.8992
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 56.14
                  if (shiftAvg <= 0.29383) {
                    if (avgErr <= 4.7128) {
                      return 2;
                    } else {  // if avgErr > 4.7128
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.29383
                    if (avgErr <= 8.3446) {
                      return 1;
                    } else {  // if avgErr > 8.3446
                      return 1;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 1.3377
              if (maxEdgeLength <= 52.144) {
                if (shiftNcc <= 0.76332) {
                  if (area <= 1950.7) {
                    if (avgErr <= 9.3987) {
                      return 1;
                    } else {  // if avgErr > 9.3987
                      return 2;
                    }
                  } else {  // if area > 1950.7
                    return 3;
                  }
                } else {  // if shiftNcc > 0.76332
                  if (avgErr <= 8.2708) {
                    if (area <= 1029.5) {
                      return 2;
                    } else {  // if area > 1029.5
                      return 1;
                    }
                  } else {  // if avgErr > 8.2708
                    if (xyRatio <= 0.56049) {
                      return 1;
                    } else {  // if xyRatio > 0.56049
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 52.144
                if (xyRatio <= 0.31859) {
                  if (avgErr <= 9.3301) {
                    if (xyRatio <= 0.26406) {
                      return 2;
                    } else {  // if xyRatio > 0.26406
                      return 1;
                    }
                  } else {  // if avgErr > 9.3301
                    if (area <= 930.38) {
                      return 2;
                    } else {  // if area > 930.38
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.31859
                  if (confidence <= 0.8299) {
                    if (shiftNcc <= 0.97009) {
                      return 1;
                    } else {  // if shiftNcc > 0.97009
                      return 1;
                    }
                  } else {  // if confidence > 0.8299
                    if (avgErr <= 9.7992) {
                      return 1;
                    } else {  // if avgErr > 9.7992
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 2026
            if (avgErr <= 4.4816) {
              if (area <= 3410.9) {
                if (xyRatio <= 0.51585) {
                  if (area <= 2872.7) {
                    if (shiftStd <= 0.13948) {
                      return 3;
                    } else {  // if shiftStd > 0.13948
                      return 2;
                    }
                  } else {  // if area > 2872.7
                    return 1;
                  }
                } else {  // if xyRatio > 0.51585
                  if (shiftAvg <= 1.8622) {
                    if (maxEdgeLength <= 51.134) {
                      return 3;
                    } else {  // if maxEdgeLength > 51.134
                      return 2;
                    }
                  } else {  // if shiftAvg > 1.8622
                    if (shiftNcc <= 0.99487) {
                      return 1;
                    } else {  // if shiftNcc > 0.99487
                      return 3;
                    }
                  }
                }
              } else {  // if area > 3410.9
                return 1;
              }
            } else {  // if avgErr > 4.4816
              if (avgErr <= 8.0107) {
                if (xyRatio <= 0.47865) {
                  if (area <= 2285.7) {
                    if (shiftStd <= 0.06959) {
                      return 1;
                    } else {  // if shiftStd > 0.06959
                      return 1;
                    }
                  } else {  // if area > 2285.7
                    if (shiftAvg <= 1.9793) {
                      return 1;
                    } else {  // if shiftAvg > 1.9793
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.47865
                  if (area <= 2249.1) {
                    if (confidence <= 0.84147) {
                      return 1;
                    } else {  // if confidence > 0.84147
                      return 2;
                    }
                  } else {  // if area > 2249.1
                    if (xyRatio <= 1.056) {
                      return 1;
                    } else {  // if xyRatio > 1.056
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 8.0107
                if (area <= 3103.7) {
                  if (shiftAvg <= 0.87179) {
                    if (xyRatio <= 0.50262) {
                      return 1;
                    } else {  // if xyRatio > 0.50262
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.87179
                    if (shiftStd <= 7.1223) {
                      return 1;
                    } else {  // if shiftStd > 7.1223
                      return 2;
                    }
                  }
                } else {  // if area > 3103.7
                  if (shiftAvg <= 2.3604) {
                    if (maxEdgeLength <= 81.897) {
                      return 1;
                    } else {  // if maxEdgeLength > 81.897
                      return 1;
                    }
                  } else {  // if shiftAvg > 2.3604
                    if (shiftNcc <= 0.87998) {
                      return 1;
                    } else {  // if shiftNcc > 0.87998
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if confidence > 0.87237
        if (confidence <= 0.89281) {
          if (area <= 2091.7) {
            if (xyRatio <= 0.49395) {
              if (avgErr <= 7.0572) {
                if (area <= 986.52) {
                  if (shiftStd <= 0.029881) {
                    if (area <= 728.97) {
                      return 3;
                    } else {  // if area > 728.97
                      return 1;
                    }
                  } else {  // if shiftStd > 0.029881
                    if (avgErr <= 4.3923) {
                      return 3;
                    } else {  // if avgErr > 4.3923
                      return 1;
                    }
                  }
                } else {  // if area > 986.52
                  if (xyRatio <= 0.46029) {
                    if (shiftAvg <= 0.34569) {
                      return 1;
                    } else {  // if shiftAvg > 0.34569
                      return 1;
                    }
                  } else {  // if xyRatio > 0.46029
                    if (maxEdgeLength <= 54.531) {
                      return 1;
                    } else {  // if maxEdgeLength > 54.531
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 7.0572
                if (confidence <= 0.88551) {
                  if (maxEdgeLength <= 43.258) {
                    if (avgErr <= 8.8343) {
                      return 1;
                    } else {  // if avgErr > 8.8343
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 43.258
                    if (shiftAvg <= 1.3851) {
                      return 1;
                    } else {  // if shiftAvg > 1.3851
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.88551
                  if (avgErr <= 11.565) {
                    if (maxEdgeLength <= 66.285) {
                      return 1;
                    } else {  // if maxEdgeLength > 66.285
                      return 1;
                    }
                  } else {  // if avgErr > 11.565
                    if (maxEdgeLength <= 44.221) {
                      return 2;
                    } else {  // if maxEdgeLength > 44.221
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.49395
              if (shiftAvg <= 1.8108) {
                if (maxEdgeLength <= 49.087) {
                  if (xyRatio <= 0.7) {
                    if (area <= 1339.8) {
                      return 2;
                    } else {  // if area > 1339.8
                      return 2;
                    }
                  } else {  // if xyRatio > 0.7
                    if (xyRatio <= 1.196) {
                      return 1;
                    } else {  // if xyRatio > 1.196
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 49.087
                  if (avgErr <= 5.8561) {
                    if (maxEdgeLength <= 54.113) {
                      return 1;
                    } else {  // if maxEdgeLength > 54.113
                      return 2;
                    }
                  } else {  // if avgErr > 5.8561
                    if (xyRatio <= 0.55773) {
                      return 1;
                    } else {  // if xyRatio > 0.55773
                      return 2;
                    }
                  }
                }
              } else {  // if shiftAvg > 1.8108
                if (shiftStd <= 0.38974) {
                  if (maxEdgeLength <= 52.213) {
                    if (maxEdgeLength <= 38.475) {
                      return 2;
                    } else {  // if maxEdgeLength > 38.475
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 52.213
                    if (area <= 2051.9) {
                      return 1;
                    } else {  // if area > 2051.9
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.38974
                  if (shiftAvg <= 7.1929) {
                    if (shiftAvg <= 1.9117) {
                      return 1;
                    } else {  // if shiftAvg > 1.9117
                      return 2;
                    }
                  } else {  // if shiftAvg > 7.1929
                    return 1;
                  }
                }
              }
            }
          } else {  // if area > 2091.7
            if (xyRatio <= 0.51188) {
              if (avgErr <= 9.3582) {
                if (xyRatio <= 0.3802) {
                  if (xyRatio <= 0.33089) {
                    if (shiftStd <= 0.34762) {
                      return 2;
                    } else {  // if shiftStd > 0.34762
                      return 1;
                    }
                  } else {  // if xyRatio > 0.33089
                    if (area <= 2575.5) {
                      return 1;
                    } else {  // if area > 2575.5
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 0.3802
                  if (xyRatio <= 0.44955) {
                    if (shiftAvg <= 0.24805) {
                      return 1;
                    } else {  // if shiftAvg > 0.24805
                      return 1;
                    }
                  } else {  // if xyRatio > 0.44955
                    if (shiftAvg <= 1.5032) {
                      return 1;
                    } else {  // if shiftAvg > 1.5032
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 9.3582
                if (shiftAvg <= 0.61515) {
                  if (shiftAvg <= 0.60535) {
                    if (avgErr <= 9.7898) {
                      return 1;
                    } else {  // if avgErr > 9.7898
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.60535
                    return 2;
                  }
                } else {  // if shiftAvg > 0.61515
                  if (avgErr <= 10.343) {
                    if (shiftStd <= 0.084377) {
                      return 2;
                    } else {  // if shiftStd > 0.084377
                      return 1;
                    }
                  } else {  // if avgErr > 10.343
                    if (shiftStd <= 2.6295) {
                      return 1;
                    } else {  // if shiftStd > 2.6295
                      return 2;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.51188
              if (avgErr <= 4.4298) {
                if (area <= 3283) {
                  if (shiftStd <= 0.018975) {
                    if (maxEdgeLength <= 50.421) {
                      return 1;
                    } else {  // if maxEdgeLength > 50.421
                      return 0;
                    }
                  } else {  // if shiftStd > 0.018975
                    if (maxEdgeLength <= 69.129) {
                      return 2;
                    } else {  // if maxEdgeLength > 69.129
                      return 1;
                    }
                  }
                } else {  // if area > 3283
                  return 1;
                }
              } else {  // if avgErr > 4.4298
                if (area <= 2370.5) {
                  if (avgErr <= 6.4159) {
                    if (xyRatio <= 0.90053) {
                      return 1;
                    } else {  // if xyRatio > 0.90053
                      return 2;
                    }
                  } else {  // if avgErr > 6.4159
                    if (maxEdgeLength <= 66.253) {
                      return 1;
                    } else {  // if maxEdgeLength > 66.253
                      return 2;
                    }
                  }
                } else {  // if area > 2370.5
                  if (avgErr <= 8.8872) {
                    if (area <= 3267.6) {
                      return 1;
                    } else {  // if area > 3267.6
                      return 1;
                    }
                  } else {  // if avgErr > 8.8872
                    if (xyRatio <= 0.79639) {
                      return 1;
                    } else {  // if xyRatio > 0.79639
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.89281
          if (maxEdgeLength <= 73.293) {
            if (confidence <= 0.90731) {
              if (avgErr <= 4.0665) {
                if (shiftAvg <= 0.034921) {
                  return 1;
                } else {  // if shiftAvg > 0.034921
                  if (area <= 1694.5) {
                    return 1;
                  } else {  // if area > 1694.5
                    if (xyRatio <= 0.62587) {
                      return 2;
                    } else {  // if xyRatio > 0.62587
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 4.0665
                if (avgErr <= 9.9887) {
                  if (area <= 1652.3) {
                    if (xyRatio <= 0.56758) {
                      return 1;
                    } else {  // if xyRatio > 0.56758
                      return 1;
                    }
                  } else {  // if area > 1652.3
                    if (area <= 2599) {
                      return 1;
                    } else {  // if area > 2599
                      return 1;
                    }
                  }
                } else {  // if avgErr > 9.9887
                  if (maxEdgeLength <= 57.938) {
                    if (shiftAvg <= 0.33023) {
                      return 1;
                    } else {  // if shiftAvg > 0.33023
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 57.938
                    if (shiftNcc <= 0.41418) {
                      return 1;
                    } else {  // if shiftNcc > 0.41418
                      return 1;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.90731
              if (shiftAvg <= 18.121) {
                if (avgErr <= 4.0482) {
                  if (area <= 2921.5) {
                    if (confidence <= 0.90849) {
                      return 1;
                    } else {  // if confidence > 0.90849
                      return 2;
                    }
                  } else {  // if area > 2921.5
                    return 1;
                  }
                } else {  // if avgErr > 4.0482
                  if (maxEdgeLength <= 48.3) {
                    if (xyRatio <= 0.78836) {
                      return 1;
                    } else {  // if xyRatio > 0.78836
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 48.3
                    if (area <= 1894.3) {
                      return 1;
                    } else {  // if area > 1894.3
                      return 1;
                    }
                  }
                }
              } else {  // if shiftAvg > 18.121
                if (maxEdgeLength <= 59.972) {
                  return 3;
                } else {  // if maxEdgeLength > 59.972
                  if (confidence <= 0.91405) {
                    return 1;
                  } else {  // if confidence > 0.91405
                    if (shiftAvg <= 73.595) {
                      return 0;
                    } else {  // if shiftAvg > 73.595
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 73.293
            if (avgErr <= 6.1969) {
              if (shiftAvg <= 0.5444) {
                if (shiftNcc <= 0.88911) {
                  if (avgErr <= 5.8638) {
                    if (shiftStd <= 0.072436) {
                      return 1;
                    } else {  // if shiftStd > 0.072436
                      return 1;
                    }
                  } else {  // if avgErr > 5.8638
                    if (area <= 2904.8) {
                      return 1;
                    } else {  // if area > 2904.8
                      return 0;
                    }
                  }
                } else {  // if shiftNcc > 0.88911
                  if (shiftStd <= 0.046751) {
                    if (avgErr <= 6.1224) {
                      return 0;
                    } else {  // if avgErr > 6.1224
                      return 1;
                    }
                  } else {  // if shiftStd > 0.046751
                    if (shiftAvg <= 0.35633) {
                      return 0;
                    } else {  // if shiftAvg > 0.35633
                      return 1;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.5444
                if (xyRatio <= 0.99259) {
                  if (xyRatio <= 0.34363) {
                    if (avgErr <= 5.8857) {
                      return 2;
                    } else {  // if avgErr > 5.8857
                      return 1;
                    }
                  } else {  // if xyRatio > 0.34363
                    if (confidence <= 0.9464) {
                      return 1;
                    } else {  // if confidence > 0.9464
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 0.99259
                  if (maxEdgeLength <= 77.861) {
                    return 2;
                  } else {  // if maxEdgeLength > 77.861
                    if (shiftAvg <= 10.357) {
                      return 0;
                    } else {  // if shiftAvg > 10.357
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 6.1969
              if (area <= 2304) {
                if (confidence <= 0.90641) {
                  if (xyRatio <= 0.38377) {
                    if (confidence <= 0.89819) {
                      return 1;
                    } else {  // if confidence > 0.89819
                      return 1;
                    }
                  } else {  // if xyRatio > 0.38377
                    if (avgErr <= 8.3554) {
                      return 0;
                    } else {  // if avgErr > 8.3554
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.90641
                  if (xyRatio <= 0.3951) {
                    if (shiftStd <= 0.030678) {
                      return 0;
                    } else {  // if shiftStd > 0.030678
                      return 1;
                    }
                  } else {  // if xyRatio > 0.3951
                    if (avgErr <= 8.7529) {
                      return 0;
                    } else {  // if avgErr > 8.7529
                      return 1;
                    }
                  }
                }
              } else {  // if area > 2304
                if (avgErr <= 10.576) {
                  if (shiftAvg <= 1.7685) {
                    if (confidence <= 0.92152) {
                      return 1;
                    } else {  // if confidence > 0.92152
                      return 1;
                    }
                  } else {  // if shiftAvg > 1.7685
                    if (area <= 5761.5) {
                      return 1;
                    } else {  // if area > 5761.5
                      return 0;
                    }
                  }
                } else {  // if avgErr > 10.576
                  if (xyRatio <= 0.6988) {
                    if (shiftAvg <= 0.53391) {
                      return 1;
                    } else {  // if shiftAvg > 0.53391
                      return 1;
                    }
                  } else {  // if xyRatio > 0.6988
                    if (shiftAvg <= 44.255) {
                      return 0;
                    } else {  // if shiftAvg > 44.255
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 86.765
      if (maxEdgeLength <= 126.41) {
        if (maxEdgeLength <= 100.43) {
          if (avgErr <= 11.353) {
            if (avgErr <= 6.9583) {
              if (shiftAvg <= 0.58963) {
                if (area <= 2691.5) {
                  if (shiftAvg <= 0.40865) {
                    if (confidence <= 0.92201) {
                      return 2;
                    } else {  // if confidence > 0.92201
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.40865
                    if (confidence <= 0.89056) {
                      return 2;
                    } else {  // if confidence > 0.89056
                      return 1;
                    }
                  }
                } else {  // if area > 2691.5
                  if (confidence <= 0.93503) {
                    if (shiftNcc <= 0.69545) {
                      return 1;
                    } else {  // if shiftNcc > 0.69545
                      return 1;
                    }
                  } else {  // if confidence > 0.93503
                    if (area <= 3917.1) {
                      return 1;
                    } else {  // if area > 3917.1
                      return 0;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.58963
                if (confidence <= 0.92118) {
                  if (xyRatio <= 0.24525) {
                    if (confidence <= 0.8731) {
                      return 3;
                    } else {  // if confidence > 0.8731
                      return 2;
                    }
                  } else {  // if xyRatio > 0.24525
                    if (shiftAvg <= 3.2761) {
                      return 1;
                    } else {  // if shiftAvg > 3.2761
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.92118
                  if (maxEdgeLength <= 92.32) {
                    if (avgErr <= 6.5336) {
                      return 0;
                    } else {  // if avgErr > 6.5336
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 92.32
                    if (shiftNcc <= 0.80609) {
                      return 1;
                    } else {  // if shiftNcc > 0.80609
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgErr > 6.9583
              if (xyRatio <= 0.34404) {
                if (shiftAvg <= 0.47729) {
                  if (xyRatio <= 0.30009) {
                    if (avgErr <= 9.7497) {
                      return 3;
                    } else {  // if avgErr > 9.7497
                      return 1;
                    }
                  } else {  // if xyRatio > 0.30009
                    if (area <= 2956.5) {
                      return 1;
                    } else {  // if area > 2956.5
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.47729
                  if (confidence <= 0.88595) {
                    if (shiftNcc <= 0.32652) {
                      return 2;
                    } else {  // if shiftNcc > 0.32652
                      return 1;
                    }
                  } else {  // if confidence > 0.88595
                    if (xyRatio <= 0.29224) {
                      return 1;
                    } else {  // if xyRatio > 0.29224
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.34404
                if (confidence <= 0.90486) {
                  if (shiftNcc <= 0.96389) {
                    if (shiftNcc <= 0.71168) {
                      return 1;
                    } else {  // if shiftNcc > 0.71168
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.96389
                    if (shiftAvg <= 3.0639) {
                      return 1;
                    } else {  // if shiftAvg > 3.0639
                      return 0;
                    }
                  }
                } else {  // if confidence > 0.90486
                  if (maxEdgeLength <= 95.714) {
                    if (shiftAvg <= 0.23098) {
                      return 0;
                    } else {  // if shiftAvg > 0.23098
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 95.714
                    if (shiftAvg <= 1.3972) {
                      return 1;
                    } else {  // if shiftAvg > 1.3972
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if avgErr > 11.353
            if (area <= 4668.6) {
              if (area <= 3564.3) {
                if (area <= 2549.6) {
                  if (shiftAvg <= 1.234) {
                    if (shiftNcc <= 0.45615) {
                      return 2;
                    } else {  // if shiftNcc > 0.45615
                      return 1;
                    }
                  } else {  // if shiftAvg > 1.234
                    if (shiftStd <= 1.9447) {
                      return 2;
                    } else {  // if shiftStd > 1.9447
                      return 1;
                    }
                  }
                } else {  // if area > 2549.6
                  if (shiftStd <= 6.1241) {
                    if (maxEdgeLength <= 92.317) {
                      return 1;
                    } else {  // if maxEdgeLength > 92.317
                      return 1;
                    }
                  } else {  // if shiftStd > 6.1241
                    if (confidence <= 0.91579) {
                      return 1;
                    } else {  // if confidence > 0.91579
                      return 0;
                    }
                  }
                }
              } else {  // if area > 3564.3
                if (confidence <= 0.91767) {
                  if (confidence <= 0.80719) {
                    if (confidence <= 0.80536) {
                      return 0;
                    } else {  // if confidence > 0.80536
                      return 2;
                    }
                  } else {  // if confidence > 0.80719
                    if (shiftNcc <= 0.037109) {
                      return 0;
                    } else {  // if shiftNcc > 0.037109
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.91767
                  if (shiftAvg <= 0.64127) {
                    if (confidence <= 0.92781) {
                      return 1;
                    } else {  // if confidence > 0.92781
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.64127
                    if (area <= 3746.6) {
                      return 1;
                    } else {  // if area > 3746.6
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 4668.6
              if (shiftAvg <= 11.26) {
                if (confidence <= 0.92513) {
                  if (shiftNcc <= 0.53578) {
                    if (xyRatio <= 0.49489) {
                      return 2;
                    } else {  // if xyRatio > 0.49489
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.53578
                    if (confidence <= 0.88809) {
                      return 1;
                    } else {  // if confidence > 0.88809
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.92513
                  if (shiftStd <= 1.5269) {
                    if (shiftAvg <= 9.0138) {
                      return 0;
                    } else {  // if shiftAvg > 9.0138
                      return 1;
                    }
                  } else {  // if shiftStd > 1.5269
                    if (avgErr <= 13.605) {
                      return 1;
                    } else {  // if avgErr > 13.605
                      return 0;
                    }
                  }
                }
              } else {  // if shiftAvg > 11.26
                if (confidence <= 0.87479) {
                  if (confidence <= 0.86191) {
                    if (shiftAvg <= 15.444) {
                      return 0;
                    } else {  // if shiftAvg > 15.444
                      return 1;
                    }
                  } else {  // if confidence > 0.86191
                    return 2;
                  }
                } else {  // if confidence > 0.87479
                  if (shiftStd <= 7.1658) {
                    if (maxEdgeLength <= 100.16) {
                      return 0;
                    } else {  // if maxEdgeLength > 100.16
                      return 1;
                    }
                  } else {  // if shiftStd > 7.1658
                    if (avgErr <= 12.366) {
                      return 0;
                    } else {  // if avgErr > 12.366
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 100.43
          if (shiftNcc <= 0.78542) {
            if (confidence <= 0.93125) {
              if (avgErr <= 7.64) {
                if (maxEdgeLength <= 103.3) {
                  if (shiftAvg <= 1.5837) {
                    if (shiftNcc <= 0.579) {
                      return 1;
                    } else {  // if shiftNcc > 0.579
                      return 1;
                    }
                  } else {  // if shiftAvg > 1.5837
                    if (shiftAvg <= 2.9454) {
                      return 0;
                    } else {  // if shiftAvg > 2.9454
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 103.3
                  if (shiftStd <= 0.37836) {
                    if (xyRatio <= 0.46399) {
                      return 1;
                    } else {  // if xyRatio > 0.46399
                      return 1;
                    }
                  } else {  // if shiftStd > 0.37836
                    if (shiftNcc <= 0.51312) {
                      return 1;
                    } else {  // if shiftNcc > 0.51312
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 7.64
                if (confidence <= 0.88933) {
                  if (shiftStd <= 0.99709) {
                    if (xyRatio <= 0.36129) {
                      return 2;
                    } else {  // if xyRatio > 0.36129
                      return 0;
                    }
                  } else {  // if shiftStd > 0.99709
                    if (area <= 3119.7) {
                      return 2;
                    } else {  // if area > 3119.7
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.88933
                  if (xyRatio <= 0.3477) {
                    if (maxEdgeLength <= 114.89) {
                      return 1;
                    } else {  // if maxEdgeLength > 114.89
                      return 1;
                    }
                  } else {  // if xyRatio > 0.3477
                    if (shiftNcc <= 0.69057) {
                      return 1;
                    } else {  // if shiftNcc > 0.69057
                      return 0;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.93125
              if (shiftStd <= 0.88147) {
                if (shiftAvg <= 0.048447) {
                  if (maxEdgeLength <= 108.14) {
                    return 1;
                  } else {  // if maxEdgeLength > 108.14
                    if (shiftNcc <= 0.013309) {
                      return 1;
                    } else {  // if shiftNcc > 0.013309
                      return 0;
                    }
                  }
                } else {  // if shiftAvg > 0.048447
                  if (shiftNcc <= 0.76448) {
                    if (area <= 4372.5) {
                      return 0;
                    } else {  // if area > 4372.5
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.76448
                    if (confidence <= 0.94668) {
                      return 1;
                    } else {  // if confidence > 0.94668
                      return 0;
                    }
                  }
                }
              } else {  // if shiftStd > 0.88147
                if (maxEdgeLength <= 118.56) {
                  if (xyRatio <= 0.31157) {
                    if (confidence <= 0.94284) {
                      return 1;
                    } else {  // if confidence > 0.94284
                      return 0;
                    }
                  } else {  // if xyRatio > 0.31157
                    if (avgErr <= 7.9926) {
                      return 1;
                    } else {  // if avgErr > 7.9926
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 118.56
                  if (maxEdgeLength <= 124.7) {
                    if (maxEdgeLength <= 122.36) {
                      return 1;
                    } else {  // if maxEdgeLength > 122.36
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 124.7
                    if (shiftAvg <= 2.4507) {
                      return 0;
                    } else {  // if shiftAvg > 2.4507
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if shiftNcc > 0.78542
            if (shiftAvg <= 0.81192) {
              if (maxEdgeLength <= 118.07) {
                if (shiftNcc <= 0.95433) {
                  if (maxEdgeLength <= 115.09) {
                    if (xyRatio <= 0.59137) {
                      return 1;
                    } else {  // if xyRatio > 0.59137
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 115.09
                    if (area <= 5308.1) {
                      return 0;
                    } else {  // if area > 5308.1
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.95433
                  if (confidence <= 0.92955) {
                    if (confidence <= 0.89164) {
                      return 2;
                    } else {  // if confidence > 0.89164
                      return 0;
                    }
                  } else {  // if confidence > 0.92955
                    if (area <= 6250.5) {
                      return 1;
                    } else {  // if area > 6250.5
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 118.07
                if (shiftAvg <= 0.81014) {
                  if (xyRatio <= 0.45389) {
                    if (xyRatio <= 0.43709) {
                      return 0;
                    } else {  // if xyRatio > 0.43709
                      return 1;
                    }
                  } else {  // if xyRatio > 0.45389
                    if (confidence <= 0.91289) {
                      return 1;
                    } else {  // if confidence > 0.91289
                      return 0;
                    }
                  }
                } else {  // if shiftAvg > 0.81014
                  return 1;
                }
              }
            } else {  // if shiftAvg > 0.81192
              if (xyRatio <= 0.30797) {
                if (confidence <= 0.89596) {
                  if (shiftAvg <= 4.7434) {
                    if (area <= 3229) {
                      return 1;
                    } else {  // if area > 3229
                      return 2;
                    }
                  } else {  // if shiftAvg > 4.7434
                    if (maxEdgeLength <= 111.59) {
                      return 0;
                    } else {  // if maxEdgeLength > 111.59
                      return 2;
                    }
                  }
                } else {  // if confidence > 0.89596
                  if (confidence <= 0.93652) {
                    if (maxEdgeLength <= 106.75) {
                      return 1;
                    } else {  // if maxEdgeLength > 106.75
                      return 1;
                    }
                  } else {  // if confidence > 0.93652
                    if (maxEdgeLength <= 104.48) {
                      return 2;
                    } else {  // if maxEdgeLength > 104.48
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.30797
                if (maxEdgeLength <= 106.39) {
                  if (avgErr <= 10.384) {
                    if (shiftAvg <= 7.2476) {
                      return 0;
                    } else {  // if shiftAvg > 7.2476
                      return 0;
                    }
                  } else {  // if avgErr > 10.384
                    if (area <= 5465.4) {
                      return 1;
                    } else {  // if area > 5465.4
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 106.39
                  if (confidence <= 0.86617) {
                    if (shiftNcc <= 0.94504) {
                      return 1;
                    } else {  // if shiftNcc > 0.94504
                      return 0;
                    }
                  } else {  // if confidence > 0.86617
                    if (shiftAvg <= 4.2616) {
                      return 0;
                    } else {  // if shiftAvg > 4.2616
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 126.41
        if (confidence <= 0.92898) {
          if (shiftNcc <= 0.73121) {
            if (xyRatio <= 0.36046) {
              if (shiftNcc <= 0.34436) {
                if (shiftNcc <= -0.012897) {
                  if (shiftNcc <= -0.019713) {
                    if (maxEdgeLength <= 214.81) {
                      return 3;
                    } else {  // if maxEdgeLength > 214.81
                      return 1;
                    }
                  } else {  // if shiftNcc > -0.019713
                    return 2;
                  }
                } else {  // if shiftNcc > -0.012897
                  if (avgErr <= 7.9529) {
                    if (xyRatio <= 0.30828) {
                      return 1;
                    } else {  // if xyRatio > 0.30828
                      return 0;
                    }
                  } else {  // if avgErr > 7.9529
                    if (area <= 19969) {
                      return 1;
                    } else {  // if area > 19969
                      return 0;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.34436
                if (maxEdgeLength <= 170.96) {
                  if (confidence <= 0.86546) {
                    if (shiftStd <= 4.7591) {
                      return 3;
                    } else {  // if shiftStd > 4.7591
                      return 2;
                    }
                  } else {  // if confidence > 0.86546
                    if (xyRatio <= 0.32743) {
                      return 1;
                    } else {  // if xyRatio > 0.32743
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 170.96
                  if (confidence <= 0.91798) {
                    if (shiftStd <= 8.2694) {
                      return 1;
                    } else {  // if shiftStd > 8.2694
                      return 0;
                    }
                  } else {  // if confidence > 0.91798
                    if (shiftNcc <= 0.72212) {
                      return 0;
                    } else {  // if shiftNcc > 0.72212
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.36046
              if (confidence <= 0.87809) {
                if (avgErr <= 7.7814) {
                  if (xyRatio <= 0.45771) {
                    if (shiftAvg <= 2.6724) {
                      return 3;
                    } else {  // if shiftAvg > 2.6724
                      return 2;
                    }
                  } else {  // if xyRatio > 0.45771
                    if (avgErr <= 6.8246) {
                      return 0;
                    } else {  // if avgErr > 6.8246
                      return 1;
                    }
                  }
                } else {  // if avgErr > 7.7814
                  if (shiftAvg <= 14.673) {
                    if (area <= 12931) {
                      return 1;
                    } else {  // if area > 12931
                      return 1;
                    }
                  } else {  // if shiftAvg > 14.673
                    if (xyRatio <= 0.6974) {
                      return 0;
                    } else {  // if xyRatio > 0.6974
                      return 1;
                    }
                  }
                }
              } else {  // if confidence > 0.87809
                if (maxEdgeLength <= 173.67) {
                  if (maxEdgeLength <= 157.25) {
                    if (avgErr <= 13.238) {
                      return 0;
                    } else {  // if avgErr > 13.238
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 157.25
                    if (confidence <= 0.92207) {
                      return 1;
                    } else {  // if confidence > 0.92207
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 173.67
                  if (avgErr <= 9.3642) {
                    if (shiftAvg <= 6.3103) {
                      return 0;
                    } else {  // if shiftAvg > 6.3103
                      return 0;
                    }
                  } else {  // if avgErr > 9.3642
                    if (shiftAvg <= 0.33837) {
                      return 1;
                    } else {  // if shiftAvg > 0.33837
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if shiftNcc > 0.73121
            if (confidence <= 0.91605) {
              if (shiftAvg <= 1.2911) {
                if (area <= 6948.8) {
                  return 1;
                } else {  // if area > 6948.8
                  if (confidence <= 0.89916) {
                    if (shiftStd <= 0.26173) {
                      return 0;
                    } else {  // if shiftStd > 0.26173
                      return 1;
                    }
                  } else {  // if confidence > 0.89916
                    return 0;
                  }
                }
              } else {  // if shiftAvg > 1.2911
                if (maxEdgeLength <= 218.99) {
                  if (xyRatio <= 0.43524) {
                    if (shiftStd <= 7.6127) {
                      return 0;
                    } else {  // if shiftStd > 7.6127
                      return 0;
                    }
                  } else {  // if xyRatio > 0.43524
                    if (shiftAvg <= 17.716) {
                      return 0;
                    } else {  // if shiftAvg > 17.716
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 218.99
                  if (shiftAvg <= 534.34) {
                    if (shiftNcc <= 0.98357) {
                      return 0;
                    } else {  // if shiftNcc > 0.98357
                      return 0;
                    }
                  } else {  // if shiftAvg > 534.34
                    return 1;
                  }
                }
              }
            } else {  // if confidence > 0.91605
              if (xyRatio <= 0.54034) {
                if (xyRatio <= 0.42071) {
                  if (maxEdgeLength <= 172.36) {
                    if (maxEdgeLength <= 154.68) {
                      return 0;
                    } else {  // if maxEdgeLength > 154.68
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 172.36
                    if (area <= 25056) {
                      return 0;
                    } else {  // if area > 25056
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.42071
                  if (shiftAvg <= 5.7741) {
                    if (xyRatio <= 0.44829) {
                      return 0;
                    } else {  // if xyRatio > 0.44829
                      return 0;
                    }
                  } else {  // if shiftAvg > 5.7741
                    if (confidence <= 0.9289) {
                      return 0;
                    } else {  // if confidence > 0.9289
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.54034
                if (area <= 20157) {
                  if (shiftNcc <= 0.81952) {
                    if (area <= 11080) {
                      return 0;
                    } else {  // if area > 11080
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.81952
                    if (xyRatio <= 0.54296) {
                      return 1;
                    } else {  // if xyRatio > 0.54296
                      return 0;
                    }
                  }
                } else {  // if area > 20157
                  if (maxEdgeLength <= 165.83) {
                    if (shiftAvg <= 63.534) {
                      return 3;
                    } else {  // if shiftAvg > 63.534
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 165.83
                    return 0;
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.92898
          if (maxEdgeLength <= 181.66) {
            if (area <= 7777.4) {
              if (shiftNcc <= 0.65642) {
                if (xyRatio <= 0.43291) {
                  if (area <= 7595.2) {
                    if (confidence <= 0.93837) {
                      return 1;
                    } else {  // if confidence > 0.93837
                      return 0;
                    }
                  } else {  // if area > 7595.2
                    return 1;
                  }
                } else {  // if xyRatio > 0.43291
                  if (shiftAvg <= 0.90478) {
                    return 0;
                  } else {  // if shiftAvg > 0.90478
                    if (avgErr <= 13.52) {
                      return 1;
                    } else {  // if avgErr > 13.52
                      return 0;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.65642
                if (avgErr <= 9.5636) {
                  if (confidence <= 0.93864) {
                    if (maxEdgeLength <= 137.78) {
                      return 0;
                    } else {  // if maxEdgeLength > 137.78
                      return 0;
                    }
                  } else {  // if confidence > 0.93864
                    if (xyRatio <= 0.47426) {
                      return 0;
                    } else {  // if xyRatio > 0.47426
                      return 1;
                    }
                  }
                } else {  // if avgErr > 9.5636
                  if (shiftNcc <= 0.86909) {
                    if (shiftAvg <= 1.9372) {
                      return 0;
                    } else {  // if shiftAvg > 1.9372
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.86909
                    if (xyRatio <= 0.30353) {
                      return 1;
                    } else {  // if xyRatio > 0.30353
                      return 0;
                    }
                  }
                }
              }
            } else {  // if area > 7777.4
              if (maxEdgeLength <= 130.19) {
                if (xyRatio <= 0.50884) {
                  if (confidence <= 0.9502) {
                    if (shiftNcc <= 0.72611) {
                      return 0;
                    } else {  // if shiftNcc > 0.72611
                      return 0;
                    }
                  } else {  // if confidence > 0.9502
                    if (shiftStd <= 0.24788) {
                      return 1;
                    } else {  // if shiftStd > 0.24788
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 0.50884
                  if (maxEdgeLength <= 129.43) {
                    if (shiftAvg <= 9.1259) {
                      return 0;
                    } else {  // if shiftAvg > 9.1259
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 129.43
                    if (area <= 10859) {
                      return 1;
                    } else {  // if area > 10859
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 130.19
                if (avgErr <= 8.2581) {
                  if (maxEdgeLength <= 158.09) {
                    if (shiftNcc <= 0.48718) {
                      return 0;
                    } else {  // if shiftNcc > 0.48718
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 158.09
                    if (area <= 11094) {
                      return 0;
                    } else {  // if area > 11094
                      return 0;
                    }
                  }
                } else {  // if avgErr > 8.2581
                  if (xyRatio <= 0.64594) {
                    if (xyRatio <= 0.31575) {
                      return 0;
                    } else {  // if xyRatio > 0.31575
                      return 0;
                    }
                  } else {  // if xyRatio > 0.64594
                    if (area <= 16866) {
                      return 1;
                    } else {  // if area > 16866
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 181.66
            if (confidence <= 0.93418) {
              if (area <= 12049) {
                if (maxEdgeLength <= 206.82) {
                  return 1;
                } else {  // if maxEdgeLength > 206.82
                  return 0;
                }
              } else {  // if area > 12049
                if (confidence <= 0.93412) {
                  if (maxEdgeLength <= 209.71) {
                    if (avgErr <= 24.723) {
                      return 0;
                    } else {  // if avgErr > 24.723
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 209.71
                    if (maxEdgeLength <= 218.67) {
                      return 0;
                    } else {  // if maxEdgeLength > 218.67
                      return 0;
                    }
                  }
                } else {  // if confidence > 0.93412
                  return 1;
                }
              }
            } else {  // if confidence > 0.93418
              if (xyRatio <= 0.26117) {
                if (shiftStd <= 8.2955) {
                  return 0;
                } else {  // if shiftStd > 8.2955
                  return 1;
                }
              } else {  // if xyRatio > 0.26117
                if (confidence <= 0.9505) {
                  if (confidence <= 0.95049) {
                    if (maxEdgeLength <= 331.47) {
                      return 0;
                    } else {  // if maxEdgeLength > 331.47
                      return 0;
                    }
                  } else {  // if confidence > 0.95049
                    return 1;
                  }
                } else {  // if confidence > 0.9505
                  if (shiftAvg <= 199.1) {
                    if (avgErr <= 8.7834) {
                      return 0;
                    } else {  // if avgErr > 8.7834
                      return 0;
                    }
                  } else {  // if shiftAvg > 199.1
                    if (shiftAvg <= 201.07) {
                      return 1;
                    } else {  // if shiftAvg > 201.07
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
