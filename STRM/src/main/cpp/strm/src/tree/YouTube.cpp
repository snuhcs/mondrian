#include "strm/tree/YouTube.hpp"

namespace rm {

float
YouTube(float width, float height, float maxEdgeLength, float type, float origin, float xyRatio,
        float avgShiftX, float avgShiftY, float avgShift, float stdShiftX, float stdShiftY,
        float stdShift, float avgErr, float ncc) {
  if (width <= 75.721) {
    if (height <= 170.1) {
      if (width <= 47.007) {
        if (avgShiftX <= -0.39291) {
          if (avgShiftY <= -0.52736) {
            return 1;
          } else {  // if avgShiftY > -0.52736
            if (avgShiftY <= 6.2041) {
              if (stdShiftY <= 3.8737) {
                if (ncc <= 0.99848) {
                  if (avgErr <= 5.2005) {
                    if (avgErr <= 4.872) {
                      return 2;
                    } else {  // if avgErr > 4.872
                      return 1;
                    }
                  } else {  // if avgErr > 5.2005
                    if (height <= 24.42) {
                      return 1;
                    } else {  // if height > 24.42
                      return 2;
                    }
                  }
                } else {  // if ncc > 0.99848
                  if (avgShiftY <= 0.12154) {
                    return 1;
                  } else {  // if avgShiftY > 0.12154
                    if (ncc <= 0.99906) {
                      return 1;
                    } else {  // if ncc > 0.99906
                      return 2;
                    }
                  }
                }
              } else {  // if stdShiftY > 3.8737
                return 0;
              }
            } else {  // if avgShiftY > 6.2041
              return 0;
            }
          }
        } else {  // if avgShiftX > -0.39291
          if (xyRatio <= 0.10435) {
            if (avgErr <= 0.6968) {
              return 1;
            } else {  // if avgErr > 0.6968
              return 2;
            }
          } else {  // if xyRatio > 0.10435
            if (xyRatio <= 1.8422) {
              if (xyRatio <= 0.2634) {
                if (xyRatio <= 0.25923) {
                  if (avgShiftX <= -0.23635) {
                    if (avgShiftX <= -0.26287) {
                      return 2;
                    } else {  // if avgShiftX > -0.26287
                      return 0;
                    }
                  } else {  // if avgShiftX > -0.23635
                    if (ncc <= 0.969) {
                      return 2;
                    } else {  // if ncc > 0.969
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.25923
                  if (avgShiftX <= -0.0070082) {
                    return 2;
                  } else {  // if avgShiftX > -0.0070082
                    if (stdShiftX <= 0.01794) {
                      return 2;
                    } else {  // if stdShiftX > 0.01794
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.2634
                if (avgShiftY <= -0.22092) {
                  if (avgShiftY <= -0.22622) {
                    if (avgShiftY <= -0.28547) {
                      return 2;
                    } else {  // if avgShiftY > -0.28547
                      return 2;
                    }
                  } else {  // if avgShiftY > -0.22622
                    return 1;
                  }
                } else {  // if avgShiftY > -0.22092
                  if (stdShiftX <= 0.32576) {
                    if (ncc <= 0.33344) {
                      return 2;
                    } else {  // if ncc > 0.33344
                      return 2;
                    }
                  } else {  // if stdShiftX > 0.32576
                    if (stdShift <= 0.12376) {
                      return 1;
                    } else {  // if stdShift > 0.12376
                      return 2;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 1.8422
              if (xyRatio <= 1.8942) {
                return 1;
              } else {  // if xyRatio > 1.8942
                if (xyRatio <= 2.2427) {
                  if (avgErr <= 3.5738) {
                    if (avgErr <= 2.6737) {
                      return 2;
                    } else {  // if avgErr > 2.6737
                      return 1;
                    }
                  } else {  // if avgErr > 3.5738
                    return 2;
                  }
                } else {  // if xyRatio > 2.2427
                  if (height <= 21.067) {
                    return 2;
                  } else {  // if height > 21.067
                    if (avgErr <= 11.109) {
                      return 2;
                    } else {  // if avgErr > 11.109
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if width > 47.007
        if (avgShiftX <= -8.7941) {
          if (avgShiftX <= -9.9724) {
            if (stdShift <= 0.21518) {
              return 2;
            } else {  // if stdShift > 0.21518
              return 1;
            }
          } else {  // if avgShiftX > -9.9724
            return 0;
          }
        } else {  // if avgShiftX > -8.7941
          if (xyRatio <= 2.5406) {
            if (width <= 47.933) {
              if (avgShift <= 1.6912) {
                if (height <= 123.35) {
                  if (stdShiftY <= 0.047033) {
                    if (xyRatio <= 0.44267) {
                      return 2;
                    } else {  // if xyRatio > 0.44267
                      return 2;
                    }
                  } else {  // if stdShiftY > 0.047033
                    if (avgShift <= 0.045646) {
                      return 1;
                    } else {  // if avgShift > 0.045646
                      return 2;
                    }
                  }
                } else {  // if height > 123.35
                  if (avgShiftX <= -0.074426) {
                    return 2;
                  } else {  // if avgShiftX > -0.074426
                    return 1;
                  }
                }
              } else {  // if avgShift > 1.6912
                if (stdShiftY <= 0.43653) {
                  return 0;
                } else {  // if stdShiftY > 0.43653
                  if (avgShiftY <= -0.41544) {
                    if (stdShift <= 0.4844) {
                      return 2;
                    } else {  // if stdShift > 0.4844
                      return 0;
                    }
                  } else {  // if avgShiftY > -0.41544
                    return 2;
                  }
                }
              }
            } else {  // if width > 47.933
              if (height <= 145.29) {
                if (maxEdgeLength <= 115.93) {
                  if (maxEdgeLength <= 115.85) {
                    if (height <= 104.48) {
                      return 2;
                    } else {  // if height > 104.48
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 115.85
                    if (avgShiftX <= 0.0051473) {
                      return 2;
                    } else {  // if avgShiftX > 0.0051473
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 115.93
                  if (stdShift <= 0.097912) {
                    if (stdShiftY <= 0.0013981) {
                      return 2;
                    } else {  // if stdShiftY > 0.0013981
                      return 2;
                    }
                  } else {  // if stdShift > 0.097912
                    if (xyRatio <= 0.28973) {
                      return 2;
                    } else {  // if xyRatio > 0.28973
                      return 2;
                    }
                  }
                }
              } else {  // if height > 145.29
                if (avgShiftY <= 0.027253) {
                  if (width <= 74.196) {
                    if (xyRatio <= 0.34762) {
                      return 2;
                    } else {  // if xyRatio > 0.34762
                      return 2;
                    }
                  } else {  // if width > 74.196
                    if (avgShiftX <= -0.53545) {
                      return 1;
                    } else {  // if avgShiftX > -0.53545
                      return 2;
                    }
                  }
                } else {  // if avgShiftY > 0.027253
                  if (ncc <= 0.66926) {
                    if (avgErr <= 3.3382) {
                      return 2;
                    } else {  // if avgErr > 3.3382
                      return 1;
                    }
                  } else {  // if ncc > 0.66926
                    if (avgShift <= 0.079145) {
                      return 1;
                    } else {  // if avgShift > 0.079145
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 2.5406
            return 1;
          }
        }
      }
    } else {  // if height > 170.1
      if (height <= 360) {
        if (avgShiftY <= -0.24086) {
          if (avgErr <= 2.5042) {
            return 2;
          } else {  // if avgErr > 2.5042
            if (stdShiftY <= 1.4151) {
              if (stdShift <= 0.78085) {
                if (stdShift <= 0.25573) {
                  return 1;
                } else {  // if stdShift > 0.25573
                  if (avgErr <= 5.8119) {
                    if (maxEdgeLength <= 172.71) {
                      return 1;
                    } else {  // if maxEdgeLength > 172.71
                      return 0;
                    }
                  } else {  // if avgErr > 5.8119
                    return 2;
                  }
                }
              } else {  // if stdShift > 0.78085
                if (stdShift <= 1.2643) {
                  return 1;
                } else {  // if stdShift > 1.2643
                  if (stdShift <= 1.3309) {
                    return 2;
                  } else {  // if stdShift > 1.3309
                    if (avgShift <= 0.25702) {
                      return 1;
                    } else {  // if avgShift > 0.25702
                      return 1;
                    }
                  }
                }
              }
            } else {  // if stdShiftY > 1.4151
              return 2;
            }
          }
        } else {  // if avgShiftY > -0.24086
          if (xyRatio <= 0.11468) {
            if (stdShift <= 95.885) {
              if (ncc <= 0.98526) {
                return 1;
              } else {  // if ncc > 0.98526
                return 2;
              }
            } else {  // if stdShift > 95.885
              return 0;
            }
          } else {  // if xyRatio > 0.11468
            if (stdShiftX <= 0.014493) {
              if (xyRatio <= 0.3084) {
                if (xyRatio <= 0.2527) {
                  return 1;
                } else {  // if xyRatio > 0.2527
                  if (ncc <= 0.3204) {
                    if (xyRatio <= 0.30786) {
                      return 2;
                    } else {  // if xyRatio > 0.30786
                      return 0;
                    }
                  } else {  // if ncc > 0.3204
                    return 1;
                  }
                }
              } else {  // if xyRatio > 0.3084
                if (avgShiftY <= 0.0074728) {
                  if (xyRatio <= 0.31628) {
                    if (avgShiftY <= -0.0059297) {
                      return 2;
                    } else {  // if avgShiftY > -0.0059297
                      return 1;
                    }
                  } else {  // if xyRatio > 0.31628
                    return 1;
                  }
                } else {  // if avgShiftY > 0.0074728
                  return 2;
                }
              }
            } else {  // if stdShiftX > 0.014493
              if (avgShiftX <= 0.015238) {
                if (stdShiftX <= 1.912) {
                  if (height <= 176.28) {
                    if (stdShiftX <= 0.35035) {
                      return 1;
                    } else {  // if stdShiftX > 0.35035
                      return 2;
                    }
                  } else {  // if height > 176.28
                    if (xyRatio <= 0.13707) {
                      return 1;
                    } else {  // if xyRatio > 0.13707
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 1.912
                  return 1;
                }
              } else {  // if avgShiftX > 0.015238
                if (ncc <= 0.95479) {
                  if (avgErr <= 0.91506) {
                    if (stdShiftY <= 0.036268) {
                      return 2;
                    } else {  // if stdShiftY > 0.036268
                      return 1;
                    }
                  } else {  // if avgErr > 0.91506
                    if (avgErr <= 2.7684) {
                      return 2;
                    } else {  // if avgErr > 2.7684
                      return 2;
                    }
                  }
                } else {  // if ncc > 0.95479
                  return 1;
                }
              }
            }
          }
        }
      } else {  // if height > 360
        if (width <= 55.245) {
          return 0;
        } else {  // if width > 55.245
          return 1;
        }
      }
    }
  } else {  // if width > 75.721
    if (height <= 302.05) {
      if (height <= 198.69) {
        if (maxEdgeLength <= 163.63) {
          if (width <= 146.65) {
            if (maxEdgeLength <= 151) {
              if (xyRatio <= 1.1129) {
                if (xyRatio <= 0.74758) {
                  if (height <= 126.2) {
                    if (height <= 123.79) {
                      return 2;
                    } else {  // if height > 123.79
                      return 0;
                    }
                  } else {  // if height > 126.2
                    if (avgErr <= 12.366) {
                      return 2;
                    } else {  // if avgErr > 12.366
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 0.74758
                  if (ncc <= 0.588) {
                    if (stdShiftY <= 0.020799) {
                      return 0;
                    } else {  // if stdShiftY > 0.020799
                      return 2;
                    }
                  } else {  // if ncc > 0.588
                    if (stdShiftX <= 0.060954) {
                      return 0;
                    } else {  // if stdShiftX > 0.060954
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 1.1129
                if (xyRatio <= 1.6478) {
                  if (width <= 91) {
                    if (avgShiftX <= 0.35412) {
                      return 2;
                    } else {  // if avgShiftX > 0.35412
                      return 0;
                    }
                  } else {  // if width > 91
                    if (stdShiftY <= 0.0021917) {
                      return 1;
                    } else {  // if stdShiftY > 0.0021917
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 1.6478
                  if (xyRatio <= 1.6923) {
                    if (xyRatio <= 1.6716) {
                      return 1;
                    } else {  // if xyRatio > 1.6716
                      return 0;
                    }
                  } else {  // if xyRatio > 1.6923
                    if (avgShiftY <= 0.058705) {
                      return 2;
                    } else {  // if avgShiftY > 0.058705
                      return 0;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 151
              if (avgShiftX <= -0.19041) {
                return 2;
              } else {  // if avgShiftX > -0.19041
                if (ncc <= 0.41644) {
                  if (stdShift <= 0.22168) {
                    if (avgShiftX <= 0.0027506) {
                      return 1;
                    } else {  // if avgShiftX > 0.0027506
                      return 2;
                    }
                  } else {  // if stdShift > 0.22168
                    return 2;
                  }
                } else {  // if ncc > 0.41644
                  if (avgShiftX <= 3.3885) {
                    return 1;
                  } else {  // if avgShiftX > 3.3885
                    if (stdShift <= 0.57817) {
                      return 0;
                    } else {  // if stdShift > 0.57817
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if width > 146.65
            if (stdShiftX <= 0.0062551) {
              return 2;
            } else {  // if stdShiftX > 0.0062551
              if (stdShift <= 1.6121) {
                return 0;
              } else {  // if stdShift > 1.6121
                return 2;
              }
            }
          }
        } else {  // if maxEdgeLength > 163.63
          if (xyRatio <= 3.9812) {
            if (height <= 176.84) {
              if (maxEdgeLength <= 172.54) {
                if (ncc <= 0.71415) {
                  if (avgShiftY <= -0.29446) {
                    return 0;
                  } else {  // if avgShiftY > -0.29446
                    if (width <= 91.093) {
                      return 1;
                    } else {  // if width > 91.093
                      return 2;
                    }
                  }
                } else {  // if ncc > 0.71415
                  if (stdShiftY <= 0.47203) {
                    if (avgShiftY <= 0.14829) {
                      return 1;
                    } else {  // if avgShiftY > 0.14829
                      return 2;
                    }
                  } else {  // if stdShiftY > 0.47203
                    if (stdShiftX <= 1.8088) {
                      return 1;
                    } else {  // if stdShiftX > 1.8088
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 172.54
                if (ncc <= 0.98345) {
                  if (avgShiftX <= 4.4607) {
                    if (avgErr <= 10.183) {
                      return 1;
                    } else {  // if avgErr > 10.183
                      return 1;
                    }
                  } else {  // if avgShiftX > 4.4607
                    if (avgShiftX <= 4.8446) {
                      return 0;
                    } else {  // if avgShiftX > 4.8446
                      return 1;
                    }
                  }
                } else {  // if ncc > 0.98345
                  if (avgShiftY <= 0.63255) {
                    if (width <= 190) {
                      return 0;
                    } else {  // if width > 190
                      return 2;
                    }
                  } else {  // if avgShiftY > 0.63255
                    return 1;
                  }
                }
              }
            } else {  // if height > 176.84
              if (xyRatio <= 0.472) {
                if (width <= 84.242) {
                  if (avgShift <= 5.6545) {
                    if (avgShiftY <= -1.1794) {
                      return 1;
                    } else {  // if avgShiftY > -1.1794
                      return 2;
                    }
                  } else {  // if avgShift > 5.6545
                    if (ncc <= 0.98444) {
                      return 1;
                    } else {  // if ncc > 0.98444
                      return 2;
                    }
                  }
                } else {  // if width > 84.242
                  if (width <= 87.878) {
                    if (ncc <= 0.88589) {
                      return 0;
                    } else {  // if ncc > 0.88589
                      return 1;
                    }
                  } else {  // if width > 87.878
                    if (avgErr <= 6.7475) {
                      return 1;
                    } else {  // if avgErr > 6.7475
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.472
                if (width <= 133.22) {
                  if (avgShiftY <= -0.4918) {
                    if (avgShiftY <= -0.57566) {
                      return 1;
                    } else {  // if avgShiftY > -0.57566
                      return 0;
                    }
                  } else {  // if avgShiftY > -0.4918
                    if (avgShift <= 2.6536e-06) {
                      return 0;
                    } else {  // if avgShift > 2.6536e-06
                      return 2;
                    }
                  }
                } else {  // if width > 133.22
                  if (stdShiftY <= 1.0848) {
                    if (avgShift <= 0.20188) {
                      return 2;
                    } else {  // if avgShift > 0.20188
                      return 1;
                    }
                  } else {  // if stdShiftY > 1.0848
                    if (height <= 187) {
                      return 0;
                    } else {  // if height > 187
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 3.9812
            if (avgErr <= 10.711) {
              if (ncc <= 0.96442) {
                if (ncc <= 0.96225) {
                  if (height <= 51.196) {
                    if (width <= 194) {
                      return 2;
                    } else {  // if width > 194
                      return 1;
                    }
                  } else {  // if height > 51.196
                    return 2;
                  }
                } else {  // if ncc > 0.96225
                  return 1;
                }
              } else {  // if ncc > 0.96442
                return 2;
              }
            } else {  // if avgErr > 10.711
              return 1;
            }
          }
        }
      } else {  // if height > 198.69
        if (maxEdgeLength <= 247.37) {
          if (stdShiftY <= 2.3818) {
            if (avgErr <= 10.867) {
              if (height <= 218.27) {
                if (width <= 93.832) {
                  if (avgShiftY <= -1.8998) {
                    if (stdShiftX <= 0.93639) {
                      return 2;
                    } else {  // if stdShiftX > 0.93639
                      return 1;
                    }
                  } else {  // if avgShiftY > -1.8998
                    if (ncc <= -0.00014882) {
                      return 2;
                    } else {  // if ncc > -0.00014882
                      return 1;
                    }
                  }
                } else {  // if width > 93.832
                  if (width <= 104.9) {
                    return 2;
                  } else {  // if width > 104.9
                    return 1;
                  }
                }
              } else {  // if height > 218.27
                if (avgShift <= 8.6304e-07) {
                  if (width <= 148.05) {
                    return 1;
                  } else {  // if width > 148.05
                    return 0;
                  }
                } else {  // if avgShift > 8.6304e-07
                  if (avgShift <= 6.9447) {
                    if (ncc <= 0.87492) {
                      return 1;
                    } else {  // if ncc > 0.87492
                      return 1;
                    }
                  } else {  // if avgShift > 6.9447
                    if (ncc <= 0.8604) {
                      return 2;
                    } else {  // if ncc > 0.8604
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgErr > 10.867
              return 2;
            }
          } else {  // if stdShiftY > 2.3818
            return 2;
          }
        } else {  // if maxEdgeLength > 247.37
          if (xyRatio <= 0.43865) {
            if (xyRatio <= 0.34745) {
              if (avgErr <= 7.5621) {
                if (stdShiftX <= 0.0036413) {
                  return 0;
                } else {  // if stdShiftX > 0.0036413
                  if (width <= 94.918) {
                    return 0;
                  } else {  // if width > 94.918
                    if (stdShiftY <= 0.99071) {
                      return 1;
                    } else {  // if stdShiftY > 0.99071
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 7.5621
                if (avgShiftX <= -1.1219) {
                  return 1;
                } else {  // if avgShiftX > -1.1219
                  return 0;
                }
              }
            } else {  // if xyRatio > 0.34745
              if (ncc <= -0.0078153) {
                return 1;
              } else {  // if ncc > -0.0078153
                if (avgShiftX <= -1.2106) {
                  if (stdShiftY <= 2.2203) {
                    return 0;
                  } else {  // if stdShiftY > 2.2203
                    return 1;
                  }
                } else {  // if avgShiftX > -1.2106
                  if (avgShiftX <= 0.23017) {
                    return 0;
                  } else {  // if avgShiftX > 0.23017
                    if (avgShiftY <= -1.5538) {
                      return 0;
                    } else {  // if avgShiftY > -1.5538
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.43865
            if (avgShiftX <= -0.11305) {
              if (stdShiftX <= 0.22594) {
                return 0;
              } else {  // if stdShiftX > 0.22594
                return 1;
              }
            } else {  // if avgShiftX > -0.11305
              if (xyRatio <= 0.6218) {
                if (height <= 279.43) {
                  if (avgShiftY <= -0.052447) {
                    if (ncc <= 0.14023) {
                      return 0;
                    } else {  // if ncc > 0.14023
                      return 1;
                    }
                  } else {  // if avgShiftY > -0.052447
                    if (avgShiftY <= -0.0018253) {
                      return 1;
                    } else {  // if avgShiftY > -0.0018253
                      return 1;
                    }
                  }
                } else {  // if height > 279.43
                  if (avgErr <= 0.28502) {
                    if (avgErr <= 0.28216) {
                      return 1;
                    } else {  // if avgErr > 0.28216
                      return 0;
                    }
                  } else {  // if avgErr > 0.28502
                    return 1;
                  }
                }
              } else {  // if xyRatio > 0.6218
                if (ncc <= -0.020406) {
                  return 0;
                } else {  // if ncc > -0.020406
                  if (avgShiftY <= 0.018671) {
                    if (stdShiftY <= 0.025685) {
                      return 0;
                    } else {  // if stdShiftY > 0.025685
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.018671
                    if (ncc <= 0.36565) {
                      return 0;
                    } else {  // if ncc > 0.36565
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if height > 302.05
      if (width <= 146.46) {
        if (avgShift <= 10.309) {
          if (height <= 329.62) {
            if (avgShiftX <= -0.69302) {
              if (stdShiftX <= 1.3027) {
                return 1;
              } else {  // if stdShiftX > 1.3027
                return 0;
              }
            } else {  // if avgShiftX > -0.69302
              if (avgShiftX <= 0.25359) {
                return 0;
              } else {  // if avgShiftX > 0.25359
                if (avgShiftX <= 0.40579) {
                  return 1;
                } else {  // if avgShiftX > 0.40579
                  return 0;
                }
              }
            }
          } else {  // if height > 329.62
            return 1;
          }
        } else {  // if avgShift > 10.309
          if (avgShift <= 343.98) {
            if (ncc <= 0.78973) {
              if (stdShiftX <= 1.3899) {
                return 0;
              } else {  // if stdShiftX > 1.3899
                return 1;
              }
            } else {  // if ncc > 0.78973
              return 1;
            }
          } else {  // if avgShift > 343.98
            return 0;
          }
        }
      } else {  // if width > 146.46
        if (maxEdgeLength <= 383.71) {
          if (avgShiftX <= -2.486) {
            return 1;
          } else {  // if avgShiftX > -2.486
            if (stdShiftY <= 2.7162) {
              return 0;
            } else {  // if stdShiftY > 2.7162
              if (stdShiftY <= 2.8272) {
                if (height <= 366.58) {
                  return 1;
                } else {  // if height > 366.58
                  return 0;
                }
              } else {  // if stdShiftY > 2.8272
                return 0;
              }
            }
          }
        } else {  // if maxEdgeLength > 383.71
          return 0;
        }
      }
    }
  }
}

} // namespace rm
