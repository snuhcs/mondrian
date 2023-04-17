#include "mondrian/tree/MTA.hpp"

namespace md {

int MTA(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr, float confidence) {
  if (area <= 5071.5) {
    if (area <= 4093.5) {
      if (xyRatio <= 1.1926) {
        if (shiftStd <= 0.015581) {
          if (avgErr <= 0.60696) {
            if (shiftStd <= 0.00011193) {
              if (xyRatio <= 0.62606) {
                if (shiftNcc <= -0.092252) {
                  return 0;
                } else {  // if shiftNcc > -0.092252
                  if (shiftStd <= 9.1283e-05) {
                    if (shiftStd <= 1.7766e-06) {
                      return 4;
                    } else {  // if shiftStd > 1.7766e-06
                      return 4;
                    }
                  } else {  // if shiftStd > 9.1283e-05
                    if (shiftNcc <= 0.029914) {
                      return 0;
                    } else {  // if shiftNcc > 0.029914
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 0.62606
                if (shiftAvg <= 2.4915e-05) {
                  if (area <= 3042) {
                    if (maxEdgeLength <= 43.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 43.5
                      return 4;
                    }
                  } else {  // if area > 3042
                    if (xyRatio <= 1.0348) {
                      return 4;
                    } else {  // if xyRatio > 1.0348
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 2.4915e-05
                  if (shiftAvg <= 3.8359e-05) {
                    if (avgErr <= 0.092083) {
                      return 1;
                    } else {  // if avgErr > 0.092083
                      return 0;
                    }
                  } else {  // if shiftAvg > 3.8359e-05
                    if (xyRatio <= 0.70128) {
                      return 0;
                    } else {  // if xyRatio > 0.70128
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftStd > 0.00011193
              if (area <= 2792.5) {
                if (area <= 2384) {
                  if (shiftNcc <= 0.30038) {
                    if (area <= 2231.5) {
                      return 4;
                    } else {  // if area > 2231.5
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.30038
                    if (shiftStd <= 0.00030063) {
                      return 1;
                    } else {  // if shiftStd > 0.00030063
                      return 0;
                    }
                  }
                } else {  // if area > 2384
                  if (xyRatio <= 0.55404) {
                    return 4;
                  } else {  // if xyRatio > 0.55404
                    if (xyRatio <= 1.1596) {
                      return 1;
                    } else {  // if xyRatio > 1.1596
                      return 4;
                    }
                  }
                }
              } else {  // if area > 2792.5
                if (maxEdgeLength <= 64.5) {
                  if (area <= 3500) {
                    if (shiftNcc <= -0.034285) {
                      return 4;
                    } else {  // if shiftNcc > -0.034285
                      return 2;
                    }
                  } else {  // if area > 3500
                    if (shiftNcc <= -0.0060839) {
                      return 3;
                    } else {  // if shiftNcc > -0.0060839
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 64.5
                  if (shiftNcc <= 0.057178) {
                    if (area <= 3927.5) {
                      return 4;
                    } else {  // if area > 3927.5
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.057178
                    if (area <= 3668) {
                      return 0;
                    } else {  // if area > 3668
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if avgErr > 0.60696
            if (shiftAvg <= 0.0060063) {
              if (xyRatio <= 0.85374) {
                if (shiftStd <= 0.00035314) {
                  if (shiftAvg <= 9.7414e-05) {
                    if (avgErr <= 0.61366) {
                      return 2;
                    } else {  // if avgErr > 0.61366
                      return 4;
                    }
                  } else {  // if shiftAvg > 9.7414e-05
                    if (shiftStd <= 0.00015409) {
                      return 0;
                    } else {  // if shiftStd > 0.00015409
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.00035314
                  if (shiftNcc <= -0.010106) {
                    if (shiftStd <= 0.011713) {
                      return 4;
                    } else {  // if shiftStd > 0.011713
                      return 0;
                    }
                  } else {  // if shiftNcc > -0.010106
                    if (shiftNcc <= -0.0032392) {
                      return 0;
                    } else {  // if shiftNcc > -0.0032392
                      return 4;
                    }
                  }
                }
              } else {  // if xyRatio > 0.85374
                if (xyRatio <= 0.85611) {
                  if (shiftAvg <= 5.3103e-05) {
                    return 1;
                  } else {  // if shiftAvg > 5.3103e-05
                    if (maxEdgeLength <= 58.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 58.5
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.85611
                  if (maxEdgeLength <= 55.5) {
                    if (avgErr <= 0.78499) {
                      return 1;
                    } else {  // if avgErr > 0.78499
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 55.5
                    if (shiftStd <= 0.0011234) {
                      return 4;
                    } else {  // if shiftStd > 0.0011234
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 0.0060063
              if (xyRatio <= 0.79567) {
                if (shiftNcc <= 0.33872) {
                  return 0;
                } else {  // if shiftNcc > 0.33872
                  return 4;
                }
              } else {  // if xyRatio > 0.79567
                if (xyRatio <= 1.1103) {
                  return 1;
                } else {  // if xyRatio > 1.1103
                  return 0;
                }
              }
            }
          }
        } else {  // if shiftStd > 0.015581
          if (maxEdgeLength <= 64.5) {
            if (area <= 2934) {
              if (area <= 2307) {
                if (maxEdgeLength <= 48.5) {
                  if (area <= 1934.5) {
                    if (avgErr <= 7.0351) {
                      return 4;
                    } else {  // if avgErr > 7.0351
                      return 1;
                    }
                  } else {  // if area > 1934.5
                    if (area <= 2139) {
                      return 2;
                    } else {  // if area > 2139
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 48.5
                  if (xyRatio <= 0.72388) {
                    if (shiftAvg <= 0.009308) {
                      return 3;
                    } else {  // if shiftAvg > 0.009308
                      return 4;
                    }
                  } else {  // if xyRatio > 0.72388
                    if (shiftAvg <= 0.27563) {
                      return 0;
                    } else {  // if shiftAvg > 0.27563
                      return 4;
                    }
                  }
                }
              } else {  // if area > 2307
                if (area <= 2807) {
                  if (shiftAvg <= 0.273) {
                    if (xyRatio <= 0.72272) {
                      return 4;
                    } else {  // if xyRatio > 0.72272
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.273
                    if (shiftStd <= 0.1615) {
                      return 1;
                    } else {  // if shiftStd > 0.1615
                      return 4;
                    }
                  }
                } else {  // if area > 2807
                  if (xyRatio <= 1.0481) {
                    if (shiftStd <= 0.082561) {
                      return 4;
                    } else {  // if shiftStd > 0.082561
                      return 1;
                    }
                  } else {  // if xyRatio > 1.0481
                    if (area <= 2853) {
                      return 3;
                    } else {  // if area > 2853
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 2934
              if (area <= 3539) {
                if (maxEdgeLength <= 63.5) {
                  if (avgErr <= 5.7706) {
                    if (xyRatio <= 0.9649) {
                      return 4;
                    } else {  // if xyRatio > 0.9649
                      return 2;
                    }
                  } else {  // if avgErr > 5.7706
                    if (avgErr <= 7.4019) {
                      return 2;
                    } else {  // if avgErr > 7.4019
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 63.5
                  if (avgErr <= 14.411) {
                    if (shiftStd <= 0.022921) {
                      return 3;
                    } else {  // if shiftStd > 0.022921
                      return 4;
                    }
                  } else {  // if avgErr > 14.411
                    return 3;
                  }
                }
              } else {  // if area > 3539
                if (xyRatio <= 0.98425) {
                  if (shiftStd <= 0.18379) {
                    if (shiftAvg <= 0.017215) {
                      return 4;
                    } else {  // if shiftAvg > 0.017215
                      return 3;
                    }
                  } else {  // if shiftStd > 0.18379
                    return 4;
                  }
                } else {  // if xyRatio > 0.98425
                  if (shiftAvg <= 0.0017235) {
                    if (shiftNcc <= 0.0087757) {
                      return 3;
                    } else {  // if shiftNcc > 0.0087757
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.0017235
                    if (shiftStd <= 0.44707) {
                      return 3;
                    } else {  // if shiftStd > 0.44707
                      return 4;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 64.5
            if (xyRatio <= 0.77963) {
              if (avgErr <= 4.1454) {
                if (area <= 3973) {
                  return 4;
                } else {  // if area > 3973
                  if (avgErr <= 2.3375) {
                    return 4;
                  } else {  // if avgErr > 2.3375
                    return 3;
                  }
                }
              } else {  // if avgErr > 4.1454
                if (shiftAvg <= 0.35878) {
                  if (shiftNcc <= 0.67521) {
                    if (xyRatio <= 0.65924) {
                      return 4;
                    } else {  // if xyRatio > 0.65924
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.67521
                    if (shiftStd <= 0.043526) {
                      return 2;
                    } else {  // if shiftStd > 0.043526
                      return 0;
                    }
                  }
                } else {  // if shiftAvg > 0.35878
                  return 4;
                }
              }
            } else {  // if xyRatio > 0.77963
              if (xyRatio <= 1.0908) {
                if (xyRatio <= 0.91676) {
                  if (avgErr <= 5.327) {
                    if (area <= 4075.5) {
                      return 4;
                    } else {  // if area > 4075.5
                      return 0;
                    }
                  } else {  // if avgErr > 5.327
                    if (shiftAvg <= 0.13665) {
                      return 0;
                    } else {  // if shiftAvg > 0.13665
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 0.91676
                  if (shiftAvg <= 0.0041166) {
                    if (avgErr <= 9.6162) {
                      return 4;
                    } else {  // if avgErr > 9.6162
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.0041166
                    if (avgErr <= 7.9205) {
                      return 1;
                    } else {  // if avgErr > 7.9205
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 1.0908
                if (maxEdgeLength <= 68.5) {
                  if (avgErr <= 10.21) {
                    if (shiftAvg <= 0.051255) {
                      return 0;
                    } else {  // if shiftAvg > 0.051255
                      return 0;
                    }
                  } else {  // if avgErr > 10.21
                    if (avgErr <= 11.761) {
                      return 4;
                    } else {  // if avgErr > 11.761
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 68.5
                  if (avgErr <= 4.6024) {
                    return 1;
                  } else {  // if avgErr > 4.6024
                    if (xyRatio <= 1.1796) {
                      return 3;
                    } else {  // if xyRatio > 1.1796
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if xyRatio > 1.1926
        if (maxEdgeLength <= 64.5) {
          if (area <= 2873.5) {
            if (area <= 2298.5) {
              if (area <= 1874.5) {
                if (area <= 1780) {
                  return 1;
                } else {  // if area > 1780
                  return 4;
                }
              } else {  // if area > 1874.5
                if (shiftStd <= 0.010603) {
                  if (shiftNcc <= -0.038556) {
                    if (maxEdgeLength <= 61) {
                      return 0;
                    } else {  // if maxEdgeLength > 61
                      return 4;
                    }
                  } else {  // if shiftNcc > -0.038556
                    return 4;
                  }
                } else {  // if shiftStd > 0.010603
                  if (avgErr <= 9.3714) {
                    if (shiftNcc <= 0.92525) {
                      return 0;
                    } else {  // if shiftNcc > 0.92525
                      return 0;
                    }
                  } else {  // if avgErr > 9.3714
                    if (area <= 2146) {
                      return 4;
                    } else {  // if area > 2146
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 2298.5
              if (shiftAvg <= 0.0013802) {
                if (area <= 2720) {
                  if (maxEdgeLength <= 60.5) {
                    if (shiftAvg <= 3.2339e-06) {
                      return 3;
                    } else {  // if shiftAvg > 3.2339e-06
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 60.5
                    return 1;
                  }
                } else {  // if area > 2720
                  if (shiftStd <= 0.02127) {
                    return 1;
                  } else {  // if shiftStd > 0.02127
                    return 4;
                  }
                }
              } else {  // if shiftAvg > 0.0013802
                if (shiftStd <= 0.050829) {
                  if (shiftAvg <= 0.15046) {
                    if (xyRatio <= 1.5831) {
                      return 1;
                    } else {  // if xyRatio > 1.5831
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.15046
                    return 2;
                  }
                } else {  // if shiftStd > 0.050829
                  if (avgErr <= 11.317) {
                    if (shiftNcc <= 0.72127) {
                      return 1;
                    } else {  // if shiftNcc > 0.72127
                      return 3;
                    }
                  } else {  // if avgErr > 11.317
                    if (maxEdgeLength <= 60.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 60.5
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 2873.5
            if (area <= 3360) {
              if (shiftStd <= 0.012833) {
                if (avgErr <= 0.75641) {
                  return 2;
                } else {  // if avgErr > 0.75641
                  if (shiftNcc <= 0.0042448) {
                    return 2;
                  } else {  // if shiftNcc > 0.0042448
                    return 4;
                  }
                }
              } else {  // if shiftStd > 0.012833
                if (area <= 2936) {
                  if (area <= 2894.5) {
                    if (maxEdgeLength <= 62) {
                      return 2;
                    } else {  // if maxEdgeLength > 62
                      return 2;
                    }
                  } else {  // if area > 2894.5
                    if (shiftNcc <= 0.17756) {
                      return 4;
                    } else {  // if shiftNcc > 0.17756
                      return 1;
                    }
                  }
                } else {  // if area > 2936
                  if (avgErr <= 15.065) {
                    if (shiftNcc <= 0.75312) {
                      return 2;
                    } else {  // if shiftNcc > 0.75312
                      return 2;
                    }
                  } else {  // if avgErr > 15.065
                    return 4;
                  }
                }
              }
            } else {  // if area > 3360
              return 3;
            }
          }
        } else {  // if maxEdgeLength > 64.5
          if (area <= 4016.5) {
            if (area <= 2638.5) {
              if (xyRatio <= 1.8745) {
                if (avgErr <= 3.9776) {
                  if (area <= 2479) {
                    return 4;
                  } else {  // if area > 2479
                    return 3;
                  }
                } else {  // if avgErr > 3.9776
                  return 0;
                }
              } else {  // if xyRatio > 1.8745
                if (shiftAvg <= 0.034399) {
                  if (shiftStd <= 0.028346) {
                    return 4;
                  } else {  // if shiftStd > 0.028346
                    return 0;
                  }
                } else {  // if shiftAvg > 0.034399
                  return 4;
                }
              }
            } else {  // if area > 2638.5
              if (shiftAvg <= 0.030825) {
                if (shiftStd <= 5.4785e-05) {
                  if (area <= 3396.5) {
                    if (shiftNcc <= 0.037055) {
                      return 2;
                    } else {  // if shiftNcc > 0.037055
                      return 0;
                    }
                  } else {  // if area > 3396.5
                    if (shiftAvg <= 1.7591e-06) {
                      return 3;
                    } else {  // if shiftAvg > 1.7591e-06
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 5.4785e-05
                  if (avgErr <= 0.37861) {
                    if (area <= 3967.5) {
                      return 0;
                    } else {  // if area > 3967.5
                      return 1;
                    }
                  } else {  // if avgErr > 0.37861
                    if (shiftStd <= 0.00013745) {
                      return 4;
                    } else {  // if shiftStd > 0.00013745
                      return 0;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.030825
                if (avgErr <= 9.9872) {
                  if (avgErr <= 1.5377) {
                    if (xyRatio <= 1.577) {
                      return 2;
                    } else {  // if xyRatio > 1.577
                      return 3;
                    }
                  } else {  // if avgErr > 1.5377
                    if (shiftStd <= 1.6392) {
                      return 0;
                    } else {  // if shiftStd > 1.6392
                      return 4;
                    }
                  }
                } else {  // if avgErr > 9.9872
                  if (xyRatio <= 1.7501) {
                    if (shiftAvg <= 2.1549) {
                      return 0;
                    } else {  // if shiftAvg > 2.1549
                      return 4;
                    }
                  } else {  // if xyRatio > 1.7501
                    if (area <= 3319) {
                      return 4;
                    } else {  // if area > 3319
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if area > 4016.5
            if (area <= 4036) {
              if (xyRatio <= 1.7117) {
                if (avgErr <= 2.2143) {
                  return 4;
                } else {  // if avgErr > 2.2143
                  if (shiftAvg <= 0.034365) {
                    if (avgErr <= 8.7529) {
                      return 1;
                    } else {  // if avgErr > 8.7529
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.034365
                    return 1;
                  }
                }
              } else {  // if xyRatio > 1.7117
                if (maxEdgeLength <= 91) {
                  return 0;
                } else {  // if maxEdgeLength > 91
                  return 4;
                }
              }
            } else {  // if area > 4036
              if (xyRatio <= 1.8821) {
                if (xyRatio <= 1.6569) {
                  if (avgErr <= 1.9779) {
                    if (avgErr <= 1.7552) {
                      return 4;
                    } else {  // if avgErr > 1.7552
                      return 3;
                    }
                  } else {  // if avgErr > 1.9779
                    if (xyRatio <= 1.4209) {
                      return 1;
                    } else {  // if xyRatio > 1.4209
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 1.6569
                  if (maxEdgeLength <= 86.5) {
                    return 1;
                  } else {  // if maxEdgeLength > 86.5
                    return 0;
                  }
                }
              } else {  // if xyRatio > 1.8821
                if (shiftStd <= 0.16306) {
                  return 0;
                } else {  // if shiftStd > 0.16306
                  if (shiftNcc <= 0.17116) {
                    return 0;
                  } else {  // if shiftNcc > 0.17116
                    if (xyRatio <= 2.5375) {
                      return 4;
                    } else {  // if xyRatio > 2.5375
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if area > 4093.5
      if (shiftAvg <= 0.0032442) {
        if (xyRatio <= 1.1452) {
          if (xyRatio <= 0.74208) {
            if (shiftStd <= 5.1951e-05) {
              if (avgErr <= 0.31764) {
                return 4;
              } else {  // if avgErr > 0.31764
                return 3;
              }
            } else {  // if shiftStd > 5.1951e-05
              return 4;
            }
          } else {  // if xyRatio > 0.74208
            if (avgErr <= 0.54044) {
              if (shiftAvg <= 6.4235e-06) {
                if (avgErr <= 0.18825) {
                  if (maxEdgeLength <= 70.5) {
                    return 3;
                  } else {  // if maxEdgeLength > 70.5
                    if (area <= 4240.5) {
                      return 3;
                    } else {  // if area > 4240.5
                      return 4;
                    }
                  }
                } else {  // if avgErr > 0.18825
                  if (maxEdgeLength <= 76.5) {
                    return 4;
                  } else {  // if maxEdgeLength > 76.5
                    return 1;
                  }
                }
              } else {  // if shiftAvg > 6.4235e-06
                if (shiftStd <= 0.00090943) {
                  if (shiftStd <= 9.9799e-05) {
                    return 4;
                  } else {  // if shiftStd > 9.9799e-05
                    return 1;
                  }
                } else {  // if shiftStd > 0.00090943
                  if (shiftNcc <= 0.033871) {
                    return 0;
                  } else {  // if shiftNcc > 0.033871
                    if (shiftAvg <= 0.00016549) {
                      return 4;
                    } else {  // if shiftAvg > 0.00016549
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 0.54044
              if (shiftStd <= 0.0021356) {
                if (shiftStd <= 0.00027364) {
                  return 4;
                } else {  // if shiftStd > 0.00027364
                  if (avgErr <= 0.71325) {
                    if (area <= 4383) {
                      return 4;
                    } else {  // if area > 4383
                      return 1;
                    }
                  } else {  // if avgErr > 0.71325
                    if (shiftNcc <= 0.16831) {
                      return 4;
                    } else {  // if shiftNcc > 0.16831
                      return 3;
                    }
                  }
                }
              } else {  // if shiftStd > 0.0021356
                if (shiftAvg <= 0.0023311) {
                  if (shiftAvg <= 0.0014796) {
                    if (shiftStd <= 0.0022573) {
                      return 1;
                    } else {  // if shiftStd > 0.0022573
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.0014796
                    if (shiftAvg <= 0.0020353) {
                      return 1;
                    } else {  // if shiftAvg > 0.0020353
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.0023311
                  if (avgErr <= 1.7061) {
                    return 1;
                  } else {  // if avgErr > 1.7061
                    if (area <= 4680) {
                      return 4;
                    } else {  // if area > 4680
                      return 3;
                    }
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > 1.1452
          if (avgErr <= 3.3711) {
            if (avgErr <= 0.33347) {
              if (shiftStd <= 4.4952e-05) {
                if (shiftNcc <= 0.0063192) {
                  return 3;
                } else {  // if shiftNcc > 0.0063192
                  return 4;
                }
              } else {  // if shiftStd > 4.4952e-05
                if (area <= 4977.5) {
                  if (maxEdgeLength <= 88) {
                    if (avgErr <= 0.19338) {
                      return 1;
                    } else {  // if avgErr > 0.19338
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 88
                    return 4;
                  }
                } else {  // if area > 4977.5
                  return 2;
                }
              }
            } else {  // if avgErr > 0.33347
              if (shiftAvg <= 3.1153e-05) {
                if (maxEdgeLength <= 87.5) {
                  if (maxEdgeLength <= 80.5) {
                    if (avgErr <= 0.57446) {
                      return 3;
                    } else {  // if avgErr > 0.57446
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 80.5
                    return 3;
                  }
                } else {  // if maxEdgeLength > 87.5
                  if (shiftStd <= 0.00020885) {
                    return 4;
                  } else {  // if shiftStd > 0.00020885
                    return 1;
                  }
                }
              } else {  // if shiftAvg > 3.1153e-05
                if (area <= 4534.5) {
                  if (shiftStd <= 0.014198) {
                    if (shiftNcc <= 0.019805) {
                      return 3;
                    } else {  // if shiftNcc > 0.019805
                      return 4;
                    }
                  } else {  // if shiftStd > 0.014198
                    if (avgErr <= 1.8992) {
                      return 1;
                    } else {  // if avgErr > 1.8992
                      return 4;
                    }
                  }
                } else {  // if area > 4534.5
                  if (xyRatio <= 1.8238) {
                    if (shiftAvg <= 0.00029159) {
                      return 1;
                    } else {  // if shiftAvg > 0.00029159
                      return 1;
                    }
                  } else {  // if xyRatio > 1.8238
                    if (xyRatio <= 1.93) {
                      return 4;
                    } else {  // if xyRatio > 1.93
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if avgErr > 3.3711
            if (shiftNcc <= 0.13751) {
              if (shiftAvg <= 4.2213e-05) {
                return 4;
              } else {  // if shiftAvg > 4.2213e-05
                if (avgErr <= 3.6608) {
                  if (avgErr <= 3.5308) {
                    return 1;
                  } else {  // if avgErr > 3.5308
                    if (shiftStd <= 0.050766) {
                      return 1;
                    } else {  // if shiftStd > 0.050766
                      return 3;
                    }
                  }
                } else {  // if avgErr > 3.6608
                  if (area <= 5053) {
                    if (maxEdgeLength <= 71) {
                      return 1;
                    } else {  // if maxEdgeLength > 71
                      return 1;
                    }
                  } else {  // if area > 5053
                    return 2;
                  }
                }
              }
            } else {  // if shiftNcc > 0.13751
              if (maxEdgeLength <= 75.5) {
                return 1;
              } else {  // if maxEdgeLength > 75.5
                if (maxEdgeLength <= 84.5) {
                  if (xyRatio <= 1.3766) {
                    return 4;
                  } else {  // if xyRatio > 1.3766
                    return 3;
                  }
                } else {  // if maxEdgeLength > 84.5
                  return 4;
                }
              }
            }
          }
        }
      } else {  // if shiftAvg > 0.0032442
        if (maxEdgeLength <= 97.5) {
          if (area <= 4929.5) {
            if (xyRatio <= 0.87412) {
              if (shiftNcc <= 0.17594) {
                if (shiftAvg <= 0.0040676) {
                  if (shiftAvg <= 0.0039211) {
                    return 1;
                  } else {  // if shiftAvg > 0.0039211
                    if (avgErr <= 13.025) {
                      return 3;
                    } else {  // if avgErr > 13.025
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.0040676
                  if (avgErr <= 13.215) {
                    if (avgErr <= 2.1959) {
                      return 1;
                    } else {  // if avgErr > 2.1959
                      return 4;
                    }
                  } else {  // if avgErr > 13.215
                    if (avgErr <= 16.239) {
                      return 1;
                    } else {  // if avgErr > 16.239
                      return 3;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.17594
                if (avgErr <= 6.6781) {
                  if (shiftStd <= 0.026944) {
                    return 4;
                  } else {  // if shiftStd > 0.026944
                    if (avgErr <= 1.9679) {
                      return 3;
                    } else {  // if avgErr > 1.9679
                      return 1;
                    }
                  }
                } else {  // if avgErr > 6.6781
                  if (shiftAvg <= 0.045896) {
                    if (xyRatio <= 0.85111) {
                      return 4;
                    } else {  // if xyRatio > 0.85111
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.045896
                    if (maxEdgeLength <= 72.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 72.5
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.87412
              if (area <= 4217) {
                if (shiftNcc <= 0.084857) {
                  if (shiftStd <= 0.22135) {
                    if (shiftNcc <= 0.016572) {
                      return 4;
                    } else {  // if shiftNcc > 0.016572
                      return 0;
                    }
                  } else {  // if shiftStd > 0.22135
                    if (shiftNcc <= 0.081292) {
                      return 1;
                    } else {  // if shiftNcc > 0.081292
                      return 4;
                    }
                  }
                } else {  // if shiftNcc > 0.084857
                  if (avgErr <= 8.7263) {
                    if (area <= 4214) {
                      return 1;
                    } else {  // if area > 4214
                      return 0;
                    }
                  } else {  // if avgErr > 8.7263
                    if (shiftAvg <= 0.024166) {
                      return 3;
                    } else {  // if shiftAvg > 0.024166
                      return 1;
                    }
                  }
                }
              } else {  // if area > 4217
                if (shiftAvg <= 0.046102) {
                  if (avgErr <= 2.6073) {
                    if (maxEdgeLength <= 67.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 67.5
                      return 1;
                    }
                  } else {  // if avgErr > 2.6073
                    if (maxEdgeLength <= 76.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 76.5
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.046102
                  if (shiftStd <= 0.90952) {
                    if (avgErr <= 8.209) {
                      return 1;
                    } else {  // if avgErr > 8.209
                      return 1;
                    }
                  } else {  // if shiftStd > 0.90952
                    if (area <= 4488) {
                      return 1;
                    } else {  // if area > 4488
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 4929.5
            if (area <= 5061.5) {
              if (area <= 5051) {
                if (shiftNcc <= 0.12632) {
                  if (avgErr <= 1.9677) {
                    return 1;
                  } else {  // if avgErr > 1.9677
                    if (shiftAvg <= 0.043988) {
                      return 4;
                    } else {  // if shiftAvg > 0.043988
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.12632
                  if (xyRatio <= 1.1604) {
                    if (xyRatio <= 1.1119) {
                      return 1;
                    } else {  // if xyRatio > 1.1119
                      return 2;
                    }
                  } else {  // if xyRatio > 1.1604
                    if (xyRatio <= 1.783) {
                      return 1;
                    } else {  // if xyRatio > 1.783
                      return 1;
                    }
                  }
                }
              } else {  // if area > 5051
                if (shiftStd <= 0.044357) {
                  return 4;
                } else {  // if shiftStd > 0.044357
                  return 2;
                }
              }
            } else {  // if area > 5061.5
              return 1;
            }
          }
        } else {  // if maxEdgeLength > 97.5
          if (shiftAvg <= 0.45442) {
            if (area <= 4585.5) {
              if (shiftNcc <= 0.19567) {
                if (shiftNcc <= 0.033472) {
                  if (shiftAvg <= 0.042502) {
                    return 0;
                  } else {  // if shiftAvg > 0.042502
                    return 3;
                  }
                } else {  // if shiftNcc > 0.033472
                  return 4;
                }
              } else {  // if shiftNcc > 0.19567
                if (maxEdgeLength <= 99.5) {
                  return 1;
                } else {  // if maxEdgeLength > 99.5
                  if (shiftStd <= 0.18231) {
                    return 0;
                  } else {  // if shiftStd > 0.18231
                    if (xyRatio <= 2.4102) {
                      return 1;
                    } else {  // if xyRatio > 2.4102
                      return 0;
                    }
                  }
                }
              }
            } else {  // if area > 4585.5
              if (shiftNcc <= -0.00060971) {
                if (area <= 4889.5) {
                  return 4;
                } else {  // if area > 4889.5
                  return 2;
                }
              } else {  // if shiftNcc > -0.00060971
                if (avgErr <= 1.9923) {
                  if (maxEdgeLength <= 103.5) {
                    if (shiftAvg <= 0.088473) {
                      return 0;
                    } else {  // if shiftAvg > 0.088473
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 103.5
                    return 1;
                  }
                } else {  // if avgErr > 1.9923
                  if (maxEdgeLength <= 98.5) {
                    if (shiftStd <= 0.14191) {
                      return 0;
                    } else {  // if shiftStd > 0.14191
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 98.5
                    return 0;
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.45442
            if (shiftStd <= 0.17026) {
              if (avgErr <= 15.484) {
                return 0;
              } else {  // if avgErr > 15.484
                return 4;
              }
            } else {  // if shiftStd > 0.17026
              if (avgErr <= 5.1269) {
                return 3;
              } else {  // if avgErr > 5.1269
                if (avgErr <= 14.771) {
                  if (xyRatio <= 3.0658) {
                    return 4;
                  } else {  // if xyRatio > 3.0658
                    if (area <= 4439) {
                      return 4;
                    } else {  // if area > 4439
                      return 3;
                    }
                  }
                } else {  // if avgErr > 14.771
                  if (shiftStd <= 1.365) {
                    if (shiftStd <= 0.2757) {
                      return 0;
                    } else {  // if shiftStd > 0.2757
                      return 4;
                    }
                  } else {  // if shiftStd > 1.365
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    }
  } else {  // if area > 5071.5
    if (area <= 6139.5) {
      if (maxEdgeLength <= 97.5) {
        if (area <= 5196) {
          if (area <= 5186) {
            if (shiftNcc <= 0.082568) {
              if (maxEdgeLength <= 72.5) {
                if (shiftAvg <= 0.05791) {
                  return 2;
                } else {  // if shiftAvg > 0.05791
                  return 3;
                }
              } else {  // if maxEdgeLength > 72.5
                if (maxEdgeLength <= 81.5) {
                  if (xyRatio <= 1.2578) {
                    if (shiftStd <= 0.56599) {
                      return 4;
                    } else {  // if shiftStd > 0.56599
                      return 1;
                    }
                  } else {  // if xyRatio > 1.2578
                    if (shiftAvg <= 4.8728e-05) {
                      return 3;
                    } else {  // if shiftAvg > 4.8728e-05
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 81.5
                  if (xyRatio <= 1.4661) {
                    if (xyRatio <= 1.4051) {
                      return 2;
                    } else {  // if xyRatio > 1.4051
                      return 2;
                    }
                  } else {  // if xyRatio > 1.4661
                    if (xyRatio <= 1.5393) {
                      return 1;
                    } else {  // if xyRatio > 1.5393
                      return 2;
                    }
                  }
                }
              }
            } else {  // if shiftNcc > 0.082568
              if (area <= 5108) {
                if (area <= 5101.5) {
                  if (maxEdgeLength <= 90) {
                    if (shiftStd <= 0.026131) {
                      return 1;
                    } else {  // if shiftStd > 0.026131
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 90
                    if (shiftStd <= 0.040618) {
                      return 2;
                    } else {  // if shiftStd > 0.040618
                      return 1;
                    }
                  }
                } else {  // if area > 5101.5
                  if (shiftStd <= 0.44365) {
                    if (xyRatio <= 1.0024) {
                      return 1;
                    } else {  // if xyRatio > 1.0024
                      return 1;
                    }
                  } else {  // if shiftStd > 0.44365
                    if (maxEdgeLength <= 84.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 84.5
                      return 4;
                    }
                  }
                }
              } else {  // if area > 5108
                if (area <= 5183.5) {
                  if (shiftStd <= 0.83487) {
                    if (avgErr <= 6.7211) {
                      return 2;
                    } else {  // if avgErr > 6.7211
                      return 2;
                    }
                  } else {  // if shiftStd > 0.83487
                    return 1;
                  }
                } else {  // if area > 5183.5
                  if (area <= 5184.5) {
                    if (maxEdgeLength <= 76.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 76.5
                      return 1;
                    }
                  } else {  // if area > 5184.5
                    if (avgErr <= 2.16) {
                      return 3;
                    } else {  // if avgErr > 2.16
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 5186
            if (shiftStd <= 0.021297) {
              if (avgErr <= 3.2351) {
                return 4;
              } else {  // if avgErr > 3.2351
                return 3;
              }
            } else {  // if shiftStd > 0.021297
              if (avgErr <= 14.081) {
                if (shiftNcc <= 0.047666) {
                  return 3;
                } else {  // if shiftNcc > 0.047666
                  return 1;
                }
              } else {  // if avgErr > 14.081
                if (shiftNcc <= 0.30247) {
                  return 1;
                } else {  // if shiftNcc > 0.30247
                  return 4;
                }
              }
            }
          }
        } else {  // if area > 5196
          if (xyRatio <= 0.91192) {
            if (area <= 5375) {
              if (shiftAvg <= 0.00024209) {
                return 4;
              } else {  // if shiftAvg > 0.00024209
                if (shiftStd <= 4.8491) {
                  if (area <= 5231) {
                    if (maxEdgeLength <= 79) {
                      return 1;
                    } else {  // if maxEdgeLength > 79
                      return 2;
                    }
                  } else {  // if area > 5231
                    return 2;
                  }
                } else {  // if shiftStd > 4.8491
                  return 4;
                }
              }
            } else {  // if area > 5375
              if (shiftAvg <= 0.045927) {
                if (xyRatio <= 0.81551) {
                  if (xyRatio <= 0.7764) {
                    return 4;
                  } else {  // if xyRatio > 0.7764
                    return 2;
                  }
                } else {  // if xyRatio > 0.81551
                  if (shiftAvg <= 0.036642) {
                    return 4;
                  } else {  // if shiftAvg > 0.036642
                    if (shiftAvg <= 0.038865) {
                      return 3;
                    } else {  // if shiftAvg > 0.038865
                      return 4;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.045927
                if (xyRatio <= 0.76438) {
                  if (shiftAvg <= 0.063113) {
                    return 2;
                  } else {  // if shiftAvg > 0.063113
                    return 4;
                  }
                } else {  // if xyRatio > 0.76438
                  if (shiftAvg <= 0.064777) {
                    if (shiftAvg <= 0.060678) {
                      return 4;
                    } else {  // if shiftAvg > 0.060678
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.064777
                    return 2;
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.91192
            if (shiftStd <= 0.0087088) {
              if (avgErr <= 0.43142) {
                if (shiftStd <= 0.00015369) {
                  if (shiftStd <= 9.1716e-05) {
                    if (shiftStd <= 4.8965e-05) {
                      return 4;
                    } else {  // if shiftStd > 4.8965e-05
                      return 2;
                    }
                  } else {  // if shiftStd > 9.1716e-05
                    return 4;
                  }
                } else {  // if shiftStd > 0.00015369
                  if (maxEdgeLength <= 74.5) {
                    return 4;
                  } else {  // if maxEdgeLength > 74.5
                    if (avgErr <= 0.11666) {
                      return 4;
                    } else {  // if avgErr > 0.11666
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 0.43142
                if (avgErr <= 0.91135) {
                  if (avgErr <= 0.44169) {
                    return 3;
                  } else {  // if avgErr > 0.44169
                    if (xyRatio <= 0.99324) {
                      return 2;
                    } else {  // if xyRatio > 0.99324
                      return 4;
                    }
                  }
                } else {  // if avgErr > 0.91135
                  if (maxEdgeLength <= 75.5) {
                    return 4;
                  } else {  // if maxEdgeLength > 75.5
                    if (shiftAvg <= 0.0012804) {
                      return 2;
                    } else {  // if shiftAvg > 0.0012804
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftStd > 0.0087088
              if (area <= 6032.5) {
                if (xyRatio <= 1.4274) {
                  if (shiftAvg <= 0.037698) {
                    if (avgErr <= 4.5116) {
                      return 2;
                    } else {  // if avgErr > 4.5116
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.037698
                    if (shiftStd <= 2.4997) {
                      return 2;
                    } else {  // if shiftStd > 2.4997
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 1.4274
                  if (shiftStd <= 0.015886) {
                    if (avgErr <= 2.665) {
                      return 4;
                    } else {  // if avgErr > 2.665
                      return 2;
                    }
                  } else {  // if shiftStd > 0.015886
                    if (shiftStd <= 5.8226) {
                      return 2;
                    } else {  // if shiftStd > 5.8226
                      return 4;
                    }
                  }
                }
              } else {  // if area > 6032.5
                if (area <= 6086.5) {
                  if (area <= 6063.5) {
                    if (area <= 6040) {
                      return 3;
                    } else {  // if area > 6040
                      return 2;
                    }
                  } else {  // if area > 6063.5
                    if (maxEdgeLength <= 90) {
                      return 2;
                    } else {  // if maxEdgeLength > 90
                      return 3;
                    }
                  }
                } else {  // if area > 6086.5
                  if (shiftNcc <= 0.81987) {
                    if (maxEdgeLength <= 85.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 85.5
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.81987
                    if (avgErr <= 16.318) {
                      return 2;
                    } else {  // if avgErr > 16.318
                      return 4;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 97.5
        if (xyRatio <= 2.3921) {
          if (shiftNcc <= -0.02714) {
            return 2;
          } else {  // if shiftNcc > -0.02714
            if (shiftAvg <= 2.6212e-06) {
              if (shiftStd <= 0.00013096) {
                return 2;
              } else {  // if shiftStd > 0.00013096
                return 4;
              }
            } else {  // if shiftAvg > 2.6212e-06
              if (avgErr <= 13.071) {
                if (shiftNcc <= 0.61706) {
                  if (shiftNcc <= 0.37932) {
                    if (avgErr <= 7.7329) {
                      return 0;
                    } else {  // if avgErr > 7.7329
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.37932
                    if (area <= 5733) {
                      return 0;
                    } else {  // if area > 5733
                      return 0;
                    }
                  }
                } else {  // if shiftNcc > 0.61706
                  if (shiftNcc <= 0.90121) {
                    if (shiftNcc <= 0.66162) {
                      return 0;
                    } else {  // if shiftNcc > 0.66162
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.90121
                    if (xyRatio <= 1.8798) {
                      return 0;
                    } else {  // if xyRatio > 1.8798
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 13.071
                if (maxEdgeLength <= 103.5) {
                  if (avgErr <= 13.641) {
                    if (maxEdgeLength <= 101) {
                      return 2;
                    } else {  // if maxEdgeLength > 101
                      return 0;
                    }
                  } else {  // if avgErr > 13.641
                    if (shiftNcc <= 0.78106) {
                      return 0;
                    } else {  // if shiftNcc > 0.78106
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 103.5
                  if (shiftAvg <= 0.059435) {
                    return 0;
                  } else {  // if shiftAvg > 0.059435
                    if (shiftStd <= 0.22981) {
                      return 0;
                    } else {  // if shiftStd > 0.22981
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > 2.3921
          if (avgErr <= 6.35) {
            return 0;
          } else {  // if avgErr > 6.35
            if (area <= 5300.5) {
              return 0;
            } else {  // if area > 5300.5
              return 4;
            }
          }
        }
      }
    } else {  // if area > 6139.5
      if (area <= 9202) {
        if (maxEdgeLength <= 97.5) {
          if (xyRatio <= 1.012) {
            if (shiftNcc <= 0.69698) {
              if (area <= 6161) {
                if (shiftNcc <= 0.031824) {
                  if (xyRatio <= 0.92645) {
                    return 3;
                  } else {  // if xyRatio > 0.92645
                    return 4;
                  }
                } else {  // if shiftNcc > 0.031824
                  return 2;
                }
              } else {  // if area > 6161
                if (maxEdgeLength <= 85.5) {
                  if (avgErr <= 2.5319) {
                    return 4;
                  } else {  // if avgErr > 2.5319
                    if (shiftStd <= 0.022175) {
                      return 4;
                    } else {  // if shiftStd > 0.022175
                      return 3;
                    }
                  }
                } else {  // if maxEdgeLength > 85.5
                  if (shiftStd <= 0.13849) {
                    if (xyRatio <= 0.80337) {
                      return 3;
                    } else {  // if xyRatio > 0.80337
                      return 4;
                    }
                  } else {  // if shiftStd > 0.13849
                    if (avgErr <= 6.0308) {
                      return 3;
                    } else {  // if avgErr > 6.0308
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftNcc > 0.69698
              if (avgErr <= 4.7781) {
                if (area <= 6389) {
                  return 2;
                } else {  // if area > 6389
                  return 3;
                }
              } else {  // if avgErr > 4.7781
                if (shiftAvg <= 0.46345) {
                  if (shiftAvg <= 0.10693) {
                    if (area <= 7627.5) {
                      return 3;
                    } else {  // if area > 7627.5
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.10693
                    return 4;
                  }
                } else {  // if shiftAvg > 0.46345
                  if (maxEdgeLength <= 91) {
                    if (shiftAvg <= 0.52444) {
                      return 4;
                    } else {  // if shiftAvg > 0.52444
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 91
                    return 4;
                  }
                }
              }
            }
          } else {  // if xyRatio > 1.012
            if (area <= 6252) {
              if (maxEdgeLength <= 88.5) {
                if (xyRatio <= 1.2099) {
                  if (area <= 6146) {
                    if (shiftStd <= 0.14945) {
                      return 2;
                    } else {  // if shiftStd > 0.14945
                      return 4;
                    }
                  } else {  // if area > 6146
                    if (shiftStd <= 0.093355) {
                      return 4;
                    } else {  // if shiftStd > 0.093355
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 1.2099
                  if (xyRatio <= 1.2483) {
                    if (shiftStd <= 0.042848) {
                      return 2;
                    } else {  // if shiftStd > 0.042848
                      return 2;
                    }
                  } else {  // if xyRatio > 1.2483
                    if (shiftNcc <= 0.021803) {
                      return 4;
                    } else {  // if shiftNcc > 0.021803
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 88.5
                if (xyRatio <= 1.5078) {
                  if (area <= 6207) {
                    if (area <= 6181.5) {
                      return 3;
                    } else {  // if area > 6181.5
                      return 2;
                    }
                  } else {  // if area > 6207
                    if (shiftAvg <= 0.0020507) {
                      return 3;
                    } else {  // if shiftAvg > 0.0020507
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 1.5078
                  return 2;
                }
              }
            } else {  // if area > 6252
              if (avgErr <= 11.947) {
                if (maxEdgeLength <= 90.5) {
                  if (shiftAvg <= 0.78388) {
                    if (shiftStd <= 0.13909) {
                      return 3;
                    } else {  // if shiftStd > 0.13909
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.78388
                    if (avgErr <= 10.259) {
                      return 4;
                    } else {  // if avgErr > 10.259
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 90.5
                  if (area <= 6288.5) {
                    if (maxEdgeLength <= 91.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 91.5
                      return 3;
                    }
                  } else {  // if area > 6288.5
                    if (shiftStd <= 2.4054) {
                      return 3;
                    } else {  // if shiftStd > 2.4054
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 11.947
                if (area <= 7781) {
                  if (shiftNcc <= 0.87404) {
                    if (avgErr <= 12.311) {
                      return 4;
                    } else {  // if avgErr > 12.311
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.87404
                    if (maxEdgeLength <= 87) {
                      return 3;
                    } else {  // if maxEdgeLength > 87
                      return 4;
                    }
                  }
                } else {  // if area > 7781
                  if (shiftNcc <= 0.16318) {
                    if (shiftAvg <= 0.0059272) {
                      return 4;
                    } else {  // if shiftAvg > 0.0059272
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.16318
                    if (avgErr <= 16.116) {
                      return 4;
                    } else {  // if avgErr > 16.116
                      return 3;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 97.5
          if (xyRatio <= 1.1582) {
            if (avgErr <= 14.946) {
              if (shiftNcc <= 0.45451) {
                if (avgErr <= 11.526) {
                  if (avgErr <= 2.5781) {
                    return 0;
                  } else {  // if avgErr > 2.5781
                    if (shiftStd <= 1.8242) {
                      return 4;
                    } else {  // if shiftStd > 1.8242
                      return 0;
                    }
                  }
                } else {  // if avgErr > 11.526
                  if (area <= 8918) {
                    if (shiftNcc <= 0.16086) {
                      return 0;
                    } else {  // if shiftNcc > 0.16086
                      return 4;
                    }
                  } else {  // if area > 8918
                    return 1;
                  }
                }
              } else {  // if shiftNcc > 0.45451
                if (maxEdgeLength <= 98.5) {
                  return 4;
                } else {  // if maxEdgeLength > 98.5
                  if (shiftAvg <= 5.6311) {
                    if (xyRatio <= 1.1503) {
                      return 0;
                    } else {  // if xyRatio > 1.1503
                      return 3;
                    }
                  } else {  // if shiftAvg > 5.6311
                    return 4;
                  }
                }
              }
            } else {  // if avgErr > 14.946
              if (xyRatio <= 1.1388) {
                return 4;
              } else {  // if xyRatio > 1.1388
                if (shiftAvg <= 0.17918) {
                  return 4;
                } else {  // if shiftAvg > 0.17918
                  return 3;
                }
              }
            }
          } else {  // if xyRatio > 1.1582
            if (area <= 8957) {
              if (avgErr <= 13.524) {
                if (shiftAvg <= 0.074082) {
                  if (maxEdgeLength <= 111.5) {
                    if (area <= 8931.5) {
                      return 0;
                    } else {  // if area > 8931.5
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 111.5
                    if (shiftStd <= 0.00018048) {
                      return 0;
                    } else {  // if shiftStd > 0.00018048
                      return 0;
                    }
                  }
                } else {  // if shiftAvg > 0.074082
                  if (avgErr <= 4.0785) {
                    if (shiftStd <= 0.12968) {
                      return 0;
                    } else {  // if shiftStd > 0.12968
                      return 0;
                    }
                  } else {  // if avgErr > 4.0785
                    if (shiftStd <= 5.0899) {
                      return 0;
                    } else {  // if shiftStd > 5.0899
                      return 0;
                    }
                  }
                }
              } else {  // if avgErr > 13.524
                if (maxEdgeLength <= 104.5) {
                  if (avgErr <= 14.19) {
                    if (shiftAvg <= 0.47812) {
                      return 3;
                    } else {  // if shiftAvg > 0.47812
                      return 4;
                    }
                  } else {  // if avgErr > 14.19
                    if (shiftStd <= 0.033354) {
                      return 4;
                    } else {  // if shiftStd > 0.033354
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 104.5
                  if (xyRatio <= 1.546) {
                    if (shiftNcc <= 0.84338) {
                      return 0;
                    } else {  // if shiftNcc > 0.84338
                      return 3;
                    }
                  } else {  // if xyRatio > 1.546
                    if (shiftAvg <= 6.9274) {
                      return 0;
                    } else {  // if shiftAvg > 6.9274
                      return 4;
                    }
                  }
                }
              }
            } else {  // if area > 8957
              if (avgErr <= 5.1203) {
                if (shiftStd <= 0.0002478) {
                  return 3;
                } else {  // if shiftStd > 0.0002478
                  if (area <= 9040.5) {
                    if (shiftStd <= 0.0020197) {
                      return 1;
                    } else {  // if shiftStd > 0.0020197
                      return 0;
                    }
                  } else {  // if area > 9040.5
                    return 1;
                  }
                }
              } else {  // if avgErr > 5.1203
                if (avgErr <= 6.9863) {
                  if (shiftStd <= 2.4218) {
                    if (maxEdgeLength <= 106.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 106.5
                      return 0;
                    }
                  } else {  // if shiftStd > 2.4218
                    return 3;
                  }
                } else {  // if avgErr > 6.9863
                  if (xyRatio <= 1.4312) {
                    if (maxEdgeLength <= 112.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 112.5
                      return 1;
                    }
                  } else {  // if xyRatio > 1.4312
                    if (area <= 8969) {
                      return 1;
                    } else {  // if area > 8969
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 9202
        if (area <= 13926) {
          if (area <= 11566) {
            if (xyRatio <= 1.2321) {
              if (avgErr <= 5.6747) {
                if (area <= 9433) {
                  if (shiftAvg <= 0.036861) {
                    return 4;
                  } else {  // if shiftAvg > 0.036861
                    if (area <= 9347) {
                      return 1;
                    } else {  // if area > 9347
                      return 0;
                    }
                  }
                } else {  // if area > 9433
                  if (area <= 11398) {
                    if (shiftNcc <= 0.002031) {
                      return 4;
                    } else {  // if shiftNcc > 0.002031
                      return 1;
                    }
                  } else {  // if area > 11398
                    return 2;
                  }
                }
              } else {  // if avgErr > 5.6747
                if (xyRatio <= 1.127) {
                  if (shiftStd <= 0.68533) {
                    if (avgErr <= 6.0484) {
                      return 4;
                    } else {  // if avgErr > 6.0484
                      return 4;
                    }
                  } else {  // if shiftStd > 0.68533
                    if (shiftAvg <= 0.38918) {
                      return 1;
                    } else {  // if shiftAvg > 0.38918
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 1.127
                  if (shiftStd <= 0.82159) {
                    if (avgErr <= 8.8856) {
                      return 4;
                    } else {  // if avgErr > 8.8856
                      return 1;
                    }
                  } else {  // if shiftStd > 0.82159
                    return 1;
                  }
                }
              }
            } else {  // if xyRatio > 1.2321
              if (area <= 11292) {
                if (area <= 9399.5) {
                  if (avgErr <= 8.5783) {
                    if (shiftAvg <= 1.5989) {
                      return 1;
                    } else {  // if shiftAvg > 1.5989
                      return 0;
                    }
                  } else {  // if avgErr > 8.5783
                    if (xyRatio <= 1.7539) {
                      return 0;
                    } else {  // if xyRatio > 1.7539
                      return 1;
                    }
                  }
                } else {  // if area > 9399.5
                  if (avgErr <= 1.75) {
                    if (area <= 9491) {
                      return 3;
                    } else {  // if area > 9491
                      return 4;
                    }
                  } else {  // if avgErr > 1.75
                    if (area <= 11168) {
                      return 1;
                    } else {  // if area > 11168
                      return 1;
                    }
                  }
                }
              } else {  // if area > 11292
                if (area <= 11308) {
                  return 2;
                } else {  // if area > 11308
                  if (area <= 11376) {
                    if (shiftNcc <= 0.66022) {
                      return 1;
                    } else {  // if shiftNcc > 0.66022
                      return 1;
                    }
                  } else {  // if area > 11376
                    if (area <= 11458) {
                      return 2;
                    } else {  // if area > 11458
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 11566
            if (xyRatio <= 1.1327) {
              if (shiftAvg <= 0.23401) {
                if (shiftNcc <= 0.047661) {
                  if (avgErr <= 7.5473) {
                    return 4;
                  } else {  // if avgErr > 7.5473
                    return 2;
                  }
                } else {  // if shiftNcc > 0.047661
                  return 4;
                }
              } else {  // if shiftAvg > 0.23401
                if (avgErr <= 6.1294) {
                  if (shiftNcc <= 0.95962) {
                    return 2;
                  } else {  // if shiftNcc > 0.95962
                    return 3;
                  }
                } else {  // if avgErr > 6.1294
                  if (xyRatio <= 1.0453) {
                    return 4;
                  } else {  // if xyRatio > 1.0453
                    if (area <= 13024) {
                      return 2;
                    } else {  // if area > 13024
                      return 3;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 1.1327
              if (area <= 11836) {
                if (area <= 11833) {
                  if (avgErr <= 14.693) {
                    if (shiftAvg <= 0.50435) {
                      return 1;
                    } else {  // if shiftAvg > 0.50435
                      return 2;
                    }
                  } else {  // if avgErr > 14.693
                    if (shiftStd <= 0.90211) {
                      return 1;
                    } else {  // if shiftStd > 0.90211
                      return 1;
                    }
                  }
                } else {  // if area > 11833
                  return 1;
                }
              } else {  // if area > 11836
                if (area <= 13612) {
                  if (maxEdgeLength <= 124.5) {
                    if (shiftAvg <= 0.28182) {
                      return 4;
                    } else {  // if shiftAvg > 0.28182
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 124.5
                    if (shiftNcc <= -0.021709) {
                      return 2;
                    } else {  // if shiftNcc > -0.021709
                      return 2;
                    }
                  }
                } else {  // if area > 13612
                  if (area <= 13620) {
                    return 3;
                  } else {  // if area > 13620
                    if (area <= 13726) {
                      return 2;
                    } else {  // if area > 13726
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 13926
          if (area <= 21121) {
            if (maxEdgeLength <= 194.5) {
              if (area <= 14294) {
                if (area <= 14284) {
                  if (area <= 14144) {
                    if (area <= 14108) {
                      return 3;
                    } else {  // if area > 14108
                      return 2;
                    }
                  } else {  // if area > 14144
                    if (xyRatio <= 1.1035) {
                      return 4;
                    } else {  // if xyRatio > 1.1035
                      return 3;
                    }
                  }
                } else {  // if area > 14284
                  return 2;
                }
              } else {  // if area > 14294
                if (xyRatio <= 0.97535) {
                  if (avgErr <= 6.8016) {
                    return 3;
                  } else {  // if avgErr > 6.8016
                    return 4;
                  }
                } else {  // if xyRatio > 0.97535
                  if (xyRatio <= 1.1416) {
                    if (area <= 14398) {
                      return 2;
                    } else {  // if area > 14398
                      return 3;
                    }
                  } else {  // if xyRatio > 1.1416
                    if (area <= 20688) {
                      return 3;
                    } else {  // if area > 20688
                      return 3;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 194.5
              if (xyRatio <= 2.1425) {
                return 0;
              } else {  // if xyRatio > 2.1425
                if (xyRatio <= 2.1806) {
                  return 3;
                } else {  // if xyRatio > 2.1806
                  return 0;
                }
              }
            }
          } else {  // if area > 21121
            if (area <= 36973) {
              if (maxEdgeLength <= 194.5) {
                if (maxEdgeLength <= 163.5) {
                  if (area <= 21806) {
                    if (shiftNcc <= 0.97009) {
                      return 3;
                    } else {  // if shiftNcc > 0.97009
                      return 4;
                    }
                  } else {  // if area > 21806
                    return 4;
                  }
                } else {  // if maxEdgeLength > 163.5
                  if (xyRatio <= 1.6214) {
                    return 4;
                  } else {  // if xyRatio > 1.6214
                    if (xyRatio <= 1.6376) {
                      return 3;
                    } else {  // if xyRatio > 1.6376
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 194.5
                if (shiftStd <= 60.704) {
                  if (xyRatio <= 1.2561) {
                    return 4;
                  } else {  // if xyRatio > 1.2561
                    if (xyRatio <= 2.3206) {
                      return 0;
                    } else {  // if xyRatio > 2.3206
                      return 0;
                    }
                  }
                } else {  // if shiftStd > 60.704
                  if (shiftNcc <= 0.65692) {
                    return 1;
                  } else {  // if shiftNcc > 0.65692
                    if (maxEdgeLength <= 278.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 278.5
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 36973
              if (area <= 45474) {
                if (shiftAvg <= 6.1812) {
                  if (shiftNcc <= 0.30129) {
                    if (area <= 37989) {
                      return 4;
                    } else {  // if area > 37989
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.30129
                    return 4;
                  }
                } else {  // if shiftAvg > 6.1812
                  if (area <= 38277) {
                    if (shiftStd <= 7.6456) {
                      return 1;
                    } else {  // if shiftStd > 7.6456
                      return 1;
                    }
                  } else {  // if area > 38277
                    if (maxEdgeLength <= 218) {
                      return 2;
                    } else {  // if maxEdgeLength > 218
                      return 1;
                    }
                  }
                }
              } else {  // if area > 45474
                if (area <= 56757) {
                  if (area <= 46922) {
                    if (shiftStd <= 1.4383) {
                      return 1;
                    } else {  // if shiftStd > 1.4383
                      return 2;
                    }
                  } else {  // if area > 46922
                    if (xyRatio <= 2.386) {
                      return 2;
                    } else {  // if xyRatio > 2.386
                      return 4;
                    }
                  }
                } else {  // if area > 56757
                  if (maxEdgeLength <= 329.5) {
                    return 3;
                  } else {  // if maxEdgeLength > 329.5
                    if (shiftStd <= 4.888) {
                      return 4;
                    } else {  // if shiftStd > 4.888
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

} // namespace md
