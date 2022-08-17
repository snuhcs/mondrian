#include "strm/tree/VIRAT.hpp"

namespace rm {

float VIRAT(float width, float height, float maxEdgeLength, float type, float origin, float xyRatio,
            float avgShiftX, float avgShiftY, float avgShift, float stdShiftX, float stdShiftY,
            float stdShift, float avgErr, float ncc) {
  if (height <= 131.38) {
    if (maxEdgeLength <= 125.55) {
      return 0;
    } else {  // if maxEdgeLength > 125.55
      if (width <= 65.4) {
        if (width <= 63.417) {
          if (stdShiftX <= 0.15425) {
            if (height <= 130.46) {
              if (avgShiftY <= 0.59933) {
                if (avgErr <= 0.36726) {
                  if (xyRatio <= 0.38148) {
                    return 0;
                  } else {  // if xyRatio > 0.38148
                    return 1;
                  }
                } else {  // if avgErr > 0.36726
                  if (avgShiftY <= 0.26809) {
                    if (avgErr <= 5.293) {
                      return 0;
                    } else {  // if avgErr > 5.293
                      return 0;
                    }
                  } else {  // if avgShiftY > 0.26809
                    if (stdShiftY <= 0.2155) {
                      return 0;
                    } else {  // if stdShiftY > 0.2155
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftY > 0.59933
                return 1;
              }
            } else {  // if height > 130.46
              return 1;
            }
          } else {  // if stdShiftX > 0.15425
            if (avgShiftY <= -0.21445) {
              if (width <= 59.184) {
                if (avgShiftX <= 0.74157) {
                  return 0;
                } else {  // if avgShiftX > 0.74157
                  return 1;
                }
              } else {  // if width > 59.184
                return 0;
              }
            } else {  // if avgShiftY > -0.21445
              if (avgShiftX <= -0.74174) {
                if (height <= 125.83) {
                  return 1;
                } else {  // if height > 125.83
                  return 0;
                }
              } else {  // if avgShiftX > -0.74174
                if (avgShiftX <= 1.9456) {
                  if (avgErr <= 3.824) {
                    if (stdShiftX <= 0.2446) {
                      return 1;
                    } else {  // if stdShiftX > 0.2446
                      return 0;
                    }
                  } else {  // if avgErr > 3.824
                    if (xyRatio <= 0.36644) {
                      return 1;
                    } else {  // if xyRatio > 0.36644
                      return 1;
                    }
                  }
                } else {  // if avgShiftX > 1.9456
                  return 0;
                }
              }
            }
          }
        } else {  // if width > 63.417
          if (avgErr <= 5.3091) {
            return 1;
          } else {  // if avgErr > 5.3091
            if (width <= 65.209) {
              if (avgShift <= 1.743) {
                return 0;
              } else {  // if avgShift > 1.743
                return 1;
              }
            } else {  // if width > 65.209
              return 0;
            }
          }
        }
      } else {  // if width > 65.4
        if (stdShift <= 0.0023478) {
          if (stdShiftX <= 0.034617) {
            if (avgErr <= 0.15767) {
              if (stdShift <= 0.00025005) {
                return 1;
              } else {  // if stdShift > 0.00025005
                return 2;
              }
            } else {  // if avgErr > 0.15767
              if (avgShiftY <= 0.0043804) {
                return 0;
              } else {  // if avgShiftY > 0.0043804
                return 2;
              }
            }
          } else {  // if stdShiftX > 0.034617
            if (maxEdgeLength <= 268.87) {
              return 2;
            } else {  // if maxEdgeLength > 268.87
              if (stdShiftX <= 0.041871) {
                return 0;
              } else {  // if stdShiftX > 0.041871
                return 2;
              }
            }
          }
        } else {  // if stdShift > 0.0023478
          if (height <= 130.09) {
            if (xyRatio <= 2.3273) {
              if (stdShift <= 0.10369) {
                if (avgShift <= 0.071) {
                  if (xyRatio <= 2.1936) {
                    if (avgShiftY <= 0.037724) {
                      return 0;
                    } else {  // if avgShiftY > 0.037724
                      return 1;
                    }
                  } else {  // if xyRatio > 2.1936
                    if (maxEdgeLength <= 262.58) {
                      return 1;
                    } else {  // if maxEdgeLength > 262.58
                      return 0;
                    }
                  }
                } else {  // if avgShift > 0.071
                  if (width <= 136.86) {
                    return 1;
                  } else {  // if width > 136.86
                    if (stdShiftY <= 0.1345) {
                      return 2;
                    } else {  // if stdShiftY > 0.1345
                      return 1;
                    }
                  }
                }
              } else {  // if stdShift > 0.10369
                if (avgShift <= 0.0068112) {
                  return 2;
                } else {  // if avgShift > 0.0068112
                  if (avgShiftY <= 1.4429) {
                    if (stdShiftY <= 0.097411) {
                      return 0;
                    } else {  // if stdShiftY > 0.097411
                      return 0;
                    }
                  } else {  // if avgShiftY > 1.4429
                    return 2;
                  }
                }
              }
            } else {  // if xyRatio > 2.3273
              if (avgShiftY <= 0.95747) {
                if (width <= 288.63) {
                  if (avgErr <= 0.31956) {
                    if (avgShiftY <= -0.0038189) {
                      return 2;
                    } else {  // if avgShiftY > -0.0038189
                      return 0;
                    }
                  } else {  // if avgErr > 0.31956
                    if (ncc <= 0.99279) {
                      return 0;
                    } else {  // if ncc > 0.99279
                      return 0;
                    }
                  }
                } else {  // if width > 288.63
                  if (xyRatio <= 2.9162) {
                    if (stdShiftX <= 0.055997) {
                      return 1;
                    } else {  // if stdShiftX > 0.055997
                      return 0;
                    }
                  } else {  // if xyRatio > 2.9162
                    if (stdShift <= 0.12158) {
                      return 0;
                    } else {  // if stdShift > 0.12158
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftY > 0.95747
                if (avgErr <= 4.8662) {
                  return 0;
                } else {  // if avgErr > 4.8662
                  return 2;
                }
              }
            }
          } else {  // if height > 130.09
            if (height <= 131.15) {
              return 0;
            } else {  // if height > 131.15
              if (avgShiftY <= 0.029425) {
                return 0;
              } else {  // if avgShiftY > 0.029425
                if (stdShift <= 0.0098722) {
                  return 1;
                } else {  // if stdShift > 0.0098722
                  return 0;
                }
              }
            }
          }
        }
      }
    }
  } else {  // if height > 131.38
    if (maxEdgeLength <= 154.61) {
      if (xyRatio <= 0.61445) {
        if (width <= 60.374) {
          if (avgShiftX <= -0.057773) {
            if (avgShiftX <= -0.85466) {
              if (ncc <= 0.78119) {
                return 0;
              } else {  // if ncc > 0.78119
                if (stdShiftX <= 0.36158) {
                  if (stdShiftX <= 0.16656) {
                    return 0;
                  } else {  // if stdShiftX > 0.16656
                    if (avgShiftX <= -0.86231) {
                      return 1;
                    } else {  // if avgShiftX > -0.86231
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 0.36158
                  return 0;
                }
              }
            } else {  // if avgShiftX > -0.85466
              if (maxEdgeLength <= 142.61) {
                if (ncc <= 0.98808) {
                  if (width <= 59.786) {
                    return 1;
                  } else {  // if width > 59.786
                    if (ncc <= 0.88786) {
                      return 0;
                    } else {  // if ncc > 0.88786
                      return 1;
                    }
                  }
                } else {  // if ncc > 0.98808
                  if (avgShift <= 0.74665) {
                    return 2;
                  } else {  // if avgShift > 0.74665
                    return 1;
                  }
                }
              } else {  // if maxEdgeLength > 142.61
                if (stdShiftX <= 0.049737) {
                  return 0;
                } else {  // if stdShiftX > 0.049737
                  if (avgShift <= 0.70089) {
                    if (avgShift <= 0.27294) {
                      return 1;
                    } else {  // if avgShift > 0.27294
                      return 1;
                    }
                  } else {  // if avgShift > 0.70089
                    if (height <= 147.66) {
                      return 1;
                    } else {  // if height > 147.66
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if avgShiftX > -0.057773
            if (xyRatio <= 0.3213) {
              if (stdShift <= 0.01648) {
                if (stdShiftY <= 0.060978) {
                  return 0;
                } else {  // if stdShiftY > 0.060978
                  if (avgShiftX <= -0.028405) {
                    return 0;
                  } else {  // if avgShiftX > -0.028405
                    return 1;
                  }
                }
              } else {  // if stdShift > 0.01648
                if (height <= 143.22) {
                  if (stdShiftX <= 0.35603) {
                    if (stdShiftX <= 0.14179) {
                      return 0;
                    } else {  // if stdShiftX > 0.14179
                      return 0;
                    }
                  } else {  // if stdShiftX > 0.35603
                    return 1;
                  }
                } else {  // if height > 143.22
                  if (avgShift <= 1.6463) {
                    if (stdShiftX <= 0.19142) {
                      return 1;
                    } else {  // if stdShiftX > 0.19142
                      return 2;
                    }
                  } else {  // if avgShift > 1.6463
                    if (avgErr <= 6.0101) {
                      return 0;
                    } else {  // if avgErr > 6.0101
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.3213
              return 1;
            }
          }
        } else {  // if width > 60.374
          if (stdShiftX <= 0.12199) {
            if (avgErr <= 0.41153) {
              if (xyRatio <= 0.47594) {
                if (maxEdgeLength <= 143.04) {
                  if (avgShiftX <= 0.067151) {
                    return 1;
                  } else {  // if avgShiftX > 0.067151
                    return 2;
                  }
                } else {  // if maxEdgeLength > 143.04
                  if (avgShiftY <= -0.034119) {
                    if (ncc <= 0.61821) {
                      return 1;
                    } else {  // if ncc > 0.61821
                      return 2;
                    }
                  } else {  // if avgShiftY > -0.034119
                    if (ncc <= 0.093711) {
                      return 0;
                    } else {  // if ncc > 0.093711
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.47594
                return 0;
              }
            } else {  // if avgErr > 0.41153
              if (stdShiftX <= 0.088835) {
                if (avgShiftX <= 0.056509) {
                  if (stdShiftY <= 0.21277) {
                    if (avgShift <= 0.0005948) {
                      return 1;
                    } else {  // if avgShift > 0.0005948
                      return 0;
                    }
                  } else {  // if stdShiftY > 0.21277
                    if (avgErr <= 3.1797) {
                      return 1;
                    } else {  // if avgErr > 3.1797
                      return 0;
                    }
                  }
                } else {  // if avgShiftX > 0.056509
                  if (xyRatio <= 0.39389) {
                    if (avgShiftY <= -0.02093) {
                      return 0;
                    } else {  // if avgShiftY > -0.02093
                      return 2;
                    }
                  } else {  // if xyRatio > 0.39389
                    if (stdShiftX <= 0.086905) {
                      return 1;
                    } else {  // if stdShiftX > 0.086905
                      return 0;
                    }
                  }
                }
              } else {  // if stdShiftX > 0.088835
                if (avgShiftY <= -0.1562) {
                  if (width <= 66.835) {
                    return 1;
                  } else {  // if width > 66.835
                    if (ncc <= 0.85942) {
                      return 2;
                    } else {  // if ncc > 0.85942
                      return 0;
                    }
                  }
                } else {  // if avgShiftY > -0.1562
                  if (avgErr <= 5.4201) {
                    return 0;
                  } else {  // if avgErr > 5.4201
                    return 2;
                  }
                }
              }
            }
          } else {  // if stdShiftX > 0.12199
            if (height <= 147.25) {
              if (height <= 135.8) {
                if (xyRatio <= 0.37008) {
                  if (stdShiftX <= 0.15395) {
                    return 1;
                  } else {  // if stdShiftX > 0.15395
                    return 0;
                  }
                } else {  // if xyRatio > 0.37008
                  if (width <= 65.649) {
                    if (stdShiftX <= 0.19144) {
                      return 0;
                    } else {  // if stdShiftX > 0.19144
                      return 1;
                    }
                  } else {  // if width > 65.649
                    if (stdShift <= 0.082329) {
                      return 0;
                    } else {  // if stdShift > 0.082329
                      return 0;
                    }
                  }
                }
              } else {  // if height > 135.8
                if (height <= 141.84) {
                  if (ncc <= 0.065925) {
                    if (ncc <= -0.039287) {
                      return 1;
                    } else {  // if ncc > -0.039287
                      return 0;
                    }
                  } else {  // if ncc > 0.065925
                    if (stdShiftY <= 0.64992) {
                      return 1;
                    } else {  // if stdShiftY > 0.64992
                      return 1;
                    }
                  }
                } else {  // if height > 141.84
                  if (stdShift <= 0.61874) {
                    if (maxEdgeLength <= 145.83) {
                      return 1;
                    } else {  // if maxEdgeLength > 145.83
                      return 1;
                    }
                  } else {  // if stdShift > 0.61874
                    if (avgShiftX <= 1.8636) {
                      return 0;
                    } else {  // if avgShiftX > 1.8636
                      return 1;
                    }
                  }
                }
              }
            } else {  // if height > 147.25
              if (avgShiftX <= 1.2251) {
                if (avgErr <= 5.2603) {
                  if (avgErr <= 3.8936) {
                    if (avgErr <= 2.3643) {
                      return 0;
                    } else {  // if avgErr > 2.3643
                      return 2;
                    }
                  } else {  // if avgErr > 3.8936
                    return 0;
                  }
                } else {  // if avgErr > 5.2603
                  if (avgShiftX <= 0.88881) {
                    if (avgShiftY <= 0.53025) {
                      return 1;
                    } else {  // if avgShiftY > 0.53025
                      return 2;
                    }
                  } else {  // if avgShiftX > 0.88881
                    if (avgShift <= 2.7784) {
                      return 2;
                    } else {  // if avgShift > 2.7784
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftX > 1.2251
                if (avgShift <= 5.6383) {
                  if (stdShiftY <= 1.0775) {
                    if (avgErr <= 6.6232) {
                      return 0;
                    } else {  // if avgErr > 6.6232
                      return 2;
                    }
                  } else {  // if stdShiftY > 1.0775
                    return 0;
                  }
                } else {  // if avgShift > 5.6383
                  if (avgShiftX <= 2.4015) {
                    return 1;
                  } else {  // if avgShiftX > 2.4015
                    return 2;
                  }
                }
              }
            }
          }
        }
      } else {  // if xyRatio > 0.61445
        if (xyRatio <= 1.0398) {
          if (stdShiftY <= 0.4833) {
            if (avgShiftX <= -0.45497) {
              return 0;
            } else {  // if avgShiftX > -0.45497
              if (avgErr <= 5.1491) {
                if (stdShift <= 0.020957) {
                  if (maxEdgeLength <= 141.43) {
                    if (avgShiftY <= -0.033007) {
                      return 0;
                    } else {  // if avgShiftY > -0.033007
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 141.43
                    if (stdShiftX <= 0.043448) {
                      return 1;
                    } else {  // if stdShiftX > 0.043448
                      return 2;
                    }
                  }
                } else {  // if stdShift > 0.020957
                  if (ncc <= -0.011451) {
                    if (stdShift <= 0.29695) {
                      return 1;
                    } else {  // if stdShift > 0.29695
                      return 2;
                    }
                  } else {  // if ncc > -0.011451
                    return 1;
                  }
                }
              } else {  // if avgErr > 5.1491
                if (stdShiftY <= 0.14445) {
                  return 1;
                } else {  // if stdShiftY > 0.14445
                  return 2;
                }
              }
            }
          } else {  // if stdShiftY > 0.4833
            if (stdShift <= 0.89387) {
              return 0;
            } else {  // if stdShift > 0.89387
              return 1;
            }
          }
        } else {  // if xyRatio > 1.0398
          if (avgShiftY <= -1.2769) {
            return 1;
          } else {  // if avgShiftY > -1.2769
            if (ncc <= 0.9828) {
              if (avgErr <= 4.4803) {
                if (stdShiftX <= 0.79748) {
                  return 1;
                } else {  // if stdShiftX > 0.79748
                  return 2;
                }
              } else {  // if avgErr > 4.4803
                if (avgShiftY <= -1.0122) {
                  return 2;
                } else {  // if avgShiftY > -1.0122
                  return 1;
                }
              }
            } else {  // if ncc > 0.9828
              if (ncc <= 0.99011) {
                return 2;
              } else {  // if ncc > 0.99011
                if (avgShiftY <= -1.186) {
                  return 2;
                } else {  // if avgShiftY > -1.186
                  return 1;
                }
              }
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 154.61
      if (width <= 330.15) {
        if (xyRatio <= 0.3533) {
          if (stdShiftY <= 0.24934) {
            if (width <= 72.878) {
              if (xyRatio <= 0.2134) {
                if (avgErr <= 0.70509) {
                  return 2;
                } else {  // if avgErr > 0.70509
                  if (ncc <= 0.43263) {
                    return 1;
                  } else {  // if ncc > 0.43263
                    if (stdShiftX <= 0.058335) {
                      return 1;
                    } else {  // if stdShiftX > 0.058335
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.2134
                if (xyRatio <= 0.31077) {
                  if (avgShiftY <= -0.018741) {
                    if (avgShiftX <= 0.20833) {
                      return 1;
                    } else {  // if avgShiftX > 0.20833
                      return 0;
                    }
                  } else {  // if avgShiftY > -0.018741
                    if (stdShiftY <= 0.19551) {
                      return 0;
                    } else {  // if stdShiftY > 0.19551
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.31077
                  if (stdShiftX <= 0.24489) {
                    if (stdShiftX <= 0.014525) {
                      return 2;
                    } else {  // if stdShiftX > 0.014525
                      return 1;
                    }
                  } else {  // if stdShiftX > 0.24489
                    if (stdShift <= 0.14883) {
                      return 2;
                    } else {  // if stdShift > 0.14883
                      return 1;
                    }
                  }
                }
              }
            } else {  // if width > 72.878
              if (width <= 108.47) {
                if (xyRatio <= 0.2911) {
                  if (height <= 273.88) {
                    if (stdShiftY <= 0.13638) {
                      return 2;
                    } else {  // if stdShiftY > 0.13638
                      return 0;
                    }
                  } else {  // if height > 273.88
                    if (width <= 96.05) {
                      return 1;
                    } else {  // if width > 96.05
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.2911
                  if (xyRatio <= 0.32884) {
                    if (stdShiftX <= 0.016018) {
                      return 0;
                    } else {  // if stdShiftX > 0.016018
                      return 2;
                    }
                  } else {  // if xyRatio > 0.32884
                    if (ncc <= 0.96314) {
                      return 0;
                    } else {  // if ncc > 0.96314
                      return 2;
                    }
                  }
                }
              } else {  // if width > 108.47
                if (ncc <= -0.018671) {
                  return 0;
                } else {  // if ncc > -0.018671
                  if (avgShiftX <= -0.015543) {
                    return 0;
                  } else {  // if avgShiftX > -0.015543
                    return 1;
                  }
                }
              }
            }
          } else {  // if stdShiftY > 0.24934
            if (maxEdgeLength <= 269.31) {
              if (xyRatio <= 0.28356) {
                if (avgShift <= 0.1284) {
                  if (ncc <= 0.40794) {
                    if (xyRatio <= 0.24443) {
                      return 2;
                    } else {  // if xyRatio > 0.24443
                      return 0;
                    }
                  } else {  // if ncc > 0.40794
                    return 0;
                  }
                } else {  // if avgShift > 0.1284
                  if (ncc <= 0.6301) {
                    if (maxEdgeLength <= 262.36) {
                      return 2;
                    } else {  // if maxEdgeLength > 262.36
                      return 0;
                    }
                  } else {  // if ncc > 0.6301
                    if (avgErr <= 3.1812) {
                      return 1;
                    } else {  // if avgErr > 3.1812
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.28356
                if (xyRatio <= 0.3531) {
                  if (stdShiftX <= 0.353) {
                    if (avgShift <= 0.84917) {
                      return 0;
                    } else {  // if avgShift > 0.84917
                      return 0;
                    }
                  } else {  // if stdShiftX > 0.353
                    if (avgErr <= 4.6427) {
                      return 2;
                    } else {  // if avgErr > 4.6427
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.3531
                  if (avgShiftY <= 0.37367) {
                    return 1;
                  } else {  // if avgShiftY > 0.37367
                    return 2;
                  }
                }
              }
            } else {  // if maxEdgeLength > 269.31
              if (ncc <= 0.23194) {
                if (stdShiftX <= 0.32162) {
                  return 2;
                } else {  // if stdShiftX > 0.32162
                  return 0;
                }
              } else {  // if ncc > 0.23194
                if (avgShiftX <= 0.45431) {
                  if (avgErr <= 3.2134) {
                    return 0;
                  } else {  // if avgErr > 3.2134
                    if (stdShiftY <= 0.32644) {
                      return 2;
                    } else {  // if stdShiftY > 0.32644
                      return 0;
                    }
                  }
                } else {  // if avgShiftX > 0.45431
                  return 2;
                }
              }
            }
          }
        } else {  // if xyRatio > 0.3533
          if (height <= 195.61) {
            if (avgShiftY <= -1.5016) {
              if (width <= 180) {
                if (avgShift <= 3.6463) {
                  return 1;
                } else {  // if avgShift > 3.6463
                  if (xyRatio <= 1.0482) {
                    if (height <= 170) {
                      return 0;
                    } else {  // if height > 170
                      return 2;
                    }
                  } else {  // if xyRatio > 1.0482
                    if (stdShift <= 0.06764) {
                      return 2;
                    } else {  // if stdShift > 0.06764
                      return 1;
                    }
                  }
                }
              } else {  // if width > 180
                if (stdShiftX <= 0.4536) {
                  return 1;
                } else {  // if stdShiftX > 0.4536
                  if (ncc <= 0.98591) {
                    return 1;
                  } else {  // if ncc > 0.98591
                    return 2;
                  }
                }
              }
            } else {  // if avgShiftY > -1.5016
              if (xyRatio <= 2.091) {
                if (xyRatio <= 2.0392) {
                  if (avgShiftX <= 0.76371) {
                    if (height <= 181.87) {
                      return 0;
                    } else {  // if height > 181.87
                      return 0;
                    }
                  } else {  // if avgShiftX > 0.76371
                    if (width <= 73.547) {
                      return 0;
                    } else {  // if width > 73.547
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 2.0392
                  if (maxEdgeLength <= 328.68) {
                    if (height <= 168.47) {
                      return 1;
                    } else {  // if height > 168.47
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 328.68
                    if (maxEdgeLength <= 329.04) {
                      return 0;
                    } else {  // if maxEdgeLength > 329.04
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 2.091
                if (maxEdgeLength <= 274.75) {
                  return 1;
                } else {  // if maxEdgeLength > 274.75
                  if (ncc <= 0.081307) {
                    if (stdShiftY <= 0.032939) {
                      return 0;
                    } else {  // if stdShiftY > 0.032939
                      return 0;
                    }
                  } else {  // if ncc > 0.081307
                    if (avgErr <= 0.37093) {
                      return 0;
                    } else {  // if avgErr > 0.37093
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if height > 195.61
            if (avgShiftY <= -0.0044824) {
              if (maxEdgeLength <= 317.76) {
                if (maxEdgeLength <= 316.81) {
                  if (ncc <= 0.85939) {
                    if (xyRatio <= 0.56226) {
                      return 0;
                    } else {  // if xyRatio > 0.56226
                      return 2;
                    }
                  } else {  // if ncc > 0.85939
                    if (avgErr <= 3.931) {
                      return 2;
                    } else {  // if avgErr > 3.931
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 316.81
                  return 0;
                }
              } else {  // if maxEdgeLength > 317.76
                if (avgShiftX <= 0.080034) {
                  if (avgShiftY <= -0.013813) {
                    return 2;
                  } else {  // if avgShiftY > -0.013813
                    if (stdShiftY <= 0.038821) {
                      return 2;
                    } else {  // if stdShiftY > 0.038821
                      return 0;
                    }
                  }
                } else {  // if avgShiftX > 0.080034
                  if (avgShift <= 0.032555) {
                    return 0;
                  } else {  // if avgShift > 0.032555
                    return 2;
                  }
                }
              }
            } else {  // if avgShiftY > -0.0044824
              if (avgShiftX <= 3.8981) {
                if (avgShiftY <= 3.48) {
                  if (stdShiftX <= 0.13406) {
                    if (ncc <= 0.33289) {
                      return 0;
                    } else {  // if ncc > 0.33289
                      return 2;
                    }
                  } else {  // if stdShiftX > 0.13406
                    if (avgShift <= 0.14462) {
                      return 0;
                    } else {  // if avgShift > 0.14462
                      return 0;
                    }
                  }
                } else {  // if avgShiftY > 3.48
                  if (avgShift <= 39.554) {
                    if (avgShiftX <= -4.104) {
                      return 2;
                    } else {  // if avgShiftX > -4.104
                      return 0;
                    }
                  } else {  // if avgShift > 39.554
                    return 0;
                  }
                }
              } else {  // if avgShiftX > 3.8981
                return 1;
              }
            }
          }
        }
      } else {  // if width > 330.15
        if (xyRatio <= 2.0086) {
          if (stdShiftY <= 0.53263) {
            if (avgErr <= 0.61816) {
              if (avgShiftY <= 0.0015113) {
                return 0;
              } else {  // if avgShiftY > 0.0015113
                if (stdShiftY <= 0.01948) {
                  return 0;
                } else {  // if stdShiftY > 0.01948
                  return 2;
                }
              }
            } else {  // if avgErr > 0.61816
              return 2;
            }
          } else {  // if stdShiftY > 0.53263
            if (xyRatio <= 1.5471) {
              return 0;
            } else {  // if xyRatio > 1.5471
              return 1;
            }
          }
        } else {  // if xyRatio > 2.0086
          if (height <= 170.62) {
            if (avgShiftX <= 0.066115) {
              if (avgShift <= 0.00081506) {
                return 1;
              } else {  // if avgShift > 0.00081506
                return 2;
              }
            } else {  // if avgShiftX > 0.066115
              return 1;
            }
          } else {  // if height > 170.62
            if (height <= 175.93) {
              if (stdShiftY <= 0.66608) {
                if (avgShiftY <= 0.20435) {
                  if (stdShiftX <= 0.038729) {
                    if (avgShiftY <= 0.0096059) {
                      return 2;
                    } else {  // if avgShiftY > 0.0096059
                      return 1;
                    }
                  } else {  // if stdShiftX > 0.038729
                    return 2;
                  }
                } else {  // if avgShiftY > 0.20435
                  if (height <= 173.23) {
                    return 2;
                  } else {  // if height > 173.23
                    return 1;
                  }
                }
              } else {  // if stdShiftY > 0.66608
                if (avgShiftY <= -0.061501) {
                  return 2;
                } else {  // if avgShiftY > -0.061501
                  return 1;
                }
              }
            } else {  // if height > 175.93
              return 1;
            }
          }
        }
      }
    }
  }
}

} // namespace rm
