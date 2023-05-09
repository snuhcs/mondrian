#include "mondrian/tree/MTA.hpp"

namespace md {

int MTA(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr) {
  if (xyRatio <= 1.0567) {
    if (maxEdgeLength <= 87.5) {
      if (area <= 5108) {
        if (area <= 4087.5) {
          if (maxEdgeLength <= 64.5) {
            if (area <= 2913) {
              if (area <= 2334.5) {
                if (area <= 1929.5) {
                  if (maxEdgeLength <= 51.5) {
                    if (xyRatio <= 0.95235) {
                      return 3;
                    } else {  // if xyRatio > 0.95235
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 51.5
                    if (avgErr <= 12.456) {
                      return 0;
                    } else {  // if avgErr > 12.456
                      return 3;
                    }
                  }
                } else {  // if area > 1929.5
                  if (maxEdgeLength <= 48.5) {
                    if (shiftAvg <= 0.020547) {
                      return 4;
                    } else {  // if shiftAvg > 0.020547
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 48.5
                    if (xyRatio <= 0.92939) {
                      return 0;
                    } else {  // if xyRatio > 0.92939
                      return 1;
                    }
                  }
                }
              } else {  // if area > 2334.5
                if (shiftNcc <= 0.53551) {
                  if (area <= 2807) {
                    if (avgErr <= 0.81892) {
                      return 1;
                    } else {  // if avgErr > 0.81892
                      return 4;
                    }
                  } else {  // if area > 2807
                    if (shiftAvg <= 0.00030559) {
                      return 4;
                    } else {  // if shiftAvg > 0.00030559
                      return 4;
                    }
                  }
                } else {  // if shiftNcc > 0.53551
                  if (shiftAvg <= 0.53711) {
                    if (shiftAvg <= 0.10565) {
                      return 1;
                    } else {  // if shiftAvg > 0.10565
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.53711
                    if (shiftStd <= 0.9212) {
                      return 4;
                    } else {  // if shiftStd > 0.9212
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 2913
              if (area <= 3686) {
                if (shiftStd <= 0.014357) {
                  if (avgErr <= 0.69765) {
                    if (shiftAvg <= 1.2401e-05) {
                      return 4;
                    } else {  // if shiftAvg > 1.2401e-05
                      return 2;
                    }
                  } else {  // if avgErr > 0.69765
                    if (shiftStd <= 0.0025375) {
                      return 4;
                    } else {  // if shiftStd > 0.0025375
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.014357
                  if (shiftAvg <= 1.3872) {
                    if (shiftAvg <= 0.098424) {
                      return 2;
                    } else {  // if shiftAvg > 0.098424
                      return 2;
                    }
                  } else {  // if shiftAvg > 1.3872
                    if (shiftAvg <= 18.135) {
                      return 4;
                    } else {  // if shiftAvg > 18.135
                      return 3;
                    }
                  }
                }
              } else {  // if area > 3686
                if (shiftStd <= 0.00256) {
                  if (xyRatio <= 0.96043) {
                    if (shiftStd <= 0.00016438) {
                      return 3;
                    } else {  // if shiftStd > 0.00016438
                      return 4;
                    }
                  } else {  // if xyRatio > 0.96043
                    if (avgErr <= 0.39897) {
                      return 4;
                    } else {  // if avgErr > 0.39897
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.00256
                  if (maxEdgeLength <= 61.5) {
                    if (shiftAvg <= 0.012568) {
                      return 4;
                    } else {  // if shiftAvg > 0.012568
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 61.5
                    if (shiftStd <= 1.4185) {
                      return 3;
                    } else {  // if shiftStd > 1.4185
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 64.5
            if (avgErr <= 5.4819) {
              if (area <= 2976) {
                if (avgErr <= 1.4296) {
                  if (area <= 2221) {
                    if (maxEdgeLength <= 71.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 71.5
                      return 2;
                    }
                  } else {  // if area > 2221
                    if (area <= 2742.5) {
                      return 1;
                    } else {  // if area > 2742.5
                      return 0;
                    }
                  }
                } else {  // if avgErr > 1.4296
                  if (shiftNcc <= 0.23832) {
                    if (shiftStd <= 0.00059733) {
                      return 1;
                    } else {  // if shiftStd > 0.00059733
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.23832
                    if (shiftNcc <= 0.32206) {
                      return 1;
                    } else {  // if shiftNcc > 0.32206
                      return 4;
                    }
                  }
                }
              } else {  // if area > 2976
                if (area <= 3447.5) {
                  if (avgErr <= 0.85967) {
                    if (maxEdgeLength <= 68.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 68.5
                      return 4;
                    }
                  } else {  // if avgErr > 0.85967
                    if (shiftStd <= 0.0042065) {
                      return 4;
                    } else {  // if shiftStd > 0.0042065
                      return 4;
                    }
                  }
                } else {  // if area > 3447.5
                  if (shiftStd <= 0.0081361) {
                    if (avgErr <= 1.1072) {
                      return 4;
                    } else {  // if avgErr > 1.1072
                      return 4;
                    }
                  } else {  // if shiftStd > 0.0081361
                    if (xyRatio <= 0.93135) {
                      return 4;
                    } else {  // if xyRatio > 0.93135
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 5.4819
              if (shiftStd <= 0.29156) {
                if (avgErr <= 9.5723) {
                  if (xyRatio <= 0.93135) {
                    if (xyRatio <= 0.62593) {
                      return 4;
                    } else {  // if xyRatio > 0.62593
                      return 0;
                    }
                  } else {  // if xyRatio > 0.93135
                    if (avgErr <= 6.363) {
                      return 3;
                    } else {  // if avgErr > 6.363
                      return 1;
                    }
                  }
                } else {  // if avgErr > 9.5723
                  if (shiftStd <= 0.098668) {
                    if (shiftStd <= 0.094734) {
                      return 4;
                    } else {  // if shiftStd > 0.094734
                      return 3;
                    }
                  } else {  // if shiftStd > 0.098668
                    if (xyRatio <= 0.71606) {
                      return 4;
                    } else {  // if xyRatio > 0.71606
                      return 4;
                    }
                  }
                }
              } else {  // if shiftStd > 0.29156
                if (xyRatio <= 0.92366) {
                  if (area <= 2780) {
                    if (shiftStd <= 4.6189) {
                      return 1;
                    } else {  // if shiftStd > 4.6189
                      return 3;
                    }
                  } else {  // if area > 2780
                    if (shiftAvg <= 0.0079564) {
                      return 0;
                    } else {  // if shiftAvg > 0.0079564
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.92366
                  if (area <= 3995.5) {
                    return 1;
                  } else {  // if area > 3995.5
                    if (area <= 4028) {
                      return 0;
                    } else {  // if area > 4028
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 4087.5
          if (shiftStd <= 0.0020045) {
            if (avgErr <= 0.47498) {
              if (avgErr <= 0.34429) {
                if (shiftNcc <= 0.07072) {
                  if (maxEdgeLength <= 66.5) {
                    return 1;
                  } else {  // if maxEdgeLength > 66.5
                    if (shiftNcc <= 0.056319) {
                      return 4;
                    } else {  // if shiftNcc > 0.056319
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.07072
                  if (area <= 5004) {
                    if (area <= 4237.5) {
                      return 3;
                    } else {  // if area > 4237.5
                      return 4;
                    }
                  } else {  // if area > 5004
                    return 2;
                  }
                }
              } else {  // if avgErr > 0.34429
                if (avgErr <= 0.40497) {
                  return 1;
                } else {  // if avgErr > 0.40497
                  if (xyRatio <= 0.98645) {
                    return 4;
                  } else {  // if xyRatio > 0.98645
                    if (shiftNcc <= 0.053791) {
                      return 1;
                    } else {  // if shiftNcc > 0.053791
                      return 3;
                    }
                  }
                }
              }
            } else {  // if avgErr > 0.47498
              if (shiftNcc <= -0.061546) {
                return 1;
              } else {  // if shiftNcc > -0.061546
                if (shiftAvg <= 0.00010574) {
                  if (shiftAvg <= 5.586e-07) {
                    if (avgErr <= 0.66125) {
                      return 1;
                    } else {  // if avgErr > 0.66125
                      return 4;
                    }
                  } else {  // if shiftAvg > 5.586e-07
                    if (avgErr <= 0.56189) {
                      return 4;
                    } else {  // if avgErr > 0.56189
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.00010574
                  if (area <= 4323) {
                    return 1;
                  } else {  // if area > 4323
                    return 4;
                  }
                }
              }
            }
          } else {  // if shiftStd > 0.0020045
            if (xyRatio <= 0.81998) {
              if (shiftStd <= 0.13091) {
                if (shiftAvg <= 3.7468e-05) {
                  return 1;
                } else {  // if shiftAvg > 3.7468e-05
                  if (shiftAvg <= 0.00041796) {
                    if (area <= 4826.5) {
                      return 4;
                    } else {  // if area > 4826.5
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.00041796
                    if (shiftStd <= 0.013712) {
                      return 1;
                    } else {  // if shiftStd > 0.013712
                      return 4;
                    }
                  }
                }
              } else {  // if shiftStd > 0.13091
                if (avgErr <= 7.6457) {
                  if (xyRatio <= 0.79873) {
                    if (avgErr <= 7.3039) {
                      return 4;
                    } else {  // if avgErr > 7.3039
                      return 4;
                    }
                  } else {  // if xyRatio > 0.79873
                    if (shiftNcc <= 0.29631) {
                      return 1;
                    } else {  // if shiftNcc > 0.29631
                      return 4;
                    }
                  }
                } else {  // if avgErr > 7.6457
                  if (shiftNcc <= -0.018046) {
                    return 4;
                  } else {  // if shiftNcc > -0.018046
                    if (shiftAvg <= 7.5723) {
                      return 4;
                    } else {  // if shiftAvg > 7.5723
                      return 4;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.81998
              if (area <= 4945) {
                if (shiftAvg <= 0.013749) {
                  if (maxEdgeLength <= 68.5) {
                    if (shiftStd <= 0.0061067) {
                      return 4;
                    } else {  // if shiftStd > 0.0061067
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 68.5
                    if (maxEdgeLength <= 72.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 72.5
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.013749
                  if (xyRatio <= 0.9124) {
                    if (maxEdgeLength <= 68.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 68.5
                      return 1;
                    }
                  } else {  // if xyRatio > 0.9124
                    if (maxEdgeLength <= 64.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 64.5
                      return 1;
                    }
                  }
                }
              } else {  // if area > 4945
                if (xyRatio <= 1.0214) {
                  if (maxEdgeLength <= 74.5) {
                    if (shiftAvg <= 0.50072) {
                      return 1;
                    } else {  // if shiftAvg > 0.50072
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 74.5
                    if (shiftAvg <= 0.11169) {
                      return 2;
                    } else {  // if shiftAvg > 0.11169
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 1.0214
                  if (shiftStd <= 0.04749) {
                    if (avgErr <= 1.0958) {
                      return 2;
                    } else {  // if avgErr > 1.0958
                      return 4;
                    }
                  } else {  // if shiftStd > 0.04749
                    if (shiftAvg <= 0.62281) {
                      return 2;
                    } else {  // if shiftAvg > 0.62281
                      return 4;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 5108
        if (maxEdgeLength <= 81.5) {
          if (shiftAvg <= 2.7671e-05) {
            if (avgErr <= 0.28746) {
              return 2;
            } else {  // if avgErr > 0.28746
              if (shiftAvg <= 3.334e-06) {
                return 2;
              } else {  // if shiftAvg > 3.334e-06
                return 4;
              }
            }
          } else {  // if shiftAvg > 2.7671e-05
            if (shiftAvg <= 0.47664) {
              if (xyRatio <= 0.94838) {
                if (area <= 5190) {
                  if (avgErr <= 9.3841) {
                    if (area <= 5141.5) {
                      return 2;
                    } else {  // if area > 5141.5
                      return 4;
                    }
                  } else {  // if avgErr > 9.3841
                    if (shiftNcc <= 0.25181) {
                      return 3;
                    } else {  // if shiftNcc > 0.25181
                      return 2;
                    }
                  }
                } else {  // if area > 5190
                  if (shiftNcc <= -0.020151) {
                    return 2;
                  } else {  // if shiftNcc > -0.020151
                    if (shiftAvg <= 0.0051396) {
                      return 4;
                    } else {  // if shiftAvg > 0.0051396
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.94838
                if (area <= 6279.5) {
                  if (avgErr <= 2.6352) {
                    if (avgErr <= 1.7743) {
                      return 2;
                    } else {  // if avgErr > 1.7743
                      return 4;
                    }
                  } else {  // if avgErr > 2.6352
                    if (area <= 5147.5) {
                      return 2;
                    } else {  // if area > 5147.5
                      return 2;
                    }
                  }
                } else {  // if area > 6279.5
                  if (shiftNcc <= 0.29593) {
                    if (shiftStd <= 0.27851) {
                      return 4;
                    } else {  // if shiftStd > 0.27851
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.29593
                    if (shiftAvg <= 0.39186) {
                      return 2;
                    } else {  // if shiftAvg > 0.39186
                      return 3;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.47664
              if (maxEdgeLength <= 80.5) {
                if (shiftAvg <= 17.238) {
                  if (shiftAvg <= 2.6784) {
                    if (area <= 6005) {
                      return 2;
                    } else {  // if area > 6005
                      return 2;
                    }
                  } else {  // if shiftAvg > 2.6784
                    if (avgErr <= 16.423) {
                      return 4;
                    } else {  // if avgErr > 16.423
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 17.238
                  return 2;
                }
              } else {  // if maxEdgeLength > 80.5
                return 4;
              }
            }
          }
        } else {  // if maxEdgeLength > 81.5
          if (xyRatio <= 0.91716) {
            if (shiftAvg <= 0.18279) {
              if (maxEdgeLength <= 84.5) {
                if (shiftAvg <= 0.00070407) {
                  return 4;
                } else {  // if shiftAvg > 0.00070407
                  if (shiftStd <= 0.11944) {
                    if (xyRatio <= 0.77252) {
                      return 3;
                    } else {  // if xyRatio > 0.77252
                      return 2;
                    }
                  } else {  // if shiftStd > 0.11944
                    if (area <= 6220.5) {
                      return 4;
                    } else {  // if area > 6220.5
                      return 3;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 84.5
                if (avgErr <= 18.106) {
                  if (shiftNcc <= 0.33557) {
                    if (avgErr <= 11.714) {
                      return 4;
                    } else {  // if avgErr > 11.714
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.33557
                    return 3;
                  }
                } else {  // if avgErr > 18.106
                  return 3;
                }
              }
            } else {  // if shiftAvg > 0.18279
              if (avgErr <= 14.058) {
                if (avgErr <= 5.923) {
                  if (shiftAvg <= 0.51103) {
                    if (shiftStd <= 0.12909) {
                      return 3;
                    } else {  // if shiftStd > 0.12909
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.51103
                    return 2;
                  }
                } else {  // if avgErr > 5.923
                  if (shiftNcc <= 0.34754) {
                    if (shiftAvg <= 0.24057) {
                      return 4;
                    } else {  // if shiftAvg > 0.24057
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.34754
                    return 4;
                  }
                }
              } else {  // if avgErr > 14.058
                if (xyRatio <= 0.89809) {
                  return 2;
                } else {  // if xyRatio > 0.89809
                  return 4;
                }
              }
            }
          } else {  // if xyRatio > 0.91716
            if (shiftNcc <= 0.79907) {
              if (avgErr <= 7.2262) {
                if (shiftStd <= 0.38674) {
                  if (shiftStd <= 0.28281) {
                    if (shiftAvg <= 0.061497) {
                      return 3;
                    } else {  // if shiftAvg > 0.061497
                      return 3;
                    }
                  } else {  // if shiftStd > 0.28281
                    if (shiftAvg <= 0.11113) {
                      return 3;
                    } else {  // if shiftAvg > 0.11113
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.38674
                  return 3;
                }
              } else {  // if avgErr > 7.2262
                if (xyRatio <= 0.97576) {
                  if (avgErr <= 13.613) {
                    return 4;
                  } else {  // if avgErr > 13.613
                    return 3;
                  }
                } else {  // if xyRatio > 0.97576
                  if (shiftNcc <= 0.54676) {
                    return 3;
                  } else {  // if shiftNcc > 0.54676
                    if (shiftNcc <= 0.55003) {
                      return 4;
                    } else {  // if shiftNcc > 0.55003
                      return 3;
                    }
                  }
                }
              }
            } else {  // if shiftNcc > 0.79907
              if (avgErr <= 5.2775) {
                return 3;
              } else {  // if avgErr > 5.2775
                if (shiftAvg <= 0.61516) {
                  if (shiftAvg <= 0.46465) {
                    if (shiftNcc <= 0.82053) {
                      return 3;
                    } else {  // if shiftNcc > 0.82053
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.46465
                    return 3;
                  }
                } else {  // if shiftAvg > 0.61516
                  return 4;
                }
              }
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 87.5
      if (xyRatio <= 0.89529) {
        if (xyRatio <= 0.76906) {
          if (shiftStd <= 50.636) {
            if (xyRatio <= 0.69857) {
              if (maxEdgeLength <= 105.5) {
                if (maxEdgeLength <= 104.5) {
                  return 4;
                } else {  // if maxEdgeLength > 104.5
                  return 3;
                }
              } else {  // if maxEdgeLength > 105.5
                return 4;
              }
            } else {  // if xyRatio > 0.69857
              if (xyRatio <= 0.69901) {
                return 3;
              } else {  // if xyRatio > 0.69901
                if (avgErr <= 5.7005) {
                  if (avgErr <= 5.5778) {
                    return 4;
                  } else {  // if avgErr > 5.5778
                    return 0;
                  }
                } else {  // if avgErr > 5.7005
                  if (shiftAvg <= 107.77) {
                    return 4;
                  } else {  // if shiftAvg > 107.77
                    if (shiftAvg <= 110.66) {
                      return 3;
                    } else {  // if shiftAvg > 110.66
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if shiftStd > 50.636
            if (avgErr <= 11.755) {
              return 1;
            } else {  // if avgErr > 11.755
              return 4;
            }
          }
        } else {  // if xyRatio > 0.76906
          if (area <= 14812) {
            if (shiftNcc <= 0.59862) {
              if (xyRatio <= 0.77775) {
                if (shiftNcc <= 0.44548) {
                  if (xyRatio <= 0.77205) {
                    return 1;
                  } else {  // if xyRatio > 0.77205
                    return 4;
                  }
                } else {  // if shiftNcc > 0.44548
                  return 2;
                }
              } else {  // if xyRatio > 0.77775
                if (shiftStd <= 0.68393) {
                  if (shiftNcc <= 0.27885) {
                    return 4;
                  } else {  // if shiftNcc > 0.27885
                    if (shiftNcc <= 0.30437) {
                      return 3;
                    } else {  // if shiftNcc > 0.30437
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.68393
                  if (area <= 11694) {
                    if (shiftNcc <= 0.54562) {
                      return 4;
                    } else {  // if shiftNcc > 0.54562
                      return 0;
                    }
                  } else {  // if area > 11694
                    if (avgErr <= 8.5449) {
                      return 3;
                    } else {  // if avgErr > 8.5449
                      return 2;
                    }
                  }
                }
              }
            } else {  // if shiftNcc > 0.59862
              if (avgErr <= 14.123) {
                if (avgErr <= 5.4924) {
                  if (avgErr <= 4.9548) {
                    return 4;
                  } else {  // if avgErr > 4.9548
                    return 1;
                  }
                } else {  // if avgErr > 5.4924
                  if (shiftStd <= 2.5685) {
                    return 4;
                  } else {  // if shiftStd > 2.5685
                    if (avgErr <= 9.8967) {
                      return 1;
                    } else {  // if avgErr > 9.8967
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 14.123
                if (maxEdgeLength <= 93.5) {
                  if (maxEdgeLength <= 89.5) {
                    return 2;
                  } else {  // if maxEdgeLength > 89.5
                    return 3;
                  }
                } else {  // if maxEdgeLength > 93.5
                  return 4;
                }
              }
            }
          } else {  // if area > 14812
            if (shiftAvg <= 12.52) {
              if (avgErr <= 8.4912) {
                return 4;
              } else {  // if avgErr > 8.4912
                if (shiftStd <= 2.9571) {
                  return 3;
                } else {  // if shiftStd > 2.9571
                  return 4;
                }
              }
            } else {  // if shiftAvg > 12.52
              if (avgErr <= 9.5413) {
                return 1;
              } else {  // if avgErr > 9.5413
                if (shiftAvg <= 259.07) {
                  if (maxEdgeLength <= 140.5) {
                    if (shiftNcc <= 0.98567) {
                      return 4;
                    } else {  // if shiftNcc > 0.98567
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 140.5
                    return 4;
                  }
                } else {  // if shiftAvg > 259.07
                  if (shiftNcc <= 0.91829) {
                    return 4;
                  } else {  // if shiftNcc > 0.91829
                    return 0;
                  }
                }
              }
            }
          }
        }
      } else {  // if xyRatio > 0.89529
        if (area <= 15124) {
          if (maxEdgeLength <= 97.5) {
            if (shiftNcc <= 0.46754) {
              if (xyRatio <= 0.97905) {
                if (shiftStd <= 0.15715) {
                  return 4;
                } else {  // if shiftStd > 0.15715
                  if (shiftAvg <= 0.54826) {
                    if (shiftAvg <= 0.13077) {
                      return 3;
                    } else {  // if shiftAvg > 0.13077
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.54826
                    return 3;
                  }
                }
              } else {  // if xyRatio > 0.97905
                if (avgErr <= 12.468) {
                  if (shiftStd <= 0.06526) {
                    if (shiftStd <= 0.028486) {
                      return 3;
                    } else {  // if shiftStd > 0.028486
                      return 4;
                    }
                  } else {  // if shiftStd > 0.06526
                    return 3;
                  }
                } else {  // if avgErr > 12.468
                  if (xyRatio <= 1.0105) {
                    return 3;
                  } else {  // if xyRatio > 1.0105
                    return 4;
                  }
                }
              }
            } else {  // if shiftNcc > 0.46754
              if (avgErr <= 5.595) {
                return 3;
              } else {  // if avgErr > 5.595
                if (avgErr <= 12.037) {
                  if (area <= 8274.5) {
                    return 4;
                  } else {  // if area > 8274.5
                    if (area <= 8415) {
                      return 3;
                    } else {  // if area > 8415
                      return 4;
                    }
                  }
                } else {  // if avgErr > 12.037
                  if (shiftAvg <= 0.14154) {
                    return 4;
                  } else {  // if shiftAvg > 0.14154
                    if (maxEdgeLength <= 94) {
                      return 4;
                    } else {  // if maxEdgeLength > 94
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 97.5
            if (area <= 11338) {
              if (avgErr <= 6.7621) {
                if (area <= 8962.5) {
                  return 4;
                } else {  // if area > 8962.5
                  if (shiftStd <= 0.24902) {
                    return 1;
                  } else {  // if shiftStd > 0.24902
                    if (shiftNcc <= 0.34477) {
                      return 3;
                    } else {  // if shiftNcc > 0.34477
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 6.7621
                if (xyRatio <= 1.0205) {
                  if (shiftAvg <= 14.724) {
                    if (shiftNcc <= 0.11733) {
                      return 0;
                    } else {  // if shiftNcc > 0.11733
                      return 4;
                    }
                  } else {  // if shiftAvg > 14.724
                    return 1;
                  }
                } else {  // if xyRatio > 1.0205
                  if (maxEdgeLength <= 100.5) {
                    return 1;
                  } else {  // if maxEdgeLength > 100.5
                    if (xyRatio <= 1.0488) {
                      return 1;
                    } else {  // if xyRatio > 1.0488
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 11338
              if (shiftAvg <= 5.329) {
                if (shiftAvg <= 0.44705) {
                  if (shiftStd <= 0.70853) {
                    if (xyRatio <= 1.0274) {
                      return 4;
                    } else {  // if xyRatio > 1.0274
                      return 2;
                    }
                  } else {  // if shiftStd > 0.70853
                    return 2;
                  }
                } else {  // if shiftAvg > 0.44705
                  if (maxEdgeLength <= 122.5) {
                    if (area <= 11552) {
                      return 1;
                    } else {  // if area > 11552
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 122.5
                    return 3;
                  }
                }
              } else {  // if shiftAvg > 5.329
                if (shiftStd <= 54.995) {
                  if (area <= 11594) {
                    if (shiftAvg <= 11.613) {
                      return 2;
                    } else {  // if shiftAvg > 11.613
                      return 4;
                    }
                  } else {  // if area > 11594
                    return 4;
                  }
                } else {  // if shiftStd > 54.995
                  return 2;
                }
              }
            }
          }
        } else {  // if area > 15124
          if (shiftAvg <= 15.955) {
            if (shiftAvg <= 1.3786) {
              if (area <= 21335) {
                if (xyRatio <= 0.96556) {
                  if (area <= 16242) {
                    return 3;
                  } else {  // if area > 16242
                    return 4;
                  }
                } else {  // if xyRatio > 0.96556
                  if (avgErr <= 18.193) {
                    return 3;
                  } else {  // if avgErr > 18.193
                    return 4;
                  }
                }
              } else {  // if area > 21335
                return 4;
              }
            } else {  // if shiftAvg > 1.3786
              if (area <= 15491) {
                if (shiftNcc <= 0.85771) {
                  return 3;
                } else {  // if shiftNcc > 0.85771
                  return 4;
                }
              } else {  // if area > 15491
                return 3;
              }
            }
          } else {  // if shiftAvg > 15.955
            if (shiftNcc <= 0.97496) {
              if (xyRatio <= 1.0427) {
                if (avgErr <= 11.561) {
                  if (avgErr <= 11.177) {
                    return 4;
                  } else {  // if avgErr > 11.177
                    return 3;
                  }
                } else {  // if avgErr > 11.561
                  if (area <= 17022) {
                    return 3;
                  } else {  // if area > 17022
                    if (shiftNcc <= 0.55717) {
                      return 3;
                    } else {  // if shiftNcc > 0.55717
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 1.0427
                return 2;
              }
            } else {  // if shiftNcc > 0.97496
              if (shiftAvg <= 208.48) {
                if (avgErr <= 16.306) {
                  if (area <= 22673) {
                    if (shiftNcc <= 0.99505) {
                      return 3;
                    } else {  // if shiftNcc > 0.99505
                      return 4;
                    }
                  } else {  // if area > 22673
                    if (shiftStd <= 0.34326) {
                      return 1;
                    } else {  // if shiftStd > 0.34326
                      return 3;
                    }
                  }
                } else {  // if avgErr > 16.306
                  return 4;
                }
              } else {  // if shiftAvg > 208.48
                if (maxEdgeLength <= 217.5) {
                  return 0;
                } else {  // if maxEdgeLength > 217.5
                  return 2;
                }
              }
            }
          }
        }
      }
    }
  } else {  // if xyRatio > 1.0567
    if (area <= 14529) {
      if (area <= 5071.5) {
        if (area <= 4087.5) {
          if (maxEdgeLength <= 64.5) {
            if (area <= 2903.5) {
              if (area <= 2348) {
                if (maxEdgeLength <= 48.5) {
                  if (area <= 1818.5) {
                    if (avgErr <= 2.7878) {
                      return 3;
                    } else {  // if avgErr > 2.7878
                      return 1;
                    }
                  } else {  // if area > 1818.5
                    if (avgErr <= 3.6168) {
                      return 3;
                    } else {  // if avgErr > 3.6168
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 48.5
                  if (shiftNcc <= 0.087832) {
                    if (area <= 1942.5) {
                      return 0;
                    } else {  // if area > 1942.5
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.087832
                    if (area <= 2247) {
                      return 0;
                    } else {  // if area > 2247
                      return 0;
                    }
                  }
                }
              } else {  // if area > 2348
                if (shiftStd <= 0.0069902) {
                  if (avgErr <= 0.75244) {
                    if (avgErr <= 0.28192) {
                      return 4;
                    } else {  // if avgErr > 0.28192
                      return 3;
                    }
                  } else {  // if avgErr > 0.75244
                    if (shiftAvg <= 0.0011398) {
                      return 4;
                    } else {  // if shiftAvg > 0.0011398
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 0.0069902
                  if (avgErr <= 7.8994) {
                    if (area <= 2811) {
                      return 1;
                    } else {  // if area > 2811
                      return 1;
                    }
                  } else {  // if avgErr > 7.8994
                    if (shiftStd <= 0.15476) {
                      return 1;
                    } else {  // if shiftStd > 0.15476
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 2903.5
              if (area <= 3559) {
                if (shiftStd <= 0.0057459) {
                  if (avgErr <= 0.66632) {
                    if (xyRatio <= 1.1524) {
                      return 4;
                    } else {  // if xyRatio > 1.1524
                      return 2;
                    }
                  } else {  // if avgErr > 0.66632
                    if (xyRatio <= 1.1554) {
                      return 4;
                    } else {  // if xyRatio > 1.1554
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.0057459
                  if (avgErr <= 2.5793) {
                    if (xyRatio <= 1.2001) {
                      return 2;
                    } else {  // if xyRatio > 1.2001
                      return 2;
                    }
                  } else {  // if avgErr > 2.5793
                    if (shiftNcc <= 0.4031) {
                      return 2;
                    } else {  // if shiftNcc > 0.4031
                      return 2;
                    }
                  }
                }
              } else {  // if area > 3559
                if (maxEdgeLength <= 63.5) {
                  if (xyRatio <= 1.0776) {
                    if (shiftStd <= 0.28603) {
                      return 3;
                    } else {  // if shiftStd > 0.28603
                      return 4;
                    }
                  } else {  // if xyRatio > 1.0776
                    if (shiftStd <= 0.02065) {
                      return 4;
                    } else {  // if shiftStd > 0.02065
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 63.5
                  if (shiftStd <= 0.0057323) {
                    if (avgErr <= 0.24905) {
                      return 3;
                    } else {  // if avgErr > 0.24905
                      return 4;
                    }
                  } else {  // if shiftStd > 0.0057323
                    if (shiftNcc <= 0.22786) {
                      return 3;
                    } else {  // if shiftNcc > 0.22786
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 64.5
            if (shiftStd <= 0.008474) {
              if (avgErr <= 0.58028) {
                if (shiftStd <= 6.734e-06) {
                  if (xyRatio <= 2.318) {
                    if (area <= 3564) {
                      return 4;
                    } else {  // if area > 3564
                      return 2;
                    }
                  } else {  // if xyRatio > 2.318
                    if (shiftNcc <= 0.12539) {
                      return 3;
                    } else {  // if shiftNcc > 0.12539
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 6.734e-06
                  if (avgErr <= 0.3365) {
                    if (xyRatio <= 1.082) {
                      return 4;
                    } else {  // if xyRatio > 1.082
                      return 0;
                    }
                  } else {  // if avgErr > 0.3365
                    if (shiftNcc <= -0.019636) {
                      return 0;
                    } else {  // if shiftNcc > -0.019636
                      return 0;
                    }
                  }
                }
              } else {  // if avgErr > 0.58028
                if (area <= 2142) {
                  if (avgErr <= 1.2761) {
                    return 3;
                  } else {  // if avgErr > 1.2761
                    if (shiftNcc <= 0.029287) {
                      return 4;
                    } else {  // if shiftNcc > 0.029287
                      return 0;
                    }
                  }
                } else {  // if area > 2142
                  if (shiftStd <= 0.0016146) {
                    if (shiftStd <= 0.0002274) {
                      return 4;
                    } else {  // if shiftStd > 0.0002274
                      return 4;
                    }
                  } else {  // if shiftStd > 0.0016146
                    if (xyRatio <= 2.0645) {
                      return 0;
                    } else {  // if xyRatio > 2.0645
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftStd > 0.008474
              if (area <= 3962.5) {
                if (xyRatio <= 2.0482) {
                  if (shiftNcc <= 0.050561) {
                    if (avgErr <= 4.9761) {
                      return 0;
                    } else {  // if avgErr > 4.9761
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.050561
                    if (avgErr <= 9.3676) {
                      return 0;
                    } else {  // if avgErr > 9.3676
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 2.0482
                  if (shiftStd <= 0.13018) {
                    if (area <= 2766) {
                      return 3;
                    } else {  // if area > 2766
                      return 0;
                    }
                  } else {  // if shiftStd > 0.13018
                    if (area <= 3250.5) {
                      return 4;
                    } else {  // if area > 3250.5
                      return 0;
                    }
                  }
                }
              } else {  // if area > 3962.5
                if (maxEdgeLength <= 75.5) {
                  if (maxEdgeLength <= 74.5) {
                    if (xyRatio <= 1.3065) {
                      return 0;
                    } else {  // if xyRatio > 1.3065
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 74.5
                    if (avgErr <= 8.8689) {
                      return 1;
                    } else {  // if avgErr > 8.8689
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 75.5
                  if (area <= 4083) {
                    if (shiftNcc <= 0.11579) {
                      return 0;
                    } else {  // if shiftNcc > 0.11579
                      return 0;
                    }
                  } else {  // if area > 4083
                    return 1;
                  }
                }
              }
            }
          }
        } else {  // if area > 4087.5
          if (shiftStd <= 0.006458) {
            if (avgErr <= 0.63331) {
              if (shiftStd <= 8.2597e-05) {
                if (area <= 4483.5) {
                  if (shiftAvg <= 4.3702e-07) {
                    if (shiftStd <= 2.4816e-05) {
                      return 4;
                    } else {  // if shiftStd > 2.4816e-05
                      return 0;
                    }
                  } else {  // if shiftAvg > 4.3702e-07
                    if (shiftStd <= 5.2652e-05) {
                      return 1;
                    } else {  // if shiftStd > 5.2652e-05
                      return 4;
                    }
                  }
                } else {  // if area > 4483.5
                  if (avgErr <= 0.2229) {
                    if (maxEdgeLength <= 85.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 85.5
                      return 1;
                    }
                  } else {  // if avgErr > 0.2229
                    if (avgErr <= 0.47632) {
                      return 3;
                    } else {  // if avgErr > 0.47632
                      return 1;
                    }
                  }
                }
              } else {  // if shiftStd > 8.2597e-05
                if (area <= 4886) {
                  if (area <= 4233) {
                    if (area <= 4111) {
                      return 1;
                    } else {  // if area > 4111
                      return 4;
                    }
                  } else {  // if area > 4233
                    if (shiftAvg <= 0.00029386) {
                      return 1;
                    } else {  // if shiftAvg > 0.00029386
                      return 4;
                    }
                  }
                } else {  // if area > 4886
                  if (xyRatio <= 1.5578) {
                    if (shiftStd <= 0.000223) {
                      return 4;
                    } else {  // if shiftStd > 0.000223
                      return 1;
                    }
                  } else {  // if xyRatio > 1.5578
                    if (avgErr <= 0.43809) {
                      return 4;
                    } else {  // if avgErr > 0.43809
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgErr > 0.63331
              if (shiftStd <= 0.00069358) {
                if (shiftStd <= 0.00015821) {
                  if (shiftAvg <= 2.523e-06) {
                    if (avgErr <= 0.77222) {
                      return 4;
                    } else {  // if avgErr > 0.77222
                      return 1;
                    }
                  } else {  // if shiftAvg > 2.523e-06
                    return 4;
                  }
                } else {  // if shiftStd > 0.00015821
                  if (maxEdgeLength <= 97.5) {
                    if (xyRatio <= 1.5278) {
                      return 4;
                    } else {  // if xyRatio > 1.5278
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 97.5
                    if (shiftNcc <= -0.017508) {
                      return 4;
                    } else {  // if shiftNcc > -0.017508
                      return 0;
                    }
                  }
                }
              } else {  // if shiftStd > 0.00069358
                if (area <= 4676) {
                  if (area <= 4306) {
                    if (area <= 4160.5) {
                      return 4;
                    } else {  // if area > 4160.5
                      return 1;
                    }
                  } else {  // if area > 4306
                    if (avgErr <= 2.5324) {
                      return 4;
                    } else {  // if avgErr > 2.5324
                      return 1;
                    }
                  }
                } else {  // if area > 4676
                  if (xyRatio <= 1.1722) {
                    if (avgErr <= 1.221) {
                      return 4;
                    } else {  // if avgErr > 1.221
                      return 1;
                    }
                  } else {  // if xyRatio > 1.1722
                    if (avgErr <= 1.5033) {
                      return 1;
                    } else {  // if avgErr > 1.5033
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if shiftStd > 0.006458
            if (maxEdgeLength <= 97.5) {
              if (area <= 4227) {
                if (area <= 4210.5) {
                  if (area <= 4149) {
                    if (area <= 4130.5) {
                      return 1;
                    } else {  // if area > 4130.5
                      return 0;
                    }
                  } else {  // if area > 4149
                    if (xyRatio <= 1.2224) {
                      return 1;
                    } else {  // if xyRatio > 1.2224
                      return 1;
                    }
                  }
                } else {  // if area > 4210.5
                  if (area <= 4213) {
                    if (shiftAvg <= 0.12916) {
                      return 0;
                    } else {  // if shiftAvg > 0.12916
                      return 0;
                    }
                  } else {  // if area > 4213
                    if (area <= 4222.5) {
                      return 1;
                    } else {  // if area > 4222.5
                      return 0;
                    }
                  }
                }
              } else {  // if area > 4227
                if (area <= 4997) {
                  if (shiftNcc <= 0.083144) {
                    if (avgErr <= 9.8054) {
                      return 1;
                    } else {  // if avgErr > 9.8054
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.083144
                    if (xyRatio <= 1.3504) {
                      return 1;
                    } else {  // if xyRatio > 1.3504
                      return 1;
                    }
                  }
                } else {  // if area > 4997
                  if (area <= 5003.5) {
                    if (shiftNcc <= 0.013197) {
                      return 3;
                    } else {  // if shiftNcc > 0.013197
                      return 2;
                    }
                  } else {  // if area > 5003.5
                    if (xyRatio <= 1.6636) {
                      return 1;
                    } else {  // if xyRatio > 1.6636
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 97.5
              if (avgErr <= 10.928) {
                if (maxEdgeLength <= 105.5) {
                  if (shiftAvg <= 0.0022295) {
                    return 4;
                  } else {  // if shiftAvg > 0.0022295
                    if (shiftStd <= 0.197) {
                      return 0;
                    } else {  // if shiftStd > 0.197
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 105.5
                  if (xyRatio <= 2.3721) {
                    return 4;
                  } else {  // if xyRatio > 2.3721
                    return 3;
                  }
                }
              } else {  // if avgErr > 10.928
                if (shiftAvg <= 0.37104) {
                  if (shiftNcc <= 0.050102) {
                    if (shiftAvg <= 0.010117) {
                      return 1;
                    } else {  // if shiftAvg > 0.010117
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.050102
                    return 0;
                  }
                } else {  // if shiftAvg > 0.37104
                  if (maxEdgeLength <= 108.5) {
                    if (shiftAvg <= 0.95167) {
                      return 4;
                    } else {  // if shiftAvg > 0.95167
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 108.5
                    if (shiftStd <= 0.69356) {
                      return 1;
                    } else {  // if shiftStd > 0.69356
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 5071.5
        if (maxEdgeLength <= 97.5) {
          if (area <= 6405) {
            if (area <= 5211) {
              if (area <= 5101.5) {
                if (maxEdgeLength <= 90) {
                  if (xyRatio <= 1.2446) {
                    if (area <= 5087) {
                      return 1;
                    } else {  // if area > 5087
                      return 2;
                    }
                  } else {  // if xyRatio > 1.2446
                    if (shiftNcc <= 0.38281) {
                      return 2;
                    } else {  // if shiftNcc > 0.38281
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 90
                  if (maxEdgeLength <= 95) {
                    if (shiftStd <= 0.47702) {
                      return 1;
                    } else {  // if shiftStd > 0.47702
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 95
                    return 2;
                  }
                }
              } else {  // if area > 5101.5
                if (area <= 5134) {
                  if (maxEdgeLength <= 91.5) {
                    if (shiftNcc <= 0.23737) {
                      return 1;
                    } else {  // if shiftNcc > 0.23737
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 91.5
                    if (shiftStd <= 0.031054) {
                      return 4;
                    } else {  // if shiftStd > 0.031054
                      return 2;
                    }
                  }
                } else {  // if area > 5134
                  if (maxEdgeLength <= 78.5) {
                    if (xyRatio <= 1.1335) {
                      return 2;
                    } else {  // if xyRatio > 1.1335
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 78.5
                    if (area <= 5168) {
                      return 2;
                    } else {  // if area > 5168
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 5211
              if (shiftAvg <= 0.00018359) {
                if (avgErr <= 0.7606) {
                  if (xyRatio <= 1.1679) {
                    if (avgErr <= 0.43012) {
                      return 4;
                    } else {  // if avgErr > 0.43012
                      return 3;
                    }
                  } else {  // if xyRatio > 1.1679
                    if (shiftStd <= 8.4263e-05) {
                      return 4;
                    } else {  // if shiftStd > 8.4263e-05
                      return 2;
                    }
                  }
                } else {  // if avgErr > 0.7606
                  if (shiftStd <= 0.00054621) {
                    if (shiftNcc <= 0.027186) {
                      return 4;
                    } else {  // if shiftNcc > 0.027186
                      return 4;
                    }
                  } else {  // if shiftStd > 0.00054621
                    if (shiftAvg <= 0.00014695) {
                      return 2;
                    } else {  // if shiftAvg > 0.00014695
                      return 4;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.00018359
                if (shiftAvg <= 0.049419) {
                  if (area <= 6342) {
                    if (shiftAvg <= 0.006001) {
                      return 2;
                    } else {  // if shiftAvg > 0.006001
                      return 2;
                    }
                  } else {  // if area > 6342
                    if (area <= 6367.5) {
                      return 3;
                    } else {  // if area > 6367.5
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.049419
                  if (avgErr <= 10.187) {
                    if (xyRatio <= 1.1786) {
                      return 2;
                    } else {  // if xyRatio > 1.1786
                      return 2;
                    }
                  } else {  // if avgErr > 10.187
                    if (avgErr <= 10.313) {
                      return 2;
                    } else {  // if avgErr > 10.313
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 6405
            if (area <= 6601.5) {
              if (maxEdgeLength <= 88.5) {
                if (xyRatio <= 1.1667) {
                  if (xyRatio <= 1.0974) {
                    if (maxEdgeLength <= 83.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 83.5
                      return 2;
                    }
                  } else {  // if xyRatio > 1.0974
                    if (avgErr <= 5.4517) {
                      return 3;
                    } else {  // if avgErr > 5.4517
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 1.1667
                  if (shiftNcc <= -0.019831) {
                    return 4;
                  } else {  // if shiftNcc > -0.019831
                    if (avgErr <= 0.88251) {
                      return 3;
                    } else {  // if avgErr > 0.88251
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 88.5
                if (xyRatio <= 1.4403) {
                  if (area <= 6460.5) {
                    if (xyRatio <= 1.4149) {
                      return 3;
                    } else {  // if xyRatio > 1.4149
                      return 3;
                    }
                  } else {  // if area > 6460.5
                    if (area <= 6491.5) {
                      return 2;
                    } else {  // if area > 6491.5
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 1.4403
                  if (shiftNcc <= 0.1572) {
                    if (shiftAvg <= 0.0052587) {
                      return 2;
                    } else {  // if shiftAvg > 0.0052587
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.1572
                    return 2;
                  }
                }
              }
            } else {  // if area > 6601.5
              if (shiftAvg <= 1.0238) {
                if (avgErr <= 1.4871) {
                  if (shiftAvg <= 0.010668) {
                    if (avgErr <= 1.1837) {
                      return 3;
                    } else {  // if avgErr > 1.1837
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.010668
                    return 3;
                  }
                } else {  // if avgErr > 1.4871
                  if (xyRatio <= 1.1529) {
                    if (shiftAvg <= 0.042857) {
                      return 3;
                    } else {  // if shiftAvg > 0.042857
                      return 3;
                    }
                  } else {  // if xyRatio > 1.1529
                    if (shiftAvg <= 0.41907) {
                      return 3;
                    } else {  // if shiftAvg > 0.41907
                      return 3;
                    }
                  }
                }
              } else {  // if shiftAvg > 1.0238
                if (shiftNcc <= 0.78988) {
                  if (shiftStd <= 3.7988) {
                    if (shiftStd <= 1.313) {
                      return 3;
                    } else {  // if shiftStd > 1.313
                      return 4;
                    }
                  } else {  // if shiftStd > 3.7988
                    return 3;
                  }
                } else {  // if shiftNcc > 0.78988
                  if (xyRatio <= 1.2308) {
                    return 4;
                  } else {  // if xyRatio > 1.2308
                    if (xyRatio <= 1.2875) {
                      return 3;
                    } else {  // if xyRatio > 1.2875
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 97.5
          if (area <= 9244.5) {
            if (area <= 8960.5) {
              if (avgErr <= 1.5875) {
                if (area <= 6271) {
                  if (shiftStd <= 0.0010813) {
                    if (xyRatio <= 1.6363) {
                      return 0;
                    } else {  // if xyRatio > 1.6363
                      return 4;
                    }
                  } else {  // if shiftStd > 0.0010813
                    if (avgErr <= 1.4342) {
                      return 0;
                    } else {  // if avgErr > 1.4342
                      return 3;
                    }
                  }
                } else {  // if area > 6271
                  if (shiftAvg <= 0.036268) {
                    if (area <= 7408) {
                      return 0;
                    } else {  // if area > 7408
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.036268
                    return 3;
                  }
                }
              } else {  // if avgErr > 1.5875
                if (area <= 6376) {
                  if (area <= 5182) {
                    if (shiftStd <= 0.29088) {
                      return 0;
                    } else {  // if shiftStd > 0.29088
                      return 4;
                    }
                  } else {  // if area > 5182
                    if (shiftAvg <= 0.02401) {
                      return 0;
                    } else {  // if shiftAvg > 0.02401
                      return 0;
                    }
                  }
                } else {  // if area > 6376
                  if (shiftAvg <= 0.065871) {
                    if (maxEdgeLength <= 104.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 104.5
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.065871
                    if (avgErr <= 15.13) {
                      return 0;
                    } else {  // if avgErr > 15.13
                      return 0;
                    }
                  }
                }
              }
            } else {  // if area > 8960.5
              if (area <= 8969) {
                return 1;
              } else {  // if area > 8969
                if (area <= 9151.5) {
                  if (shiftStd <= 3.714) {
                    if (avgErr <= 11.908) {
                      return 0;
                    } else {  // if avgErr > 11.908
                      return 0;
                    }
                  } else {  // if shiftStd > 3.714
                    return 4;
                  }
                } else {  // if area > 9151.5
                  if (area <= 9163.5) {
                    return 1;
                  } else {  // if area > 9163.5
                    if (area <= 9237) {
                      return 0;
                    } else {  // if area > 9237
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if area > 9244.5
            if (area <= 11436) {
              if (area <= 9416.5) {
                if (area <= 9382) {
                  if (area <= 9319.5) {
                    if (area <= 9270.5) {
                      return 1;
                    } else {  // if area > 9270.5
                      return 0;
                    }
                  } else {  // if area > 9319.5
                    if (shiftNcc <= 0.92243) {
                      return 1;
                    } else {  // if shiftNcc > 0.92243
                      return 0;
                    }
                  }
                } else {  // if area > 9382
                  if (area <= 9404.5) {
                    if (area <= 9400.5) {
                      return 0;
                    } else {  // if area > 9400.5
                      return 1;
                    }
                  } else {  // if area > 9404.5
                    return 0;
                  }
                }
              } else {  // if area > 9416.5
                if (area <= 11262) {
                  if (xyRatio <= 1.1011) {
                    if (avgErr <= 13.492) {
                      return 1;
                    } else {  // if avgErr > 13.492
                      return 4;
                    }
                  } else {  // if xyRatio > 1.1011
                    if (area <= 9631) {
                      return 1;
                    } else {  // if area > 9631
                      return 1;
                    }
                  }
                } else {  // if area > 11262
                  if (area <= 11318) {
                    if (area <= 11313) {
                      return 1;
                    } else {  // if area > 11313
                      return 2;
                    }
                  } else {  // if area > 11318
                    if (xyRatio <= 1.2234) {
                      return 4;
                    } else {  // if xyRatio > 1.2234
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 11436
              if (area <= 11813) {
                if (xyRatio <= 1.1337) {
                  return 4;
                } else {  // if xyRatio > 1.1337
                  if (shiftAvg <= 0.040406) {
                    if (shiftNcc <= 0.21444) {
                      return 1;
                    } else {  // if shiftNcc > 0.21444
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.040406
                    if (avgErr <= 10.069) {
                      return 2;
                    } else {  // if avgErr > 10.069
                      return 2;
                    }
                  }
                }
              } else {  // if area > 11813
                if (area <= 14206) {
                  if (maxEdgeLength <= 123.5) {
                    if (shiftNcc <= 0.34532) {
                      return 2;
                    } else {  // if shiftNcc > 0.34532
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 123.5
                    if (maxEdgeLength <= 182) {
                      return 2;
                    } else {  // if maxEdgeLength > 182
                      return 4;
                    }
                  }
                } else {  // if area > 14206
                  if (area <= 14239) {
                    if (maxEdgeLength <= 146.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 146.5
                      return 3;
                    }
                  } else {  // if area > 14239
                    if (area <= 14352) {
                      return 2;
                    } else {  // if area > 14352
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if area > 14529
      if (maxEdgeLength <= 194.5) {
        if (area <= 14934) {
          if (maxEdgeLength <= 137.5) {
            if (xyRatio <= 1.2534) {
              if (maxEdgeLength <= 132.5) {
                if (avgErr <= 7.9641) {
                  return 4;
                } else {  // if avgErr > 7.9641
                  if (shiftAvg <= 0.96375) {
                    return 2;
                  } else {  // if shiftAvg > 0.96375
                    if (shiftNcc <= 0.63515) {
                      return 3;
                    } else {  // if shiftNcc > 0.63515
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 132.5
                return 3;
              }
            } else {  // if xyRatio > 1.2534
              return 2;
            }
          } else {  // if maxEdgeLength > 137.5
            if (avgErr <= 15.839) {
              if (shiftAvg <= 13.537) {
                if (shiftStd <= 0.42841) {
                  if (xyRatio <= 1.8655) {
                    if (xyRatio <= 1.5722) {
                      return 3;
                    } else {  // if xyRatio > 1.5722
                      return 3;
                    }
                  } else {  // if xyRatio > 1.8655
                    return 2;
                  }
                } else {  // if shiftStd > 0.42841
                  if (xyRatio <= 1.9826) {
                    if (area <= 14907) {
                      return 3;
                    } else {  // if area > 14907
                      return 2;
                    }
                  } else {  // if xyRatio > 1.9826
                    if (maxEdgeLength <= 172.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 172.5
                      return 3;
                    }
                  }
                }
              } else {  // if shiftAvg > 13.537
                return 2;
              }
            } else {  // if avgErr > 15.839
              if (avgErr <= 17.227) {
                if (shiftStd <= 15.235) {
                  if (maxEdgeLength <= 155) {
                    if (shiftNcc <= 0.37251) {
                      return 2;
                    } else {  // if shiftNcc > 0.37251
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 155
                    return 2;
                  }
                } else {  // if shiftStd > 15.235
                  return 3;
                }
              } else {  // if avgErr > 17.227
                if (avgErr <= 20.147) {
                  return 3;
                } else {  // if avgErr > 20.147
                  if (area <= 14716) {
                    return 4;
                  } else {  // if area > 14716
                    return 2;
                  }
                }
              }
            }
          }
        } else {  // if area > 14934
          if (shiftAvg <= 60.65) {
            if (shiftNcc <= 0.33932) {
              if (xyRatio <= 1.252) {
                if (area <= 19611) {
                  if (maxEdgeLength <= 128.5) {
                    return 2;
                  } else {  // if maxEdgeLength > 128.5
                    if (shiftNcc <= 0.33035) {
                      return 3;
                    } else {  // if shiftNcc > 0.33035
                      return 4;
                    }
                  }
                } else {  // if area > 19611
                  if (shiftAvg <= 1.5067) {
                    if (shiftStd <= 1.7765) {
                      return 3;
                    } else {  // if shiftStd > 1.7765
                      return 4;
                    }
                  } else {  // if shiftAvg > 1.5067
                    return 3;
                  }
                }
              } else {  // if xyRatio > 1.252
                if (xyRatio <= 2.1176) {
                  if (shiftStd <= 15.427) {
                    if (xyRatio <= 1.9558) {
                      return 3;
                    } else {  // if xyRatio > 1.9558
                      return 3;
                    }
                  } else {  // if shiftStd > 15.427
                    if (maxEdgeLength <= 159.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 159.5
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 2.1176
                  return 4;
                }
              }
            } else {  // if shiftNcc > 0.33932
              if (shiftAvg <= 15.918) {
                if (xyRatio <= 2.2544) {
                  if (shiftStd <= 3.689) {
                    if (xyRatio <= 1.1134) {
                      return 3;
                    } else {  // if xyRatio > 1.1134
                      return 3;
                    }
                  } else {  // if shiftStd > 3.689
                    if (shiftStd <= 3.7084) {
                      return 4;
                    } else {  // if shiftStd > 3.7084
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 2.2544
                  if (area <= 16062) {
                    return 3;
                  } else {  // if area > 16062
                    return 4;
                  }
                }
              } else {  // if shiftAvg > 15.918
                if (shiftAvg <= 15.958) {
                  return 4;
                } else {  // if shiftAvg > 15.958
                  if (xyRatio <= 1.0859) {
                    if (maxEdgeLength <= 144.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 144.5
                      return 4;
                    }
                  } else {  // if xyRatio > 1.0859
                    if (area <= 21704) {
                      return 3;
                    } else {  // if area > 21704
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 60.65
            if (shiftAvg <= 215.08) {
              if (avgErr <= 21.389) {
                return 4;
              } else {  // if avgErr > 21.389
                return 3;
              }
            } else {  // if shiftAvg > 215.08
              return 3;
            }
          }
        }
      } else {  // if maxEdgeLength > 194.5
        if (area <= 37128) {
          if (area <= 35671) {
            if (shiftStd <= 15.077) {
              if (xyRatio <= 2.8539) {
                if (shiftNcc <= 0.9882) {
                  if (area <= 35106) {
                    if (avgErr <= 12.755) {
                      return 0;
                    } else {  // if avgErr > 12.755
                      return 0;
                    }
                  } else {  // if area > 35106
                    if (shiftAvg <= 7.2147) {
                      return 0;
                    } else {  // if shiftAvg > 7.2147
                      return 0;
                    }
                  }
                } else {  // if shiftNcc > 0.9882
                  if (avgErr <= 12.443) {
                    return 0;
                  } else {  // if avgErr > 12.443
                    if (shiftAvg <= 25.002) {
                      return 3;
                    } else {  // if shiftAvg > 25.002
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 2.8539
                return 3;
              }
            } else {  // if shiftStd > 15.077
              if (area <= 26884) {
                if (shiftNcc <= 0.79634) {
                  if (area <= 26725) {
                    if (shiftNcc <= 0.53889) {
                      return 0;
                    } else {  // if shiftNcc > 0.53889
                      return 0;
                    }
                  } else {  // if area > 26725
                    if (maxEdgeLength <= 225.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 225.5
                      return 4;
                    }
                  }
                } else {  // if shiftNcc > 0.79634
                  if (xyRatio <= 1.9149) {
                    return 3;
                  } else {  // if xyRatio > 1.9149
                    return 4;
                  }
                }
              } else {  // if area > 26884
                if (shiftStd <= 15.488) {
                  if (area <= 31847) {
                    if (maxEdgeLength <= 231.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 231.5
                      return 4;
                    }
                  } else {  // if area > 31847
                    return 3;
                  }
                } else {  // if shiftStd > 15.488
                  if (shiftNcc <= 0.94244) {
                    if (avgErr <= 18.361) {
                      return 0;
                    } else {  // if avgErr > 18.361
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.94244
                    if (shiftNcc <= 0.95765) {
                      return 4;
                    } else {  // if shiftNcc > 0.95765
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if area > 35671
            if (xyRatio <= 1.5574) {
              if (maxEdgeLength <= 234.5) {
                if (area <= 36370) {
                  if (avgErr <= 15.839) {
                    return 0;
                  } else {  // if avgErr > 15.839
                    if (avgErr <= 16.262) {
                      return 1;
                    } else {  // if avgErr > 16.262
                      return 0;
                    }
                  }
                } else {  // if area > 36370
                  if (shiftNcc <= 0.79813) {
                    if (xyRatio <= 1.4453) {
                      return 1;
                    } else {  // if xyRatio > 1.4453
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.79813
                    if (shiftNcc <= 0.95482) {
                      return 0;
                    } else {  // if shiftNcc > 0.95482
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 234.5
                return 1;
              }
            } else {  // if xyRatio > 1.5574
              if (xyRatio <= 1.938) {
                if (area <= 35710) {
                  return 1;
                } else {  // if area > 35710
                  if (shiftAvg <= 90.762) {
                    if (area <= 36945) {
                      return 0;
                    } else {  // if area > 36945
                      return 0;
                    }
                  } else {  // if shiftAvg > 90.762
                    return 1;
                  }
                }
              } else {  // if xyRatio > 1.938
                if (area <= 36718) {
                  if (maxEdgeLength <= 271) {
                    return 1;
                  } else {  // if maxEdgeLength > 271
                    if (shiftAvg <= 26.748) {
                      return 4;
                    } else {  // if shiftAvg > 26.748
                      return 1;
                    }
                  }
                } else {  // if area > 36718
                  return 0;
                }
              }
            }
          }
        } else {  // if area > 37128
          if (area <= 46334) {
            if (area <= 37915) {
              if (maxEdgeLength <= 241.5) {
                if (avgErr <= 21.846) {
                  if (shiftNcc <= 0.99179) {
                    if (area <= 37875) {
                      return 1;
                    } else {  // if area > 37875
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.99179
                    return 0;
                  }
                } else {  // if avgErr > 21.846
                  return 0;
                }
              } else {  // if maxEdgeLength > 241.5
                if (maxEdgeLength <= 245.5) {
                  return 0;
                } else {  // if maxEdgeLength > 245.5
                  if (avgErr <= 15.748) {
                    if (area <= 37294) {
                      return 0;
                    } else {  // if area > 37294
                      return 1;
                    }
                  } else {  // if avgErr > 15.748
                    return 4;
                  }
                }
              }
            } else {  // if area > 37915
              if (area <= 45120) {
                if (shiftAvg <= 37.8) {
                  if (shiftStd <= 15.261) {
                    if (shiftAvg <= 22.047) {
                      return 1;
                    } else {  // if shiftAvg > 22.047
                      return 1;
                    }
                  } else {  // if shiftStd > 15.261
                    if (shiftStd <= 15.515) {
                      return 3;
                    } else {  // if shiftStd > 15.515
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 37.8
                  if (maxEdgeLength <= 259) {
                    if (maxEdgeLength <= 220.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 220.5
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 259
                    if (shiftAvg <= 39.96) {
                      return 4;
                    } else {  // if shiftAvg > 39.96
                      return 1;
                    }
                  }
                }
              } else {  // if area > 45120
                if (shiftAvg <= 23.842) {
                  if (area <= 45882) {
                    return 1;
                  } else {  // if area > 45882
                    if (shiftStd <= 7.3177) {
                      return 2;
                    } else {  // if shiftStd > 7.3177
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 23.842
                  if (xyRatio <= 1.9476) {
                    if (shiftAvg <= 32.16) {
                      return 1;
                    } else {  // if shiftAvg > 32.16
                      return 2;
                    }
                  } else {  // if xyRatio > 1.9476
                    return 1;
                  }
                }
              }
            }
          } else {  // if area > 46334
            if (area <= 58278) {
              if (area <= 47434) {
                if (shiftAvg <= 37.244) {
                  if (shiftStd <= 1.3233) {
                    if (maxEdgeLength <= 248) {
                      return 1;
                    } else {  // if maxEdgeLength > 248
                      return 4;
                    }
                  } else {  // if shiftStd > 1.3233
                    if (area <= 47416) {
                      return 2;
                    } else {  // if area > 47416
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 37.244
                  if (shiftNcc <= 0.83808) {
                    return 2;
                  } else {  // if shiftNcc > 0.83808
                    return 1;
                  }
                }
              } else {  // if area > 47434
                if (maxEdgeLength <= 305.5) {
                  if (shiftNcc <= 0.31501) {
                    if (shiftNcc <= 0.24207) {
                      return 2;
                    } else {  // if shiftNcc > 0.24207
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.31501
                    if (area <= 49168) {
                      return 2;
                    } else {  // if area > 49168
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 305.5
                  if (xyRatio <= 1.6505) {
                    return 3;
                  } else {  // if xyRatio > 1.6505
                    return 2;
                  }
                }
              }
            } else {  // if area > 58278
              if (shiftAvg <= 28.686) {
                if (shiftAvg <= 27.819) {
                  return 3;
                } else {  // if shiftAvg > 27.819
                  return 2;
                }
              } else {  // if shiftAvg > 28.686
                return 3;
              }
            }
          }
        }
      }
    }
  }
}

} // namespace md
