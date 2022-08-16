#include "strm/tree/MTA.hpp"

namespace rm {

float MTA(float width, float height, float type, float xyRatio, float avgShiftX, float avgShiftY,
          float avgShift, float stdShiftX, float stdShiftY, float stdShift, float avgErr,
          float ncc) {
  if (height <= -0.67498) {
    if (width <= 0.28666) {
      if (avgShiftY <= 0.85943) {
        return 0;
      } else {  // if avgShiftY > 0.85943
        if (avgShiftX <= -0.81494) {
          return 1;
        } else {  // if avgShiftX > -0.81494
          return 0;
        }
      }
    } else {  // if width > 0.28666
      if (width <= 1.24) {
        return 1;
      } else {  // if width > 1.24
        if (height <= -1.4722) {
          if (avgShiftY <= -0.088087) {
            return 2;
          } else {  // if avgShiftY > -0.088087
            if (avgShiftY <= 0.050611) {
              return 0;
            } else {  // if avgShiftY > 0.050611
              if (stdShiftY <= -0.24654) {
                return 2;
              } else {  // if stdShiftY > -0.24654
                return 0;
              }
            }
          }
        } else {  // if height > -1.4722
          if (avgShiftX <= -1.4626) {
            return 0;
          } else {  // if avgShiftX > -1.4626
            if (avgShift <= -0.28568) {
              if (stdShiftX <= 0.19504) {
                return 2;
              } else {  // if stdShiftX > 0.19504
                return 0;
              }
            } else {  // if avgShift > -0.28568
              return 2;
            }
          }
        }
      }
    }
  } else {  // if height > -0.67498
    if (height <= -0.018103) {
      if (height <= -0.53957) {
        if (xyRatio <= -0.17485) {
          if (avgShiftY <= -0.40358) {
            if (height <= -0.59242) {
              return 0;
            } else {  // if height > -0.59242
              return 1;
            }
          } else {  // if avgShiftY > -0.40358
            if (width <= -0.89601) {
              if (avgErr <= -0.90838) {
                return 0;
              } else {  // if avgErr > -0.90838
                if (stdShiftX <= -0.32939) {
                  return 1;
                } else {  // if stdShiftX > -0.32939
                  if (stdShiftX <= -0.31879) {
                    return 0;
                  } else {  // if stdShiftX > -0.31879
                    if (stdShift <= -0.050799) {
                      return 1;
                    } else {  // if stdShift > -0.050799
                      return 0;
                    }
                  }
                }
              }
            } else {  // if width > -0.89601
              if (avgErr <= -0.27865) {
                if (stdShiftY <= -0.64731) {
                  return 0;
                } else {  // if stdShiftY > -0.64731
                  if (avgShiftX <= 0.048465) {
                    if (avgShift <= -0.34262) {
                      return 1;
                    } else {  // if avgShift > -0.34262
                      return 1;
                    }
                  } else {  // if avgShiftX > 0.048465
                    if (height <= -0.57547) {
                      return 0;
                    } else {  // if height > -0.57547
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > -0.27865
                if (avgShift <= -0.33017) {
                  if (avgErr <= -0.063776) {
                    if (stdShift <= -0.056782) {
                      return 1;
                    } else {  // if stdShift > -0.056782
                      return 0;
                    }
                  } else {  // if avgErr > -0.063776
                    if (avgShiftY <= 0.34606) {
                      return 1;
                    } else {  // if avgShiftY > 0.34606
                      return 0;
                    }
                  }
                } else {  // if avgShift > -0.33017
                  if (avgShift <= -0.30491) {
                    if (avgShiftY <= 0.58369) {
                      return 1;
                    } else {  // if avgShiftY > 0.58369
                      return 0;
                    }
                  } else {  // if avgShift > -0.30491
                    if (width <= -0.78328) {
                      return 1;
                    } else {  // if width > -0.78328
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > -0.17485
          if (height <= -0.57805) {
            if (stdShiftY <= -0.6284) {
              if (stdShiftY <= -0.69943) {
                return 0;
              } else {  // if stdShiftY > -0.69943
                return 1;
              }
            } else {  // if stdShiftY > -0.6284
              if (ncc <= -0.85576) {
                if (avgShift <= -0.33859) {
                  if (stdShiftY <= -0.35955) {
                    if (avgErr <= -1.3733) {
                      return 0;
                    } else {  // if avgErr > -1.3733
                      return 1;
                    }
                  } else {  // if stdShiftY > -0.35955
                    return 0;
                  }
                } else {  // if avgShift > -0.33859
                  if (avgErr <= 0.41467) {
                    if (xyRatio <= -0.041845) {
                      return 1;
                    } else {  // if xyRatio > -0.041845
                      return 1;
                    }
                  } else {  // if avgErr > 0.41467
                    if (ncc <= -1.926) {
                      return 1;
                    } else {  // if ncc > -1.926
                      return 0;
                    }
                  }
                }
              } else {  // if ncc > -0.85576
                if (stdShiftY <= -0.45442) {
                  if (avgErr <= -1.1848) {
                    if (stdShiftY <= -0.49267) {
                      return 1;
                    } else {  // if stdShiftY > -0.49267
                      return 0;
                    }
                  } else {  // if avgErr > -1.1848
                    return 0;
                  }
                } else {  // if stdShiftY > -0.45442
                  if (stdShiftY <= -0.23876) {
                    if (avgShiftX <= -0.49321) {
                      return 0;
                    } else {  // if avgShiftX > -0.49321
                      return 1;
                    }
                  } else {  // if stdShiftY > -0.23876
                    if (height <= -0.62443) {
                      return 0;
                    } else {  // if height > -0.62443
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if height > -0.57805
            if (stdShiftX <= 0.12232) {
              if (avgErr <= 0.24163) {
                if (width <= -0.21304) {
                  if (avgErr <= -0.84217) {
                    if (avgErr <= -0.9566) {
                      return 1;
                    } else {  // if avgErr > -0.9566
                      return 0;
                    }
                  } else {  // if avgErr > -0.84217
                    if (avgShiftY <= -0.47252) {
                      return 1;
                    } else {  // if avgShiftY > -0.47252
                      return 1;
                    }
                  }
                } else {  // if width > -0.21304
                  if (xyRatio <= 0.49621) {
                    if (ncc <= 0.71264) {
                      return 0;
                    } else {  // if ncc > 0.71264
                      return 1;
                    }
                  } else {  // if xyRatio > 0.49621
                    return 1;
                  }
                }
              } else {  // if avgErr > 0.24163
                if (avgShiftY <= -2.0777) {
                  return 1;
                } else {  // if avgShiftY > -2.0777
                  return 0;
                }
              }
            } else {  // if stdShiftX > 0.12232
              if (avgShiftX <= -0.77515) {
                return 1;
              } else {  // if avgShiftX > -0.77515
                if (stdShift <= -0.035372) {
                  if (width <= -0.14758) {
                    return 1;
                  } else {  // if width > -0.14758
                    return 0;
                  }
                } else {  // if stdShift > -0.035372
                  return 0;
                }
              }
            }
          }
        }
      } else {  // if height > -0.53957
        if (xyRatio <= 0.095281) {
          if (width <= -0.85793) {
            if (width <= -0.8674) {
              if (stdShiftY <= -0.14526) {
                return 1;
              } else {  // if stdShiftY > -0.14526
                if (avgErr <= 0.564) {
                  return 1;
                } else {  // if avgErr > 0.564
                  if (avgShiftX <= -0.61336) {
                    return 1;
                  } else {  // if avgShiftX > -0.61336
                    return 0;
                  }
                }
              }
            } else {  // if width > -0.8674
              return 0;
            }
          } else {  // if width > -0.85793
            if (avgShift <= -0.3456) {
              return 0;
            } else {  // if avgShift > -0.3456
              if (height <= -0.021077) {
                if (avgShift <= -0.34343) {
                  if (avgShiftY <= 0.13227) {
                    return 0;
                  } else {  // if avgShiftY > 0.13227
                    return 1;
                  }
                } else {  // if avgShift > -0.34343
                  if (height <= -0.51096) {
                    if (avgShiftX <= 0.39138) {
                      return 1;
                    } else {  // if avgShiftX > 0.39138
                      return 0;
                    }
                  } else {  // if height > -0.51096
                    return 1;
                  }
                }
              } else {  // if height > -0.021077
                if (ncc <= -1.0683) {
                  return 2;
                } else {  // if ncc > -1.0683
                  if (avgErr <= -0.17185) {
                    return 2;
                  } else {  // if avgErr > -0.17185
                    return 1;
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > 0.095281
          if (height <= -0.40231) {
            if (height <= -0.53205) {
              if (avgShiftX <= 0.5273) {
                if (avgErr <= -1.3498) {
                  return 0;
                } else {  // if avgErr > -1.3498
                  if (avgShiftX <= 0.31061) {
                    return 1;
                  } else {  // if avgShiftX > 0.31061
                    if (avgShift <= -0.28423) {
                      return 0;
                    } else {  // if avgShift > -0.28423
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftX > 0.5273
                return 0;
              }
            } else {  // if height > -0.53205
              if (stdShiftY <= -0.40018) {
                return 0;
              } else {  // if stdShiftY > -0.40018
                if (avgShiftY <= -0.28738) {
                  if (stdShiftY <= 0.20494) {
                    return 0;
                  } else {  // if stdShiftY > 0.20494
                    return 1;
                  }
                } else {  // if avgShiftY > -0.28738
                  if (stdShiftX <= 0.19001) {
                    if (avgShift <= -0.28525) {
                      return 0;
                    } else {  // if avgShift > -0.28525
                      return 1;
                    }
                  } else {  // if stdShiftX > 0.19001
                    if (stdShiftX <= 1.5534) {
                      return 0;
                    } else {  // if stdShiftX > 1.5534
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if height > -0.40231
            if (width <= 1.3603) {
              if (width <= 0.81889) {
                if (avgShiftX <= 1.4038) {
                  return 1;
                } else {  // if avgShiftX > 1.4038
                  if (avgShiftX <= 1.5924) {
                    return 0;
                  } else {  // if avgShiftX > 1.5924
                    return 1;
                  }
                }
              } else {  // if width > 0.81889
                if (avgShiftY <= -0.10914) {
                  return 0;
                } else {  // if avgShiftY > -0.10914
                  return 1;
                }
              }
            } else {  // if width > 1.3603
              return 2;
            }
          }
        }
      }
    } else {  // if height > -0.018103
      if (height <= 0.40461) {
        if (avgShiftY <= -0.013187) {
          if (height <= 0.013925) {
            if (avgShiftX <= -1.2672) {
              if (avgErr <= 1.5262) {
                return 2;
              } else {  // if avgErr > 1.5262
                return 1;
              }
            } else {  // if avgShiftX > -1.2672
              if (stdShift <= -0.039838) {
                if (stdShift <= -0.044549) {
                  if (stdShiftX <= -0.33951) {
                    if (stdShift <= -0.054037) {
                      return 1;
                    } else {  // if stdShift > -0.054037
                      return 2;
                    }
                  } else {  // if stdShiftX > -0.33951
                    if (avgShift <= -0.046039) {
                      return 1;
                    } else {  // if avgShift > -0.046039
                      return 1;
                    }
                  }
                } else {  // if stdShift > -0.044549
                  if (ncc <= 0.061919) {
                    return 1;
                  } else {  // if ncc > 0.061919
                    return 2;
                  }
                }
              } else {  // if stdShift > -0.039838
                if (avgShiftY <= -2.1377) {
                  if (stdShiftX <= 1.453) {
                    return 2;
                  } else {  // if stdShiftX > 1.453
                    return 1;
                  }
                } else {  // if avgShiftY > -2.1377
                  return 1;
                }
              }
            }
          } else {  // if height > 0.013925
            if (avgShiftX <= 0.23142) {
              if (xyRatio <= 0.50899) {
                if (stdShift <= 0.11165) {
                  if (ncc <= 0.73079) {
                    if (xyRatio <= -0.48399) {
                      return 1;
                    } else {  // if xyRatio > -0.48399
                      return 2;
                    }
                  } else {  // if ncc > 0.73079
                    if (ncc <= 0.73383) {
                      return 1;
                    } else {  // if ncc > 0.73383
                      return 2;
                    }
                  }
                } else {  // if stdShift > 0.11165
                  if (avgShiftX <= -1.905) {
                    return 2;
                  } else {  // if avgShiftX > -1.905
                    return 1;
                  }
                }
              } else {  // if xyRatio > 0.50899
                if (avgShiftX <= -0.43686) {
                  return 1;
                } else {  // if avgShiftX > -0.43686
                  return 0;
                }
              }
            } else {  // if avgShiftX > 0.23142
              if (avgShift <= -0.2574) {
                if (avgShiftY <= -0.17548) {
                  if (avgShiftY <= -0.2108) {
                    if (width <= 0.22133) {
                      return 1;
                    } else {  // if width > 0.22133
                      return 2;
                    }
                  } else {  // if avgShiftY > -0.2108
                    return 2;
                  }
                } else {  // if avgShiftY > -0.17548
                  if (height <= 0.049684) {
                    if (stdShiftX <= -0.20385) {
                      return 1;
                    } else {  // if stdShiftX > -0.20385
                      return 2;
                    }
                  } else {  // if height > 0.049684
                    if (avgShiftX <= 0.48824) {
                      return 1;
                    } else {  // if avgShiftX > 0.48824
                      return 2;
                    }
                  }
                }
              } else {  // if avgShift > -0.2574
                if (stdShiftX <= 0.99053) {
                  if (width <= -0.039407) {
                    if (width <= -0.14806) {
                      return 2;
                    } else {  // if width > -0.14806
                      return 1;
                    }
                  } else {  // if width > -0.039407
                    if (stdShift <= -0.054545) {
                      return 1;
                    } else {  // if stdShift > -0.054545
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 0.99053
                  return 1;
                }
              }
            }
          }
        } else {  // if avgShiftY > -0.013187
          if (stdShiftX <= -0.096617) {
            if (avgShiftX <= 0.97055) {
              if (xyRatio <= 0.0073405) {
                if (width <= 0.066467) {
                  if (xyRatio <= -0.20557) {
                    if (height <= -0.01298) {
                      return 1;
                    } else {  // if height > -0.01298
                      return 2;
                    }
                  } else {  // if xyRatio > -0.20557
                    if (avgShiftX <= -0.14919) {
                      return 1;
                    } else {  // if avgShiftX > -0.14919
                      return 2;
                    }
                  }
                } else {  // if width > 0.066467
                  return 2;
                }
              } else {  // if xyRatio > 0.0073405
                if (width <= 0.37658) {
                  if (ncc <= 0.701) {
                    if (avgShiftY <= 0.029795) {
                      return 2;
                    } else {  // if avgShiftY > 0.029795
                      return 1;
                    }
                  } else {  // if ncc > 0.701
                    return 2;
                  }
                } else {  // if width > 0.37658
                  return 2;
                }
              }
            } else {  // if avgShiftX > 0.97055
              return 1;
            }
          } else {  // if stdShiftX > -0.096617
            if (avgShiftX <= -0.083291) {
              if (stdShiftY <= 1.5559) {
                if (avgShiftY <= 2.9779) {
                  if (stdShiftY <= -0.17078) {
                    if (avgErr <= -0.089108) {
                      return 2;
                    } else {  // if avgErr > -0.089108
                      return 1;
                    }
                  } else {  // if stdShiftY > -0.17078
                    if (avgShiftX <= -1.7072) {
                      return 2;
                    } else {  // if avgShiftX > -1.7072
                      return 2;
                    }
                  }
                } else {  // if avgShiftY > 2.9779
                  return 1;
                }
              } else {  // if stdShiftY > 1.5559
                if (avgShiftY <= 1.2815) {
                  return 2;
                } else {  // if avgShiftY > 1.2815
                  return 1;
                }
              }
            } else {  // if avgShiftX > -0.083291
              if (avgShiftX <= 0.48628) {
                if (height <= 0.14553) {
                  if (ncc <= 0.68336) {
                    if (height <= 0.017541) {
                      return 1;
                    } else {  // if height > 0.017541
                      return 1;
                    }
                  } else {  // if ncc > 0.68336
                    return 2;
                  }
                } else {  // if height > 0.14553
                  if (width <= 0.13663) {
                    return 2;
                  } else {  // if width > 0.13663
                    if (avgShift <= -0.069972) {
                      return 1;
                    } else {  // if avgShift > -0.069972
                      return 2;
                    }
                  }
                }
              } else {  // if avgShiftX > 0.48628
                if (avgErr <= 0.020052) {
                  return 2;
                } else {  // if avgErr > 0.020052
                  if (stdShiftX <= 0.16663) {
                    if (xyRatio <= -0.17608) {
                      return 1;
                    } else {  // if xyRatio > -0.17608
                      return 2;
                    }
                  } else {  // if stdShiftX > 0.16663
                    if (stdShift <= 0.23023) {
                      return 2;
                    } else {  // if stdShift > 0.23023
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if height > 0.40461
        if (xyRatio <= -0.26816) {
          if (height <= 0.41936) {
            if (avgShiftY <= -0.3623) {
              if (avgErr <= -0.55274) {
                return 2;
              } else {  // if avgErr > -0.55274
                return 1;
              }
            } else {  // if avgShiftY > -0.3623
              return 1;
            }
          } else {  // if height > 0.41936
            if (avgShiftX <= 1.8091) {
              if (avgShiftY <= -0.7136) {
                if (height <= 2.2298) {
                  if (avgErr <= 0.75137) {
                    if (stdShiftY <= 3.2577) {
                      return 2;
                    } else {  // if stdShiftY > 3.2577
                      return 1;
                    }
                  } else {  // if avgErr > 0.75137
                    if (avgShiftX <= 0.44706) {
                      return 2;
                    } else {  // if avgShiftX > 0.44706
                      return 0;
                    }
                  }
                } else {  // if height > 2.2298
                  if (avgShiftY <= -2.3436) {
                    return 0;
                  } else {  // if avgShiftY > -2.3436
                    if (stdShiftY <= 1.3029) {
                      return 0;
                    } else {  // if stdShiftY > 1.3029
                      return 2;
                    }
                  }
                }
              } else {  // if avgShiftY > -0.7136
                if (xyRatio <= -0.44867) {
                  if (avgShiftY <= 1.3664) {
                    if (avgShiftY <= -0.64668) {
                      return 0;
                    } else {  // if avgShiftY > -0.64668
                      return 2;
                    }
                  } else {  // if avgShiftY > 1.3664
                    if (avgShiftY <= 1.9388) {
                      return 1;
                    } else {  // if avgShiftY > 1.9388
                      return 2;
                    }
                  }
                } else {  // if xyRatio > -0.44867
                  if (width <= 0.050901) {
                    if (avgShiftY <= -0.39613) {
                      return 2;
                    } else {  // if avgShiftY > -0.39613
                      return 0;
                    }
                  } else {  // if width > 0.050901
                    if (width <= 0.33968) {
                      return 2;
                    } else {  // if width > 0.33968
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgShiftX > 1.8091
              if (avgShift <= 1.0124) {
                return 1;
              } else {  // if avgShift > 1.0124
                if (stdShiftX <= 3.7792) {
                  if (width <= 2.0137) {
                    if (avgShift <= 2.9075) {
                      return 2;
                    } else {  // if avgShift > 2.9075
                      return 1;
                    }
                  } else {  // if width > 2.0137
                    if (avgShiftX <= 2.9204) {
                      return 0;
                    } else {  // if avgShiftX > 2.9204
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 3.7792
                  return 1;
                }
              }
            }
          }
        } else {  // if xyRatio > -0.26816
          if (avgShiftX <= 0.76012) {
            if (avgErr <= -0.49856) {
              if (ncc <= 0.45975) {
                if (avgShiftX <= 0.70196) {
                  if (xyRatio <= 0.26059) {
                    if (stdShiftX <= -0.38474) {
                      return 2;
                    } else {  // if stdShiftX > -0.38474
                      return 2;
                    }
                  } else {  // if xyRatio > 0.26059
                    if (avgShiftX <= -0.095507) {
                      return 0;
                    } else {  // if avgShiftX > -0.095507
                      return 1;
                    }
                  }
                } else {  // if avgShiftX > 0.70196
                  if (width <= 0.58808) {
                    return 1;
                  } else {  // if width > 0.58808
                    return 0;
                  }
                }
              } else {  // if ncc > 0.45975
                if (avgShiftY <= 0.12251) {
                  if (xyRatio <= 0.085354) {
                    if (avgShiftX <= 0.6693) {
                      return 1;
                    } else {  // if avgShiftX > 0.6693
                      return 2;
                    }
                  } else {  // if xyRatio > 0.085354
                    return 2;
                  }
                } else {  // if avgShiftY > 0.12251
                  if (width <= 1.3187) {
                    return 0;
                  } else {  // if width > 1.3187
                    if (height <= 1.0298) {
                      return 1;
                    } else {  // if height > 1.0298
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgErr > -0.49856
              if (avgShiftX <= -1.7745) {
                if (stdShift <= 0.33985) {
                  if (avgShiftY <= 1.1935) {
                    if (stdShiftY <= 1.1269) {
                      return 1;
                    } else {  // if stdShiftY > 1.1269
                      return 2;
                    }
                  } else {  // if avgShiftY > 1.1935
                    if (avgShiftX <= -2.0812) {
                      return 0;
                    } else {  // if avgShiftX > -2.0812
                      return 2;
                    }
                  }
                } else {  // if stdShift > 0.33985
                  if (avgErr <= 0.94736) {
                    return 2;
                  } else {  // if avgErr > 0.94736
                    if (avgErr <= 0.98463) {
                      return 0;
                    } else {  // if avgErr > 0.98463
                      return 2;
                    }
                  }
                }
              } else {  // if avgShiftX > -1.7745
                if (avgShiftX <= -1.5626) {
                  if (avgShiftY <= -2.2047) {
                    return 2;
                  } else {  // if avgShiftY > -2.2047
                    return 0;
                  }
                } else {  // if avgShiftX > -1.5626
                  if (avgShiftX <= -1.0242) {
                    if (avgErr <= 0.083218) {
                      return 2;
                    } else {  // if avgErr > 0.083218
                      return 2;
                    }
                  } else {  // if avgShiftX > -1.0242
                    if (height <= 1.8933) {
                      return 0;
                    } else {  // if height > 1.8933
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if avgShiftX > 0.76012
            if (xyRatio <= 0.28375) {
              if (xyRatio <= 0.095036) {
                if (xyRatio <= 0.031724) {
                  if (width <= 2.3536) {
                    if (height <= 2.2699) {
                      return 2;
                    } else {  // if height > 2.2699
                      return 1;
                    }
                  } else {  // if width > 2.3536
                    if (xyRatio <= -0.12223) {
                      return 1;
                    } else {  // if xyRatio > -0.12223
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.031724
                  if (ncc <= 0.24388) {
                    if (stdShiftX <= 1.1195) {
                      return 2;
                    } else {  // if stdShiftX > 1.1195
                      return 0;
                    }
                  } else {  // if ncc > 0.24388
                    if (avgShiftX <= 2.5836) {
                      return 0;
                    } else {  // if avgShiftX > 2.5836
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.095036
                if (ncc <= 0.13137) {
                  return 2;
                } else {  // if ncc > 0.13137
                  if (avgErr <= 1.064) {
                    return 1;
                  } else {  // if avgErr > 1.064
                    if (stdShiftY <= 1.7237) {
                      return 2;
                    } else {  // if stdShiftY > 1.7237
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.28375
              if (avgErr <= 0.040393) {
                if (avgErr <= -0.81639) {
                  return 0;
                } else {  // if avgErr > -0.81639
                  if (stdShiftY <= 0.78134) {
                    if (avgShiftY <= -3.8657) {
                      return 0;
                    } else {  // if avgShiftY > -3.8657
                      return 2;
                    }
                  } else {  // if stdShiftY > 0.78134
                    if (avgShiftY <= -0.86239) {
                      return 2;
                    } else {  // if avgShiftY > -0.86239
                      return 0;
                    }
                  }
                }
              } else {  // if avgErr > 0.040393
                return 0;
              }
            }
          }
        }
      }
    }
  }
}

} // namespace rm
