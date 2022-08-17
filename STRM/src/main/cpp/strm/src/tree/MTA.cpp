#include "strm/tree/MTA.hpp"

namespace rm {

float MTA(float width, float height, float maxEdgeLength, float type, float origin, float xyRatio,
          float avgShiftX, float avgShiftY, float avgShift, float stdShiftX, float stdShiftY,
          float stdShift, float avgErr, float ncc) {
  if (maxEdgeLength <= 85.45) {
    if (avgShiftY <= 1.109) {
      return 0;
    } else {  // if avgShiftY > 1.109
      if (stdShiftX <= 0.179) {
        return 1;
      } else {  // if stdShiftX > 0.179
        return 0;
      }
    }
  } else {  // if maxEdgeLength > 85.45
    if (maxEdgeLength <= 125.32) {
      if (height <= 93.668) {
        if (xyRatio <= 0.503) {
          if (avgShiftY <= -0.75966) {
            if (width <= 50.56) {
              return 0;
            } else {  // if width > 50.56
              return 1;
            }
          } else {  // if avgShiftY > -0.75966
            if (width <= 46.821) {
              if (avgShift <= 0.010451) {
                return 0;
              } else {  // if avgShift > 0.010451
                if (stdShiftX <= 0.22183) {
                  return 1;
                } else {  // if stdShiftX > 0.22183
                  if (stdShiftX <= 0.23317) {
                    return 0;
                  } else {  // if stdShiftX > 0.23317
                    if (stdShift <= 0.273) {
                      return 1;
                    } else {  // if stdShift > 0.273
                      return 0;
                    }
                  }
                }
              }
            } else {  // if width > 46.821
              if (avgErr <= 6.612) {
                if (stdShiftY <= 0.062112) {
                  return 0;
                } else {  // if stdShiftY > 0.062112
                  if (avgShiftX <= 0.334) {
                    if (avgShift <= 0.068023) {
                      return 1;
                    } else {  // if avgShift > 0.068023
                      return 1;
                    }
                  } else {  // if avgShiftX > 0.334
                    if (maxEdgeLength <= 91.49) {
                      return 0;
                    } else {  // if maxEdgeLength > 91.49
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 6.612
                if (avgShift <= 0.32976) {
                  if (avgErr <= 7.2967) {
                    if (stdShift <= 0.015197) {
                      return 1;
                    } else {  // if stdShift > 0.015197
                      return 0;
                    }
                  } else {  // if avgErr > 7.2967
                    if (avgShiftY <= 0.34947) {
                      return 1;
                    } else {  // if avgShiftY > 0.34947
                      return 0;
                    }
                  }
                } else {  // if avgShift > 0.32976
                  if (avgShift <= 0.86095) {
                    if (avgShiftY <= 0.70105) {
                      return 1;
                    } else {  // if avgShiftY > 0.70105
                      return 0;
                    }
                  } else {  // if avgShift > 0.86095
                    if (width <= 50.777) {
                      return 1;
                    } else {  // if width > 50.777
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > 0.503
          if (xyRatio <= 1.118) {
            if (maxEdgeLength <= 91.333) {
              if (avgShift <= 3.308) {
                if (stdShiftX <= 0.55345) {
                  if (avgShiftX <= 1.3268) {
                    if (avgShiftY <= 0.68017) {
                      return 0;
                    } else {  // if avgShiftY > 0.68017
                      return 1;
                    }
                  } else {  // if avgShiftX > 1.3268
                    if (stdShiftX <= 0.19261) {
                      return 0;
                    } else {  // if stdShiftX > 0.19261
                      return 1;
                    }
                  }
                } else {  // if stdShiftX > 0.55345
                  if (avgErr <= 9.8076) {
                    if (avgShiftY <= 0.98095) {
                      return 1;
                    } else {  // if avgShiftY > 0.98095
                      return 0;
                    }
                  } else {  // if avgErr > 9.8076
                    return 0;
                  }
                }
              } else {  // if avgShift > 3.308
                if (xyRatio <= 0.89422) {
                  return 0;
                } else {  // if xyRatio > 0.89422
                  return 1;
                }
              }
            } else {  // if maxEdgeLength > 91.333
              if (xyRatio <= 0.69506) {
                if (avgErr <= 8.2914) {
                  if (avgShiftX <= 0.75839) {
                    if (avgShift <= 12.788) {
                      return 1;
                    } else {  // if avgShift > 12.788
                      return 0;
                    }
                  } else {  // if avgShiftX > 0.75839
                    if (avgShiftX <= 0.8781) {
                      return 0;
                    } else {  // if avgShiftX > 0.8781
                      return 1;
                    }
                  }
                } else {  // if avgErr > 8.2914
                  if (avgShiftY <= -3.2366) {
                    return 1;
                  } else {  // if avgShiftY > -3.2366
                    return 0;
                  }
                }
              } else {  // if xyRatio > 0.69506
                if (avgErr <= 3.7247) {
                  return 1;
                } else {  // if avgErr > 3.7247
                  if (height <= 86.544) {
                    return 1;
                  } else {  // if height > 86.544
                    if (avgShiftX <= -4.5632) {
                      return 1;
                    } else {  // if avgShiftX > -4.5632
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 1.118
            if (stdShiftY <= 0.020167) {
              return 0;
            } else {  // if stdShiftY > 0.020167
              return 1;
            }
          }
        }
      } else {  // if height > 93.668
        if (xyRatio <= 0.64289) {
          if (width <= 48.157) {
            if (width <= 47.825) {
              if (stdShiftY <= 0.4662) {
                return 1;
              } else {  // if stdShiftY > 0.4662
                if (avgErr <= 9.297) {
                  return 1;
                } else {  // if avgErr > 9.297
                  if (avgShiftX <= -1.1467) {
                    return 1;
                  } else {  // if avgShiftX > -1.1467
                    return 0;
                  }
                }
              }
            } else {  // if width > 47.825
              return 0;
            }
          } else {  // if width > 48.157
            if (avgShift <= 0.0054763) {
              return 0;
            } else {  // if avgShift > 0.0054763
              if (maxEdgeLength <= 125.14) {
                if (avgShift <= 0.050969) {
                  if (stdShiftY <= 0.07693) {
                    return 0;
                  } else {  // if stdShiftY > 0.07693
                    return 1;
                  }
                } else {  // if avgShift > 0.050969
                  if (maxEdgeLength <= 95.405) {
                    if (avgShiftX <= 1.1012) {
                      return 1;
                    } else {  // if avgShiftX > 1.1012
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 95.405
                    return 1;
                  }
                }
              } else {  // if maxEdgeLength > 125.14
                if (avgErr <= 6.9523) {
                  return 2;
                } else {  // if avgErr > 6.9523
                  if (avgShift <= 0.86456) {
                    return 2;
                  } else {  // if avgShift > 0.86456
                    return 1;
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > 0.64289
          if (width <= 80.222) {
            if (stdShiftY <= 0.40062) {
              if (avgShift <= 2.7661) {
                if (ncc <= 0.86138) {
                  return 1;
                } else {  // if ncc > 0.86138
                  if (maxEdgeLength <= 103) {
                    return 0;
                  } else {  // if maxEdgeLength > 103
                    return 1;
                  }
                }
              } else {  // if avgShift > 2.7661
                if (stdShift <= 0.73213) {
                  return 1;
                } else {  // if stdShift > 0.73213
                  return 0;
                }
              }
            } else {  // if stdShiftY > 0.40062
              if (avgErr <= 4.6083) {
                if (avgShift <= 2.2264) {
                  return 0;
                } else {  // if avgShift > 2.2264
                  return 1;
                }
              } else {  // if avgErr > 4.6083
                if (stdShift <= 1.051) {
                  return 1;
                } else {  // if stdShift > 1.051
                  if (maxEdgeLength <= 102.4) {
                    return 0;
                  } else {  // if maxEdgeLength > 102.4
                    return 1;
                  }
                }
              }
            }
          } else {  // if width > 80.222
            if (xyRatio <= 0.77998) {
              return 1;
            } else {  // if xyRatio > 0.77998
              if (avgShiftY <= -0.38778) {
                if (stdShiftY <= 0.26004) {
                  if (stdShift <= 0.087213) {
                    return 1;
                  } else {  // if stdShift > 0.087213
                    return 0;
                  }
                } else {  // if stdShiftY > 0.26004
                  if (ncc <= 0.97639) {
                    return 1;
                  } else {  // if ncc > 0.97639
                    if (ncc <= 0.98655) {
                      return 0;
                    } else {  // if ncc > 0.98655
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftY > -0.38778
                if (avgShiftX <= -0.97215) {
                  return 0;
                } else {  // if avgShiftX > -0.97215
                  if (avgShiftX <= 1.4053) {
                    return 1;
                  } else {  // if avgShiftX > 1.4053
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 125.32
      if (maxEdgeLength <= 150.98) {
        if (avgShiftY <= -0.18206) {
          if (maxEdgeLength <= 127.84) {
            if (avgShiftX <= -2.6095) {
              if (avgShift <= 13.187) {
                return 2;
              } else {  // if avgShift > 13.187
                return 1;
              }
            } else {  // if avgShiftX > -2.6095
              if (stdShift <= 0.74526) {
                if (stdShift <= 0.53847) {
                  if (stdShiftX <= 0.211) {
                    if (avgShiftX <= -0.066121) {
                      return 2;
                    } else {  // if avgShiftX > -0.066121
                      return 1;
                    }
                  } else {  // if stdShiftX > 0.211
                    if (avgShift <= 6.3037) {
                      return 1;
                    } else {  // if avgShift > 6.3037
                      return 1;
                    }
                  }
                } else {  // if stdShift > 0.53847
                  if (avgShift <= 1.4288) {
                    return 1;
                  } else {  // if avgShift > 1.4288
                    return 2;
                  }
                }
              } else {  // if stdShift > 0.74526
                if (avgShift <= 14.764) {
                  return 1;
                } else {  // if avgShift > 14.764
                  if (stdShiftX <= 2.1284) {
                    return 2;
                  } else {  // if stdShiftX > 2.1284
                    return 1;
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 127.84
            if (avgShiftX <= 0.74332) {
              if (width <= 104) {
                if (stdShiftX <= 0.12746) {
                  return 1;
                } else {  // if stdShiftX > 0.12746
                  if (xyRatio <= 0.34291) {
                    return 1;
                  } else {  // if xyRatio > 0.34291
                    if (ncc <= 0.99511) {
                      return 2;
                    } else {  // if ncc > 0.99511
                      return 1;
                    }
                  }
                }
              } else {  // if width > 104
                if (stdShiftY <= 0.40806) {
                  return 2;
                } else {  // if stdShiftY > 0.40806
                  if (stdShift <= 0.89456) {
                    return 1;
                  } else {  // if stdShift > 0.89456
                    if (stdShiftX <= 1.8072) {
                      return 2;
                    } else {  // if stdShiftX > 1.8072
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgShiftX > 0.74332
              if (avgShift <= 1.8599) {
                if (avgShiftY <= -0.42218) {
                  if (avgShiftY <= -0.47444) {
                    if (xyRatio <= 0.6017) {
                      return 1;
                    } else {  // if xyRatio > 0.6017
                      return 2;
                    }
                  } else {  // if avgShiftY > -0.47444
                    return 2;
                  }
                } else {  // if avgShiftY > -0.42218
                  if (height <= 129.43) {
                    if (avgShiftY <= -0.33987) {
                      return 1;
                    } else {  // if avgShiftY > -0.33987
                      return 2;
                    }
                  } else {  // if height > 129.43
                    if (avgShiftX <= 1.3179) {
                      return 1;
                    } else {  // if avgShiftX > 1.3179
                      return 2;
                    }
                  }
                }
              } else {  // if avgShift > 1.8599
                if (stdShiftX <= 1.6337) {
                  if (ncc <= 0.99878) {
                    if (xyRatio <= 0.50929) {
                      return 2;
                    } else {  // if xyRatio > 0.50929
                      return 2;
                    }
                  } else {  // if ncc > 0.99878
                    return 1;
                  }
                } else {  // if stdShiftX > 1.6337
                  return 1;
                }
              }
            }
          }
        } else {  // if avgShiftY > -0.18206
          if (avgShiftX <= -0.21115) {
            if (stdShiftY <= 1.7734) {
              if (ncc <= 0.97313) {
                if (stdShift <= 0.33258) {
                  return 2;
                } else {  // if stdShift > 0.33258
                  if (stdShift <= 0.33687) {
                    return 1;
                  } else {  // if stdShift > 0.33687
                    if (stdShiftX <= 0.25221) {
                      return 2;
                    } else {  // if stdShiftX > 0.25221
                      return 2;
                    }
                  }
                }
              } else {  // if ncc > 0.97313
                if (maxEdgeLength <= 131.04) {
                  if (stdShiftX <= 0.1146) {
                    return 2;
                  } else {  // if stdShiftX > 0.1146
                    return 1;
                  }
                } else {  // if maxEdgeLength > 131.04
                  if (avgShiftX <= -2.3523) {
                    if (avgShiftY <= 0.43929) {
                      return 2;
                    } else {  // if avgShiftY > 0.43929
                      return 1;
                    }
                  } else {  // if avgShiftX > -2.3523
                    return 2;
                  }
                }
              }
            } else {  // if stdShiftY > 1.7734
              if (avgShiftY <= 1.7335) {
                return 2;
              } else {  // if avgShiftY > 1.7335
                return 1;
              }
            }
          } else {  // if avgShiftX > -0.21115
            if (stdShiftX <= 0.44113) {
              if (xyRatio <= 0.59735) {
                if (avgShiftX <= 2.2717) {
                  if (width <= 80.596) {
                    if (width <= 77.514) {
                      return 2;
                    } else {  // if width > 77.514
                      return 2;
                    }
                  } else {  // if width > 80.596
                    return 2;
                  }
                } else {  // if avgShiftX > 2.2717
                  return 1;
                }
              } else {  // if xyRatio > 0.59735
                if (xyRatio <= 0.65212) {
                  if (avgErr <= 5.3846) {
                    return 2;
                  } else {  // if avgErr > 5.3846
                    if (avgShiftY <= -0.11846) {
                      return 2;
                    } else {  // if avgShiftY > -0.11846
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.65212
                  return 2;
                }
              }
            } else {  // if stdShiftX > 0.44113
              if (width <= 88.648) {
                if (xyRatio <= 0.58627) {
                  if (avgShiftX <= 2.5383) {
                    if (avgShiftX <= 1.3135) {
                      return 2;
                    } else {  // if avgShiftX > 1.3135
                      return 2;
                    }
                  } else {  // if avgShiftX > 2.5383
                    return 1;
                  }
                } else {  // if xyRatio > 0.58627
                  return 1;
                }
              } else {  // if width > 88.648
                if (avgErr <= 8.4291) {
                  return 2;
                } else {  // if avgErr > 8.4291
                  if (maxEdgeLength <= 146.82) {
                    return 2;
                  } else {  // if maxEdgeLength > 146.82
                    return 1;
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 150.98
        if (origin <= 2.5) {
          if (avgErr <= 5.2592) {
            if (stdShiftY <= 0.53105) {
              if (avgShiftY <= -0.38827) {
                if (stdShiftX <= 0.76622) {
                  if (avgErr <= 3.3958) {
                    if (avgErr <= 3.0962) {
                      return 1;
                    } else {  // if avgErr > 3.0962
                      return 2;
                    }
                  } else {  // if avgErr > 3.3958
                    if (stdShiftY <= 0.40308) {
                      return 1;
                    } else {  // if stdShiftY > 0.40308
                      return 1;
                    }
                  }
                } else {  // if stdShiftX > 0.76622
                  if (stdShift <= 1.5989) {
                    if (stdShiftY <= 0.52026) {
                      return 2;
                    } else {  // if stdShiftY > 0.52026
                      return 1;
                    }
                  } else {  // if stdShift > 1.5989
                    return 1;
                  }
                }
              } else {  // if avgShiftY > -0.38827
                if (stdShift <= 0.82634) {
                  if (ncc <= 0.98321) {
                    if (stdShiftX <= 0.41045) {
                      return 2;
                    } else {  // if stdShiftX > 0.41045
                      return 0;
                    }
                  } else {  // if ncc > 0.98321
                    return 0;
                  }
                } else {  // if stdShift > 0.82634
                  if (avgShiftY <= -0.18029) {
                    return 2;
                  } else {  // if avgShiftY > -0.18029
                    return 1;
                  }
                }
              }
            } else {  // if stdShiftY > 0.53105
              if (stdShift <= 5.9485) {
                if (avgShiftY <= -1.0972) {
                  return 2;
                } else {  // if avgShiftY > -1.0972
                  if (avgShiftY <= -1.0261) {
                    if (stdShift <= 2.8801) {
                      return 0;
                    } else {  // if stdShift > 2.8801
                      return 1;
                    }
                  } else {  // if avgShiftY > -1.0261
                    if (stdShiftX <= 0.59632) {
                      return 0;
                    } else {  // if stdShiftX > 0.59632
                      return 2;
                    }
                  }
                }
              } else {  // if stdShift > 5.9485
                if (xyRatio <= 0.71429) {
                  return 1;
                } else {  // if xyRatio > 0.71429
                  if (avgShift <= 23.243) {
                    return 0;
                  } else {  // if avgShift > 23.243
                    return 1;
                  }
                }
              }
            }
          } else {  // if avgErr > 5.2592
            if (height <= 267.38) {
              if (xyRatio <= 0.45299) {
                if (avgShiftX <= 4.5638) {
                  if (maxEdgeLength <= 153.1) {
                    if (avgShift <= 6.4203) {
                      return 2;
                    } else {  // if avgShift > 6.4203
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 153.1
                    if (avgErr <= 9.2813) {
                      return 2;
                    } else {  // if avgErr > 9.2813
                      return 2;
                    }
                  }
                } else {  // if avgShiftX > 4.5638
                  if (avgShift <= 28.558) {
                    return 1;
                  } else {  // if avgShift > 28.558
                    if (stdShiftY <= 0.74621) {
                      return 1;
                    } else {  // if stdShiftY > 0.74621
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.45299
                if (maxEdgeLength <= 262.16) {
                  if (ncc <= 0.74499) {
                    if (stdShiftX <= 5.126) {
                      return 2;
                    } else {  // if stdShiftX > 5.126
                      return 1;
                    }
                  } else {  // if ncc > 0.74499
                    if (stdShiftX <= 1.1132) {
                      return 2;
                    } else {  // if stdShiftX > 1.1132
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 262.16
                  if (stdShiftY <= 0.62851) {
                    if (avgShiftY <= -1.342) {
                      return 1;
                    } else {  // if avgShiftY > -1.342
                      return 0;
                    }
                  } else {  // if stdShiftY > 0.62851
                    if (avgShiftY <= -3.9034) {
                      return 0;
                    } else {  // if avgShiftY > -3.9034
                      return 0;
                    }
                  }
                }
              }
            } else {  // if height > 267.38
              if (stdShiftY <= 2.0388) {
                if (avgShift <= 86.294) {
                  if (width <= 133.22) {
                    return 0;
                  } else {  // if width > 133.22
                    if (avgShiftX <= 3.3611) {
                      return 1;
                    } else {  // if avgShiftX > 3.3611
                      return 1;
                    }
                  }
                } else {  // if avgShift > 86.294
                  return 2;
                }
              } else {  // if stdShiftY > 2.0388
                if (width <= 197.51) {
                  if (avgShift <= 36.205) {
                    if (stdShiftX <= 1.2679) {
                      return 2;
                    } else {  // if stdShiftX > 1.2679
                      return 0;
                    }
                  } else {  // if avgShift > 36.205
                    if (avgErr <= 9.4126) {
                      return 1;
                    } else {  // if avgErr > 9.4126
                      return 2;
                    }
                  }
                } else {  // if width > 197.51
                  return 1;
                }
              }
            }
          }
        } else {  // if origin > 2.5
          if (avgShift <= 0.44875) {
            if (avgErr <= 3.7483) {
              if (stdShiftX <= 0.22957) {
                return 2;
              } else {  // if stdShiftX > 0.22957
                if (avgShiftX <= 0.24772) {
                  return 0;
                } else {  // if avgShiftX > 0.24772
                  return 2;
                }
              }
            } else {  // if avgErr > 3.7483
              if (avgShiftX <= -0.01848) {
                if (avgShiftY <= 0.5061) {
                  if (ncc <= 0.49407) {
                    return 2;
                  } else {  // if ncc > 0.49407
                    return 0;
                  }
                } else {  // if avgShiftY > 0.5061
                  return 1;
                }
              } else {  // if avgShiftX > -0.01848
                if (xyRatio <= 0.52932) {
                  return 2;
                } else {  // if xyRatio > 0.52932
                  if (avgShiftY <= 0.15637) {
                    if (avgShiftY <= 0.12565) {
                      return 0;
                    } else {  // if avgShiftY > 0.12565
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.15637
                    if (ncc <= 0.58715) {
                      return 2;
                    } else {  // if ncc > 0.58715
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if avgShift > 0.44875
            if (width <= 93) {
              if (avgShiftX <= -1.106) {
                if (avgShift <= 1.7447) {
                  if (stdShiftY <= 0.3547) {
                    return 0;
                  } else {  // if stdShiftY > 0.3547
                    return 2;
                  }
                } else {  // if avgShift > 1.7447
                  if (avgErr <= 8.6981) {
                    return 0;
                  } else {  // if avgErr > 8.6981
                    if (stdShiftY <= 1.9356) {
                      return 2;
                    } else {  // if stdShiftY > 1.9356
                      return 0;
                    }
                  }
                }
              } else {  // if avgShiftX > -1.106
                if (avgErr <= 4.8085) {
                  return 2;
                } else {  // if avgErr > 4.8085
                  if (stdShiftY <= 0.7884) {
                    return 0;
                  } else {  // if stdShiftY > 0.7884
                    if (stdShift <= 0.93827) {
                      return 2;
                    } else {  // if stdShift > 0.93827
                      return 0;
                    }
                  }
                }
              }
            } else {  // if width > 93
              if (ncc <= 0.54367) {
                if (stdShift <= 19.252) {
                  if (avgShiftX <= -3.9088) {
                    return 2;
                  } else {  // if avgShiftX > -3.9088
                    if (stdShiftY <= 0.26874) {
                      return 2;
                    } else {  // if stdShiftY > 0.26874
                      return 0;
                    }
                  }
                } else {  // if stdShift > 19.252
                  return 2;
                }
              } else {  // if ncc > 0.54367
                if (stdShiftY <= 0.79004) {
                  if (stdShiftX <= 0.98328) {
                    if (avgShiftX <= 1.3556) {
                      return 2;
                    } else {  // if avgShiftX > 1.3556
                      return 0;
                    }
                  } else {  // if stdShiftX > 0.98328
                    if (avgErr <= 10.435) {
                      return 2;
                    } else {  // if avgErr > 10.435
                      return 0;
                    }
                  }
                } else {  // if stdShiftY > 0.79004
                  if (ncc <= 0.70819) {
                    if (avgShiftX <= 2.072) {
                      return 0;
                    } else {  // if avgShiftX > 2.072
                      return 0;
                    }
                  } else {  // if ncc > 0.70819
                    if (height <= 173) {
                      return 0;
                    } else {  // if height > 173
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
}

} // namespace rm
