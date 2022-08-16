#include "strm/tree/VIRAT.hpp"

namespace rm {

float VIRAT(float width, float height, float type, float xyRatio, float avgShiftX, float avgShiftY,
            float avgShift, float stdShiftX, float stdShiftY, float stdShift, float avgErr,
            float ncc) {
  if (height <= -0.67856) {
    if (height <= -0.80074) {
      if (width <= -0.14632) {
        return 0;
      } else {  // if width > -0.14632
        if (xyRatio <= 1.3276) {
          if (width <= -0.047774) {
            return 1;
          } else {  // if width > -0.047774
            if (avgShiftY <= -0.51116) {
              return 1;
            } else {  // if avgShiftY > -0.51116
              if (avgShiftX <= -0.30648) {
                return 2;
              } else {  // if avgShiftX > -0.30648
                if (avgShiftX <= -0.22712) {
                  return 0;
                } else {  // if avgShiftX > -0.22712
                  return 2;
                }
              }
            }
          }
        } else {  // if xyRatio > 1.3276
          if (stdShiftY <= -0.70157) {
            if (avgShiftX <= -0.15504) {
              return 2;
            } else {  // if avgShiftX > -0.15504
              return 0;
            }
          } else {  // if stdShiftY > -0.70157
            if (avgShiftY <= 1.2787) {
              if (stdShiftY <= -0.46423) {
                if (avgShiftX <= 1.1045) {
                  if (height <= -1.5466) {
                    if (avgErr <= -0.98897) {
                      return 1;
                    } else {  // if avgErr > -0.98897
                      return 0;
                    }
                  } else {  // if height > -1.5466
                    if (width <= 1.4817) {
                      return 0;
                    } else {  // if width > 1.4817
                      return 0;
                    }
                  }
                } else {  // if avgShiftX > 1.1045
                  return 2;
                }
              } else {  // if stdShiftY > -0.46423
                return 0;
              }
            } else {  // if avgShiftY > 1.2787
              if (avgErr <= 0.73932) {
                return 0;
              } else {  // if avgErr > 0.73932
                return 2;
              }
            }
          }
        }
      }
    } else {  // if height > -0.80074
      if (avgShift <= -0.33063) {
        if (stdShiftY <= -0.67547) {
          if (xyRatio <= 1.8421) {
            if (width <= 1.2077) {
              if (avgShiftX <= -0.15261) {
                if (ncc <= -1.3927) {
                  return 2;
                } else {  // if ncc > -1.3927
                  return 0;
                }
              } else {  // if avgShiftX > -0.15261
                return 1;
              }
            } else {  // if width > 1.2077
              return 2;
            }
          } else {  // if xyRatio > 1.8421
            return 0;
          }
        } else {  // if stdShiftY > -0.67547
          if (height <= -0.70617) {
            if (xyRatio <= -0.65391) {
              if (height <= -0.79462) {
                return 1;
              } else {  // if height > -0.79462
                if (avgShiftX <= 0.022433) {
                  return 0;
                } else {  // if avgShiftX > 0.022433
                  return 1;
                }
              }
            } else {  // if xyRatio > -0.65391
              if (avgShiftY <= 0.052298) {
                if (avgErr <= -0.95121) {
                  if (xyRatio <= 1.8828) {
                    if (stdShiftY <= -0.62468) {
                      return 1;
                    } else {  // if stdShiftY > -0.62468
                      return 0;
                    }
                  } else {  // if xyRatio > 1.8828
                    return 0;
                  }
                } else {  // if avgErr > -0.95121
                  return 0;
                }
              } else {  // if avgShiftY > 0.052298
                return 1;
              }
            }
          } else {  // if height > -0.70617
            if (width <= -0.59988) {
              if (stdShift <= -0.28462) {
                if (stdShift <= -0.28756) {
                  return 0;
                } else {  // if stdShift > -0.28756
                  return 1;
                }
              } else {  // if stdShift > -0.28462
                if (stdShift <= -0.28385) {
                  if (stdShift <= -0.28409) {
                    return 0;
                  } else {  // if stdShift > -0.28409
                    return 1;
                  }
                } else {  // if stdShift > -0.28385
                  return 0;
                }
              }
            } else {  // if width > -0.59988
              return 0;
            }
          }
        }
      } else {  // if avgShift > -0.33063
        if (avgShift <= -0.099973) {
          if (height <= -0.7475) {
            if (stdShiftX <= -0.39509) {
              if (width <= -0.89209) {
                if (ncc <= 0.51829) {
                  if (stdShiftY <= -0.15558) {
                    return 0;
                  } else {  // if stdShiftY > -0.15558
                    return 1;
                  }
                } else {  // if ncc > 0.51829
                  return 0;
                }
              } else {  // if width > -0.89209
                if (stdShiftY <= -0.092938) {
                  return 1;
                } else {  // if stdShiftY > -0.092938
                  if (avgShiftX <= -0.66105) {
                    return 1;
                  } else {  // if avgShiftX > -0.66105
                    return 0;
                  }
                }
              }
            } else {  // if stdShiftX > -0.39509
              if (avgErr <= 0.42522) {
                return 0;
              } else {  // if avgErr > 0.42522
                if (avgShiftX <= -0.73142) {
                  return 1;
                } else {  // if avgShiftX > -0.73142
                  if (avgShift <= -0.27201) {
                    return 1;
                  } else {  // if avgShift > -0.27201
                    return 0;
                  }
                }
              }
            }
          } else {  // if height > -0.7475
            if (stdShiftX <= -0.029697) {
              return 1;
            } else {  // if stdShiftX > -0.029697
              if (width <= -0.90513) {
                return 1;
              } else {  // if width > -0.90513
                return 0;
              }
            }
          }
        } else {  // if avgShift > -0.099973
          if (avgShiftX <= 1.2219) {
            if (avgShiftY <= 1.9271) {
              if (stdShift <= -0.256) {
                return 1;
              } else {  // if stdShift > -0.256
                if (stdShiftX <= 1.8783) {
                  if (avgShiftY <= -1.068) {
                    if (avgShiftY <= -1.1508) {
                      return 0;
                    } else {  // if avgShiftY > -1.1508
                      return 1;
                    }
                  } else {  // if avgShiftY > -1.068
                    return 0;
                  }
                } else {  // if stdShiftX > 1.8783
                  return 1;
                }
              }
            } else {  // if avgShiftY > 1.9271
              return 2;
            }
          } else {  // if avgShiftX > 1.2219
            if (stdShift <= 5.019) {
              return 1;
            } else {  // if stdShift > 5.019
              return 0;
            }
          }
        }
      }
    }
  } else {  // if height > -0.67856
    if (width <= 1.9068) {
      if (height <= -0.24376) {
        if (width <= 1.3816) {
          if (width <= -0.54182) {
            if (xyRatio <= -0.53823) {
              if (height <= -0.45475) {
                if (xyRatio <= -0.76472) {
                  if (avgShift <= -0.36473) {
                    if (stdShift <= -0.28333) {
                      return 1;
                    } else {  // if stdShift > -0.28333
                      return 0;
                    }
                  } else {  // if avgShift > -0.36473
                    if (avgShiftX <= -1.0457) {
                      return 0;
                    } else {  // if avgShiftX > -1.0457
                      return 1;
                    }
                  }
                } else {  // if xyRatio > -0.76472
                  if (avgErr <= -0.98316) {
                    if (avgShiftX <= -0.46955) {
                      return 0;
                    } else {  // if avgShiftX > -0.46955
                      return 1;
                    }
                  } else {  // if avgErr > -0.98316
                    if (stdShiftX <= -0.51602) {
                      return 0;
                    } else {  // if stdShiftX > -0.51602
                      return 1;
                    }
                  }
                }
              } else {  // if height > -0.45475
                if (avgErr <= -0.98831) {
                  if (height <= -0.41102) {
                    if (stdShiftY <= -0.54708) {
                      return 1;
                    } else {  // if stdShiftY > -0.54708
                      return 2;
                    }
                  } else {  // if height > -0.41102
                    if (stdShift <= -0.25206) {
                      return 2;
                    } else {  // if stdShift > -0.25206
                      return 1;
                    }
                  }
                } else {  // if avgErr > -0.98831
                  if (avgShiftY <= 0.29991) {
                    if (avgShiftX <= 1.6659) {
                      return 0;
                    } else {  // if avgShiftX > 1.6659
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.29991
                    if (avgShiftX <= -0.95666) {
                      return 2;
                    } else {  // if avgShiftX > -0.95666
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > -0.53823
              if (height <= -0.47186) {
                if (stdShiftY <= 0.61447) {
                  if (width <= -0.63175) {
                    if (stdShiftY <= 0.26884) {
                      return 0;
                    } else {  // if stdShiftY > 0.26884
                      return 0;
                    }
                  } else {  // if width > -0.63175
                    if (avgShift <= -0.36236) {
                      return 0;
                    } else {  // if avgShift > -0.36236
                      return 1;
                    }
                  }
                } else {  // if stdShiftY > 0.61447
                  return 1;
                }
              } else {  // if height > -0.47186
                if (avgShiftY <= -0.32769) {
                  return 1;
                } else {  // if avgShiftY > -0.32769
                  if (xyRatio <= -0.51768) {
                    return 1;
                  } else {  // if xyRatio > -0.51768
                    return 2;
                  }
                }
              }
            }
          } else {  // if width > -0.54182
            if (avgShiftY <= 0.53058) {
              if (xyRatio <= 0.20522) {
                if (xyRatio <= -0.36363) {
                  if (height <= -0.54241) {
                    return 0;
                  } else {  // if height > -0.54241
                    if (avgShiftX <= 0.74643) {
                      return 1;
                    } else {  // if avgShiftX > 0.74643
                      return 1;
                    }
                  }
                } else {  // if xyRatio > -0.36363
                  if (stdShiftY <= 0.30035) {
                    if (avgShiftX <= -0.59657) {
                      return 0;
                    } else {  // if avgShiftX > -0.59657
                      return 1;
                    }
                  } else {  // if stdShiftY > 0.30035
                    if (stdShift <= 0.249) {
                      return 0;
                    } else {  // if stdShift > 0.249
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.20522
                if (stdShiftY <= -0.65805) {
                  return 0;
                } else {  // if stdShiftY > -0.65805
                  if (ncc <= 1.2043) {
                    if (stdShiftY <= -0.65577) {
                      return 2;
                    } else {  // if stdShiftY > -0.65577
                      return 1;
                    }
                  } else {  // if ncc > 1.2043
                    if (avgShiftY <= -1.6931) {
                      return 1;
                    } else {  // if avgShiftY > -1.6931
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgShiftY > 0.53058
              if (avgShift <= 0.87849) {
                return 0;
              } else {  // if avgShift > 0.87849
                if (avgShiftY <= 2.2989) {
                  if (avgShiftY <= 1.8261) {
                    return 2;
                  } else {  // if avgShiftY > 1.8261
                    if (ncc <= 1.2408) {
                      return 1;
                    } else {  // if ncc > 1.2408
                      return 2;
                    }
                  }
                } else {  // if avgShiftY > 2.2989
                  return 0;
                }
              }
            }
          }
        } else {  // if width > 1.3816
          if (ncc <= -1.3134) {
            if (stdShift <= -0.2862) {
              if (avgShift <= -0.36559) {
                return 2;
              } else {  // if avgShift > -0.36559
                return 0;
              }
            } else {  // if stdShift > -0.2862
              if (xyRatio <= 1.8378) {
                if (xyRatio <= 1.7997) {
                  if (ncc <= -1.5341) {
                    return 2;
                  } else {  // if ncc > -1.5341
                    return 0;
                  }
                } else {  // if xyRatio > 1.7997
                  return 2;
                }
              } else {  // if xyRatio > 1.8378
                if (avgShift <= -0.36549) {
                  return 2;
                } else {  // if avgShift > -0.36549
                  return 0;
                }
              }
            }
          } else {  // if ncc > -1.3134
            if (avgErr <= -1.0499) {
              if (xyRatio <= 1.7943) {
                return 0;
              } else {  // if xyRatio > 1.7943
                return 2;
              }
            } else {  // if avgErr > -1.0499
              return 0;
            }
          }
        }
      } else {  // if height > -0.24376
        if (xyRatio <= -0.74557) {
          if (stdShiftY <= -0.20219) {
            if (width <= -0.47027) {
              if (height <= 2.3108) {
                if (width <= -0.70793) {
                  if (avgShiftY <= 0.47812) {
                    if (stdShift <= -0.2786) {
                      return 2;
                    } else {  // if stdShift > -0.2786
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.47812
                    return 0;
                  }
                } else {  // if width > -0.70793
                  if (xyRatio <= -0.78917) {
                    if (ncc <= -0.97594) {
                      return 0;
                    } else {  // if ncc > -0.97594
                      return 0;
                    }
                  } else {  // if xyRatio > -0.78917
                    if (stdShift <= -0.23973) {
                      return 2;
                    } else {  // if stdShift > -0.23973
                      return 1;
                    }
                  }
                }
              } else {  // if height > 2.3108
                if (stdShiftX <= -0.68552) {
                  return 1;
                } else {  // if stdShiftX > -0.68552
                  if (stdShiftY <= -0.51629) {
                    return 0;
                  } else {  // if stdShiftY > -0.51629
                    if (avgShiftX <= -0.030984) {
                      return 1;
                    } else {  // if avgShiftX > -0.030984
                      return 0;
                    }
                  }
                }
              }
            } else {  // if width > -0.47027
              if (height <= 2.6967) {
                if (stdShiftX <= -0.9256) {
                  return 0;
                } else {  // if stdShiftX > -0.9256
                  if (stdShift <= -0.27277) {
                    return 2;
                  } else {  // if stdShift > -0.27277
                    if (stdShift <= -0.269) {
                      return 0;
                    } else {  // if stdShift > -0.269
                      return 2;
                    }
                  }
                }
              } else {  // if height > 2.6967
                if (xyRatio <= -0.80398) {
                  if (avgShiftX <= -0.14727) {
                    if (ncc <= -1.3861) {
                      return 0;
                    } else {  // if ncc > -1.3861
                      return 2;
                    }
                  } else {  // if avgShiftX > -0.14727
                    return 0;
                  }
                } else {  // if xyRatio > -0.80398
                  if (avgErr <= -0.95113) {
                    if (ncc <= -1.5927) {
                      return 0;
                    } else {  // if ncc > -1.5927
                      return 1;
                    }
                  } else {  // if avgErr > -0.95113
                    if (ncc <= -0.62086) {
                      return 2;
                    } else {  // if ncc > -0.62086
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if stdShiftY > -0.20219
            if (height <= 2.215) {
              if (ncc <= 0.31884) {
                if (ncc <= -0.25217) {
                  if (stdShiftX <= -0.22596) {
                    if (height <= 1.15) {
                      return 0;
                    } else {  // if height > 1.15
                      return 1;
                    }
                  } else {  // if stdShiftX > -0.22596
                    if (avgShift <= -0.35167) {
                      return 2;
                    } else {  // if avgShift > -0.35167
                      return 2;
                    }
                  }
                } else {  // if ncc > -0.25217
                  if (ncc <= 0.0065454) {
                    if (width <= -0.83405) {
                      return 2;
                    } else {  // if width > -0.83405
                      return 0;
                    }
                  } else {  // if ncc > 0.0065454
                    if (xyRatio <= -0.77286) {
                      return 2;
                    } else {  // if xyRatio > -0.77286
                      return 0;
                    }
                  }
                }
              } else {  // if ncc > 0.31884
                if (stdShiftX <= 1.0241) {
                  if (avgShiftX <= 0.37062) {
                    if (stdShiftX <= -0.25608) {
                      return 2;
                    } else {  // if stdShiftX > -0.25608
                      return 1;
                    }
                  } else {  // if avgShiftX > 0.37062
                    if (width <= -0.81197) {
                      return 0;
                    } else {  // if width > -0.81197
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 1.0241
                  if (avgShiftX <= 0.68125) {
                    if (stdShift <= 0.31407) {
                      return 2;
                    } else {  // if stdShift > 0.31407
                      return 0;
                    }
                  } else {  // if avgShiftX > 0.68125
                    if (avgShiftY <= -2.0513) {
                      return 0;
                    } else {  // if avgShiftY > -2.0513
                      return 1;
                    }
                  }
                }
              }
            } else {  // if height > 2.215
              if (ncc <= -0.89275) {
                if (stdShift <= 0.040023) {
                  return 2;
                } else {  // if stdShift > 0.040023
                  return 0;
                }
              } else {  // if ncc > -0.89275
                if (avgShiftX <= 0.29349) {
                  if (avgErr <= 0.080045) {
                    return 0;
                  } else {  // if avgErr > 0.080045
                    if (stdShiftX <= -0.089992) {
                      return 0;
                    } else {  // if stdShiftX > -0.089992
                      return 2;
                    }
                  }
                } else {  // if avgShiftX > 0.29349
                  return 2;
                }
              }
            }
          }
        } else {  // if xyRatio > -0.74557
          if (xyRatio <= -0.43815) {
            if (xyRatio <= -0.56864) {
              if (height <= 2.5704) {
                if (ncc <= 1.1914) {
                  if (xyRatio <= -0.73446) {
                    if (height <= -0.095202) {
                      return 0;
                    } else {  // if height > -0.095202
                      return 0;
                    }
                  } else {  // if xyRatio > -0.73446
                    if (width <= -0.57117) {
                      return 0;
                    } else {  // if width > -0.57117
                      return 0;
                    }
                  }
                } else {  // if ncc > 1.1914
                  if (avgErr <= 0.49128) {
                    if (stdShiftX <= 0.17661) {
                      return 1;
                    } else {  // if stdShiftX > 0.17661
                      return 0;
                    }
                  } else {  // if avgErr > 0.49128
                    if (stdShiftY <= -0.19286) {
                      return 0;
                    } else {  // if stdShiftY > -0.19286
                      return 1;
                    }
                  }
                }
              } else {  // if height > 2.5704
                return 1;
              }
            } else {  // if xyRatio > -0.56864
              if (ncc <= 0.72831) {
                if (xyRatio <= -0.56262) {
                  return 0;
                } else {  // if xyRatio > -0.56262
                  if (width <= -0.52945) {
                    if (stdShiftX <= 0.76756) {
                      return 1;
                    } else {  // if stdShiftX > 0.76756
                      return 0;
                    }
                  } else {  // if width > -0.52945
                    if (ncc <= -0.83513) {
                      return 0;
                    } else {  // if ncc > -0.83513
                      return 0;
                    }
                  }
                }
              } else {  // if ncc > 0.72831
                if (height <= -0.13884) {
                  if (ncc <= 1.0582) {
                    return 0;
                  } else {  // if ncc > 1.0582
                    return 1;
                  }
                } else {  // if height > -0.13884
                  if (avgShiftX <= -0.83697) {
                    return 2;
                  } else {  // if avgShiftX > -0.83697
                    if (ncc <= 1.056) {
                      return 0;
                    } else {  // if ncc > 1.056
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > -0.43815
            if (avgShiftY <= -1.1532) {
              if (ncc <= 1.2183) {
                if (avgShiftX <= 1.6303) {
                  if (avgShiftY <= -3.6718) {
                    if (stdShiftX <= 0.8764) {
                      return 2;
                    } else {  // if stdShiftX > 0.8764
                      return 0;
                    }
                  } else {  // if avgShiftY > -3.6718
                    if (avgShift <= 0.30796) {
                      return 0;
                    } else {  // if avgShift > 0.30796
                      return 2;
                    }
                  }
                } else {  // if avgShiftX > 1.6303
                  if (avgShift <= 1.9132) {
                    if (stdShiftX <= 2.1398) {
                      return 1;
                    } else {  // if stdShiftX > 2.1398
                      return 0;
                    }
                  } else {  // if avgShift > 1.9132
                    return 1;
                  }
                }
              } else {  // if ncc > 1.2183
                if (width <= 0.38693) {
                  if (avgErr <= 0.3264) {
                    if (avgShift <= 0.9351) {
                      return 1;
                    } else {  // if avgShift > 0.9351
                      return 2;
                    }
                  } else {  // if avgErr > 0.3264
                    if (avgErr <= 0.39567) {
                      return 0;
                    } else {  // if avgErr > 0.39567
                      return 1;
                    }
                  }
                } else {  // if width > 0.38693
                  return 1;
                }
              }
            } else {  // if avgShiftY > -1.1532
              if (width <= 1.8855) {
                if (xyRatio <= 1.5416) {
                  if (height <= 0.66884) {
                    if (width <= 1.8716) {
                      return 0;
                    } else {  // if width > 1.8716
                      return 0;
                    }
                  } else {  // if height > 0.66884
                    if (avgShiftY <= 0.037388) {
                      return 2;
                    } else {  // if avgShiftY > 0.037388
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 1.5416
                  if (xyRatio <= 1.611) {
                    if (height <= 0.099486) {
                      return 1;
                    } else {  // if height > 0.099486
                      return 2;
                    }
                  } else {  // if xyRatio > 1.611
                    return 0;
                  }
                }
              } else {  // if width > 1.8855
                if (stdShiftY <= 0.62867) {
                  if (stdShift <= -0.28847) {
                    if (avgShiftY <= -0.00096834) {
                      return 0;
                    } else {  // if avgShiftY > -0.00096834
                      return 2;
                    }
                  } else {  // if stdShift > -0.28847
                    if (stdShiftY <= -0.63773) {
                      return 1;
                    } else {  // if stdShiftY > -0.63773
                      return 1;
                    }
                  }
                } else {  // if stdShiftY > 0.62867
                  return 2;
                }
              }
            }
          }
        }
      }
    } else {  // if width > 1.9068
      if (xyRatio <= 1.5008) {
        if (stdShiftY <= -0.59196) {
          if (width <= 1.9456) {
            return 2;
          } else {  // if width > 1.9456
            if (ncc <= -0.92091) {
              return 0;
            } else {  // if ncc > -0.92091
              return 2;
            }
          }
        } else {  // if stdShiftY > -0.59196
          return 0;
        }
      } else {  // if xyRatio > 1.5008
        if (xyRatio <= 1.6028) {
          if (width <= 1.9768) {
            if (stdShiftX <= -0.83844) {
              if (avgShiftY <= 0.012595) {
                return 2;
              } else {  // if avgShiftY > 0.012595
                return 1;
              }
            } else {  // if stdShiftX > -0.83844
              return 2;
            }
          } else {  // if width > 1.9768
            return 1;
          }
        } else {  // if xyRatio > 1.6028
          if (avgErr <= -1.0156) {
            return 2;
          } else {  // if avgErr > -1.0156
            if (width <= 1.9515) {
              return 1;
            } else {  // if width > 1.9515
              if (stdShiftX <= 0.58298) {
                return 2;
              } else {  // if stdShiftX > 0.58298
                return 1;
              }
            }
          }
        }
      }
    }
  }
}

} // namespace rm
