#include "strm/tree/MTA.hpp"

namespace rm {

int MTA(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr, float confidence) {
  if (confidence <= 0.81202) {
    if (confidence <= 0.41297) {
      if (maxEdgeLength <= 28.185) {
        if (confidence <= 0.2076) {
          if (area <= 175.71) {
            if (area <= 142.92) {
              if (xyRatio <= 0.26704) {
                return 3;
              } else {  // if xyRatio > 0.26704
                if (shiftAvg <= 0.013429) {
                  return 4;
                } else {  // if shiftAvg > 0.013429
                  if (shiftAvg <= 0.013519) {
                    return 3;
                  } else {  // if shiftAvg > 0.013519
                    if (maxEdgeLength <= 8.3559) {
                      return 4;
                    } else {  // if maxEdgeLength > 8.3559
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 142.92
              if (avgErr <= 1.9498) {
                if (confidence <= 0.20041) {
                  if (maxEdgeLength <= 14.351) {
                    if (shiftAvg <= 0.00018052) {
                      return 4;
                    } else {  // if shiftAvg > 0.00018052
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 14.351
                    return 4;
                  }
                } else {  // if confidence > 0.20041
                  if (xyRatio <= 0.88738) {
                    return 3;
                  } else {  // if xyRatio > 0.88738
                    return 4;
                  }
                }
              } else {  // if avgErr > 1.9498
                if (area <= 142.96) {
                  return 3;
                } else {  // if area > 142.96
                  if (maxEdgeLength <= 14.17) {
                    if (area <= 172.82) {
                      return 4;
                    } else {  // if area > 172.82
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 14.17
                    if (shiftAvg <= 0.044317) {
                      return 4;
                    } else {  // if shiftAvg > 0.044317
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if area > 175.71
            if (avgErr <= 0.39757) {
              if (xyRatio <= 0.40192) {
                if (shiftNcc <= -0.29944) {
                  if (xyRatio <= 0.32767) {
                    return 4;
                  } else {  // if xyRatio > 0.32767
                    return 3;
                  }
                } else {  // if shiftNcc > -0.29944
                  return 4;
                }
              } else {  // if xyRatio > 0.40192
                if (maxEdgeLength <= 20.817) {
                  if (confidence <= 0.16217) {
                    if (confidence <= 0.12058) {
                      return 3;
                    } else {  // if confidence > 0.12058
                      return 4;
                    }
                  } else {  // if confidence > 0.16217
                    if (maxEdgeLength <= 20.687) {
                      return 3;
                    } else {  // if maxEdgeLength > 20.687
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 20.817
                  if (avgErr <= 0.09342) {
                    return 2;
                  } else {  // if avgErr > 0.09342
                    return 3;
                  }
                }
              }
            } else {  // if avgErr > 0.39757
              if (area <= 262.94) {
                if (confidence <= 0.16877) {
                  if (area <= 175.81) {
                    return 3;
                  } else {  // if area > 175.81
                    if (shiftNcc <= -0.99784) {
                      return 3;
                    } else {  // if shiftNcc > -0.99784
                      return 4;
                    }
                  }
                } else {  // if confidence > 0.16877
                  if (shiftStd <= 9.4076e-06) {
                    if (shiftAvg <= 0.013036) {
                      return 3;
                    } else {  // if shiftAvg > 0.013036
                      return 4;
                    }
                  } else {  // if shiftStd > 9.4076e-06
                    if (shiftNcc <= 0.99999) {
                      return 4;
                    } else {  // if shiftNcc > 0.99999
                      return 3;
                    }
                  }
                }
              } else {  // if area > 262.94
                if (xyRatio <= 1.0166) {
                  if (avgErr <= 2.5598) {
                    if (maxEdgeLength <= 26.027) {
                      return 3;
                    } else {  // if maxEdgeLength > 26.027
                      return 4;
                    }
                  } else {  // if avgErr > 2.5598
                    if (maxEdgeLength <= 26.041) {
                      return 4;
                    } else {  // if maxEdgeLength > 26.041
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 1.0166
                  if (area <= 623.59) {
                    if (shiftStd <= 0.025973) {
                      return 4;
                    } else {  // if shiftStd > 0.025973
                      return 4;
                    }
                  } else {  // if area > 623.59
                    if (shiftNcc <= 0.23926) {
                      return 4;
                    } else {  // if shiftNcc > 0.23926
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.2076
          if (avgErr <= 0.68076) {
            if (area <= 159.79) {
              return 4;
            } else {  // if area > 159.79
              if (maxEdgeLength <= 26.235) {
                if (area <= 275.67) {
                  if (confidence <= 0.20865) {
                    if (maxEdgeLength <= 20.606) {
                      return 4;
                    } else {  // if maxEdgeLength > 20.606
                      return 3;
                    }
                  } else {  // if confidence > 0.20865
                    if (maxEdgeLength <= 13.787) {
                      return 4;
                    } else {  // if maxEdgeLength > 13.787
                      return 3;
                    }
                  }
                } else {  // if area > 275.67
                  if (maxEdgeLength <= 23.434) {
                    if (confidence <= 0.25884) {
                      return 3;
                    } else {  // if confidence > 0.25884
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 23.434
                    return 4;
                  }
                }
              } else {  // if maxEdgeLength > 26.235
                if (shiftStd <= 0.00016889) {
                  if (shiftNcc <= -0.17674) {
                    return 3;
                  } else {  // if shiftNcc > -0.17674
                    return 4;
                  }
                } else {  // if shiftStd > 0.00016889
                  return 3;
                }
              }
            }
          } else {  // if avgErr > 0.68076
            if (confidence <= 0.34201) {
              if (maxEdgeLength <= 22.468) {
                if (avgErr <= 1.3903) {
                  if (shiftStd <= 0.00017897) {
                    if (area <= 277.76) {
                      return 3;
                    } else {  // if area > 277.76
                      return 2;
                    }
                  } else {  // if shiftStd > 0.00017897
                    return 4;
                  }
                } else {  // if avgErr > 1.3903
                  if (area <= 174.79) {
                    if (xyRatio <= 0.81011) {
                      return 4;
                    } else {  // if xyRatio > 0.81011
                      return 4;
                    }
                  } else {  // if area > 174.79
                    if (shiftAvg <= 0.00051428) {
                      return 3;
                    } else {  // if shiftAvg > 0.00051428
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 22.468
                if (avgErr <= 1.9419) {
                  if (confidence <= 0.28261) {
                    if (shiftStd <= 0.0019112) {
                      return 4;
                    } else {  // if shiftStd > 0.0019112
                      return 4;
                    }
                  } else {  // if confidence > 0.28261
                    if (confidence <= 0.28904) {
                      return 3;
                    } else {  // if confidence > 0.28904
                      return 4;
                    }
                  }
                } else {  // if avgErr > 1.9419
                  if (maxEdgeLength <= 25.133) {
                    if (confidence <= 0.32022) {
                      return 4;
                    } else {  // if confidence > 0.32022
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 25.133
                    if (shiftNcc <= 0.99949) {
                      return 4;
                    } else {  // if shiftNcc > 0.99949
                      return 3;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.34201
              if (area <= 156.79) {
                if (xyRatio <= 0.79716) {
                  if (avgErr <= 6.6207) {
                    if (xyRatio <= 0.69721) {
                      return 4;
                    } else {  // if xyRatio > 0.69721
                      return 4;
                    }
                  } else {  // if avgErr > 6.6207
                    if (maxEdgeLength <= 16.398) {
                      return 4;
                    } else {  // if maxEdgeLength > 16.398
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.79716
                  if (xyRatio <= 0.93461) {
                    if (avgErr <= 6.306) {
                      return 3;
                    } else {  // if avgErr > 6.306
                      return 4;
                    }
                  } else {  // if xyRatio > 0.93461
                    if (shiftNcc <= 0.49958) {
                      return 4;
                    } else {  // if shiftNcc > 0.49958
                      return 3;
                    }
                  }
                }
              } else {  // if area > 156.79
                if (avgErr <= 1.1345) {
                  if (xyRatio <= 1.1225) {
                    if (maxEdgeLength <= 21.377) {
                      return 3;
                    } else {  // if maxEdgeLength > 21.377
                      return 3;
                    }
                  } else {  // if xyRatio > 1.1225
                    if (xyRatio <= 1.1409) {
                      return 4;
                    } else {  // if xyRatio > 1.1409
                      return 3;
                    }
                  }
                } else {  // if avgErr > 1.1345
                  if (shiftAvg <= 1.2306) {
                    if (confidence <= 0.34266) {
                      return 3;
                    } else {  // if confidence > 0.34266
                      return 4;
                    }
                  } else {  // if shiftAvg > 1.2306
                    if (shiftNcc <= 0.16867) {
                      return 3;
                    } else {  // if shiftNcc > 0.16867
                      return 4;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 28.185
        if (confidence <= 0.23608) {
          if (shiftAvg <= 0.14048) {
            if (avgErr <= 1.7668) {
              if (area <= 1532.6) {
                if (maxEdgeLength <= 29.335) {
                  if (shiftStd <= 0.00012039) {
                    return 4;
                  } else {  // if shiftStd > 0.00012039
                    if (area <= 608.34) {
                      return 2;
                    } else {  // if area > 608.34
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 29.335
                  if (shiftNcc <= -0.15927) {
                    if (shiftNcc <= -0.20568) {
                      return 4;
                    } else {  // if shiftNcc > -0.20568
                      return 1;
                    }
                  } else {  // if shiftNcc > -0.15927
                    if (area <= 1272.4) {
                      return 4;
                    } else {  // if area > 1272.4
                      return 4;
                    }
                  }
                }
              } else {  // if area > 1532.6
                if (shiftNcc <= -0.018351) {
                  return 3;
                } else {  // if shiftNcc > -0.018351
                  return 2;
                }
              }
            } else {  // if avgErr > 1.7668
              if (xyRatio <= 0.37434) {
                if (shiftStd <= 0.057183) {
                  if (confidence <= 0.20608) {
                    if (maxEdgeLength <= 28.254) {
                      return 3;
                    } else {  // if maxEdgeLength > 28.254
                      return 4;
                    }
                  } else {  // if confidence > 0.20608
                    if (xyRatio <= 0.34405) {
                      return 4;
                    } else {  // if xyRatio > 0.34405
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.057183
                  if (avgErr <= 11.218) {
                    if (shiftStd <= 0.073032) {
                      return 3;
                    } else {  // if shiftStd > 0.073032
                      return 4;
                    }
                  } else {  // if avgErr > 11.218
                    if (shiftStd <= 0.62138) {
                      return 3;
                    } else {  // if shiftStd > 0.62138
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 0.37434
                if (maxEdgeLength <= 79.269) {
                  if (xyRatio <= 0.50969) {
                    if (avgErr <= 5.3804) {
                      return 4;
                    } else {  // if avgErr > 5.3804
                      return 4;
                    }
                  } else {  // if xyRatio > 0.50969
                    if (maxEdgeLength <= 29.414) {
                      return 4;
                    } else {  // if maxEdgeLength > 29.414
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 79.269
                  if (shiftAvg <= 0.06278) {
                    return 3;
                  } else {  // if shiftAvg > 0.06278
                    return 4;
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.14048
            if (xyRatio <= 0.97376) {
              if (maxEdgeLength <= 71.229) {
                if (shiftAvg <= 0.40221) {
                  if (xyRatio <= 0.84829) {
                    if (shiftAvg <= 0.14533) {
                      return 3;
                    } else {  // if shiftAvg > 0.14533
                      return 4;
                    }
                  } else {  // if xyRatio > 0.84829
                    if (area <= 853.82) {
                      return 2;
                    } else {  // if area > 853.82
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.40221
                  if (shiftStd <= 0.13064) {
                    if (confidence <= 0.20063) {
                      return 4;
                    } else {  // if confidence > 0.20063
                      return 3;
                    }
                  } else {  // if shiftStd > 0.13064
                    if (shiftAvg <= 0.43946) {
                      return 3;
                    } else {  // if shiftAvg > 0.43946
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 71.229
                if (avgErr <= 8.4153) {
                  if (shiftNcc <= 0.90068) {
                    if (shiftStd <= 4.3404) {
                      return 3;
                    } else {  // if shiftStd > 4.3404
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.90068
                    return 4;
                  }
                } else {  // if avgErr > 8.4153
                  if (xyRatio <= 0.36711) {
                    if (avgErr <= 10.626) {
                      return 4;
                    } else {  // if avgErr > 10.626
                      return 3;
                    }
                  } else {  // if xyRatio > 0.36711
                    if (shiftStd <= 4.541) {
                      return 1;
                    } else {  // if shiftStd > 4.541
                      return 4;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.97376
              if (confidence <= 0.21867) {
                if (area <= 3177.9) {
                  if (confidence <= 0.10501) {
                    return 3;
                  } else {  // if confidence > 0.10501
                    if (shiftNcc <= 0.48393) {
                      return 4;
                    } else {  // if shiftNcc > 0.48393
                      return 4;
                    }
                  }
                } else {  // if area > 3177.9
                  if (avgErr <= 5.9535) {
                    return 4;
                  } else {  // if avgErr > 5.9535
                    return 3;
                  }
                }
              } else {  // if confidence > 0.21867
                return 3;
              }
            }
          }
        } else {  // if confidence > 0.23608
          if (shiftStd <= 0.00049953) {
            if (maxEdgeLength <= 32.755) {
              if (shiftAvg <= 0.00038983) {
                if (maxEdgeLength <= 28.337) {
                  return 3;
                } else {  // if maxEdgeLength > 28.337
                  if (avgErr <= 0.5086) {
                    return 3;
                  } else {  // if avgErr > 0.5086
                    if (shiftStd <= 0.00026279) {
                      return 2;
                    } else {  // if shiftStd > 0.00026279
                      return 2;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.00038983
                if (xyRatio <= 0.37395) {
                  return 4;
                } else {  // if xyRatio > 0.37395
                  return 3;
                }
              }
            } else {  // if maxEdgeLength > 32.755
              if (maxEdgeLength <= 49.613) {
                if (shiftAvg <= 0.00038439) {
                  return 4;
                } else {  // if shiftAvg > 0.00038439
                  if (avgErr <= 2.7471) {
                    return 2;
                  } else {  // if avgErr > 2.7471
                    return 3;
                  }
                }
              } else {  // if maxEdgeLength > 49.613
                return 2;
              }
            }
          } else {  // if shiftStd > 0.00049953
            if (xyRatio <= 0.70123) {
              if (area <= 348.64) {
                if (shiftAvg <= 0.080327) {
                  if (maxEdgeLength <= 31.529) {
                    if (xyRatio <= 0.42551) {
                      return 4;
                    } else {  // if xyRatio > 0.42551
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 31.529
                    if (avgErr <= 5.1637) {
                      return 4;
                    } else {  // if avgErr > 5.1637
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.080327
                  if (shiftAvg <= 0.71792) {
                    if (avgErr <= 12.278) {
                      return 3;
                    } else {  // if avgErr > 12.278
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.71792
                    if (avgErr <= 7.4014) {
                      return 3;
                    } else {  // if avgErr > 7.4014
                      return 3;
                    }
                  }
                }
              } else {  // if area > 348.64
                if (avgErr <= 10.51) {
                  if (confidence <= 0.29613) {
                    if (maxEdgeLength <= 37.012) {
                      return 3;
                    } else {  // if maxEdgeLength > 37.012
                      return 4;
                    }
                  } else {  // if confidence > 0.29613
                    if (avgErr <= 2.1172) {
                      return 2;
                    } else {  // if avgErr > 2.1172
                      return 3;
                    }
                  }
                } else {  // if avgErr > 10.51
                  if (avgErr <= 18.387) {
                    if (shiftAvg <= 0.5669) {
                      return 4;
                    } else {  // if shiftAvg > 0.5669
                      return 3;
                    }
                  } else {  // if avgErr > 18.387
                    if (maxEdgeLength <= 28.854) {
                      return 3;
                    } else {  // if maxEdgeLength > 28.854
                      return 4;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.70123
              if (shiftAvg <= 0.18302) {
                if (avgErr <= 1.7409) {
                  if (shiftNcc <= 0.21868) {
                    return 3;
                  } else {  // if shiftNcc > 0.21868
                    if (shiftAvg <= 0.0016665) {
                      return 3;
                    } else {  // if shiftAvg > 0.0016665
                      return 4;
                    }
                  }
                } else {  // if avgErr > 1.7409
                  if (xyRatio <= 2.3504) {
                    if (xyRatio <= 1.484) {
                      return 4;
                    } else {  // if xyRatio > 1.484
                      return 3;
                    }
                  } else {  // if xyRatio > 2.3504
                    if (confidence <= 0.24401) {
                      return 3;
                    } else {  // if confidence > 0.24401
                      return 4;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.18302
                if (shiftAvg <= 24.622) {
                  if (shiftStd <= 8.6008) {
                    if (avgErr <= 5.4557) {
                      return 2;
                    } else {  // if avgErr > 5.4557
                      return 3;
                    }
                  } else {  // if shiftStd > 8.6008
                    return 0;
                  }
                } else {  // if shiftAvg > 24.622
                  if (area <= 4798.7) {
                    return 4;
                  } else {  // if area > 4798.7
                    if (avgErr <= 10.48) {
                      return 4;
                    } else {  // if avgErr > 10.48
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if confidence > 0.41297
      if (confidence <= 0.66332) {
        if (area <= 366.69) {
          if (confidence <= 0.53972) {
            if (avgErr <= 1.2998) {
              if (maxEdgeLength <= 19.743) {
                if (confidence <= 0.49465) {
                  return 4;
                } else {  // if confidence > 0.49465
                  return 3;
                }
              } else {  // if maxEdgeLength > 19.743
                if (confidence <= 0.53711) {
                  if (maxEdgeLength <= 22.342) {
                    if (confidence <= 0.41608) {
                      return 4;
                    } else {  // if confidence > 0.41608
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 22.342
                    if (maxEdgeLength <= 22.399) {
                      return 4;
                    } else {  // if maxEdgeLength > 22.399
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.53711
                  if (confidence <= 0.53829) {
                    return 4;
                  } else {  // if confidence > 0.53829
                    return 3;
                  }
                }
              }
            } else {  // if avgErr > 1.2998
              if (maxEdgeLength <= 30.023) {
                if (maxEdgeLength <= 19.852) {
                  if (xyRatio <= 0.5824) {
                    if (area <= 214.65) {
                      return 4;
                    } else {  // if area > 214.65
                      return 3;
                    }
                  } else {  // if xyRatio > 0.5824
                    if (shiftAvg <= 0.15089) {
                      return 3;
                    } else {  // if shiftAvg > 0.15089
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 19.852
                  if (xyRatio <= 0.32405) {
                    if (avgErr <= 9.9759) {
                      return 4;
                    } else {  // if avgErr > 9.9759
                      return 4;
                    }
                  } else {  // if xyRatio > 0.32405
                    if (confidence <= 0.49812) {
                      return 3;
                    } else {  // if confidence > 0.49812
                      return 3;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 30.023
                if (shiftAvg <= 0.25117) {
                  if (shiftNcc <= 0.99535) {
                    if (area <= 366.16) {
                      return 3;
                    } else {  // if area > 366.16
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.99535
                    return 4;
                  }
                } else {  // if shiftAvg > 0.25117
                  if (avgErr <= 14.855) {
                    if (maxEdgeLength <= 36.88) {
                      return 3;
                    } else {  // if maxEdgeLength > 36.88
                      return 2;
                    }
                  } else {  // if avgErr > 14.855
                    if (maxEdgeLength <= 32.889) {
                      return 4;
                    } else {  // if maxEdgeLength > 32.889
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.53972
            if (shiftAvg <= 0.29208) {
              if (area <= 126.84) {
                if (confidence <= 0.64564) {
                  if (maxEdgeLength <= 12.107) {
                    return 3;
                  } else {  // if maxEdgeLength > 12.107
                    if (xyRatio <= 0.49238) {
                      return 4;
                    } else {  // if xyRatio > 0.49238
                      return 4;
                    }
                  }
                } else {  // if confidence > 0.64564
                  return 3;
                }
              } else {  // if area > 126.84
                if (area <= 324.13) {
                  if (maxEdgeLength <= 23.08) {
                    if (confidence <= 0.59134) {
                      return 3;
                    } else {  // if confidence > 0.59134
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 23.08
                    if (avgErr <= 3.731) {
                      return 4;
                    } else {  // if avgErr > 3.731
                      return 3;
                    }
                  }
                } else {  // if area > 324.13
                  if (shiftStd <= 0.037204) {
                    if (avgErr <= 5.1781) {
                      return 3;
                    } else {  // if avgErr > 5.1781
                      return 3;
                    }
                  } else {  // if shiftStd > 0.037204
                    if (confidence <= 0.55498) {
                      return 3;
                    } else {  // if confidence > 0.55498
                      return 3;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.29208
              if (xyRatio <= 0.71016) {
                if (avgErr <= 7.5262) {
                  if (avgErr <= 6.6116) {
                    if (confidence <= 0.58157) {
                      return 3;
                    } else {  // if confidence > 0.58157
                      return 3;
                    }
                  } else {  // if avgErr > 6.6116
                    if (shiftNcc <= 0.9945) {
                      return 2;
                    } else {  // if shiftNcc > 0.9945
                      return 4;
                    }
                  }
                } else {  // if avgErr > 7.5262
                  if (shiftAvg <= 0.99524) {
                    if (maxEdgeLength <= 19.513) {
                      return 3;
                    } else {  // if maxEdgeLength > 19.513
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.99524
                    if (area <= 329.37) {
                      return 2;
                    } else {  // if area > 329.37
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.71016
                if (avgErr <= 10.784) {
                  if (confidence <= 0.54278) {
                    return 3;
                  } else {  // if confidence > 0.54278
                    if (area <= 173.77) {
                      return 3;
                    } else {  // if area > 173.77
                      return 4;
                    }
                  }
                } else {  // if avgErr > 10.784
                  if (shiftAvg <= 0.66331) {
                    return 2;
                  } else {  // if shiftAvg > 0.66331
                    if (area <= 322.11) {
                      return 4;
                    } else {  // if area > 322.11
                      return 3;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 366.69
          if (shiftAvg <= 0.68196) {
            if (avgErr <= 2.3391) {
              if (xyRatio <= 0.56828) {
                if (area <= 428.53) {
                  if (confidence <= 0.62988) {
                    return 4;
                  } else {  // if confidence > 0.62988
                    return 2;
                  }
                } else {  // if area > 428.53
                  if (avgErr <= 2.3254) {
                    if (confidence <= 0.6347) {
                      return 3;
                    } else {  // if confidence > 0.6347
                      return 2;
                    }
                  } else {  // if avgErr > 2.3254
                    return 2;
                  }
                }
              } else {  // if xyRatio > 0.56828
                if (xyRatio <= 0.91865) {
                  if (confidence <= 0.58661) {
                    if (shiftAvg <= 7.643e-06) {
                      return 2;
                    } else {  // if shiftAvg > 7.643e-06
                      return 2;
                    }
                  } else {  // if confidence > 0.58661
                    if (confidence <= 0.60497) {
                      return 3;
                    } else {  // if confidence > 0.60497
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.91865
                  if (shiftStd <= 0.00037607) {
                    return 3;
                  } else {  // if shiftStd > 0.00037607
                    return 4;
                  }
                }
              }
            } else {  // if avgErr > 2.3391
              if (confidence <= 0.57041) {
                if (avgErr <= 9.3375) {
                  if (area <= 414.27) {
                    if (xyRatio <= 0.34543) {
                      return 3;
                    } else {  // if xyRatio > 0.34543
                      return 3;
                    }
                  } else {  // if area > 414.27
                    if (xyRatio <= 0.64522) {
                      return 3;
                    } else {  // if xyRatio > 0.64522
                      return 3;
                    }
                  }
                } else {  // if avgErr > 9.3375
                  if (xyRatio <= 0.58783) {
                    if (maxEdgeLength <= 33.391) {
                      return 3;
                    } else {  // if maxEdgeLength > 33.391
                      return 3;
                    }
                  } else {  // if xyRatio > 0.58783
                    if (confidence <= 0.54517) {
                      return 4;
                    } else {  // if confidence > 0.54517
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.57041
                if (shiftAvg <= 0.31428) {
                  if (area <= 419.64) {
                    if (confidence <= 0.6589) {
                      return 3;
                    } else {  // if confidence > 0.6589
                      return 3;
                    }
                  } else {  // if area > 419.64
                    if (avgErr <= 5.4574) {
                      return 3;
                    } else {  // if avgErr > 5.4574
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.31428
                  if (avgErr <= 4.764) {
                    if (maxEdgeLength <= 32.723) {
                      return 2;
                    } else {  // if maxEdgeLength > 32.723
                      return 3;
                    }
                  } else {  // if avgErr > 4.764
                    if (confidence <= 0.57707) {
                      return 2;
                    } else {  // if confidence > 0.57707
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.68196
            if (avgErr <= 9.8992) {
              if (avgErr <= 4.0982) {
                if (shiftNcc <= 0.9951) {
                  if (shiftStd <= 0.3472) {
                    if (shiftStd <= 0.094302) {
                      return 3;
                    } else {  // if shiftStd > 0.094302
                      return 3;
                    }
                  } else {  // if shiftStd > 0.3472
                    if (area <= 1799) {
                      return 4;
                    } else {  // if area > 1799
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.9951
                  return 4;
                }
              } else {  // if avgErr > 4.0982
                if (xyRatio <= 0.72594) {
                  if (area <= 615.34) {
                    if (xyRatio <= 0.43023) {
                      return 2;
                    } else {  // if xyRatio > 0.43023
                      return 2;
                    }
                  } else {  // if area > 615.34
                    if (maxEdgeLength <= 40.725) {
                      return 1;
                    } else {  // if maxEdgeLength > 40.725
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.72594
                  if (area <= 2777.7) {
                    if (area <= 685.3) {
                      return 4;
                    } else {  // if area > 685.3
                      return 3;
                    }
                  } else {  // if area > 2777.7
                    if (shiftNcc <= 0.81851) {
                      return 4;
                    } else {  // if shiftNcc > 0.81851
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 9.8992
              if (xyRatio <= 1.1611) {
                if (area <= 7510.4) {
                  if (shiftNcc <= 0.82182) {
                    if (avgErr <= 10.078) {
                      return 1;
                    } else {  // if avgErr > 10.078
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.82182
                    if (confidence <= 0.53355) {
                      return 3;
                    } else {  // if confidence > 0.53355
                      return 3;
                    }
                  }
                } else {  // if area > 7510.4
                  if (shiftAvg <= 1.7963) {
                    return 0;
                  } else {  // if shiftAvg > 1.7963
                    if (confidence <= 0.6438) {
                      return 1;
                    } else {  // if confidence > 0.6438
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 1.1611
                return 1;
              }
            }
          }
        }
      } else {  // if confidence > 0.66332
        if (maxEdgeLength <= 46.458) {
          if (shiftAvg <= 0.13802) {
            if (maxEdgeLength <= 36.59) {
              if (maxEdgeLength <= 34.703) {
                if (maxEdgeLength <= 24.068) {
                  if (confidence <= 0.71012) {
                    if (xyRatio <= 0.52976) {
                      return 3;
                    } else {  // if xyRatio > 0.52976
                      return 3;
                    }
                  } else {  // if confidence > 0.71012
                    if (confidence <= 0.77628) {
                      return 3;
                    } else {  // if confidence > 0.77628
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 24.068
                  if (area <= 376.16) {
                    if (xyRatio <= 0.36398) {
                      return 3;
                    } else {  // if xyRatio > 0.36398
                      return 3;
                    }
                  } else {  // if area > 376.16
                    if (shiftAvg <= 0.036251) {
                      return 3;
                    } else {  // if shiftAvg > 0.036251
                      return 3;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 34.703
                if (maxEdgeLength <= 35.816) {
                  if (avgErr <= 12.248) {
                    if (confidence <= 0.79533) {
                      return 3;
                    } else {  // if confidence > 0.79533
                      return 2;
                    }
                  } else {  // if avgErr > 12.248
                    if (area <= 555.56) {
                      return 2;
                    } else {  // if area > 555.56
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 35.816
                  if (shiftAvg <= 0.10584) {
                    if (shiftNcc <= 0.97973) {
                      return 3;
                    } else {  // if shiftNcc > 0.97973
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.10584
                    if (avgErr <= 8.9923) {
                      return 2;
                    } else {  // if avgErr > 8.9923
                      return 3;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 36.59
              if (avgErr <= 5.334) {
                if (xyRatio <= 0.3792) {
                  if (confidence <= 0.80042) {
                    if (avgErr <= 3.2663) {
                      return 3;
                    } else {  // if avgErr > 3.2663
                      return 3;
                    }
                  } else {  // if confidence > 0.80042
                    if (shiftNcc <= -0.059212) {
                      return 4;
                    } else {  // if shiftNcc > -0.059212
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.3792
                  if (shiftAvg <= 0.023723) {
                    if (area <= 785.92) {
                      return 3;
                    } else {  // if area > 785.92
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.023723
                    if (maxEdgeLength <= 36.725) {
                      return 2;
                    } else {  // if maxEdgeLength > 36.725
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 5.334
                if (area <= 713) {
                  if (area <= 478.3) {
                    if (xyRatio <= 0.32856) {
                      return 3;
                    } else {  // if xyRatio > 0.32856
                      return 3;
                    }
                  } else {  // if area > 478.3
                    if (avgErr <= 6.2766) {
                      return 3;
                    } else {  // if avgErr > 6.2766
                      return 3;
                    }
                  }
                } else {  // if area > 713
                  if (shiftNcc <= 0.86898) {
                    if (xyRatio <= 0.53373) {
                      return 2;
                    } else {  // if xyRatio > 0.53373
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.86898
                    if (xyRatio <= 0.42147) {
                      return 3;
                    } else {  // if xyRatio > 0.42147
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.13802
            if (confidence <= 0.75754) {
              if (shiftAvg <= 0.8348) {
                if (area <= 409.86) {
                  if (avgErr <= 9.4555) {
                    if (maxEdgeLength <= 24.452) {
                      return 3;
                    } else {  // if maxEdgeLength > 24.452
                      return 3;
                    }
                  } else {  // if avgErr > 9.4555
                    if (confidence <= 0.71027) {
                      return 3;
                    } else {  // if confidence > 0.71027
                      return 3;
                    }
                  }
                } else {  // if area > 409.86
                  if (avgErr <= 12.068) {
                    if (shiftAvg <= 0.42074) {
                      return 3;
                    } else {  // if shiftAvg > 0.42074
                      return 3;
                    }
                  } else {  // if avgErr > 12.068
                    if (shiftStd <= 0.052044) {
                      return 2;
                    } else {  // if shiftStd > 0.052044
                      return 3;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.8348
                if (avgErr <= 10.252) {
                  if (xyRatio <= 0.44091) {
                    if (area <= 291.33) {
                      return 3;
                    } else {  // if area > 291.33
                      return 2;
                    }
                  } else {  // if xyRatio > 0.44091
                    if (avgErr <= 7.7181) {
                      return 1;
                    } else {  // if avgErr > 7.7181
                      return 2;
                    }
                  }
                } else {  // if avgErr > 10.252
                  if (shiftStd <= 0.03571) {
                    if (area <= 917.28) {
                      return 3;
                    } else {  // if area > 917.28
                      return 2;
                    }
                  } else {  // if shiftStd > 0.03571
                    if (xyRatio <= 0.50895) {
                      return 3;
                    } else {  // if xyRatio > 0.50895
                      return 2;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.75754
              if (avgErr <= 11.736) {
                if (maxEdgeLength <= 35.201) {
                  if (maxEdgeLength <= 27.273) {
                    if (shiftAvg <= 0.25669) {
                      return 3;
                    } else {  // if shiftAvg > 0.25669
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 27.273
                    if (xyRatio <= 0.53984) {
                      return 3;
                    } else {  // if xyRatio > 0.53984
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 35.201
                  if (avgErr <= 5.6352) {
                    if (shiftNcc <= 0.97757) {
                      return 3;
                    } else {  // if shiftNcc > 0.97757
                      return 2;
                    }
                  } else {  // if avgErr > 5.6352
                    if (avgErr <= 8.6019) {
                      return 2;
                    } else {  // if avgErr > 8.6019
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 11.736
                if (maxEdgeLength <= 37.892) {
                  if (maxEdgeLength <= 31.007) {
                    if (maxEdgeLength <= 24.796) {
                      return 2;
                    } else {  // if maxEdgeLength > 24.796
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 31.007
                    if (confidence <= 0.76171) {
                      return 3;
                    } else {  // if confidence > 0.76171
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 37.892
                  if (area <= 560.17) {
                    if (shiftNcc <= 0.95418) {
                      return 3;
                    } else {  // if shiftNcc > 0.95418
                      return 2;
                    }
                  } else {  // if area > 560.17
                    if (confidence <= 0.80184) {
                      return 2;
                    } else {  // if confidence > 0.80184
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 46.458
          if (area <= 1721) {
            if (avgErr <= 5.6256) {
              if (area <= 1154.1) {
                if (xyRatio <= 0.33697) {
                  if (shiftAvg <= 0.37828) {
                    if (shiftStd <= 0.017318) {
                      return 2;
                    } else {  // if shiftStd > 0.017318
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.37828
                    return 3;
                  }
                } else {  // if xyRatio > 0.33697
                  if (xyRatio <= 0.49266) {
                    if (area <= 788.17) {
                      return 1;
                    } else {  // if area > 788.17
                      return 3;
                    }
                  } else {  // if xyRatio > 0.49266
                    if (maxEdgeLength <= 47.41) {
                      return 4;
                    } else {  // if maxEdgeLength > 47.41
                      return 3;
                    }
                  }
                }
              } else {  // if area > 1154.1
                if (area <= 1695.3) {
                  if (avgErr <= 4.2438) {
                    if (avgErr <= 3.2606) {
                      return 2;
                    } else {  // if avgErr > 3.2606
                      return 3;
                    }
                  } else {  // if avgErr > 4.2438
                    if (confidence <= 0.68685) {
                      return 3;
                    } else {  // if confidence > 0.68685
                      return 2;
                    }
                  }
                } else {  // if area > 1695.3
                  return 3;
                }
              }
            } else {  // if avgErr > 5.6256
              if (xyRatio <= 0.34676) {
                if (shiftNcc <= 0.7084) {
                  if (confidence <= 0.78326) {
                    if (shiftNcc <= 0.0838) {
                      return 3;
                    } else {  // if shiftNcc > 0.0838
                      return 3;
                    }
                  } else {  // if confidence > 0.78326
                    if (avgErr <= 9.6829) {
                      return 2;
                    } else {  // if avgErr > 9.6829
                      return 3;
                    }
                  }
                } else {  // if shiftNcc > 0.7084
                  if (shiftAvg <= 0.51388) {
                    if (avgErr <= 12.21) {
                      return 2;
                    } else {  // if avgErr > 12.21
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.51388
                    if (shiftStd <= 0.91887) {
                      return 2;
                    } else {  // if shiftStd > 0.91887
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.34676
                if (shiftNcc <= 0.46674) {
                  if (shiftStd <= 0.04989) {
                    if (maxEdgeLength <= 51.781) {
                      return 2;
                    } else {  // if maxEdgeLength > 51.781
                      return 1;
                    }
                  } else {  // if shiftStd > 0.04989
                    if (maxEdgeLength <= 64.989) {
                      return 3;
                    } else {  // if maxEdgeLength > 64.989
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.46674
                  if (maxEdgeLength <= 50.101) {
                    if (avgErr <= 10.119) {
                      return 2;
                    } else {  // if avgErr > 10.119
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 50.101
                    if (shiftNcc <= 0.9206) {
                      return 2;
                    } else {  // if shiftNcc > 0.9206
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 1721
            if (avgErr <= 7.9367) {
              if (avgErr <= 4.5375) {
                if (shiftAvg <= 0.90906) {
                  if (maxEdgeLength <= 58.663) {
                    if (area <= 2083.3) {
                      return 1;
                    } else {  // if area > 2083.3
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 58.663
                    if (maxEdgeLength <= 62.665) {
                      return 2;
                    } else {  // if maxEdgeLength > 62.665
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 0.90906
                  return 3;
                }
              } else {  // if avgErr > 4.5375
                if (area <= 4960.7) {
                  if (area <= 1846.9) {
                    if (area <= 1816.8) {
                      return 1;
                    } else {  // if area > 1816.8
                      return 2;
                    }
                  } else {  // if area > 1846.9
                    if (avgErr <= 7.3059) {
                      return 1;
                    } else {  // if avgErr > 7.3059
                      return 1;
                    }
                  }
                } else {  // if area > 4960.7
                  if (shiftAvg <= 6.0936) {
                    return 3;
                  } else {  // if shiftAvg > 6.0936
                    if (avgErr <= 7.5247) {
                      return 0;
                    } else {  // if avgErr > 7.5247
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 7.9367
              if (area <= 5678.9) {
                if (area <= 2024) {
                  if (shiftStd <= 0.68832) {
                    if (shiftAvg <= 0.86488) {
                      return 1;
                    } else {  // if shiftAvg > 0.86488
                      return 1;
                    }
                  } else {  // if shiftStd > 0.68832
                    if (confidence <= 0.80619) {
                      return 3;
                    } else {  // if confidence > 0.80619
                      return 2;
                    }
                  }
                } else {  // if area > 2024
                  if (shiftAvg <= 0.16175) {
                    if (xyRatio <= 0.57664) {
                      return 1;
                    } else {  // if xyRatio > 0.57664
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.16175
                    if (shiftAvg <= 1.8726) {
                      return 2;
                    } else {  // if shiftAvg > 1.8726
                      return 2;
                    }
                  }
                }
              } else {  // if area > 5678.9
                if (xyRatio <= 0.97879) {
                  if (shiftAvg <= 3.3596) {
                    return 1;
                  } else {  // if shiftAvg > 3.3596
                    if (avgErr <= 10.012) {
                      return 2;
                    } else {  // if avgErr > 10.012
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.97879
                  return 0;
                }
              }
            }
          }
        }
      }
    }
  } else {  // if confidence > 0.81202
    if (maxEdgeLength <= 83.26) {
      if (confidence <= 0.87535) {
        if (area <= 1043.8) {
          if (area <= 592.17) {
            if (avgErr <= 12.333) {
              if (xyRatio <= 0.37506) {
                if (confidence <= 0.84261) {
                  if (confidence <= 0.84094) {
                    if (shiftStd <= 0.017322) {
                      return 3;
                    } else {  // if shiftStd > 0.017322
                      return 2;
                    }
                  } else {  // if confidence > 0.84094
                    return 3;
                  }
                } else {  // if confidence > 0.84261
                  if (maxEdgeLength <= 30.236) {
                    if (area <= 285.12) {
                      return 2;
                    } else {  // if area > 285.12
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 30.236
                    return 2;
                  }
                }
              } else {  // if xyRatio > 0.37506
                if (shiftNcc <= 0.86559) {
                  if (xyRatio <= 0.46187) {
                    if (area <= 392.82) {
                      return 3;
                    } else {  // if area > 392.82
                      return 3;
                    }
                  } else {  // if xyRatio > 0.46187
                    if (shiftNcc <= 0.75299) {
                      return 2;
                    } else {  // if shiftNcc > 0.75299
                      return 3;
                    }
                  }
                } else {  // if shiftNcc > 0.86559
                  if (confidence <= 0.8312) {
                    if (shiftNcc <= 0.97014) {
                      return 3;
                    } else {  // if shiftNcc > 0.97014
                      return 2;
                    }
                  } else {  // if confidence > 0.8312
                    if (shiftNcc <= 0.95598) {
                      return 2;
                    } else {  // if shiftNcc > 0.95598
                      return 3;
                    }
                  }
                }
              }
            } else {  // if avgErr > 12.333
              if (maxEdgeLength <= 31.048) {
                if (xyRatio <= 0.28013) {
                  return 2;
                } else {  // if xyRatio > 0.28013
                  if (avgErr <= 13.14) {
                    return 2;
                  } else {  // if avgErr > 13.14
                    return 3;
                  }
                }
              } else {  // if maxEdgeLength > 31.048
                if (shiftAvg <= 0.33306) {
                  if (xyRatio <= 0.44273) {
                    if (xyRatio <= 0.38505) {
                      return 2;
                    } else {  // if xyRatio > 0.38505
                      return 2;
                    }
                  } else {  // if xyRatio > 0.44273
                    if (shiftStd <= 0.079946) {
                      return 3;
                    } else {  // if shiftStd > 0.079946
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.33306
                  if (area <= 585.39) {
                    if (xyRatio <= 0.49777) {
                      return 2;
                    } else {  // if xyRatio > 0.49777
                      return 2;
                    }
                  } else {  // if area > 585.39
                    return 1;
                  }
                }
              }
            }
          } else {  // if area > 592.17
            if (avgErr <= 5.1994) {
              if (shiftAvg <= 0.039715) {
                if (avgErr <= 4.2781) {
                  if (xyRatio <= 0.34628) {
                    return 2;
                  } else {  // if xyRatio > 0.34628
                    if (avgErr <= 4.1074) {
                      return 3;
                    } else {  // if avgErr > 4.1074
                      return 2;
                    }
                  }
                } else {  // if avgErr > 4.2781
                  if (shiftAvg <= 0.0053163) {
                    if (shiftNcc <= -0.048344) {
                      return 1;
                    } else {  // if shiftNcc > -0.048344
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.0053163
                    if (maxEdgeLength <= 49.307) {
                      return 1;
                    } else {  // if maxEdgeLength > 49.307
                      return 2;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.039715
                if (maxEdgeLength <= 47.618) {
                  if (confidence <= 0.86338) {
                    if (xyRatio <= 0.51115) {
                      return 3;
                    } else {  // if xyRatio > 0.51115
                      return 2;
                    }
                  } else {  // if confidence > 0.86338
                    if (shiftStd <= 0.080495) {
                      return 1;
                    } else {  // if shiftStd > 0.080495
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 47.618
                  if (maxEdgeLength <= 48.14) {
                    return 2;
                  } else {  // if maxEdgeLength > 48.14
                    if (xyRatio <= 0.43312) {
                      return 3;
                    } else {  // if xyRatio > 0.43312
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgErr > 5.1994
              if (avgErr <= 7.499) {
                if (xyRatio <= 0.34882) {
                  if (area <= 697.92) {
                    if (xyRatio <= 0.33693) {
                      return 3;
                    } else {  // if xyRatio > 0.33693
                      return 1;
                    }
                  } else {  // if area > 697.92
                    if (confidence <= 0.87058) {
                      return 2;
                    } else {  // if confidence > 0.87058
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.34882
                  if (xyRatio <= 0.41162) {
                    if (maxEdgeLength <= 43.887) {
                      return 1;
                    } else {  // if maxEdgeLength > 43.887
                      return 1;
                    }
                  } else {  // if xyRatio > 0.41162
                    if (confidence <= 0.8536) {
                      return 2;
                    } else {  // if confidence > 0.8536
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 7.499
                if (avgErr <= 10.845) {
                  if (xyRatio <= 0.38902) {
                    if (maxEdgeLength <= 47.584) {
                      return 1;
                    } else {  // if maxEdgeLength > 47.584
                      return 2;
                    }
                  } else {  // if xyRatio > 0.38902
                    if (area <= 760.36) {
                      return 2;
                    } else {  // if area > 760.36
                      return 2;
                    }
                  }
                } else {  // if avgErr > 10.845
                  if (shiftAvg <= 1.0289) {
                    if (confidence <= 0.82004) {
                      return 3;
                    } else {  // if confidence > 0.82004
                      return 2;
                    }
                  } else {  // if shiftAvg > 1.0289
                    if (avgErr <= 13.528) {
                      return 1;
                    } else {  // if avgErr > 13.528
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 1043.8
          if (area <= 1537) {
            if (shiftAvg <= 1.2651) {
              if (avgErr <= 4.6678) {
                if (shiftAvg <= 0.015227) {
                  return 1;
                } else {  // if shiftAvg > 0.015227
                  return 3;
                }
              } else {  // if avgErr > 4.6678
                if (maxEdgeLength <= 66.076) {
                  if (confidence <= 0.82857) {
                    if (maxEdgeLength <= 41.77) {
                      return 3;
                    } else {  // if maxEdgeLength > 41.77
                      return 2;
                    }
                  } else {  // if confidence > 0.82857
                    if (avgErr <= 7.3364) {
                      return 1;
                    } else {  // if avgErr > 7.3364
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 66.076
                  if (shiftStd <= 0.26349) {
                    if (maxEdgeLength <= 68.424) {
                      return 3;
                    } else {  // if maxEdgeLength > 68.424
                      return 2;
                    }
                  } else {  // if shiftStd > 0.26349
                    return 3;
                  }
                }
              }
            } else {  // if shiftAvg > 1.2651
              if (xyRatio <= 0.66178) {
                if (shiftNcc <= 0.67878) {
                  if (area <= 1216.9) {
                    if (maxEdgeLength <= 55.543) {
                      return 1;
                    } else {  // if maxEdgeLength > 55.543
                      return 2;
                    }
                  } else {  // if area > 1216.9
                    if (xyRatio <= 0.49401) {
                      return 2;
                    } else {  // if xyRatio > 0.49401
                      return 2;
                    }
                  }
                } else {  // if shiftNcc > 0.67878
                  if (avgErr <= 8.5125) {
                    if (area <= 1479.7) {
                      return 1;
                    } else {  // if area > 1479.7
                      return 2;
                    }
                  } else {  // if avgErr > 8.5125
                    if (shiftAvg <= 1.7807) {
                      return 1;
                    } else {  // if shiftAvg > 1.7807
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.66178
                if (shiftAvg <= 1.5933) {
                  if (avgErr <= 10.635) {
                    if (shiftStd <= 0.069196) {
                      return 3;
                    } else {  // if shiftStd > 0.069196
                      return 1;
                    }
                  } else {  // if avgErr > 10.635
                    return 3;
                  }
                } else {  // if shiftAvg > 1.5933
                  if (shiftAvg <= 1.7448) {
                    return 2;
                  } else {  // if shiftAvg > 1.7448
                    if (confidence <= 0.83926) {
                      return 2;
                    } else {  // if confidence > 0.83926
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 1537
            if (area <= 2548.2) {
              if (maxEdgeLength <= 74.449) {
                if (xyRatio <= 0.43974) {
                  if (shiftNcc <= 0.73391) {
                    if (area <= 2102.8) {
                      return 1;
                    } else {  // if area > 2102.8
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.73391
                    if (shiftAvg <= 1.1133) {
                      return 1;
                    } else {  // if shiftAvg > 1.1133
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.43974
                  if (maxEdgeLength <= 49.118) {
                    if (shiftAvg <= 2.1644) {
                      return 3;
                    } else {  // if shiftAvg > 2.1644
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 49.118
                    if (shiftStd <= 0.034779) {
                      return 2;
                    } else {  // if shiftStd > 0.034779
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 74.449
                if (maxEdgeLength <= 80.967) {
                  if (avgErr <= 9.1457) {
                    if (area <= 2000.2) {
                      return 3;
                    } else {  // if area > 2000.2
                      return 1;
                    }
                  } else {  // if avgErr > 9.1457
                    if (shiftStd <= 0.29324) {
                      return 2;
                    } else {  // if shiftStd > 0.29324
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 80.967
                  if (shiftStd <= 0.078317) {
                    return 1;
                  } else {  // if shiftStd > 0.078317
                    return 2;
                  }
                }
              }
            } else {  // if area > 2548.2
              if (avgErr <= 6.7209) {
                if (shiftStd <= 3.0342) {
                  if (shiftNcc <= 0.58243) {
                    if (xyRatio <= 0.56227) {
                      return 1;
                    } else {  // if xyRatio > 0.56227
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.58243
                    if (shiftAvg <= 0.23484) {
                      return 1;
                    } else {  // if shiftAvg > 0.23484
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 3.0342
                  return 2;
                }
              } else {  // if avgErr > 6.7209
                if (xyRatio <= 0.55491) {
                  if (shiftAvg <= 0.074834) {
                    if (shiftStd <= 0.83104) {
                      return 0;
                    } else {  // if shiftStd > 0.83104
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.074834
                    if (xyRatio <= 0.43495) {
                      return 1;
                    } else {  // if xyRatio > 0.43495
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.55491
                  if (confidence <= 0.86103) {
                    if (xyRatio <= 1.5524) {
                      return 1;
                    } else {  // if xyRatio > 1.5524
                      return 0;
                    }
                  } else {  // if confidence > 0.86103
                    if (avgErr <= 6.8764) {
                      return 2;
                    } else {  // if avgErr > 6.8764
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if confidence > 0.87535
        if (maxEdgeLength <= 48.612) {
          if (shiftAvg <= 0.21506) {
            if (maxEdgeLength <= 44.323) {
              if (area <= 856.53) {
                if (maxEdgeLength <= 42.689) {
                  if (area <= 540.78) {
                    return 3;
                  } else {  // if area > 540.78
                    return 1;
                  }
                } else {  // if maxEdgeLength > 42.689
                  if (xyRatio <= 0.36763) {
                    if (maxEdgeLength <= 44.246) {
                      return 1;
                    } else {  // if maxEdgeLength > 44.246
                      return 3;
                    }
                  } else {  // if xyRatio > 0.36763
                    if (confidence <= 0.87763) {
                      return 1;
                    } else {  // if confidence > 0.87763
                      return 2;
                    }
                  }
                }
              } else {  // if area > 856.53
                if (avgErr <= 10.471) {
                  if (confidence <= 0.87617) {
                    if (area <= 1070.5) {
                      return 1;
                    } else {  // if area > 1070.5
                      return 2;
                    }
                  } else {  // if confidence > 0.87617
                    if (avgErr <= 8.8933) {
                      return 2;
                    } else {  // if avgErr > 8.8933
                      return 2;
                    }
                  }
                } else {  // if avgErr > 10.471
                  return 1;
                }
              }
            } else {  // if maxEdgeLength > 44.323
              if (shiftStd <= 0.074155) {
                if (area <= 1112.9) {
                  if (shiftStd <= 0.0054047) {
                    if (area <= 773.5) {
                      return 1;
                    } else {  // if area > 773.5
                      return 2;
                    }
                  } else {  // if shiftStd > 0.0054047
                    if (shiftNcc <= 0.63857) {
                      return 1;
                    } else {  // if shiftNcc > 0.63857
                      return 1;
                    }
                  }
                } else {  // if area > 1112.9
                  if (shiftNcc <= 0.73534) {
                    return 1;
                  } else {  // if shiftNcc > 0.73534
                    if (shiftStd <= 0.013617) {
                      return 2;
                    } else {  // if shiftStd > 0.013617
                      return 3;
                    }
                  }
                }
              } else {  // if shiftStd > 0.074155
                if (shiftAvg <= 0.087898) {
                  if (shiftStd <= 0.083551) {
                    return 2;
                  } else {  // if shiftStd > 0.083551
                    return 1;
                  }
                } else {  // if shiftAvg > 0.087898
                  if (confidence <= 0.88018) {
                    if (avgErr <= 6.1597) {
                      return 3;
                    } else {  // if avgErr > 6.1597
                      return 1;
                    }
                  } else {  // if confidence > 0.88018
                    if (shiftStd <= 0.12229) {
                      return 2;
                    } else {  // if shiftStd > 0.12229
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.21506
            if (shiftAvg <= 1.446) {
              if (avgErr <= 8.0285) {
                if (area <= 1036.5) {
                  return 1;
                } else {  // if area > 1036.5
                  if (area <= 1087.1) {
                    if (confidence <= 0.87667) {
                      return 1;
                    } else {  // if confidence > 0.87667
                      return 2;
                    }
                  } else {  // if area > 1087.1
                    if (maxEdgeLength <= 46.281) {
                      return 1;
                    } else {  // if maxEdgeLength > 46.281
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 8.0285
                if (maxEdgeLength <= 47.12) {
                  if (area <= 776.5) {
                    if (shiftAvg <= 0.65993) {
                      return 1;
                    } else {  // if shiftAvg > 0.65993
                      return 2;
                    }
                  } else {  // if area > 776.5
                    if (shiftAvg <= 0.82888) {
                      return 2;
                    } else {  // if shiftAvg > 0.82888
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 47.12
                  if (avgErr <= 11.062) {
                    if (xyRatio <= 0.44298) {
                      return 1;
                    } else {  // if xyRatio > 0.44298
                      return 2;
                    }
                  } else {  // if avgErr > 11.062
                    if (maxEdgeLength <= 47.406) {
                      return 2;
                    } else {  // if maxEdgeLength > 47.406
                      return 3;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 1.446
              if (confidence <= 0.92837) {
                if (maxEdgeLength <= 38.384) {
                  return 3;
                } else {  // if maxEdgeLength > 38.384
                  if (maxEdgeLength <= 45.801) {
                    if (shiftStd <= 1.5456) {
                      return 1;
                    } else {  // if shiftStd > 1.5456
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 45.801
                    if (shiftStd <= 0.74261) {
                      return 2;
                    } else {  // if shiftStd > 0.74261
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.92837
                return 2;
              }
            }
          }
        } else {  // if maxEdgeLength > 48.612
          if (confidence <= 0.89863) {
            if (avgErr <= 10.713) {
              if (xyRatio <= 0.29775) {
                if (shiftStd <= 0.032627) {
                  return 1;
                } else {  // if shiftStd > 0.032627
                  if (shiftStd <= 0.24116) {
                    if (area <= 2009.2) {
                      return 2;
                    } else {  // if area > 2009.2
                      return 1;
                    }
                  } else {  // if shiftStd > 0.24116
                    if (confidence <= 0.89057) {
                      return 3;
                    } else {  // if confidence > 0.89057
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.29775
                if (area <= 3027.3) {
                  if (xyRatio <= 0.48603) {
                    if (maxEdgeLength <= 61.665) {
                      return 1;
                    } else {  // if maxEdgeLength > 61.665
                      return 1;
                    }
                  } else {  // if xyRatio > 0.48603
                    if (avgErr <= 8.1877) {
                      return 1;
                    } else {  // if avgErr > 8.1877
                      return 1;
                    }
                  }
                } else {  // if area > 3027.3
                  if (maxEdgeLength <= 82.728) {
                    if (xyRatio <= 1.006) {
                      return 1;
                    } else {  // if xyRatio > 1.006
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 82.728
                    if (shiftNcc <= 0.85162) {
                      return 1;
                    } else {  // if shiftNcc > 0.85162
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgErr > 10.713
              if (area <= 2104.1) {
                if (maxEdgeLength <= 67.656) {
                  if (area <= 1938.6) {
                    if (shiftNcc <= 0.93742) {
                      return 1;
                    } else {  // if shiftNcc > 0.93742
                      return 2;
                    }
                  } else {  // if area > 1938.6
                    if (shiftStd <= 0.91757) {
                      return 1;
                    } else {  // if shiftStd > 0.91757
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 67.656
                  if (confidence <= 0.8959) {
                    if (area <= 1693) {
                      return 1;
                    } else {  // if area > 1693
                      return 2;
                    }
                  } else {  // if confidence > 0.8959
                    if (area <= 1874.9) {
                      return 2;
                    } else {  // if area > 1874.9
                      return 1;
                    }
                  }
                }
              } else {  // if area > 2104.1
                if (shiftStd <= 0.13082) {
                  return 1;
                } else {  // if shiftStd > 0.13082
                  if (area <= 3000.4) {
                    if (avgErr <= 11.169) {
                      return 1;
                    } else {  // if avgErr > 11.169
                      return 1;
                    }
                  } else {  // if area > 3000.4
                    if (shiftStd <= 0.52799) {
                      return 3;
                    } else {  // if shiftStd > 0.52799
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.89863
            if (maxEdgeLength <= 80.223) {
              if (shiftAvg <= 20.304) {
                if (confidence <= 0.91056) {
                  if (avgErr <= 10.255) {
                    if (maxEdgeLength <= 73.982) {
                      return 1;
                    } else {  // if maxEdgeLength > 73.982
                      return 1;
                    }
                  } else {  // if avgErr > 10.255
                    if (area <= 2109.8) {
                      return 1;
                    } else {  // if area > 2109.8
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.91056
                  if (maxEdgeLength <= 52.111) {
                    if (avgErr <= 10.006) {
                      return 1;
                    } else {  // if avgErr > 10.006
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 52.111
                    if (area <= 2290.9) {
                      return 1;
                    } else {  // if area > 2290.9
                      return 1;
                    }
                  }
                }
              } else {  // if shiftAvg > 20.304
                if (shiftStd <= 1.0589) {
                  if (shiftNcc <= 0.99661) {
                    return 1;
                  } else {  // if shiftNcc > 0.99661
                    return 0;
                  }
                } else {  // if shiftStd > 1.0589
                  return 0;
                }
              }
            } else {  // if maxEdgeLength > 80.223
              if (avgErr <= 7.3195) {
                if (confidence <= 0.89897) {
                  return 2;
                } else {  // if confidence > 0.89897
                  if (maxEdgeLength <= 83.186) {
                    if (shiftAvg <= 3.6646) {
                      return 1;
                    } else {  // if shiftAvg > 3.6646
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 83.186
                    return 2;
                  }
                }
              } else {  // if avgErr > 7.3195
                if (avgErr <= 9.8262) {
                  if (area <= 2773.5) {
                    if (shiftStd <= 0.23637) {
                      return 1;
                    } else {  // if shiftStd > 0.23637
                      return 0;
                    }
                  } else {  // if area > 2773.5
                    if (shiftAvg <= 0.85504) {
                      return 0;
                    } else {  // if shiftAvg > 0.85504
                      return 0;
                    }
                  }
                } else {  // if avgErr > 9.8262
                  if (xyRatio <= 0.48581) {
                    if (shiftStd <= 2.601) {
                      return 1;
                    } else {  // if shiftStd > 2.601
                      return 2;
                    }
                  } else {  // if xyRatio > 0.48581
                    if (xyRatio <= 0.58146) {
                      return 2;
                    } else {  // if xyRatio > 0.58146
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 83.26
      if (maxEdgeLength <= 126.55) {
        if (confidence <= 0.91586) {
          if (shiftAvg <= 3.5881) {
            if (avgErr <= 9.0672) {
              if (xyRatio <= 0.33076) {
                if (confidence <= 0.8937) {
                  if (avgErr <= 8.2722) {
                    if (shiftStd <= 0.61522) {
                      return 2;
                    } else {  // if shiftStd > 0.61522
                      return 3;
                    }
                  } else {  // if avgErr > 8.2722
                    if (maxEdgeLength <= 105.97) {
                      return 1;
                    } else {  // if maxEdgeLength > 105.97
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.8937
                  if (confidence <= 0.91331) {
                    return 1;
                  } else {  // if confidence > 0.91331
                    return 2;
                  }
                }
              } else {  // if xyRatio > 0.33076
                if (confidence <= 0.90889) {
                  if (shiftStd <= 3.4888) {
                    if (area <= 2970.4) {
                      return 1;
                    } else {  // if area > 2970.4
                      return 1;
                    }
                  } else {  // if shiftStd > 3.4888
                    if (shiftAvg <= 0.61984) {
                      return 2;
                    } else {  // if shiftAvg > 0.61984
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.90889
                  if (avgErr <= 6.5846) {
                    if (shiftNcc <= 0.98886) {
                      return 1;
                    } else {  // if shiftNcc > 0.98886
                      return 0;
                    }
                  } else {  // if avgErr > 6.5846
                    if (xyRatio <= 0.48783) {
                      return 1;
                    } else {  // if xyRatio > 0.48783
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgErr > 9.0672
              if (xyRatio <= 0.41445) {
                if (confidence <= 0.89584) {
                  if (xyRatio <= 0.27026) {
                    return 2;
                  } else {  // if xyRatio > 0.27026
                    if (confidence <= 0.84805) {
                      return 1;
                    } else {  // if confidence > 0.84805
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.89584
                  if (maxEdgeLength <= 83.877) {
                    if (shiftNcc <= 0.78962) {
                      return 0;
                    } else {  // if shiftNcc > 0.78962
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 83.877
                    if (shiftNcc <= 0.97794) {
                      return 1;
                    } else {  // if shiftNcc > 0.97794
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.41445
                if (shiftNcc <= 0.94602) {
                  if (shiftStd <= 1.2203) {
                    if (shiftAvg <= 0.77744) {
                      return 0;
                    } else {  // if shiftAvg > 0.77744
                      return 1;
                    }
                  } else {  // if shiftStd > 1.2203
                    if (xyRatio <= 0.46943) {
                      return 1;
                    } else {  // if xyRatio > 0.46943
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.94602
                  if (shiftAvg <= 2.994) {
                    if (shiftAvg <= 0.85697) {
                      return 3;
                    } else {  // if shiftAvg > 0.85697
                      return 1;
                    }
                  } else {  // if shiftAvg > 2.994
                    if (shiftAvg <= 3.4948) {
                      return 0;
                    } else {  // if shiftAvg > 3.4948
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 3.5881
            if (confidence <= 0.88954) {
              if (shiftNcc <= 0.77967) {
                if (confidence <= 0.86741) {
                  if (avgErr <= 10.644) {
                    if (maxEdgeLength <= 100.84) {
                      return 2;
                    } else {  // if maxEdgeLength > 100.84
                      return 1;
                    }
                  } else {  // if avgErr > 10.644
                    if (shiftAvg <= 5.0927) {
                      return 2;
                    } else {  // if shiftAvg > 5.0927
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.86741
                  if (maxEdgeLength <= 98.76) {
                    if (area <= 5771.4) {
                      return 2;
                    } else {  // if area > 5771.4
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 98.76
                    return 1;
                  }
                }
              } else {  // if shiftNcc > 0.77967
                if (confidence <= 0.85762) {
                  if (area <= 5638.1) {
                    if (confidence <= 0.81833) {
                      return 1;
                    } else {  // if confidence > 0.81833
                      return 2;
                    }
                  } else {  // if area > 5638.1
                    if (maxEdgeLength <= 108.97) {
                      return 1;
                    } else {  // if maxEdgeLength > 108.97
                      return 0;
                    }
                  }
                } else {  // if confidence > 0.85762
                  if (area <= 4247.4) {
                    if (shiftAvg <= 3.904) {
                      return 2;
                    } else {  // if shiftAvg > 3.904
                      return 1;
                    }
                  } else {  // if area > 4247.4
                    if (shiftAvg <= 14.103) {
                      return 0;
                    } else {  // if shiftAvg > 14.103
                      return 1;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.88954
              if (maxEdgeLength <= 92.607) {
                if (xyRatio <= 0.86938) {
                  if (shiftAvg <= 3.6689) {
                    return 0;
                  } else {  // if shiftAvg > 3.6689
                    if (shiftAvg <= 3.7814) {
                      return 1;
                    } else {  // if shiftAvg > 3.7814
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.86938
                  return 0;
                }
              } else {  // if maxEdgeLength > 92.607
                if (xyRatio <= 0.542) {
                  if (shiftStd <= 0.93829) {
                    if (maxEdgeLength <= 124.17) {
                      return 1;
                    } else {  // if maxEdgeLength > 124.17
                      return 0;
                    }
                  } else {  // if shiftStd > 0.93829
                    if (maxEdgeLength <= 112.62) {
                      return 0;
                    } else {  // if maxEdgeLength > 112.62
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.542
                  if (shiftNcc <= 0.83206) {
                    if (shiftStd <= 3.2039) {
                      return 1;
                    } else {  // if shiftStd > 3.2039
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.83206
                    if (shiftAvg <= 7.9405) {
                      return 0;
                    } else {  // if shiftAvg > 7.9405
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.91586
          if (avgErr <= 6.0098) {
            if (shiftNcc <= 0.9232) {
              if (shiftAvg <= 2.0593) {
                if (shiftStd <= 0.93384) {
                  if (confidence <= 0.93358) {
                    return 1;
                  } else {  // if confidence > 0.93358
                    if (avgErr <= 5.8484) {
                      return 1;
                    } else {  // if avgErr > 5.8484
                      return 0;
                    }
                  }
                } else {  // if shiftStd > 0.93384
                  if (shiftNcc <= 0.56328) {
                    return 1;
                  } else {  // if shiftNcc > 0.56328
                    return 0;
                  }
                }
              } else {  // if shiftAvg > 2.0593
                if (shiftStd <= 0.43737) {
                  return 1;
                } else {  // if shiftStd > 0.43737
                  return 2;
                }
              }
            } else {  // if shiftNcc > 0.9232
              if (xyRatio <= 0.49541) {
                if (shiftAvg <= 1.2608) {
                  return 1;
                } else {  // if shiftAvg > 1.2608
                  return 0;
                }
              } else {  // if xyRatio > 0.49541
                return 1;
              }
            }
          } else {  // if avgErr > 6.0098
            if (xyRatio <= 0.50722) {
              if (shiftAvg <= 7.5832) {
                if (avgErr <= 11.832) {
                  if (shiftNcc <= 0.96989) {
                    if (maxEdgeLength <= 85.854) {
                      return 0;
                    } else {  // if maxEdgeLength > 85.854
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.96989
                    if (maxEdgeLength <= 110.94) {
                      return 1;
                    } else {  // if maxEdgeLength > 110.94
                      return 0;
                    }
                  }
                } else {  // if avgErr > 11.832
                  if (confidence <= 0.94363) {
                    if (xyRatio <= 0.4139) {
                      return 1;
                    } else {  // if xyRatio > 0.4139
                      return 1;
                    }
                  } else {  // if confidence > 0.94363
                    if (maxEdgeLength <= 95.109) {
                      return 1;
                    } else {  // if maxEdgeLength > 95.109
                      return 0;
                    }
                  }
                }
              } else {  // if shiftAvg > 7.5832
                if (avgErr <= 8.4655) {
                  if (shiftStd <= 1.6827) {
                    return 1;
                  } else {  // if shiftStd > 1.6827
                    return 0;
                  }
                } else {  // if avgErr > 8.4655
                  if (xyRatio <= 0.50175) {
                    if (shiftStd <= 1.4048) {
                      return 0;
                    } else {  // if shiftStd > 1.4048
                      return 0;
                    }
                  } else {  // if xyRatio > 0.50175
                    if (area <= 6053.9) {
                      return 0;
                    } else {  // if area > 6053.9
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.50722
              if (maxEdgeLength <= 114.43) {
                if (avgErr <= 9.8936) {
                  if (confidence <= 0.92444) {
                    if (shiftNcc <= 0.72973) {
                      return 1;
                    } else {  // if shiftNcc > 0.72973
                      return 0;
                    }
                  } else {  // if confidence > 0.92444
                    if (maxEdgeLength <= 98.466) {
                      return 0;
                    } else {  // if maxEdgeLength > 98.466
                      return 0;
                    }
                  }
                } else {  // if avgErr > 9.8936
                  if (shiftAvg <= 4.1624) {
                    if (xyRatio <= 0.51481) {
                      return 0;
                    } else {  // if xyRatio > 0.51481
                      return 1;
                    }
                  } else {  // if shiftAvg > 4.1624
                    if (maxEdgeLength <= 106.89) {
                      return 0;
                    } else {  // if maxEdgeLength > 106.89
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 114.43
                if (shiftAvg <= 23.404) {
                  if (xyRatio <= 0.52845) {
                    if (shiftNcc <= 0.78726) {
                      return 1;
                    } else {  // if shiftNcc > 0.78726
                      return 0;
                    }
                  } else {  // if xyRatio > 0.52845
                    if (avgErr <= 12.89) {
                      return 1;
                    } else {  // if avgErr > 12.89
                      return 0;
                    }
                  }
                } else {  // if shiftAvg > 23.404
                  if (maxEdgeLength <= 126.4) {
                    if (avgErr <= 9.1617) {
                      return 0;
                    } else {  // if avgErr > 9.1617
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 126.4
                    return 1;
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 126.55
        if (confidence <= 0.92345) {
          if (avgErr <= 8.3877) {
            if (xyRatio <= 0.42491) {
              if (confidence <= 0.9065) {
                if (shiftStd <= 1.1528) {
                  return 1;
                } else {  // if shiftStd > 1.1528
                  if (xyRatio <= 0.41152) {
                    if (shiftNcc <= 0.98592) {
                      return 0;
                    } else {  // if shiftNcc > 0.98592
                      return 2;
                    }
                  } else {  // if xyRatio > 0.41152
                    return 2;
                  }
                }
              } else {  // if confidence > 0.9065
                if (shiftStd <= 2.3231) {
                  return 0;
                } else {  // if shiftStd > 2.3231
                  return 1;
                }
              }
            } else {  // if xyRatio > 0.42491
              if (area <= 7121.3) {
                return 1;
              } else {  // if area > 7121.3
                if (shiftStd <= 2.3388) {
                  return 0;
                } else {  // if shiftStd > 2.3388
                  if (maxEdgeLength <= 135.09) {
                    return 1;
                  } else {  // if maxEdgeLength > 135.09
                    if (avgErr <= 6.2713) {
                      return 0;
                    } else {  // if avgErr > 6.2713
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if avgErr > 8.3877
            if (shiftAvg <= 39.167) {
              if (area <= 9191.3) {
                if (shiftStd <= 5.2886) {
                  if (shiftAvg <= 1.2583) {
                    return 1;
                  } else {  // if shiftAvg > 1.2583
                    if (maxEdgeLength <= 149.36) {
                      return 0;
                    } else {  // if maxEdgeLength > 149.36
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 5.2886
                  if (maxEdgeLength <= 149.35) {
                    if (shiftAvg <= 7.5882) {
                      return 0;
                    } else {  // if shiftAvg > 7.5882
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 149.35
                    return 0;
                  }
                }
              } else {  // if area > 9191.3
                if (maxEdgeLength <= 134.91) {
                  if (xyRatio <= 0.58981) {
                    if (confidence <= 0.91877) {
                      return 1;
                    } else {  // if confidence > 0.91877
                      return 1;
                    }
                  } else {  // if xyRatio > 0.58981
                    if (shiftAvg <= 15.676) {
                      return 0;
                    } else {  // if shiftAvg > 15.676
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 134.91
                  if (shiftAvg <= 23.779) {
                    if (shiftNcc <= 0.9146) {
                      return 1;
                    } else {  // if shiftNcc > 0.9146
                      return 0;
                    }
                  } else {  // if shiftAvg > 23.779
                    if (shiftAvg <= 35.999) {
                      return 0;
                    } else {  // if shiftAvg > 35.999
                      return 0;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 39.167
              if (shiftAvg <= 43.54) {
                if (shiftAvg <= 43.286) {
                  return 0;
                } else {  // if shiftAvg > 43.286
                  return 1;
                }
              } else {  // if shiftAvg > 43.54
                return 0;
              }
            }
          }
        } else {  // if confidence > 0.92345
          if (maxEdgeLength <= 138.18) {
            if (xyRatio <= 0.5539) {
              if (shiftNcc <= 0.71773) {
                if (avgErr <= 7.2376) {
                  return 0;
                } else {  // if avgErr > 7.2376
                  if (avgErr <= 11.394) {
                    if (shiftAvg <= 2.3683) {
                      return 1;
                    } else {  // if shiftAvg > 2.3683
                      return 0;
                    }
                  } else {  // if avgErr > 11.394
                    if (shiftStd <= 2.2183) {
                      return 0;
                    } else {  // if shiftStd > 2.2183
                      return 1;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.71773
                if (xyRatio <= 0.43672) {
                  if (confidence <= 0.95966) {
                    if (shiftNcc <= 0.86366) {
                      return 1;
                    } else {  // if shiftNcc > 0.86366
                      return 0;
                    }
                  } else {  // if confidence > 0.95966
                    return 1;
                  }
                } else {  // if xyRatio > 0.43672
                  if (area <= 9948.2) {
                    if (avgErr <= 8.3086) {
                      return 0;
                    } else {  // if avgErr > 8.3086
                      return 0;
                    }
                  } else {  // if area > 9948.2
                    return 1;
                  }
                }
              }
            } else {  // if xyRatio > 0.5539
              if (avgErr <= 8.7189) {
                if (shiftNcc <= 0.99502) {
                  return 0;
                } else {  // if shiftNcc > 0.99502
                  return 1;
                }
              } else {  // if avgErr > 8.7189
                if (confidence <= 0.952) {
                  if (xyRatio <= 0.57466) {
                    return 1;
                  } else {  // if xyRatio > 0.57466
                    if (xyRatio <= 0.57626) {
                      return 0;
                    } else {  // if xyRatio > 0.57626
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.952
                  return 0;
                }
              }
            }
          } else {  // if maxEdgeLength > 138.18
            if (area <= 6960.7) {
              if (shiftAvg <= 10.862) {
                return 1;
              } else {  // if shiftAvg > 10.862
                if (maxEdgeLength <= 142.27) {
                  return 0;
                } else {  // if maxEdgeLength > 142.27
                  return 1;
                }
              }
            } else {  // if area > 6960.7
              if (maxEdgeLength <= 191.33) {
                if (maxEdgeLength <= 165.07) {
                  if (xyRatio <= 0.53832) {
                    if (shiftNcc <= 0.31621) {
                      return 1;
                    } else {  // if shiftNcc > 0.31621
                      return 0;
                    }
                  } else {  // if xyRatio > 0.53832
                    if (xyRatio <= 0.55556) {
                      return 0;
                    } else {  // if xyRatio > 0.55556
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 165.07
                  if (maxEdgeLength <= 167.41) {
                    if (shiftStd <= 0.85398) {
                      return 1;
                    } else {  // if shiftStd > 0.85398
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 167.41
                    if (confidence <= 0.93764) {
                      return 0;
                    } else {  // if confidence > 0.93764
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 191.33
                if (xyRatio <= 0.70849) {
                  return 0;
                } else {  // if xyRatio > 0.70849
                  if (xyRatio <= 0.72357) {
                    return 2;
                  } else {  // if xyRatio > 0.72357
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

} // namespace rm
