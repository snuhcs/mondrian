#include "strm/tree/VIRAT.hpp"

namespace rm {

int VIRAT(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
          float shiftNcc, float avgErr, float confidence) {
  if (confidence <= 0.35362) {
    if (confidence <= 0.19591) {
      if (area <= 94.785) {
        if (confidence <= 0.15499) {
          if (area <= 78.822) {
            if (area <= 73.67) {
              if (area <= 52.261) {
                return 3;
              } else {  // if area > 52.261
                if (area <= 52.592) {
                  return 2;
                } else {  // if area > 52.592
                  if (maxEdgeLength <= 12.801) {
                    if (maxEdgeLength <= 12.8) {
                      return 3;
                    } else {  // if maxEdgeLength > 12.8
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 12.801
                    if (confidence <= 0.14154) {
                      return 3;
                    } else {  // if confidence > 0.14154
                      return 3;
                    }
                  }
                }
              }
            } else {  // if area > 73.67
              if (area <= 73.672) {
                return 2;
              } else {  // if area > 73.672
                if (confidence <= 0.1422) {
                  if (maxEdgeLength <= 11.647) {
                    if (area <= 74.363) {
                      return 2;
                    } else {  // if area > 74.363
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 11.647
                    if (xyRatio <= 0.43675) {
                      return 3;
                    } else {  // if xyRatio > 0.43675
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.1422
                  if (avgErr <= 0.00076389) {
                    if (maxEdgeLength <= 14.064) {
                      return 3;
                    } else {  // if maxEdgeLength > 14.064
                      return 2;
                    }
                  } else {  // if avgErr > 0.00076389
                    if (confidence <= 0.14221) {
                      return 2;
                    } else {  // if confidence > 0.14221
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 78.822
            if (xyRatio <= 0.47612) {
              if (confidence <= 0.12507) {
                if (area <= 79.016) {
                  if (area <= 79.013) {
                    if (maxEdgeLength <= 12.99) {
                      return 2;
                    } else {  // if maxEdgeLength > 12.99
                      return 3;
                    }
                  } else {  // if area > 79.013
                    return 2;
                  }
                } else {  // if area > 79.016
                  if (maxEdgeLength <= 14.315) {
                    if (maxEdgeLength <= 14.313) {
                      return 3;
                    } else {  // if maxEdgeLength > 14.313
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 14.315
                    if (avgErr <= 0.78056) {
                      return 3;
                    } else {  // if avgErr > 0.78056
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.12507
                if (confidence <= 0.1251) {
                  if (area <= 80.73) {
                    return 3;
                  } else {  // if area > 80.73
                    return 2;
                  }
                } else {  // if confidence > 0.1251
                  if (area <= 78.837) {
                    if (avgErr <= 0.78042) {
                      return 2;
                    } else {  // if avgErr > 0.78042
                      return 3;
                    }
                  } else {  // if area > 78.837
                    if (xyRatio <= 0.43249) {
                      return 3;
                    } else {  // if xyRatio > 0.43249
                      return 3;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.47612
              if (confidence <= 0.14378) {
                if (confidence <= 0.11894) {
                  if (xyRatio <= 0.49988) {
                    if (confidence <= 0.10902) {
                      return 3;
                    } else {  // if confidence > 0.10902
                      return 3;
                    }
                  } else {  // if xyRatio > 0.49988
                    if (xyRatio <= 0.4999) {
                      return 2;
                    } else {  // if xyRatio > 0.4999
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.11894
                  if (area <= 88.352) {
                    if (confidence <= 0.13392) {
                      return 3;
                    } else {  // if confidence > 0.13392
                      return 3;
                    }
                  } else {  // if area > 88.352
                    if (confidence <= 0.14263) {
                      return 3;
                    } else {  // if confidence > 0.14263
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.14378
                if (confidence <= 0.1438) {
                  return 2;
                } else {  // if confidence > 0.1438
                  if (area <= 87.385) {
                    if (avgErr <= 2.5558) {
                      return 3;
                    } else {  // if avgErr > 2.5558
                      return 2;
                    }
                  } else {  // if area > 87.385
                    if (xyRatio <= 0.58856) {
                      return 3;
                    } else {  // if xyRatio > 0.58856
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.15499
          if (xyRatio <= 0.45955) {
            if (confidence <= 0.17635) {
              if (area <= 86.693) {
                if (confidence <= 0.15499) {
                  return 2;
                } else {  // if confidence > 0.15499
                  if (area <= 74.66) {
                    if (confidence <= 0.17623) {
                      return 3;
                    } else {  // if confidence > 0.17623
                      return 2;
                    }
                  } else {  // if area > 74.66
                    if (area <= 74.67) {
                      return 2;
                    } else {  // if area > 74.67
                      return 3;
                    }
                  }
                }
              } else {  // if area > 86.693
                if (xyRatio <= 0.36547) {
                  return 2;
                } else {  // if xyRatio > 0.36547
                  if (confidence <= 0.16775) {
                    if (area <= 86.697) {
                      return 2;
                    } else {  // if area > 86.697
                      return 3;
                    }
                  } else {  // if confidence > 0.16775
                    if (area <= 86.736) {
                      return 2;
                    } else {  // if area > 86.736
                      return 3;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.17635
              if (xyRatio <= 0.41437) {
                if (avgErr <= 0.6159) {
                  if (confidence <= 0.18718) {
                    if (avgErr <= 0.47528) {
                      return 3;
                    } else {  // if avgErr > 0.47528
                      return 3;
                    }
                  } else {  // if confidence > 0.18718
                    if (confidence <= 0.18726) {
                      return 2;
                    } else {  // if confidence > 0.18726
                      return 3;
                    }
                  }
                } else {  // if avgErr > 0.6159
                  if (area <= 73.15) {
                    if (area <= 65.061) {
                      return 3;
                    } else {  // if area > 65.061
                      return 3;
                    }
                  } else {  // if area > 73.15
                    if (area <= 73.218) {
                      return 2;
                    } else {  // if area > 73.218
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.41437
                if (area <= 76.1) {
                  if (xyRatio <= 0.42212) {
                    if (xyRatio <= 0.42202) {
                      return 3;
                    } else {  // if xyRatio > 0.42202
                      return 2;
                    }
                  } else {  // if xyRatio > 0.42212
                    if (shiftAvg <= 0.00014787) {
                      return 3;
                    } else {  // if shiftAvg > 0.00014787
                      return 3;
                    }
                  }
                } else {  // if area > 76.1
                  if (confidence <= 0.17642) {
                    if (maxEdgeLength <= 13.836) {
                      return 2;
                    } else {  // if maxEdgeLength > 13.836
                      return 3;
                    }
                  } else {  // if confidence > 0.17642
                    if (area <= 76.166) {
                      return 2;
                    } else {  // if area > 76.166
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.45955
            if (area <= 79.745) {
              if (area <= 77.82) {
                if (area <= 71.473) {
                  return 3;
                } else {  // if area > 71.473
                  if (area <= 71.694) {
                    return 2;
                  } else {  // if area > 71.694
                    if (xyRatio <= 0.54997) {
                      return 3;
                    } else {  // if xyRatio > 0.54997
                      return 2;
                    }
                  }
                }
              } else {  // if area > 77.82
                if (shiftAvg <= 0.013394) {
                  if (confidence <= 0.19465) {
                    if (xyRatio <= 0.54315) {
                      return 3;
                    } else {  // if xyRatio > 0.54315
                      return 2;
                    }
                  } else {  // if confidence > 0.19465
                    return 2;
                  }
                } else {  // if shiftAvg > 0.013394
                  if (shiftAvg <= 0.041036) {
                    if (maxEdgeLength <= 12.829) {
                      return 3;
                    } else {  // if maxEdgeLength > 12.829
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.041036
                    if (area <= 79.382) {
                      return 3;
                    } else {  // if area > 79.382
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 79.745
              if (confidence <= 0.17021) {
                if (area <= 89.184) {
                  if (avgErr <= 2.3697) {
                    if (maxEdgeLength <= 12.21) {
                      return 2;
                    } else {  // if maxEdgeLength > 12.21
                      return 3;
                    }
                  } else {  // if avgErr > 2.3697
                    return 2;
                  }
                } else {  // if area > 89.184
                  if (confidence <= 0.15501) {
                    if (avgErr <= 0.55174) {
                      return 3;
                    } else {  // if avgErr > 0.55174
                      return 2;
                    }
                  } else {  // if confidence > 0.15501
                    if (shiftAvg <= 4.4532e-07) {
                      return 2;
                    } else {  // if shiftAvg > 4.4532e-07
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.17021
                if (xyRatio <= 0.4872) {
                  if (confidence <= 0.19119) {
                    if (area <= 92.613) {
                      return 3;
                    } else {  // if area > 92.613
                      return 3;
                    }
                  } else {  // if confidence > 0.19119
                    if (maxEdgeLength <= 13.648) {
                      return 3;
                    } else {  // if maxEdgeLength > 13.648
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.4872
                  if (confidence <= 0.19382) {
                    if (confidence <= 0.17203) {
                      return 3;
                    } else {  // if confidence > 0.17203
                      return 3;
                    }
                  } else {  // if confidence > 0.19382
                    if (area <= 87.401) {
                      return 3;
                    } else {  // if area > 87.401
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 94.785
        if (confidence <= 0.14585) {
          if (area <= 671.99) {
            if (xyRatio <= 0.49305) {
              if (shiftAvg <= 0.27057) {
                if (confidence <= 0.13064) {
                  if (xyRatio <= 0.47369) {
                    if (area <= 662.91) {
                      return 3;
                    } else {  // if area > 662.91
                      return 2;
                    }
                  } else {  // if xyRatio > 0.47369
                    if (xyRatio <= 0.47374) {
                      return 2;
                    } else {  // if xyRatio > 0.47374
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.13064
                  if (shiftStd <= 0.017199) {
                    if (xyRatio <= 0.44311) {
                      return 3;
                    } else {  // if xyRatio > 0.44311
                      return 3;
                    }
                  } else {  // if shiftStd > 0.017199
                    if (shiftStd <= 0.10156) {
                      return 2;
                    } else {  // if shiftStd > 0.10156
                      return 3;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.27057
                if (confidence <= 0.13265) {
                  if (shiftStd <= 1.2758) {
                    return 3;
                  } else {  // if shiftStd > 1.2758
                    return 2;
                  }
                } else {  // if confidence > 0.13265
                  if (area <= 477.09) {
                    return 2;
                  } else {  // if area > 477.09
                    return 1;
                  }
                }
              }
            } else {  // if xyRatio > 0.49305
              if (confidence <= 0.12372) {
                if (shiftStd <= 0.013485) {
                  if (area <= 103.13) {
                    if (confidence <= 0.11912) {
                      return 3;
                    } else {  // if confidence > 0.11912
                      return 3;
                    }
                  } else {  // if area > 103.13
                    if (area <= 120.48) {
                      return 3;
                    } else {  // if area > 120.48
                      return 3;
                    }
                  }
                } else {  // if shiftStd > 0.013485
                  if (xyRatio <= 0.59288) {
                    if (avgErr <= 1.7167) {
                      return 1;
                    } else {  // if avgErr > 1.7167
                      return 3;
                    }
                  } else {  // if xyRatio > 0.59288
                    if (xyRatio <= 0.72102) {
                      return 2;
                    } else {  // if xyRatio > 0.72102
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.12372
                if (area <= 100.45) {
                  if (confidence <= 0.12375) {
                    return 2;
                  } else {  // if confidence > 0.12375
                    if (confidence <= 0.13912) {
                      return 3;
                    } else {  // if confidence > 0.13912
                      return 3;
                    }
                  }
                } else {  // if area > 100.45
                  if (xyRatio <= 0.51338) {
                    if (area <= 100.5) {
                      return 2;
                    } else {  // if area > 100.5
                      return 3;
                    }
                  } else {  // if xyRatio > 0.51338
                    if (shiftAvg <= 0.077194) {
                      return 3;
                    } else {  // if shiftAvg > 0.077194
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 671.99
            if (area <= 1652.2) {
              if (avgErr <= 5.7803) {
                if (maxEdgeLength <= 51.982) {
                  if (area <= 682.17) {
                    return 2;
                  } else {  // if area > 682.17
                    if (shiftNcc <= 0.9907) {
                      return 3;
                    } else {  // if shiftNcc > 0.9907
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 51.982
                  if (maxEdgeLength <= 68.127) {
                    if (maxEdgeLength <= 63.899) {
                      return 3;
                    } else {  // if maxEdgeLength > 63.899
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 68.127
                    if (confidence <= 0.10308) {
                      return 2;
                    } else {  // if confidence > 0.10308
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 5.7803
                if (area <= 1465.3) {
                  if (avgErr <= 19.772) {
                    if (shiftAvg <= 0.49533) {
                      return 2;
                    } else {  // if shiftAvg > 0.49533
                      return 2;
                    }
                  } else {  // if avgErr > 19.772
                    return 3;
                  }
                } else {  // if area > 1465.3
                  return 3;
                }
              }
            } else {  // if area > 1652.2
              if (xyRatio <= 0.94513) {
                if (shiftStd <= 0.00096823) {
                  if (avgErr <= 0.58655) {
                    if (xyRatio <= 0.44763) {
                      return 2;
                    } else {  // if xyRatio > 0.44763
                      return 1;
                    }
                  } else {  // if avgErr > 0.58655
                    if (area <= 1694.9) {
                      return 2;
                    } else {  // if area > 1694.9
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 0.00096823
                  if (avgErr <= 1.5772) {
                    if (xyRatio <= 0.48701) {
                      return 2;
                    } else {  // if xyRatio > 0.48701
                      return 3;
                    }
                  } else {  // if avgErr > 1.5772
                    if (avgErr <= 5.0133) {
                      return 2;
                    } else {  // if avgErr > 5.0133
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.94513
                if (shiftNcc <= 0.99104) {
                  if (shiftStd <= 0.4014) {
                    if (avgErr <= 6.1663) {
                      return 3;
                    } else {  // if avgErr > 6.1663
                      return 3;
                    }
                  } else {  // if shiftStd > 0.4014
                    if (shiftStd <= 2.1799) {
                      return 1;
                    } else {  // if shiftStd > 2.1799
                      return 3;
                    }
                  }
                } else {  // if shiftNcc > 0.99104
                  return 2;
                }
              }
            }
          }
        } else {  // if confidence > 0.14585
          if (area <= 492.86) {
            if (xyRatio <= 0.49059) {
              if (confidence <= 0.16956) {
                if (xyRatio <= 0.43225) {
                  if (shiftStd <= 0.014247) {
                    if (shiftAvg <= 0.075969) {
                      return 3;
                    } else {  // if shiftAvg > 0.075969
                      return 2;
                    }
                  } else {  // if shiftStd > 0.014247
                    if (confidence <= 0.15958) {
                      return 3;
                    } else {  // if confidence > 0.15958
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.43225
                  if (maxEdgeLength <= 15.502) {
                    if (confidence <= 0.14587) {
                      return 2;
                    } else {  // if confidence > 0.14587
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 15.502
                    if (shiftStd <= 0.00023798) {
                      return 3;
                    } else {  // if shiftStd > 0.00023798
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.16956
                if (maxEdgeLength <= 14.418) {
                  if (confidence <= 0.19191) {
                    if (area <= 101.07) {
                      return 3;
                    } else {  // if area > 101.07
                      return 2;
                    }
                  } else {  // if confidence > 0.19191
                    if (xyRatio <= 0.4746) {
                      return 3;
                    } else {  // if xyRatio > 0.4746
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 14.418
                  if (maxEdgeLength <= 15.707) {
                    if (shiftNcc <= -0.57305) {
                      return 2;
                    } else {  // if shiftNcc > -0.57305
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 15.707
                    if (xyRatio <= 0.4052) {
                      return 3;
                    } else {  // if xyRatio > 0.4052
                      return 2;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.49059
              if (confidence <= 0.17535) {
                if (area <= 101.97) {
                  if (maxEdgeLength <= 13.322) {
                    if (maxEdgeLength <= 13.021) {
                      return 3;
                    } else {  // if maxEdgeLength > 13.021
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 13.322
                    if (area <= 94.849) {
                      return 2;
                    } else {  // if area > 94.849
                      return 3;
                    }
                  }
                } else {  // if area > 101.97
                  if (xyRatio <= 0.5269) {
                    if (maxEdgeLength <= 14.415) {
                      return 3;
                    } else {  // if maxEdgeLength > 14.415
                      return 3;
                    }
                  } else {  // if xyRatio > 0.5269
                    if (maxEdgeLength <= 13.58) {
                      return 3;
                    } else {  // if maxEdgeLength > 13.58
                      return 3;
                    }
                  }
                }
              } else {  // if confidence > 0.17535
                if (maxEdgeLength <= 13.565) {
                  if (area <= 95.322) {
                    if (maxEdgeLength <= 13.213) {
                      return 3;
                    } else {  // if maxEdgeLength > 13.213
                      return 2;
                    }
                  } else {  // if area > 95.322
                    if (shiftStd <= 9.9215e-07) {
                      return 3;
                    } else {  // if shiftStd > 9.9215e-07
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 13.565
                  if (xyRatio <= 0.53717) {
                    if (shiftAvg <= 0.025753) {
                      return 3;
                    } else {  // if shiftAvg > 0.025753
                      return 3;
                    }
                  } else {  // if xyRatio > 0.53717
                    if (xyRatio <= 0.6666) {
                      return 3;
                    } else {  // if xyRatio > 0.6666
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 492.86
            if (maxEdgeLength <= 79.646) {
              if (maxEdgeLength <= 48.084) {
                if (xyRatio <= 0.4441) {
                  if (maxEdgeLength <= 38.769) {
                    return 2;
                  } else {  // if maxEdgeLength > 38.769
                    if (shiftAvg <= 0.0020872) {
                      return 3;
                    } else {  // if shiftAvg > 0.0020872
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.4441
                  if (shiftNcc <= 0.80474) {
                    if (shiftNcc <= 0.68986) {
                      return 2;
                    } else {  // if shiftNcc > 0.68986
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.80474
                    if (shiftNcc <= 0.8922) {
                      return 3;
                    } else {  // if shiftNcc > 0.8922
                      return 3;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 48.084
                if (avgErr <= 1.7298) {
                  if (area <= 2496.7) {
                    if (confidence <= 0.16774) {
                      return 2;
                    } else {  // if confidence > 0.16774
                      return 2;
                    }
                  } else {  // if area > 2496.7
                    return 3;
                  }
                } else {  // if avgErr > 1.7298
                  if (shiftStd <= 0.020307) {
                    if (shiftAvg <= 0.0054032) {
                      return 3;
                    } else {  // if shiftAvg > 0.0054032
                      return 2;
                    }
                  } else {  // if shiftStd > 0.020307
                    if (avgErr <= 5.0038) {
                      return 2;
                    } else {  // if avgErr > 5.0038
                      return 3;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 79.646
              if (shiftNcc <= -0.015464) {
                if (shiftNcc <= -0.062538) {
                  return 1;
                } else {  // if shiftNcc > -0.062538
                  if (shiftAvg <= 1.0619) {
                    return 3;
                  } else {  // if shiftAvg > 1.0619
                    return 0;
                  }
                }
              } else {  // if shiftNcc > -0.015464
                if (xyRatio <= 0.22358) {
                  if (shiftStd <= 3.7386) {
                    if (shiftStd <= 0.018777) {
                      return 2;
                    } else {  // if shiftStd > 0.018777
                      return 2;
                    }
                  } else {  // if shiftStd > 3.7386
                    if (confidence <= 0.17119) {
                      return 0;
                    } else {  // if confidence > 0.17119
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.22358
                  if (area <= 2489.5) {
                    if (shiftNcc <= 0.8561) {
                      return 1;
                    } else {  // if shiftNcc > 0.8561
                      return 2;
                    }
                  } else {  // if area > 2489.5
                    if (shiftStd <= 0.016042) {
                      return 1;
                    } else {  // if shiftStd > 0.016042
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if confidence > 0.19591
      if (area <= 89.973) {
        if (confidence <= 0.27171) {
          if (xyRatio <= 0.44943) {
            if (confidence <= 0.22588) {
              if (xyRatio <= 0.41358) {
                if (shiftAvg <= 0.00054648) {
                  if (confidence <= 0.1963) {
                    return 2;
                  } else {  // if confidence > 0.1963
                    if (shiftAvg <= 0.00052268) {
                      return 3;
                    } else {  // if shiftAvg > 0.00052268
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.00054648
                  if (area <= 72.955) {
                    if (confidence <= 0.21081) {
                      return 3;
                    } else {  // if confidence > 0.21081
                      return 3;
                    }
                  } else {  // if area > 72.955
                    if (area <= 72.972) {
                      return 2;
                    } else {  // if area > 72.972
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.41358
                if (maxEdgeLength <= 14.235) {
                  if (maxEdgeLength <= 14.232) {
                    if (maxEdgeLength <= 13.101) {
                      return 3;
                    } else {  // if maxEdgeLength > 13.101
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 14.232
                    return 2;
                  }
                } else {  // if maxEdgeLength > 14.235
                  if (shiftAvg <= 0.076632) {
                    if (shiftAvg <= 0.0062797) {
                      return 3;
                    } else {  // if shiftAvg > 0.0062797
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.076632
                    return 2;
                  }
                }
              }
            } else {  // if confidence > 0.22588
              if (xyRatio <= 0.44191) {
                if (confidence <= 0.24835) {
                  if (xyRatio <= 0.42107) {
                    if (area <= 83.963) {
                      return 3;
                    } else {  // if area > 83.963
                      return 3;
                    }
                  } else {  // if xyRatio > 0.42107
                    if (confidence <= 0.24776) {
                      return 3;
                    } else {  // if confidence > 0.24776
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.24835
                  if (avgErr <= 1.765) {
                    if (avgErr <= 1.561) {
                      return 3;
                    } else {  // if avgErr > 1.561
                      return 3;
                    }
                  } else {  // if avgErr > 1.765
                    if (avgErr <= 2.1635) {
                      return 2;
                    } else {  // if avgErr > 2.1635
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 0.44191
                if (avgErr <= 0.34028) {
                  if (confidence <= 0.24437) {
                    if (shiftAvg <= 5.5773e-05) {
                      return 3;
                    } else {  // if shiftAvg > 5.5773e-05
                      return 2;
                    }
                  } else {  // if confidence > 0.24437
                    return 2;
                  }
                } else {  // if avgErr > 0.34028
                  if (avgErr <= 1.5886) {
                    if (confidence <= 0.22746) {
                      return 3;
                    } else {  // if confidence > 0.22746
                      return 3;
                    }
                  } else {  // if avgErr > 1.5886
                    if (shiftAvg <= 0.070731) {
                      return 3;
                    } else {  // if shiftAvg > 0.070731
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.44943
            if (confidence <= 0.21341) {
              if (area <= 85.948) {
                if (confidence <= 0.19614) {
                  if (avgErr <= 1.3147) {
                    return 2;
                  } else {  // if avgErr > 1.3147
                    if (confidence <= 0.19595) {
                      return 2;
                    } else {  // if confidence > 0.19595
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.19614
                  if (area <= 83.223) {
                    if (confidence <= 0.2091) {
                      return 3;
                    } else {  // if confidence > 0.2091
                      return 3;
                    }
                  } else {  // if area > 83.223
                    if (area <= 85.266) {
                      return 3;
                    } else {  // if area > 85.266
                      return 3;
                    }
                  }
                }
              } else {  // if area > 85.948
                if (xyRatio <= 0.51031) {
                  if (avgErr <= 0.00055556) {
                    return 2;
                  } else {  // if avgErr > 0.00055556
                    if (maxEdgeLength <= 14.053) {
                      return 3;
                    } else {  // if maxEdgeLength > 14.053
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 0.51031
                  if (confidence <= 0.19726) {
                    if (confidence <= 0.19597) {
                      return 2;
                    } else {  // if confidence > 0.19597
                      return 3;
                    }
                  } else {  // if confidence > 0.19726
                    if (maxEdgeLength <= 12.555) {
                      return 2;
                    } else {  // if maxEdgeLength > 12.555
                      return 2;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.21341
              if (area <= 80.373) {
                if (xyRatio <= 0.45039) {
                  if (avgErr <= 1.3555) {
                    return 2;
                  } else {  // if avgErr > 1.3555
                    return 3;
                  }
                } else {  // if xyRatio > 0.45039
                  if (xyRatio <= 0.49757) {
                    if (maxEdgeLength <= 12.705) {
                      return 3;
                    } else {  // if maxEdgeLength > 12.705
                      return 3;
                    }
                  } else {  // if xyRatio > 0.49757
                    if (avgErr <= 0.61694) {
                      return 2;
                    } else {  // if avgErr > 0.61694
                      return 3;
                    }
                  }
                }
              } else {  // if area > 80.373
                if (confidence <= 0.23326) {
                  if (shiftAvg <= 0.0031003) {
                    if (shiftAvg <= 0.0030416) {
                      return 2;
                    } else {  // if shiftAvg > 0.0030416
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.0031003
                    if (confidence <= 0.21388) {
                      return 2;
                    } else {  // if confidence > 0.21388
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.23326
                  if (xyRatio <= 0.45983) {
                    if (xyRatio <= 0.4518) {
                      return 2;
                    } else {  // if xyRatio > 0.4518
                      return 3;
                    }
                  } else {  // if xyRatio > 0.45983
                    if (maxEdgeLength <= 13.727) {
                      return 2;
                    } else {  // if maxEdgeLength > 13.727
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.27171
          if (area <= 86.728) {
            if (confidence <= 0.31839) {
              if (avgErr <= 0.53243) {
                if (shiftAvg <= 0.00030996) {
                  if (avgErr <= 0.056667) {
                    if (confidence <= 0.29496) {
                      return 2;
                    } else {  // if confidence > 0.29496
                      return 3;
                    }
                  } else {  // if avgErr > 0.056667
                    if (confidence <= 0.28336) {
                      return 2;
                    } else {  // if confidence > 0.28336
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.00030996
                  if (shiftAvg <= 0.00032121) {
                    return 3;
                  } else {  // if shiftAvg > 0.00032121
                    if (confidence <= 0.28767) {
                      return 3;
                    } else {  // if confidence > 0.28767
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 0.53243
                if (maxEdgeLength <= 13.877) {
                  if (area <= 84.17) {
                    if (confidence <= 0.27272) {
                      return 2;
                    } else {  // if confidence > 0.27272
                      return 2;
                    }
                  } else {  // if area > 84.17
                    if (maxEdgeLength <= 13.45) {
                      return 2;
                    } else {  // if maxEdgeLength > 13.45
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 13.877
                  if (xyRatio <= 0.36882) {
                    return 2;
                  } else {  // if xyRatio > 0.36882
                    if (xyRatio <= 0.42105) {
                      return 3;
                    } else {  // if xyRatio > 0.42105
                      return 3;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.31839
              if (area <= 74.635) {
                if (avgErr <= 0.45486) {
                  return 2;
                } else {  // if avgErr > 0.45486
                  if (area <= 72.411) {
                    if (confidence <= 0.32746) {
                      return 2;
                    } else {  // if confidence > 0.32746
                      return 2;
                    }
                  } else {  // if area > 72.411
                    if (confidence <= 0.33424) {
                      return 3;
                    } else {  // if confidence > 0.33424
                      return 2;
                    }
                  }
                }
              } else {  // if area > 74.635
                if (maxEdgeLength <= 13.469) {
                  if (confidence <= 0.31921) {
                    if (area <= 80.599) {
                      return 3;
                    } else {  // if area > 80.599
                      return 2;
                    }
                  } else {  // if confidence > 0.31921
                    return 2;
                  }
                } else {  // if maxEdgeLength > 13.469
                  if (confidence <= 0.31976) {
                    return 2;
                  } else {  // if confidence > 0.31976
                    if (area <= 81.417) {
                      return 2;
                    } else {  // if area > 81.417
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 86.728
            if (xyRatio <= 0.43994) {
              if (maxEdgeLength <= 15.06) {
                if (avgErr <= 1.8782) {
                  if (avgErr <= 1.3381) {
                    if (xyRatio <= 0.41166) {
                      return 3;
                    } else {  // if xyRatio > 0.41166
                      return 2;
                    }
                  } else {  // if avgErr > 1.3381
                    if (maxEdgeLength <= 14.365) {
                      return 2;
                    } else {  // if maxEdgeLength > 14.365
                      return 2;
                    }
                  }
                } else {  // if avgErr > 1.8782
                  return 3;
                }
              } else {  // if maxEdgeLength > 15.06
                return 2;
              }
            } else {  // if xyRatio > 0.43994
              if (shiftStd <= 0.00022378) {
                if (shiftAvg <= 3.6012e-05) {
                  if (xyRatio <= 0.45875) {
                    return 3;
                  } else {  // if xyRatio > 0.45875
                    if (avgErr <= 0.2792) {
                      return 2;
                    } else {  // if avgErr > 0.2792
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 3.6012e-05
                  if (shiftAvg <= 0.0058592) {
                    if (confidence <= 0.27772) {
                      return 2;
                    } else {  // if confidence > 0.27772
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.0058592
                    if (area <= 87.151) {
                      return 3;
                    } else {  // if area > 87.151
                      return 2;
                    }
                  }
                }
              } else {  // if shiftStd > 0.00022378
                if (maxEdgeLength <= 13.991) {
                  return 3;
                } else {  // if maxEdgeLength > 13.991
                  return 2;
                }
              }
            }
          }
        }
      } else {  // if area > 89.973
        if (confidence <= 0.23459) {
          if (maxEdgeLength <= 52.263) {
            if (xyRatio <= 0.46818) {
              if (maxEdgeLength <= 14.385) {
                if (confidence <= 0.20795) {
                  if (shiftAvg <= 0.0049) {
                    if (confidence <= 0.1968) {
                      return 2;
                    } else {  // if confidence > 0.1968
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.0049
                    if (confidence <= 0.20024) {
                      return 2;
                    } else {  // if confidence > 0.20024
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.20795
                  if (shiftAvg <= 0.015844) {
                    if (avgErr <= 1.3242) {
                      return 2;
                    } else {  // if avgErr > 1.3242
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.015844
                    if (avgErr <= 1.4287) {
                      return 2;
                    } else {  // if avgErr > 1.4287
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 14.385
                if (xyRatio <= 0.31445) {
                  if (shiftNcc <= 0.065535) {
                    return 3;
                  } else {  // if shiftNcc > 0.065535
                    if (maxEdgeLength <= 51.125) {
                      return 2;
                    } else {  // if maxEdgeLength > 51.125
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.31445
                  if (xyRatio <= 0.42892) {
                    if (maxEdgeLength <= 50.226) {
                      return 3;
                    } else {  // if maxEdgeLength > 50.226
                      return 2;
                    }
                  } else {  // if xyRatio > 0.42892
                    if (confidence <= 0.21248) {
                      return 3;
                    } else {  // if confidence > 0.21248
                      return 3;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.46818
              if (confidence <= 0.21718) {
                if (xyRatio <= 0.50924) {
                  if (confidence <= 0.21284) {
                    if (area <= 101.86) {
                      return 3;
                    } else {  // if area > 101.86
                      return 3;
                    }
                  } else {  // if confidence > 0.21284
                    if (shiftAvg <= 0.00030146) {
                      return 2;
                    } else {  // if shiftAvg > 0.00030146
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.50924
                  if (maxEdgeLength <= 13.883) {
                    if (confidence <= 0.21636) {
                      return 3;
                    } else {  // if confidence > 0.21636
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 13.883
                    if (maxEdgeLength <= 13.911) {
                      return 2;
                    } else {  // if maxEdgeLength > 13.911
                      return 2;
                    }
                  }
                }
              } else {  // if confidence > 0.21718
                if (xyRatio <= 0.4861) {
                  if (area <= 102.28) {
                    if (maxEdgeLength <= 13.771) {
                      return 3;
                    } else {  // if maxEdgeLength > 13.771
                      return 2;
                    }
                  } else {  // if area > 102.28
                    if (shiftNcc <= 0.9937) {
                      return 3;
                    } else {  // if shiftNcc > 0.9937
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.4861
                  if (maxEdgeLength <= 13.634) {
                    if (maxEdgeLength <= 12.998) {
                      return 2;
                    } else {  // if maxEdgeLength > 12.998
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 13.634
                    if (avgErr <= 1.1076) {
                      return 2;
                    } else {  // if avgErr > 1.1076
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 52.263
            if (area <= 1250.8) {
              if (shiftAvg <= 9.71) {
                if (avgErr <= 2.8295) {
                  if (xyRatio <= 0.37596) {
                    if (shiftAvg <= 0.14623) {
                      return 2;
                    } else {  // if shiftAvg > 0.14623
                      return 1;
                    }
                  } else {  // if xyRatio > 0.37596
                    return 3;
                  }
                } else {  // if avgErr > 2.8295
                  if (avgErr <= 4.4167) {
                    if (xyRatio <= 0.17872) {
                      return 3;
                    } else {  // if xyRatio > 0.17872
                      return 2;
                    }
                  } else {  // if avgErr > 4.4167
                    if (maxEdgeLength <= 54.339) {
                      return 3;
                    } else {  // if maxEdgeLength > 54.339
                      return 2;
                    }
                  }
                }
              } else {  // if shiftAvg > 9.71
                return 1;
              }
            } else {  // if area > 1250.8
              if (shiftStd <= 0.020768) {
                if (maxEdgeLength <= 81.215) {
                  if (avgErr <= 0.77359) {
                    if (confidence <= 0.22445) {
                      return 2;
                    } else {  // if confidence > 0.22445
                      return 1;
                    }
                  } else {  // if avgErr > 0.77359
                    if (confidence <= 0.22639) {
                      return 1;
                    } else {  // if confidence > 0.22639
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 81.215
                  if (xyRatio <= 0.49147) {
                    if (xyRatio <= 0.27996) {
                      return 2;
                    } else {  // if xyRatio > 0.27996
                      return 1;
                    }
                  } else {  // if xyRatio > 0.49147
                    return 3;
                  }
                }
              } else {  // if shiftStd > 0.020768
                if (area <= 6144.3) {
                  if (avgErr <= 0.72018) {
                    if (xyRatio <= 0.46338) {
                      return 2;
                    } else {  // if xyRatio > 0.46338
                      return 1;
                    }
                  } else {  // if avgErr > 0.72018
                    if (area <= 2512.1) {
                      return 1;
                    } else {  // if area > 2512.1
                      return 2;
                    }
                  }
                } else {  // if area > 6144.3
                  if (shiftAvg <= 0.8898) {
                    if (shiftStd <= 0.44374) {
                      return 1;
                    } else {  // if shiftStd > 0.44374
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.8898
                    if (shiftNcc <= 0.70027) {
                      return 2;
                    } else {  // if shiftNcc > 0.70027
                      return 3;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.23459
          if (area <= 1164.3) {
            if (confidence <= 0.27397) {
              if (xyRatio <= 0.46306) {
                if (maxEdgeLength <= 50.028) {
                  if (shiftAvg <= 0.30316) {
                    if (maxEdgeLength <= 14.394) {
                      return 2;
                    } else {  // if maxEdgeLength > 14.394
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.30316
                    if (xyRatio <= 0.43991) {
                      return 2;
                    } else {  // if xyRatio > 0.43991
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 50.028
                  if (maxEdgeLength <= 64.091) {
                    if (xyRatio <= 0.38097) {
                      return 2;
                    } else {  // if xyRatio > 0.38097
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 64.091
                    if (area <= 1090.2) {
                      return 2;
                    } else {  // if area > 1090.2
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.46306
                if (avgErr <= 0.83854) {
                  if (xyRatio <= 0.49393) {
                    if (area <= 97.806) {
                      return 2;
                    } else {  // if area > 97.806
                      return 3;
                    }
                  } else {  // if xyRatio > 0.49393
                    if (confidence <= 0.23809) {
                      return 2;
                    } else {  // if confidence > 0.23809
                      return 2;
                    }
                  }
                } else {  // if avgErr > 0.83854
                  if (confidence <= 0.23528) {
                    if (xyRatio <= 0.48847) {
                      return 3;
                    } else {  // if xyRatio > 0.48847
                      return 2;
                    }
                  } else {  // if confidence > 0.23528
                    if (confidence <= 0.24608) {
                      return 2;
                    } else {  // if confidence > 0.24608
                      return 2;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.27397
              if (xyRatio <= 0.32797) {
                if (shiftStd <= 0.1181) {
                  if (area <= 1113.8) {
                    if (shiftAvg <= 0.28694) {
                      return 2;
                    } else {  // if shiftAvg > 0.28694
                      return 1;
                    }
                  } else {  // if area > 1113.8
                    if (shiftStd <= 0.027205) {
                      return 2;
                    } else {  // if shiftStd > 0.027205
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 0.1181
                  if (confidence <= 0.28387) {
                    return 3;
                  } else {  // if confidence > 0.28387
                    if (xyRatio <= 0.25579) {
                      return 2;
                    } else {  // if xyRatio > 0.25579
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.32797
                if (xyRatio <= 0.47734) {
                  if (avgErr <= 1.703) {
                    if (maxEdgeLength <= 14.886) {
                      return 2;
                    } else {  // if maxEdgeLength > 14.886
                      return 3;
                    }
                  } else {  // if avgErr > 1.703
                    if (shiftAvg <= 0.065025) {
                      return 2;
                    } else {  // if shiftAvg > 0.065025
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.47734
                  if (maxEdgeLength <= 42.433) {
                    if (confidence <= 0.29973) {
                      return 2;
                    } else {  // if confidence > 0.29973
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 42.433
                    if (xyRatio <= 0.53627) {
                      return 1;
                    } else {  // if xyRatio > 0.53627
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 1164.3
            if (maxEdgeLength <= 75.965) {
              if (avgErr <= 6.1642) {
                if (shiftAvg <= 1.3485) {
                  if (area <= 1829.8) {
                    if (avgErr <= 0.50026) {
                      return 2;
                    } else {  // if avgErr > 0.50026
                      return 2;
                    }
                  } else {  // if area > 1829.8
                    if (area <= 2116.5) {
                      return 1;
                    } else {  // if area > 2116.5
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 1.3485
                  if (shiftNcc <= 0.99148) {
                    if (xyRatio <= 0.50909) {
                      return 1;
                    } else {  // if xyRatio > 0.50909
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.99148
                    if (xyRatio <= 0.55341) {
                      return 3;
                    } else {  // if xyRatio > 0.55341
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 6.1642
                if (area <= 1291.5) {
                  if (xyRatio <= 0.26499) {
                    return 2;
                  } else {  // if xyRatio > 0.26499
                    if (xyRatio <= 1.296) {
                      return 1;
                    } else {  // if xyRatio > 1.296
                      return 2;
                    }
                  }
                } else {  // if area > 1291.5
                  if (confidence <= 0.29301) {
                    if (shiftStd <= 1.3484) {
                      return 3;
                    } else {  // if shiftStd > 1.3484
                      return 2;
                    }
                  } else {  // if confidence > 0.29301
                    if (confidence <= 0.32188) {
                      return 1;
                    } else {  // if confidence > 0.32188
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 75.965
              if (shiftStd <= 0.038427) {
                if (xyRatio <= 0.37073) {
                  if (maxEdgeLength <= 135.49) {
                    if (shiftNcc <= 0.11689) {
                      return 1;
                    } else {  // if shiftNcc > 0.11689
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 135.49
                    return 2;
                  }
                } else {  // if xyRatio > 0.37073
                  if (area <= 4355.3) {
                    if (maxEdgeLength <= 76.12) {
                      return 3;
                    } else {  // if maxEdgeLength > 76.12
                      return 1;
                    }
                  } else {  // if area > 4355.3
                    if (xyRatio <= 0.4762) {
                      return 2;
                    } else {  // if xyRatio > 0.4762
                      return 1;
                    }
                  }
                }
              } else {  // if shiftStd > 0.038427
                if (shiftNcc <= 0.87076) {
                  if (confidence <= 0.28593) {
                    if (shiftAvg <= 0.40918) {
                      return 2;
                    } else {  // if shiftAvg > 0.40918
                      return 3;
                    }
                  } else {  // if confidence > 0.28593
                    if (area <= 38505) {
                      return 1;
                    } else {  // if area > 38505
                      return 3;
                    }
                  }
                } else {  // if shiftNcc > 0.87076
                  if (area <= 8605.2) {
                    if (shiftAvg <= 10.984) {
                      return 2;
                    } else {  // if shiftAvg > 10.984
                      return 0;
                    }
                  } else {  // if area > 8605.2
                    return 3;
                  }
                }
              }
            }
          }
        }
      }
    }
  } else {  // if confidence > 0.35362
    if (confidence <= 0.73615) {
      if (maxEdgeLength <= 66.245) {
        if (confidence <= 0.65881) {
          if (area <= 1156.9) {
            if (avgErr <= 5.0569) {
              if (area <= 1042.8) {
                if (maxEdgeLength <= 65.816) {
                  if (confidence <= 0.61657) {
                    if (maxEdgeLength <= 64.927) {
                      return 2;
                    } else {  // if maxEdgeLength > 64.927
                      return 2;
                    }
                  } else {  // if confidence > 0.61657
                    if (xyRatio <= 0.25055) {
                      return 2;
                    } else {  // if xyRatio > 0.25055
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 65.816
                  if (area <= 1014) {
                    if (avgErr <= 1.4107) {
                      return 3;
                    } else {  // if avgErr > 1.4107
                      return 2;
                    }
                  } else {  // if area > 1014
                    return 3;
                  }
                }
              } else {  // if area > 1042.8
                if (shiftStd <= 0.0037694) {
                  if (shiftStd <= 0.0035042) {
                    if (avgErr <= 0.037126) {
                      return 1;
                    } else {  // if avgErr > 0.037126
                      return 2;
                    }
                  } else {  // if shiftStd > 0.0035042
                    if (confidence <= 0.51806) {
                      return 1;
                    } else {  // if confidence > 0.51806
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.0037694
                  if (confidence <= 0.63696) {
                    if (avgErr <= 0.21733) {
                      return 1;
                    } else {  // if avgErr > 0.21733
                      return 2;
                    }
                  } else {  // if confidence > 0.63696
                    if (shiftStd <= 0.036011) {
                      return 2;
                    } else {  // if shiftStd > 0.036011
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgErr > 5.0569
              if (area <= 476.73) {
                if (shiftNcc <= 0.99375) {
                  if (shiftAvg <= 0.94543) {
                    if (shiftStd <= 0.006181) {
                      return 3;
                    } else {  // if shiftStd > 0.006181
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.94543
                    if (xyRatio <= 0.68683) {
                      return 1;
                    } else {  // if xyRatio > 0.68683
                      return 2;
                    }
                  }
                } else {  // if shiftNcc > 0.99375
                  return 3;
                }
              } else {  // if area > 476.73
                if (maxEdgeLength <= 56.835) {
                  if (xyRatio <= 0.35124) {
                    if (confidence <= 0.55421) {
                      return 2;
                    } else {  // if confidence > 0.55421
                      return 1;
                    }
                  } else {  // if xyRatio > 0.35124
                    if (shiftStd <= 0.028423) {
                      return 2;
                    } else {  // if shiftStd > 0.028423
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 56.835
                  if (area <= 1147.7) {
                    if (shiftAvg <= 0.19734) {
                      return 2;
                    } else {  // if shiftAvg > 0.19734
                      return 2;
                    }
                  } else {  // if area > 1147.7
                    if (maxEdgeLength <= 61.452) {
                      return 2;
                    } else {  // if maxEdgeLength > 61.452
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 1156.9
            if (confidence <= 0.4322) {
              if (area <= 1395.2) {
                if (shiftAvg <= 0.041393) {
                  if (xyRatio <= 0.27968) {
                    if (area <= 1185.9) {
                      return 1;
                    } else {  // if area > 1185.9
                      return 2;
                    }
                  } else {  // if xyRatio > 0.27968
                    if (maxEdgeLength <= 66.131) {
                      return 2;
                    } else {  // if maxEdgeLength > 66.131
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.041393
                  if (xyRatio <= 0.3265) {
                    if (confidence <= 0.35988) {
                      return 2;
                    } else {  // if confidence > 0.35988
                      return 1;
                    }
                  } else {  // if xyRatio > 0.3265
                    if (maxEdgeLength <= 61.988) {
                      return 2;
                    } else {  // if maxEdgeLength > 61.988
                      return 2;
                    }
                  }
                }
              } else {  // if area > 1395.2
                if (xyRatio <= 0.41473) {
                  if (area <= 1587.5) {
                    if (shiftAvg <= 0.0026383) {
                      return 1;
                    } else {  // if shiftAvg > 0.0026383
                      return 2;
                    }
                  } else {  // if area > 1587.5
                    if (shiftAvg <= 0.049767) {
                      return 3;
                    } else {  // if shiftAvg > 0.049767
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.41473
                  if (shiftStd <= 0.015604) {
                    if (avgErr <= 0.60982) {
                      return 2;
                    } else {  // if avgErr > 0.60982
                      return 3;
                    }
                  } else {  // if shiftStd > 0.015604
                    if (avgErr <= 10.944) {
                      return 2;
                    } else {  // if avgErr > 10.944
                      return 1;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.4322
              if (area <= 1313.4) {
                if (maxEdgeLength <= 64.326) {
                  if (maxEdgeLength <= 61.368) {
                    if (xyRatio <= 0.34361) {
                      return 1;
                    } else {  // if xyRatio > 0.34361
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 61.368
                    if (maxEdgeLength <= 63.493) {
                      return 2;
                    } else {  // if maxEdgeLength > 63.493
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 64.326
                  if (shiftStd <= 0.025536) {
                    if (maxEdgeLength <= 65.455) {
                      return 2;
                    } else {  // if maxEdgeLength > 65.455
                      return 1;
                    }
                  } else {  // if shiftStd > 0.025536
                    if (xyRatio <= 0.28926) {
                      return 2;
                    } else {  // if xyRatio > 0.28926
                      return 2;
                    }
                  }
                }
              } else {  // if area > 1313.4
                if (maxEdgeLength <= 63.081) {
                  if (shiftAvg <= 23.95) {
                    if (shiftStd <= 0.0042853) {
                      return 2;
                    } else {  // if shiftStd > 0.0042853
                      return 2;
                    }
                  } else {  // if shiftAvg > 23.95
                    if (maxEdgeLength <= 49.182) {
                      return 1;
                    } else {  // if maxEdgeLength > 49.182
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 63.081
                  if (confidence <= 0.44201) {
                    if (confidence <= 0.43624) {
                      return 1;
                    } else {  // if confidence > 0.43624
                      return 3;
                    }
                  } else {  // if confidence > 0.44201
                    if (confidence <= 0.6195) {
                      return 2;
                    } else {  // if confidence > 0.6195
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.65881
          if (confidence <= 0.71746) {
            if (avgErr <= 13.586) {
              if (area <= 1779.7) {
                if (area <= 647.64) {
                  if (maxEdgeLength <= 50.067) {
                    if (confidence <= 0.66132) {
                      return 1;
                    } else {  // if confidence > 0.66132
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 50.067
                    return 1;
                  }
                } else {  // if area > 647.64
                  if (xyRatio <= 0.42209) {
                    if (xyRatio <= 0.367) {
                      return 2;
                    } else {  // if xyRatio > 0.367
                      return 2;
                    }
                  } else {  // if xyRatio > 0.42209
                    if (area <= 1115.3) {
                      return 2;
                    } else {  // if area > 1115.3
                      return 1;
                    }
                  }
                }
              } else {  // if area > 1779.7
                if (maxEdgeLength <= 52.722) {
                  if (shiftStd <= 0.17816) {
                    if (confidence <= 0.68317) {
                      return 2;
                    } else {  // if confidence > 0.68317
                      return 2;
                    }
                  } else {  // if shiftStd > 0.17816
                    if (confidence <= 0.70413) {
                      return 1;
                    } else {  // if confidence > 0.70413
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 52.722
                  if (shiftStd <= 0.71249) {
                    if (shiftStd <= 0.07572) {
                      return 1;
                    } else {  // if shiftStd > 0.07572
                      return 1;
                    }
                  } else {  // if shiftStd > 0.71249
                    if (confidence <= 0.69779) {
                      return 2;
                    } else {  // if confidence > 0.69779
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 13.586
              if (area <= 1032.7) {
                if (maxEdgeLength <= 22.743) {
                  if (confidence <= 0.71408) {
                    return 2;
                  } else {  // if confidence > 0.71408
                    return 1;
                  }
                } else {  // if maxEdgeLength > 22.743
                  if (confidence <= 0.68124) {
                    if (shiftStd <= 0.25954) {
                      return 2;
                    } else {  // if shiftStd > 0.25954
                      return 1;
                    }
                  } else {  // if confidence > 0.68124
                    if (xyRatio <= 0.31244) {
                      return 1;
                    } else {  // if xyRatio > 0.31244
                      return 1;
                    }
                  }
                }
              } else {  // if area > 1032.7
                if (xyRatio <= 0.37679) {
                  if (xyRatio <= 0.31714) {
                    if (confidence <= 0.69378) {
                      return 1;
                    } else {  // if confidence > 0.69378
                      return 2;
                    }
                  } else {  // if xyRatio > 0.31714
                    if (maxEdgeLength <= 56.993) {
                      return 2;
                    } else {  // if maxEdgeLength > 56.993
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.37679
                  if (maxEdgeLength <= 56.148) {
                    if (xyRatio <= 0.38624) {
                      return 2;
                    } else {  // if xyRatio > 0.38624
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 56.148
                    if (xyRatio <= 0.38439) {
                      return 1;
                    } else {  // if xyRatio > 0.38439
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.71746
            if (xyRatio <= 0.41921) {
              if (xyRatio <= 0.33527) {
                if (shiftStd <= 0.039162) {
                  if (xyRatio <= 0.23447) {
                    if (area <= 890.18) {
                      return 2;
                    } else {  // if area > 890.18
                      return 1;
                    }
                  } else {  // if xyRatio > 0.23447
                    if (shiftAvg <= 0.046877) {
                      return 1;
                    } else {  // if shiftAvg > 0.046877
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.039162
                  if (shiftAvg <= 0.051919) {
                    if (xyRatio <= 0.28985) {
                      return 2;
                    } else {  // if xyRatio > 0.28985
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.051919
                    if (confidence <= 0.73432) {
                      return 1;
                    } else {  // if confidence > 0.73432
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.33527
                if (area <= 1190.4) {
                  if (area <= 876.49) {
                    if (maxEdgeLength <= 39.453) {
                      return 2;
                    } else {  // if maxEdgeLength > 39.453
                      return 1;
                    }
                  } else {  // if area > 876.49
                    if (xyRatio <= 0.40623) {
                      return 2;
                    } else {  // if xyRatio > 0.40623
                      return 2;
                    }
                  }
                } else {  // if area > 1190.4
                  if (maxEdgeLength <= 62.773) {
                    if (shiftStd <= 0.00097136) {
                      return 2;
                    } else {  // if shiftStd > 0.00097136
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 62.773
                    if (confidence <= 0.73134) {
                      return 2;
                    } else {  // if confidence > 0.73134
                      return 2;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.41921
              if (maxEdgeLength <= 35.578) {
                if (avgErr <= 27.361) {
                  if (confidence <= 0.7321) {
                    if (xyRatio <= 0.57538) {
                      return 2;
                    } else {  // if xyRatio > 0.57538
                      return 2;
                    }
                  } else {  // if confidence > 0.7321
                    if (xyRatio <= 0.66076) {
                      return 2;
                    } else {  // if xyRatio > 0.66076
                      return 1;
                    }
                  }
                } else {  // if avgErr > 27.361
                  return 1;
                }
              } else {  // if maxEdgeLength > 35.578
                if (area <= 1669) {
                  if (avgErr <= 1.1256) {
                    return 1;
                  } else {  // if avgErr > 1.1256
                    if (shiftAvg <= 0.016475) {
                      return 1;
                    } else {  // if shiftAvg > 0.016475
                      return 1;
                    }
                  }
                } else {  // if area > 1669
                  if (shiftStd <= 0.0045114) {
                    if (shiftNcc <= 0.98276) {
                      return 2;
                    } else {  // if shiftNcc > 0.98276
                      return 1;
                    }
                  } else {  // if shiftStd > 0.0045114
                    if (maxEdgeLength <= 53.546) {
                      return 2;
                    } else {  // if maxEdgeLength > 53.546
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 66.245
        if (confidence <= 0.59383) {
          if (area <= 1194.9) {
            if (area <= 1067.2) {
              if (shiftStd <= 0.025639) {
                if (confidence <= 0.55136) {
                  if (shiftStd <= 0.0095) {
                    if (avgErr <= 1.1314) {
                      return 2;
                    } else {  // if avgErr > 1.1314
                      return 3;
                    }
                  } else {  // if shiftStd > 0.0095
                    if (shiftStd <= 0.011204) {
                      return 2;
                    } else {  // if shiftStd > 0.011204
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.55136
                  if (maxEdgeLength <= 66.636) {
                    if (confidence <= 0.58477) {
                      return 3;
                    } else {  // if confidence > 0.58477
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 66.636
                    return 2;
                  }
                }
              } else {  // if shiftStd > 0.025639
                if (xyRatio <= 0.16684) {
                  if (confidence <= 0.36851) {
                    return 1;
                  } else {  // if confidence > 0.36851
                    if (shiftStd <= 0.27141) {
                      return 2;
                    } else {  // if shiftStd > 0.27141
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.16684
                  if (xyRatio <= 0.20687) {
                    if (area <= 941.8) {
                      return 1;
                    } else {  // if area > 941.8
                      return 1;
                    }
                  } else {  // if xyRatio > 0.20687
                    if (shiftStd <= 0.29492) {
                      return 2;
                    } else {  // if shiftStd > 0.29492
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 1067.2
              if (maxEdgeLength <= 66.469) {
                if (area <= 1180.9) {
                  return 1;
                } else {  // if area > 1180.9
                  return 2;
                }
              } else {  // if maxEdgeLength > 66.469
                if (xyRatio <= 0.14364) {
                  if (xyRatio <= 0.13302) {
                    return 2;
                  } else {  // if xyRatio > 0.13302
                    return 1;
                  }
                } else {  // if xyRatio > 0.14364
                  if (shiftStd <= 8.6804) {
                    if (area <= 1167.3) {
                      return 2;
                    } else {  // if area > 1167.3
                      return 2;
                    }
                  } else {  // if shiftStd > 8.6804
                    return 1;
                  }
                }
              }
            }
          } else {  // if area > 1194.9
            if (xyRatio <= 1.9901) {
              if (maxEdgeLength <= 135.2) {
                if (area <= 7578.2) {
                  if (xyRatio <= 0.55454) {
                    if (maxEdgeLength <= 109.74) {
                      return 1;
                    } else {  // if maxEdgeLength > 109.74
                      return 1;
                    }
                  } else {  // if xyRatio > 0.55454
                    if (confidence <= 0.58726) {
                      return 2;
                    } else {  // if confidence > 0.58726
                      return 1;
                    }
                  }
                } else {  // if area > 7578.2
                  if (shiftAvg <= 1.5002) {
                    if (maxEdgeLength <= 115.09) {
                      return 1;
                    } else {  // if maxEdgeLength > 115.09
                      return 1;
                    }
                  } else {  // if shiftAvg > 1.5002
                    if (confidence <= 0.50003) {
                      return 2;
                    } else {  // if confidence > 0.50003
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 135.2
                if (avgErr <= 1.9762) {
                  if (avgErr <= 1.5878) {
                    if (maxEdgeLength <= 139.35) {
                      return 1;
                    } else {  // if maxEdgeLength > 139.35
                      return 2;
                    }
                  } else {  // if avgErr > 1.5878
                    if (confidence <= 0.58099) {
                      return 2;
                    } else {  // if confidence > 0.58099
                      return 0;
                    }
                  }
                } else {  // if avgErr > 1.9762
                  if (avgErr <= 2.9624) {
                    if (xyRatio <= 0.2372) {
                      return 0;
                    } else {  // if xyRatio > 0.2372
                      return 1;
                    }
                  } else {  // if avgErr > 2.9624
                    if (area <= 6962.8) {
                      return 1;
                    } else {  // if area > 6962.8
                      return 0;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 1.9901
              if (shiftStd <= 0.065678) {
                if (confidence <= 0.40317) {
                  if (maxEdgeLength <= 275.62) {
                    if (xyRatio <= 2.01) {
                      return 1;
                    } else {  // if xyRatio > 2.01
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 275.62
                    if (avgErr <= 1.1074) {
                      return 1;
                    } else {  // if avgErr > 1.1074
                      return 3;
                    }
                  }
                } else {  // if confidence > 0.40317
                  if (shiftStd <= 0.0072152) {
                    if (maxEdgeLength <= 277.93) {
                      return 3;
                    } else {  // if maxEdgeLength > 277.93
                      return 1;
                    }
                  } else {  // if shiftStd > 0.0072152
                    if (shiftAvg <= 0.00014769) {
                      return 3;
                    } else {  // if shiftAvg > 0.00014769
                      return 1;
                    }
                  }
                }
              } else {  // if shiftStd > 0.065678
                if (area <= 37458) {
                  return 1;
                } else {  // if area > 37458
                  if (shiftNcc <= 0.12818) {
                    if (avgErr <= 1.7943) {
                      return 2;
                    } else {  // if avgErr > 1.7943
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.12818
                    return 3;
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.59383
          if (maxEdgeLength <= 136.11) {
            if (area <= 1940.9) {
              if (area <= 1165.8) {
                if (maxEdgeLength <= 67.121) {
                  if (area <= 1047.3) {
                    if (shiftAvg <= 0.00047162) {
                      return 2;
                    } else {  // if shiftAvg > 0.00047162
                      return 2;
                    }
                  } else {  // if area > 1047.3
                    if (shiftStd <= 0.0023907) {
                      return 1;
                    } else {  // if shiftStd > 0.0023907
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 67.121
                  if (area <= 1069.4) {
                    if (shiftNcc <= 0.87897) {
                      return 1;
                    } else {  // if shiftNcc > 0.87897
                      return 2;
                    }
                  } else {  // if area > 1069.4
                    if (shiftAvg <= 0.070684) {
                      return 2;
                    } else {  // if shiftAvg > 0.070684
                      return 1;
                    }
                  }
                }
              } else {  // if area > 1165.8
                if (xyRatio <= 0.3383) {
                  if (shiftAvg <= 0.38868) {
                    if (maxEdgeLength <= 68.032) {
                      return 1;
                    } else {  // if maxEdgeLength > 68.032
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.38868
                    if (area <= 1775.4) {
                      return 2;
                    } else {  // if area > 1775.4
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.3383
                  if (xyRatio <= 0.36781) {
                    if (maxEdgeLength <= 67.12) {
                      return 2;
                    } else {  // if maxEdgeLength > 67.12
                      return 2;
                    }
                  } else {  // if xyRatio > 0.36781
                    if (maxEdgeLength <= 70.081) {
                      return 1;
                    } else {  // if maxEdgeLength > 70.081
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 1940.9
              if (xyRatio <= 0.71774) {
                if (avgErr <= 3.0866) {
                  if (xyRatio <= 0.33439) {
                    if (avgErr <= 1.1185) {
                      return 1;
                    } else {  // if avgErr > 1.1185
                      return 1;
                    }
                  } else {  // if xyRatio > 0.33439
                    if (avgErr <= 1.4361) {
                      return 1;
                    } else {  // if avgErr > 1.4361
                      return 1;
                    }
                  }
                } else {  // if avgErr > 3.0866
                  if (shiftAvg <= 3.8329) {
                    if (area <= 3315.3) {
                      return 1;
                    } else {  // if area > 3315.3
                      return 1;
                    }
                  } else {  // if shiftAvg > 3.8329
                    if (avgErr <= 8.7167) {
                      return 1;
                    } else {  // if avgErr > 8.7167
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.71774
                if (maxEdgeLength <= 91.895) {
                  if (shiftAvg <= 6.2554) {
                    if (area <= 3394.2) {
                      return 1;
                    } else {  // if area > 3394.2
                      return 2;
                    }
                  } else {  // if shiftAvg > 6.2554
                    if (shiftNcc <= 0.97671) {
                      return 0;
                    } else {  // if shiftNcc > 0.97671
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 91.895
                  if (shiftStd <= 4.5345) {
                    if (xyRatio <= 0.72095) {
                      return 2;
                    } else {  // if xyRatio > 0.72095
                      return 1;
                    }
                  } else {  // if shiftStd > 4.5345
                    if (maxEdgeLength <= 108.12) {
                      return 2;
                    } else {  // if maxEdgeLength > 108.12
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 136.11
            if (area <= 9475.3) {
              if (xyRatio <= 0.46512) {
                if (xyRatio <= 0.31545) {
                  if (shiftStd <= 0.010183) {
                    if (confidence <= 0.67562) {
                      return 2;
                    } else {  // if confidence > 0.67562
                      return 1;
                    }
                  } else {  // if shiftStd > 0.010183
                    if (shiftAvg <= 0.31502) {
                      return 1;
                    } else {  // if shiftAvg > 0.31502
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.31545
                  if (xyRatio <= 0.40947) {
                    if (avgErr <= 3.5386) {
                      return 0;
                    } else {  // if avgErr > 3.5386
                      return 1;
                    }
                  } else {  // if xyRatio > 0.40947
                    if (shiftNcc <= 0.8054) {
                      return 1;
                    } else {  // if shiftNcc > 0.8054
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.46512
                if (maxEdgeLength <= 139.38) {
                  if (avgErr <= 2.0715) {
                    if (shiftAvg <= 9.7236e-05) {
                      return 1;
                    } else {  // if shiftAvg > 9.7236e-05
                      return 2;
                    }
                  } else {  // if avgErr > 2.0715
                    if (shiftAvg <= 0.04902) {
                      return 2;
                    } else {  // if shiftAvg > 0.04902
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 139.38
                  if (xyRatio <= 0.46653) {
                    return 2;
                  } else {  // if xyRatio > 0.46653
                    if (shiftStd <= 0.43722) {
                      return 1;
                    } else {  // if shiftStd > 0.43722
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 9475.3
              if (xyRatio <= 0.23631) {
                if (xyRatio <= 0.2055) {
                  return 0;
                } else {  // if xyRatio > 0.2055
                  if (maxEdgeLength <= 236.56) {
                    if (shiftAvg <= 0.22726) {
                      return 2;
                    } else {  // if shiftAvg > 0.22726
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 236.56
                    if (confidence <= 0.65846) {
                      return 2;
                    } else {  // if confidence > 0.65846
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.23631
                if (shiftAvg <= 3.7561) {
                  if (area <= 17570) {
                    if (maxEdgeLength <= 154.93) {
                      return 0;
                    } else {  // if maxEdgeLength > 154.93
                      return 0;
                    }
                  } else {  // if area > 17570
                    if (shiftStd <= 0.64903) {
                      return 2;
                    } else {  // if shiftStd > 0.64903
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 3.7561
                  if (shiftAvg <= 13.367) {
                    if (confidence <= 0.65051) {
                      return 3;
                    } else {  // if confidence > 0.65051
                      return 1;
                    }
                  } else {  // if shiftAvg > 13.367
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if confidence > 0.73615
      if (maxEdgeLength <= 122.13) {
        if (area <= 2821.9) {
          if (confidence <= 0.80397) {
            if (area <= 1916.4) {
              if (confidence <= 0.75621) {
                if (xyRatio <= 0.31648) {
                  if (xyRatio <= 0.23005) {
                    if (area <= 976.12) {
                      return 2;
                    } else {  // if area > 976.12
                      return 1;
                    }
                  } else {  // if xyRatio > 0.23005
                    if (shiftAvg <= 0.46286) {
                      return 1;
                    } else {  // if shiftAvg > 0.46286
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.31648
                  if (area <= 1180) {
                    if (xyRatio <= 0.41133) {
                      return 2;
                    } else {  // if xyRatio > 0.41133
                      return 1;
                    }
                  } else {  // if area > 1180
                    if (xyRatio <= 0.40454) {
                      return 1;
                    } else {  // if xyRatio > 0.40454
                      return 1;
                    }
                  }
                }
              } else {  // if confidence > 0.75621
                if (xyRatio <= 0.23399) {
                  if (shiftStd <= 0.0041782) {
                    if (area <= 1019) {
                      return 2;
                    } else {  // if area > 1019
                      return 1;
                    }
                  } else {  // if shiftStd > 0.0041782
                    if (maxEdgeLength <= 75.272) {
                      return 1;
                    } else {  // if maxEdgeLength > 75.272
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.23399
                  if (xyRatio <= 0.32162) {
                    if (shiftStd <= 0.01046) {
                      return 1;
                    } else {  // if shiftStd > 0.01046
                      return 1;
                    }
                  } else {  // if xyRatio > 0.32162
                    if (xyRatio <= 0.3943) {
                      return 1;
                    } else {  // if xyRatio > 0.3943
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 1916.4
              if (shiftStd <= 0.059841) {
                if (maxEdgeLength <= 106.9) {
                  if (maxEdgeLength <= 64.783) {
                    if (xyRatio <= 0.55646) {
                      return 1;
                    } else {  // if xyRatio > 0.55646
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 64.783
                    if (area <= 1989.4) {
                      return 1;
                    } else {  // if area > 1989.4
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 106.9
                  return 2;
                }
              } else {  // if shiftStd > 0.059841
                if (area <= 2232.8) {
                  if (avgErr <= 6.3607) {
                    if (avgErr <= 3.8131) {
                      return 1;
                    } else {  // if avgErr > 3.8131
                      return 1;
                    }
                  } else {  // if avgErr > 6.3607
                    if (confidence <= 0.80208) {
                      return 1;
                    } else {  // if confidence > 0.80208
                      return 1;
                    }
                  }
                } else {  // if area > 2232.8
                  if (shiftNcc <= 0.88868) {
                    if (maxEdgeLength <= 75.573) {
                      return 1;
                    } else {  // if maxEdgeLength > 75.573
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.88868
                    if (maxEdgeLength <= 84.526) {
                      return 1;
                    } else {  // if maxEdgeLength > 84.526
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.80397
            if (confidence <= 0.89108) {
              if (area <= 943.52) {
                if (maxEdgeLength <= 62.91) {
                  if (avgErr <= 8.5697) {
                    if (xyRatio <= 0.28699) {
                      return 1;
                    } else {  // if xyRatio > 0.28699
                      return 1;
                    }
                  } else {  // if avgErr > 8.5697
                    if (confidence <= 0.80819) {
                      return 1;
                    } else {  // if confidence > 0.80819
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 62.91
                  if (confidence <= 0.8043) {
                    return 1;
                  } else {  // if confidence > 0.8043
                    return 2;
                  }
                }
              } else {  // if area > 943.52
                if (confidence <= 0.8414) {
                  if (shiftStd <= 0.016565) {
                    if (maxEdgeLength <= 69.995) {
                      return 1;
                    } else {  // if maxEdgeLength > 69.995
                      return 1;
                    }
                  } else {  // if shiftStd > 0.016565
                    if (area <= 2316) {
                      return 1;
                    } else {  // if area > 2316
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.8414
                  if (area <= 1849.9) {
                    if (avgErr <= 2.4939) {
                      return 1;
                    } else {  // if avgErr > 2.4939
                      return 1;
                    }
                  } else {  // if area > 1849.9
                    if (xyRatio <= 0.29393) {
                      return 1;
                    } else {  // if xyRatio > 0.29393
                      return 1;
                    }
                  }
                }
              }
            } else {  // if confidence > 0.89108
              if (xyRatio <= 0.45733) {
                if (shiftAvg <= 0.2632) {
                  if (maxEdgeLength <= 77.54) {
                    if (area <= 2257) {
                      return 1;
                    } else {  // if area > 2257
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 77.54
                    if (confidence <= 0.91432) {
                      return 1;
                    } else {  // if confidence > 0.91432
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.2632
                  if (xyRatio <= 0.41303) {
                    if (shiftAvg <= 6.5081) {
                      return 1;
                    } else {  // if shiftAvg > 6.5081
                      return 1;
                    }
                  } else {  // if xyRatio > 0.41303
                    if (avgErr <= 4.9143) {
                      return 0;
                    } else {  // if avgErr > 4.9143
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.45733
                if (maxEdgeLength <= 64.672) {
                  if (area <= 2604.4) {
                    if (maxEdgeLength <= 59.452) {
                      return 1;
                    } else {  // if maxEdgeLength > 59.452
                      return 1;
                    }
                  } else {  // if area > 2604.4
                    if (avgErr <= 7.9814) {
                      return 0;
                    } else {  // if avgErr > 7.9814
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 64.672
                  if (confidence <= 0.89723) {
                    if (area <= 2769.8) {
                      return 1;
                    } else {  // if area > 2769.8
                      return 0;
                    }
                  } else {  // if confidence > 0.89723
                    if (area <= 2086.4) {
                      return 1;
                    } else {  // if area > 2086.4
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 2821.9
          if (avgErr <= 1.1341) {
            if (maxEdgeLength <= 114.87) {
              if (shiftStd <= 0.026712) {
                if (xyRatio <= 0.52792) {
                  if (area <= 4703) {
                    if (area <= 3212.2) {
                      return 0;
                    } else {  // if area > 3212.2
                      return 1;
                    }
                  } else {  // if area > 4703
                    if (xyRatio <= 0.39219) {
                      return 0;
                    } else {  // if xyRatio > 0.39219
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.52792
                  if (confidence <= 0.9015) {
                    if (area <= 2920.1) {
                      return 2;
                    } else {  // if area > 2920.1
                      return 1;
                    }
                  } else {  // if confidence > 0.9015
                    return 0;
                  }
                }
              } else {  // if shiftStd > 0.026712
                if (area <= 5201.5) {
                  if (area <= 3137.9) {
                    return 0;
                  } else {  // if area > 3137.9
                    if (maxEdgeLength <= 77.589) {
                      return 2;
                    } else {  // if maxEdgeLength > 77.589
                      return 1;
                    }
                  }
                } else {  // if area > 5201.5
                  if (xyRatio <= 0.49277) {
                    if (area <= 5670.3) {
                      return 1;
                    } else {  // if area > 5670.3
                      return 1;
                    }
                  } else {  // if xyRatio > 0.49277
                    if (area <= 5296.5) {
                      return 0;
                    } else {  // if area > 5296.5
                      return 1;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 114.87
              if (avgErr <= 0.66315) {
                if (area <= 4289.7) {
                  return 0;
                } else {  // if area > 4289.7
                  if (shiftAvg <= 1.0205e-06) {
                    if (shiftStd <= 5.0279e-06) {
                      return 1;
                    } else {  // if shiftStd > 5.0279e-06
                      return 0;
                    }
                  } else {  // if shiftAvg > 1.0205e-06
                    if (shiftNcc <= 0.3934) {
                      return 1;
                    } else {  // if shiftNcc > 0.3934
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 0.66315
                if (shiftAvg <= 0.00047141) {
                  if (maxEdgeLength <= 115.19) {
                    if (shiftStd <= 0.0054974) {
                      return 0;
                    } else {  // if shiftStd > 0.0054974
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 115.19
                    if (confidence <= 0.80721) {
                      return 1;
                    } else {  // if confidence > 0.80721
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.00047141
                  if (maxEdgeLength <= 117.97) {
                    if (shiftAvg <= 0.0041904) {
                      return 1;
                    } else {  // if shiftAvg > 0.0041904
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 117.97
                    if (maxEdgeLength <= 118.01) {
                      return 0;
                    } else {  // if maxEdgeLength > 118.01
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if avgErr > 1.1341
            if (confidence <= 0.8385) {
              if (xyRatio <= 0.99582) {
                if (xyRatio <= 0.388) {
                  if (avgErr <= 3.4716) {
                    if (shiftStd <= 0.031558) {
                      return 1;
                    } else {  // if shiftStd > 0.031558
                      return 1;
                    }
                  } else {  // if avgErr > 3.4716
                    if (confidence <= 0.81362) {
                      return 1;
                    } else {  // if confidence > 0.81362
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.388
                  if (area <= 3826.9) {
                    if (xyRatio <= 0.7036) {
                      return 1;
                    } else {  // if xyRatio > 0.7036
                      return 1;
                    }
                  } else {  // if area > 3826.9
                    if (maxEdgeLength <= 81.836) {
                      return 2;
                    } else {  // if maxEdgeLength > 81.836
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.99582
                if (area <= 4044.8) {
                  if (maxEdgeLength <= 59.624) {
                    return 2;
                  } else {  // if maxEdgeLength > 59.624
                    if (xyRatio <= 1.1933) {
                      return 1;
                    } else {  // if xyRatio > 1.1933
                      return 1;
                    }
                  }
                } else {  // if area > 4044.8
                  return 2;
                }
              }
            } else {  // if confidence > 0.8385
              if (shiftAvg <= 7.3969) {
                if (maxEdgeLength <= 116.2) {
                  if (shiftStd <= 0.042887) {
                    if (confidence <= 0.88423) {
                      return 0;
                    } else {  // if confidence > 0.88423
                      return 1;
                    }
                  } else {  // if shiftStd > 0.042887
                    if (xyRatio <= 0.50102) {
                      return 1;
                    } else {  // if xyRatio > 0.50102
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 116.2
                  if (shiftAvg <= 0.15549) {
                    if (xyRatio <= 0.32607) {
                      return 0;
                    } else {  // if xyRatio > 0.32607
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.15549
                    if (confidence <= 0.90006) {
                      return 1;
                    } else {  // if confidence > 0.90006
                      return 1;
                    }
                  }
                }
              } else {  // if shiftAvg > 7.3969
                if (avgErr <= 8.0817) {
                  if (avgErr <= 5.1106) {
                    if (maxEdgeLength <= 114.01) {
                      return 0;
                    } else {  // if maxEdgeLength > 114.01
                      return 0;
                    }
                  } else {  // if avgErr > 5.1106
                    if (xyRatio <= 0.4474) {
                      return 1;
                    } else {  // if xyRatio > 0.4474
                      return 0;
                    }
                  }
                } else {  // if avgErr > 8.0817
                  if (area <= 4548.3) {
                    if (xyRatio <= 0.42369) {
                      return 1;
                    } else {  // if xyRatio > 0.42369
                      return 1;
                    }
                  } else {  // if area > 4548.3
                    if (confidence <= 0.8883) {
                      return 1;
                    } else {  // if confidence > 0.8883
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 122.13
        if (confidence <= 0.86725) {
          if (maxEdgeLength <= 131.74) {
            if (avgErr <= 3.2916) {
              if (shiftAvg <= 0.99576) {
                if (confidence <= 0.84794) {
                  if (shiftAvg <= 0.18292) {
                    if (avgErr <= 1.8545) {
                      return 1;
                    } else {  // if avgErr > 1.8545
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.18292
                    if (xyRatio <= 0.47279) {
                      return 1;
                    } else {  // if xyRatio > 0.47279
                      return 0;
                    }
                  }
                } else {  // if confidence > 0.84794
                  if (maxEdgeLength <= 125.49) {
                    if (area <= 6530.1) {
                      return 1;
                    } else {  // if area > 6530.1
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 125.49
                    if (xyRatio <= 0.32142) {
                      return 1;
                    } else {  // if xyRatio > 0.32142
                      return 1;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.99576
                if (xyRatio <= 0.41847) {
                  if (shiftStd <= 0.8118) {
                    if (area <= 6242.4) {
                      return 0;
                    } else {  // if area > 6242.4
                      return 1;
                    }
                  } else {  // if shiftStd > 0.8118
                    if (shiftNcc <= 0.5784) {
                      return 1;
                    } else {  // if shiftNcc > 0.5784
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.41847
                  if (avgErr <= 2.5256) {
                    return 0;
                  } else {  // if avgErr > 2.5256
                    if (avgErr <= 2.6883) {
                      return 1;
                    } else {  // if avgErr > 2.6883
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgErr > 3.2916
              if (xyRatio <= 0.39579) {
                if (confidence <= 0.8519) {
                  if (xyRatio <= 0.36241) {
                    if (shiftAvg <= 1.1025) {
                      return 1;
                    } else {  // if shiftAvg > 1.1025
                      return 1;
                    }
                  } else {  // if xyRatio > 0.36241
                    if (confidence <= 0.8455) {
                      return 1;
                    } else {  // if confidence > 0.8455
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.8519
                  if (xyRatio <= 0.33446) {
                    if (avgErr <= 8.5383) {
                      return 1;
                    } else {  // if avgErr > 8.5383
                      return 1;
                    }
                  } else {  // if xyRatio > 0.33446
                    if (xyRatio <= 0.39424) {
                      return 1;
                    } else {  // if xyRatio > 0.39424
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.39579
                if (shiftAvg <= 2.3769) {
                  if (confidence <= 0.83737) {
                    if (area <= 7017.1) {
                      return 1;
                    } else {  // if area > 7017.1
                      return 1;
                    }
                  } else {  // if confidence > 0.83737
                    if (area <= 7503.6) {
                      return 1;
                    } else {  // if area > 7503.6
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 2.3769
                  if (avgErr <= 8.8235) {
                    if (area <= 7657.1) {
                      return 1;
                    } else {  // if area > 7657.1
                      return 0;
                    }
                  } else {  // if avgErr > 8.8235
                    if (maxEdgeLength <= 128.57) {
                      return 1;
                    } else {  // if maxEdgeLength > 128.57
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 131.74
            if (xyRatio <= 0.39604) {
              if (xyRatio <= 0.3298) {
                if (maxEdgeLength <= 154.45) {
                  if (confidence <= 0.84173) {
                    if (maxEdgeLength <= 139.74) {
                      return 1;
                    } else {  // if maxEdgeLength > 139.74
                      return 1;
                    }
                  } else {  // if confidence > 0.84173
                    if (maxEdgeLength <= 142.67) {
                      return 0;
                    } else {  // if maxEdgeLength > 142.67
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 154.45
                  if (xyRatio <= 0.23028) {
                    if (confidence <= 0.78474) {
                      return 2;
                    } else {  // if confidence > 0.78474
                      return 0;
                    }
                  } else {  // if xyRatio > 0.23028
                    if (maxEdgeLength <= 173.77) {
                      return 0;
                    } else {  // if maxEdgeLength > 173.77
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.3298
                if (shiftStd <= 0.02982) {
                  if (area <= 6312) {
                    if (confidence <= 0.77826) {
                      return 1;
                    } else {  // if confidence > 0.77826
                      return 0;
                    }
                  } else {  // if area > 6312
                    if (maxEdgeLength <= 141.46) {
                      return 0;
                    } else {  // if maxEdgeLength > 141.46
                      return 0;
                    }
                  }
                } else {  // if shiftStd > 0.02982
                  if (maxEdgeLength <= 162.34) {
                    if (avgErr <= 3.1213) {
                      return 0;
                    } else {  // if avgErr > 3.1213
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 162.34
                    if (area <= 9417.3) {
                      return 1;
                    } else {  // if area > 9417.3
                      return 0;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.39604
              if (xyRatio <= 0.48663) {
                if (shiftAvg <= 0.049475) {
                  if (maxEdgeLength <= 155.52) {
                    if (confidence <= 0.84865) {
                      return 1;
                    } else {  // if confidence > 0.84865
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 155.52
                    if (confidence <= 0.82285) {
                      return 0;
                    } else {  // if confidence > 0.82285
                      return 0;
                    }
                  }
                } else {  // if shiftAvg > 0.049475
                  if (shiftAvg <= 3.9264) {
                    if (maxEdgeLength <= 149.42) {
                      return 1;
                    } else {  // if maxEdgeLength > 149.42
                      return 0;
                    }
                  } else {  // if shiftAvg > 3.9264
                    if (area <= 7624.3) {
                      return 1;
                    } else {  // if area > 7624.3
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.48663
                if (xyRatio <= 0.55668) {
                  if (confidence <= 0.85504) {
                    if (confidence <= 0.81093) {
                      return 1;
                    } else {  // if confidence > 0.81093
                      return 0;
                    }
                  } else {  // if confidence > 0.85504
                    if (area <= 10594) {
                      return 0;
                    } else {  // if area > 10594
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.55668
                  if (shiftAvg <= 8.2827) {
                    if (shiftAvg <= 0.17245) {
                      return 1;
                    } else {  // if shiftAvg > 0.17245
                      return 1;
                    }
                  } else {  // if shiftAvg > 8.2827
                    if (confidence <= 0.85866) {
                      return 0;
                    } else {  // if confidence > 0.85866
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if confidence > 0.86725
          if (confidence <= 0.89918) {
            if (xyRatio <= 0.31314) {
              if (avgErr <= 4.0257) {
                if (maxEdgeLength <= 142.34) {
                  if (maxEdgeLength <= 127.39) {
                    if (xyRatio <= 0.27122) {
                      return 1;
                    } else {  // if xyRatio > 0.27122
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 127.39
                    if (confidence <= 0.89378) {
                      return 0;
                    } else {  // if confidence > 0.89378
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 142.34
                  if (xyRatio <= 0.297) {
                    if (area <= 6065.6) {
                      return 1;
                    } else {  // if area > 6065.6
                      return 0;
                    }
                  } else {  // if xyRatio > 0.297
                    if (shiftStd <= 0.079546) {
                      return 1;
                    } else {  // if shiftStd > 0.079546
                      return 0;
                    }
                  }
                }
              } else {  // if avgErr > 4.0257
                if (avgErr <= 6.5405) {
                  if (shiftStd <= 1.6005) {
                    if (shiftStd <= 0.12664) {
                      return 0;
                    } else {  // if shiftStd > 0.12664
                      return 0;
                    }
                  } else {  // if shiftStd > 1.6005
                    if (xyRatio <= 0.26421) {
                      return 1;
                    } else {  // if xyRatio > 0.26421
                      return 0;
                    }
                  }
                } else {  // if avgErr > 6.5405
                  if (confidence <= 0.89043) {
                    if (xyRatio <= 0.31261) {
                      return 1;
                    } else {  // if xyRatio > 0.31261
                      return 0;
                    }
                  } else {  // if confidence > 0.89043
                    if (maxEdgeLength <= 156.47) {
                      return 0;
                    } else {  // if maxEdgeLength > 156.47
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.31314
              if (area <= 8616.3) {
                if (xyRatio <= 0.37399) {
                  if (maxEdgeLength <= 125.41) {
                    if (confidence <= 0.88523) {
                      return 1;
                    } else {  // if confidence > 0.88523
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 125.41
                    if (maxEdgeLength <= 143.22) {
                      return 0;
                    } else {  // if maxEdgeLength > 143.22
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.37399
                  if (shiftAvg <= 0.083154) {
                    if (maxEdgeLength <= 137.65) {
                      return 1;
                    } else {  // if maxEdgeLength > 137.65
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.083154
                    if (shiftAvg <= 8.909) {
                      return 1;
                    } else {  // if shiftAvg > 8.909
                      return 0;
                    }
                  }
                }
              } else {  // if area > 8616.3
                if (shiftStd <= 0.011715) {
                  if (shiftStd <= 0.0064374) {
                    if (area <= 9047.3) {
                      return 0;
                    } else {  // if area > 9047.3
                      return 0;
                    }
                  } else {  // if shiftStd > 0.0064374
                    if (area <= 10488) {
                      return 0;
                    } else {  // if area > 10488
                      return 0;
                    }
                  }
                } else {  // if shiftStd > 0.011715
                  if (maxEdgeLength <= 139.06) {
                    if (shiftStd <= 0.26422) {
                      return 0;
                    } else {  // if shiftStd > 0.26422
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 139.06
                    if (maxEdgeLength <= 153.89) {
                      return 1;
                    } else {  // if maxEdgeLength > 153.89
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if confidence > 0.89918
            if (maxEdgeLength <= 136.67) {
              if (avgErr <= 3.8453) {
                if (area <= 6089.2) {
                  if (maxEdgeLength <= 129.03) {
                    if (maxEdgeLength <= 126.06) {
                      return 0;
                    } else {  // if maxEdgeLength > 126.06
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 129.03
                    if (xyRatio <= 0.31474) {
                      return 1;
                    } else {  // if xyRatio > 0.31474
                      return 0;
                    }
                  }
                } else {  // if area > 6089.2
                  if (maxEdgeLength <= 123.19) {
                    if (shiftAvg <= 4.9154) {
                      return 1;
                    } else {  // if shiftAvg > 4.9154
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 123.19
                    if (xyRatio <= 0.46157) {
                      return 0;
                    } else {  // if xyRatio > 0.46157
                      return 0;
                    }
                  }
                }
              } else {  // if avgErr > 3.8453
                if (shiftAvg <= 2.7617) {
                  if (shiftStd <= 0.057555) {
                    if (area <= 8320.4) {
                      return 1;
                    } else {  // if area > 8320.4
                      return 0;
                    }
                  } else {  // if shiftStd > 0.057555
                    if (shiftAvg <= 0.054992) {
                      return 1;
                    } else {  // if shiftAvg > 0.054992
                      return 0;
                    }
                  }
                } else {  // if shiftAvg > 2.7617
                  if (avgErr <= 6.6654) {
                    if (shiftAvg <= 19.823) {
                      return 0;
                    } else {  // if shiftAvg > 19.823
                      return 1;
                    }
                  } else {  // if avgErr > 6.6654
                    if (area <= 11230) {
                      return 0;
                    } else {  // if area > 11230
                      return 1;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 136.67
              if (maxEdgeLength <= 144.31) {
                if (shiftStd <= 0.011679) {
                  if (area <= 6432.1) {
                    return 0;
                  } else {  // if area > 6432.1
                    if (maxEdgeLength <= 143.57) {
                      return 1;
                    } else {  // if maxEdgeLength > 143.57
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 0.011679
                  if (area <= 10756) {
                    if (area <= 8941) {
                      return 1;
                    } else {  // if area > 8941
                      return 0;
                    }
                  } else {  // if area > 10756
                    if (shiftNcc <= 0.79052) {
                      return 1;
                    } else {  // if shiftNcc > 0.79052
                      return 0;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 144.31
                if (confidence <= 0.91491) {
                  if (maxEdgeLength <= 165.31) {
                    if (shiftAvg <= 1.0921) {
                      return 1;
                    } else {  // if shiftAvg > 1.0921
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 165.31
                    if (shiftStd <= 0.46465) {
                      return 0;
                    } else {  // if shiftStd > 0.46465
                      return 1;
                    }
                  }
                } else {  // if confidence > 0.91491
                  if (xyRatio <= 0.48306) {
                    if (xyRatio <= 0.44954) {
                      return 0;
                    } else {  // if xyRatio > 0.44954
                      return 1;
                    }
                  } else {  // if xyRatio > 0.48306
                    if (shiftStd <= 0.055543) {
                      return 0;
                    } else {  // if shiftStd > 0.055543
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
