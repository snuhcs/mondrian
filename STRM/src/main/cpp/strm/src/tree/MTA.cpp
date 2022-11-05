#include "strm/tree/MTA.hpp"

namespace rm {

float MTA(float width, float height, float maxEdgeLength, float type, float origin, float xyRatio,
          float avgShiftX, float avgShiftY, float avgShift, float stdShiftX, float stdShiftY,
          float stdShift, float avgErr, float ncc) {
  if (width <= 67.873) {
    if (maxEdgeLength <= 96.636) {
      if (maxEdgeLength <= 64.492) {
        if (height <= 46.748) {
          if (width <= 39.394) {
            if (width <= 23.493) {
              if (avgErr <= 10.742) {
                if (avgErr <= 7.8468) {
                  return 2;
                } else {  // if avgErr > 7.8468
                  return 1;
                }
              } else {  // if avgErr > 10.742
                return 2;
              }
            } else {  // if width > 23.493
              if (avgShiftY <= 0.11868) {
                if (avgShiftX <= 0.5506) {
                  return 2;
                } else {  // if avgShiftX > 0.5506
                  if (avgShift <= 0.31615) {
                    return 1;
                  } else {  // if avgShift > 0.31615
                    return 2;
                  }
                }
              } else {  // if avgShiftY > 0.11868
                if (avgShiftY <= 0.11964) {
                  return 1;
                } else {  // if avgShiftY > 0.11964
                  if (maxEdgeLength <= 45.284) {
                    return 2;
                  } else {  // if maxEdgeLength > 45.284
                    if (height <= 45.454) {
                      return 1;
                    } else {  // if height > 45.454
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if width > 39.394
            if (width <= 39.53) {
              if (avgShift <= 0.16523) {
                return 1;
              } else {  // if avgShift > 0.16523
                return 2;
              }
            } else {  // if width > 39.53
              if (maxEdgeLength <= 43.372) {
                if (maxEdgeLength <= 41.1) {
                  if (avgShift <= 0.00052843) {
                    if (height <= 32.797) {
                      return 2;
                    } else {  // if height > 32.797
                      return 1;
                    }
                  } else {  // if avgShift > 0.00052843
                    return 2;
                  }
                } else {  // if maxEdgeLength > 41.1
                  if (avgErr <= 3.7047) {
                    if (avgShiftY <= 0.058904) {
                      return 1;
                    } else {  // if avgShiftY > 0.058904
                      return 2;
                    }
                  } else {  // if avgErr > 3.7047
                    if (ncc <= 0.44751) {
                      return 2;
                    } else {  // if ncc > 0.44751
                      return 1;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 43.372
                if (height <= 46.316) {
                  if (avgShiftX <= -0.15091) {
                    if (maxEdgeLength <= 45.041) {
                      return 2;
                    } else {  // if maxEdgeLength > 45.041
                      return 1;
                    }
                  } else {  // if avgShiftX > -0.15091
                    if (height <= 23.891) {
                      return 1;
                    } else {  // if height > 23.891
                      return 2;
                    }
                  }
                } else {  // if height > 46.316
                  if (stdShift <= 1.1116e-05) {
                    return 1;
                  } else {  // if stdShift > 1.1116e-05
                    return 2;
                  }
                }
              }
            }
          }
        } else {  // if height > 46.748
          if (avgErr <= 0.65218) {
            if (height <= 47.845) {
              if (maxEdgeLength <= 46.876) {
                return 2;
              } else {  // if maxEdgeLength > 46.876
                if (xyRatio <= 0.85273) {
                  return 1;
                } else {  // if xyRatio > 0.85273
                  if (width <= 43.393) {
                    return 1;
                  } else {  // if width > 43.393
                    return 2;
                  }
                }
              }
            } else {  // if height > 47.845
              if (height <= 63.973) {
                if (height <= 63.078) {
                  if (height <= 62.562) {
                    if (width <= 46.062) {
                      return 2;
                    } else {  // if width > 46.062
                      return 2;
                    }
                  } else {  // if height > 62.562
                    if (stdShift <= 0.00072415) {
                      return 1;
                    } else {  // if stdShift > 0.00072415
                      return 2;
                    }
                  }
                } else {  // if height > 63.078
                  return 2;
                }
              } else {  // if height > 63.973
                if (ncc <= 0.86598) {
                  return 1;
                } else {  // if ncc > 0.86598
                  if (avgShiftX <= -0.0062256) {
                    return 1;
                  } else {  // if avgShiftX > -0.0062256
                    return 2;
                  }
                }
              }
            }
          } else {  // if avgErr > 0.65218
            if (maxEdgeLength <= 60.234) {
              if (width <= 36.983) {
                if (height <= 58.976) {
                  if (height <= 52.645) {
                    if (stdShiftY <= 0.14415) {
                      return 2;
                    } else {  // if stdShiftY > 0.14415
                      return 1;
                    }
                  } else {  // if height > 52.645
                    if (avgShiftY <= 0.47049) {
                      return 2;
                    } else {  // if avgShiftY > 0.47049
                      return 2;
                    }
                  }
                } else {  // if height > 58.976
                  if (maxEdgeLength <= 59.29) {
                    if (maxEdgeLength <= 59.251) {
                      return 2;
                    } else {  // if maxEdgeLength > 59.251
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 59.29
                    if (avgErr <= 3.2) {
                      return 1;
                    } else {  // if avgErr > 3.2
                      return 2;
                    }
                  }
                }
              } else {  // if width > 36.983
                if (height <= 51.305) {
                  if (width <= 41.471) {
                    if (ncc <= -0.092807) {
                      return 2;
                    } else {  // if ncc > -0.092807
                      return 2;
                    }
                  } else {  // if width > 41.471
                    if (avgErr <= 3.21) {
                      return 1;
                    } else {  // if avgErr > 3.21
                      return 2;
                    }
                  }
                } else {  // if height > 51.305
                  if (avgShiftY <= -0.081596) {
                    if (width <= 37.073) {
                      return 1;
                    } else {  // if width > 37.073
                      return 2;
                    }
                  } else {  // if avgShiftY > -0.081596
                    if (xyRatio <= 0.44655) {
                      return 2;
                    } else {  // if xyRatio > 0.44655
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 60.234
              if (width <= 38.814) {
                if (avgShiftX <= 0.38031) {
                  if (ncc <= -0.99816) {
                    return 1;
                  } else {  // if ncc > -0.99816
                    if (width <= 35.308) {
                      return 2;
                    } else {  // if width > 35.308
                      return 2;
                    }
                  }
                } else {  // if avgShiftX > 0.38031
                  if (width <= 35.17) {
                    if (height <= 61.506) {
                      return 1;
                    } else {  // if height > 61.506
                      return 2;
                    }
                  } else {  // if width > 35.17
                    if (avgErr <= 4.6592) {
                      return 2;
                    } else {  // if avgErr > 4.6592
                      return 2;
                    }
                  }
                }
              } else {  // if width > 38.814
                if (maxEdgeLength <= 64.445) {
                  if (avgErr <= 6.1387) {
                    if (stdShiftX <= 0.49591) {
                      return 2;
                    } else {  // if stdShiftX > 0.49591
                      return 1;
                    }
                  } else {  // if avgErr > 6.1387
                    if (maxEdgeLength <= 60.936) {
                      return 2;
                    } else {  // if maxEdgeLength > 60.936
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 64.445
                  if (avgShiftY <= 0.051304) {
                    return 2;
                  } else {  // if avgShiftY > 0.051304
                    return 1;
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 64.492
        if (width <= 56.998) {
          if (xyRatio <= 0.35584) {
            if (height <= 67.461) {
              if (avgShiftX <= -0.18372) {
                if (height <= 67.325) {
                  if (avgShiftY <= -0.10423) {
                    if (avgErr <= 4.7206) {
                      return 1;
                    } else {  // if avgErr > 4.7206
                      return 2;
                    }
                  } else {  // if avgShiftY > -0.10423
                    if (height <= 65.257) {
                      return 2;
                    } else {  // if height > 65.257
                      return 1;
                    }
                  }
                } else {  // if height > 67.325
                  if (avgShiftY <= -0.0035114) {
                    return 1;
                  } else {  // if avgShiftY > -0.0035114
                    if (stdShift <= 0.013928) {
                      return 1;
                    } else {  // if stdShift > 0.013928
                      return 2;
                    }
                  }
                }
              } else {  // if avgShiftX > -0.18372
                if (height <= 67.031) {
                  if (avgErr <= 0.91639) {
                    return 1;
                  } else {  // if avgErr > 0.91639
                    if (avgShiftX <= -0.0012207) {
                      return 2;
                    } else {  // if avgShiftX > -0.0012207
                      return 2;
                    }
                  }
                } else {  // if height > 67.031
                  if (avgShift <= 0.10892) {
                    return 2;
                  } else {  // if avgShift > 0.10892
                    if (stdShiftY <= 0.071921) {
                      return 2;
                    } else {  // if stdShiftY > 0.071921
                      return 1;
                    }
                  }
                }
              }
            } else {  // if height > 67.461
              if (avgShiftY <= 0.59409) {
                if (avgShiftX <= -1.2986) {
                  if (avgErr <= 8.4368) {
                    if (avgErr <= 5.8207) {
                      return 2;
                    } else {  // if avgErr > 5.8207
                      return 1;
                    }
                  } else {  // if avgErr > 8.4368
                    return 2;
                  }
                } else {  // if avgShiftX > -1.2986
                  if (xyRatio <= 0.35462) {
                    if (stdShift <= 0.010223) {
                      return 2;
                    } else {  // if stdShift > 0.010223
                      return 2;
                    }
                  } else {  // if xyRatio > 0.35462
                    if (stdShiftX <= 0.06527) {
                      return 2;
                    } else {  // if stdShiftX > 0.06527
                      return 2;
                    }
                  }
                }
              } else {  // if avgShiftY > 0.59409
                if (height <= 88.891) {
                  if (avgShiftX <= -0.97583) {
                    if (avgShiftX <= -1.0809) {
                      return 2;
                    } else {  // if avgShiftX > -1.0809
                      return 1;
                    }
                  } else {  // if avgShiftX > -0.97583
                    return 2;
                  }
                } else {  // if height > 88.891
                  if (xyRatio <= 0.34674) {
                    if (ncc <= 0.94338) {
                      return 2;
                    } else {  // if ncc > 0.94338
                      return 2;
                    }
                  } else {  // if xyRatio > 0.34674
                    return 0;
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.35584
            if (maxEdgeLength <= 91.334) {
              if (width <= 39.689) {
                if (width <= 39.667) {
                  if (maxEdgeLength <= 71.023) {
                    if (width <= 37.847) {
                      return 2;
                    } else {  // if width > 37.847
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 71.023
                    if (xyRatio <= 0.36854) {
                      return 2;
                    } else {  // if xyRatio > 0.36854
                      return 2;
                    }
                  }
                } else {  // if width > 39.667
                  if (avgShiftY <= -0.25411) {
                    return 2;
                  } else {  // if avgShiftY > -0.25411
                    return 1;
                  }
                }
              } else {  // if width > 39.689
                if (maxEdgeLength <= 72.39) {
                  if (avgShiftX <= -1.6518) {
                    if (stdShiftY <= 0.090516) {
                      return 2;
                    } else {  // if stdShiftY > 0.090516
                      return 2;
                    }
                  } else {  // if avgShiftX > -1.6518
                    if (width <= 41.114) {
                      return 2;
                    } else {  // if width > 41.114
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 72.39
                  if (height <= 72.495) {
                    if (avgShiftX <= 0.16695) {
                      return 1;
                    } else {  // if avgShiftX > 0.16695
                      return 2;
                    }
                  } else {  // if height > 72.495
                    if (width <= 49.553) {
                      return 2;
                    } else {  // if width > 49.553
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 91.334
              if (height <= 91.368) {
                if (avgErr <= 7.6365) {
                  return 0;
                } else {  // if avgErr > 7.6365
                  if (ncc <= -0.040572) {
                    return 2;
                  } else {  // if ncc > -0.040572
                    if (ncc <= 0.35501) {
                      return 0;
                    } else {  // if ncc > 0.35501
                      return 2;
                    }
                  }
                }
              } else {  // if height > 91.368
                if (avgErr <= 8.763) {
                  if (width <= 47.552) {
                    if (stdShift <= 0.05682) {
                      return 2;
                    } else {  // if stdShift > 0.05682
                      return 2;
                    }
                  } else {  // if width > 47.552
                    if (avgErr <= 7.3132) {
                      return 2;
                    } else {  // if avgErr > 7.3132
                      return 2;
                    }
                  }
                } else {  // if avgErr > 8.763
                  if (stdShiftY <= 0.065722) {
                    if (width <= 50.685) {
                      return 1;
                    } else {  // if width > 50.685
                      return 0;
                    }
                  } else {  // if stdShiftY > 0.065722
                    if (avgShiftX <= 0.24475) {
                      return 2;
                    } else {  // if avgShiftX > 0.24475
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if width > 56.998
          if (xyRatio <= 0.83303) {
            if (width <= 64.287) {
              if (avgErr <= 9.8115) {
                if (xyRatio <= 0.8218) {
                  if (xyRatio <= 0.76509) {
                    if (avgShiftX <= -0.68606) {
                      return 2;
                    } else {  // if avgShiftX > -0.68606
                      return 2;
                    }
                  } else {  // if xyRatio > 0.76509
                    return 2;
                  }
                } else {  // if xyRatio > 0.8218
                  if (stdShiftX <= 0.007409) {
                    return 1;
                  } else {  // if stdShiftX > 0.007409
                    if (avgErr <= 0.36939) {
                      return 1;
                    } else {  // if avgErr > 0.36939
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 9.8115
                if (width <= 61.008) {
                  if (width <= 59.285) {
                    if (stdShiftY <= 0.20745) {
                      return 1;
                    } else {  // if stdShiftY > 0.20745
                      return 2;
                    }
                  } else {  // if width > 59.285
                    if (avgShiftY <= -0.14249) {
                      return 2;
                    } else {  // if avgShiftY > -0.14249
                      return 0;
                    }
                  }
                } else {  // if width > 61.008
                  if (avgShiftY <= -1.1685) {
                    return 1;
                  } else {  // if avgShiftY > -1.1685
                    if (stdShiftY <= 0.10029) {
                      return 0;
                    } else {  // if stdShiftY > 0.10029
                      return 2;
                    }
                  }
                }
              }
            } else {  // if width > 64.287
              if (avgErr <= 3.0875) {
                return 1;
              } else {  // if avgErr > 3.0875
                if (height <= 91.729) {
                  if (avgShift <= 1.1581) {
                    if (ncc <= 0.35297) {
                      return 1;
                    } else {  // if ncc > 0.35297
                      return 0;
                    }
                  } else {  // if avgShift > 1.1581
                    if (avgShiftY <= 1.1471) {
                      return 2;
                    } else {  // if avgShiftY > 1.1471
                      return 0;
                    }
                  }
                } else {  // if height > 91.729
                  if (stdShift <= 0.053143) {
                    return 2;
                  } else {  // if stdShift > 0.053143
                    return 1;
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.83303
            if (avgShift <= 53.598) {
              return 2;
            } else {  // if avgShift > 53.598
              return 0;
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 96.636
      if (avgErr <= 1.9048) {
        if (xyRatio <= 0.34897) {
          return 1;
        } else {  // if xyRatio > 0.34897
          if (ncc <= -0.082318) {
            return 0;
          } else {  // if ncc > -0.082318
            return 2;
          }
        }
      } else {  // if avgErr > 1.9048
        if (avgShiftX <= -0.69698) {
          if (xyRatio <= 0.4172) {
            if (xyRatio <= 0.30586) {
              if (stdShiftX <= 0.11222) {
                return 0;
              } else {  // if stdShiftX > 0.11222
                if (ncc <= 0.88282) {
                  if (width <= 40.562) {
                    return 1;
                  } else {  // if width > 40.562
                    if (avgShiftX <= -0.77081) {
                      return 2;
                    } else {  // if avgShiftX > -0.77081
                      return 1;
                    }
                  }
                } else {  // if ncc > 0.88282
                  return 1;
                }
              }
            } else {  // if xyRatio > 0.30586
              if (stdShift <= 0.61755) {
                if (height <= 104.85) {
                  if (avgErr <= 9.5756) {
                    return 2;
                  } else {  // if avgErr > 9.5756
                    if (avgErr <= 9.7003) {
                      return 1;
                    } else {  // if avgErr > 9.7003
                      return 2;
                    }
                  }
                } else {  // if height > 104.85
                  if (maxEdgeLength <= 107.8) {
                    if (width <= 53.641) {
                      return 1;
                    } else {  // if width > 53.641
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 107.8
                    if (avgShiftX <= -0.74351) {
                      return 2;
                    } else {  // if avgShiftX > -0.74351
                      return 1;
                    }
                  }
                }
              } else {  // if stdShift > 0.61755
                if (ncc <= 0.59086) {
                  if (avgShiftY <= -0.28508) {
                    if (stdShiftX <= 0.3021) {
                      return 2;
                    } else {  // if stdShiftX > 0.3021
                      return 1;
                    }
                  } else {  // if avgShiftY > -0.28508
                    if (avgErr <= 9.3586) {
                      return 1;
                    } else {  // if avgErr > 9.3586
                      return 2;
                    }
                  }
                } else {  // if ncc > 0.59086
                  if (avgShiftX <= -3.6385) {
                    if (avgShiftY <= -1.027) {
                      return 2;
                    } else {  // if avgShiftY > -1.027
                      return 0;
                    }
                  } else {  // if avgShiftX > -3.6385
                    if (height <= 112.15) {
                      return 2;
                    } else {  // if height > 112.15
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.4172
            if (stdShiftY <= 0.13818) {
              if (avgErr <= 5.5523) {
                return 2;
              } else {  // if avgErr > 5.5523
                if (avgShiftY <= 0.18097) {
                  if (avgShiftX <= -0.74498) {
                    return 1;
                  } else {  // if avgShiftX > -0.74498
                    return 2;
                  }
                } else {  // if avgShiftY > 0.18097
                  return 2;
                }
              }
            } else {  // if stdShiftY > 0.13818
              if (xyRatio <= 0.41978) {
                if (ncc <= 0.62869) {
                  return 2;
                } else {  // if ncc > 0.62869
                  if (stdShiftY <= 0.35614) {
                    if (stdShiftX <= 0.27894) {
                      return 1;
                    } else {  // if stdShiftX > 0.27894
                      return 0;
                    }
                  } else {  // if stdShiftY > 0.35614
                    return 1;
                  }
                }
              } else {  // if xyRatio > 0.41978
                if (width <= 67.606) {
                  if (avgShift <= 4.5194) {
                    if (stdShiftX <= 0.42472) {
                      return 2;
                    } else {  // if stdShiftX > 0.42472
                      return 2;
                    }
                  } else {  // if avgShift > 4.5194
                    if (avgShiftY <= -0.57923) {
                      return 2;
                    } else {  // if avgShiftY > -0.57923
                      return 1;
                    }
                  }
                } else {  // if width > 67.606
                  if (avgShift <= 2.2657) {
                    return 2;
                  } else {  // if avgShift > 2.2657
                    if (ncc <= 0.98027) {
                      return 1;
                    } else {  // if ncc > 0.98027
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if avgShiftX > -0.69698
          if (xyRatio <= 0.58267) {
            if (avgShift <= 15.401) {
              if (width <= 51.439) {
                if (avgErr <= 2.5388) {
                  return 1;
                } else {  // if avgErr > 2.5388
                  if (ncc <= 0.71386) {
                    if (avgErr <= 14.308) {
                      return 2;
                    } else {  // if avgErr > 14.308
                      return 0;
                    }
                  } else {  // if ncc > 0.71386
                    if (ncc <= 0.72471) {
                      return 1;
                    } else {  // if ncc > 0.72471
                      return 2;
                    }
                  }
                }
              } else {  // if width > 51.439
                if (maxEdgeLength <= 111.41) {
                  if (avgShiftX <= 0.70596) {
                    if (stdShiftX <= 0.1551) {
                      return 2;
                    } else {  // if stdShiftX > 0.1551
                      return 2;
                    }
                  } else {  // if avgShiftX > 0.70596
                    if (stdShift <= 0.03635) {
                      return 1;
                    } else {  // if stdShift > 0.03635
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 111.41
                  if (xyRatio <= 0.34348) {
                    if (avgErr <= 3.3486) {
                      return 1;
                    } else {  // if avgErr > 3.3486
                      return 2;
                    }
                  } else {  // if xyRatio > 0.34348
                    if (xyRatio <= 0.49031) {
                      return 2;
                    } else {  // if xyRatio > 0.49031
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avgShift > 15.401
              if (stdShift <= 6.5951) {
                if (avgErr <= 6.8257) {
                  return 0;
                } else {  // if avgErr > 6.8257
                  return 2;
                }
              } else {  // if stdShift > 6.5951
                return 0;
              }
            }
          } else {  // if xyRatio > 0.58267
            return 1;
          }
        }
      }
    }
  } else {  // if width > 67.873
    if (maxEdgeLength <= 208.46) {
      if (maxEdgeLength <= 124.54) {
        if (stdShiftX <= 0.095445) {
          if (width <= 107) {
            if (height <= 73.497) {
              if (stdShiftY <= 0.0063951) {
                if (stdShiftX <= 0.0077211) {
                  return 2;
                } else {  // if stdShiftX > 0.0077211
                  if (stdShiftY <= 0.0045906) {
                    return 2;
                  } else {  // if stdShiftY > 0.0045906
                    return 0;
                  }
                }
              } else {  // if stdShiftY > 0.0063951
                if (ncc <= 0.17305) {
                  if (stdShiftY <= 0.017453) {
                    if (ncc <= 0.14492) {
                      return 2;
                    } else {  // if ncc > 0.14492
                      return 0;
                    }
                  } else {  // if stdShiftY > 0.017453
                    if (stdShiftX <= 0.011739) {
                      return 0;
                    } else {  // if stdShiftX > 0.011739
                      return 2;
                    }
                  }
                } else {  // if ncc > 0.17305
                  return 2;
                }
              }
            } else {  // if height > 73.497
              if (height <= 74.119) {
                if (maxEdgeLength <= 84.117) {
                  if (avgShiftY <= 0.00032552) {
                    return 2;
                  } else {  // if avgShiftY > 0.00032552
                    if (avgShiftX <= 0.004244) {
                      return 0;
                    } else {  // if avgShiftX > 0.004244
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 84.117
                  if (avgShift <= 2.4539e-05) {
                    if (stdShiftY <= 0.0071579) {
                      return 0;
                    } else {  // if stdShiftY > 0.0071579
                      return 2;
                    }
                  } else {  // if avgShift > 2.4539e-05
                    return 0;
                  }
                }
              } else {  // if height > 74.119
                if (avgShift <= 0.0010975) {
                  if (stdShiftY <= 0.089592) {
                    if (stdShiftX <= 0.0069635) {
                      return 0;
                    } else {  // if stdShiftX > 0.0069635
                      return 2;
                    }
                  } else {  // if stdShiftY > 0.089592
                    return 1;
                  }
                } else {  // if avgShift > 0.0010975
                  if (ncc <= -0.062451) {
                    return 0;
                  } else {  // if ncc > -0.062451
                    if (width <= 82.023) {
                      return 2;
                    } else {  // if width > 82.023
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if width > 107
            if (xyRatio <= 1.4452) {
              return 0;
            } else {  // if xyRatio > 1.4452
              if (stdShift <= 0.0018715) {
                return 1;
              } else {  // if stdShift > 0.0018715
                return 2;
              }
            }
          }
        } else {  // if stdShiftX > 0.095445
          if (xyRatio <= 0.6642) {
            if (avgShiftX <= -1.4499) {
              if (xyRatio <= 0.6521) {
                if (avgShift <= 5.4712) {
                  if (height <= 123.35) {
                    if (avgShiftY <= -0.71432) {
                      return 0;
                    } else {  // if avgShiftY > -0.71432
                      return 1;
                    }
                  } else {  // if height > 123.35
                    return 0;
                  }
                } else {  // if avgShift > 5.4712
                  if (xyRatio <= 0.57637) {
                    if (stdShift <= 1.6201) {
                      return 2;
                    } else {  // if stdShift > 1.6201
                      return 1;
                    }
                  } else {  // if xyRatio > 0.57637
                    if (width <= 80.576) {
                      return 1;
                    } else {  // if width > 80.576
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > 0.6521
                if (width <= 87.586) {
                  return 2;
                } else {  // if width > 87.586
                  if (avgErr <= 5.9321) {
                    return 2;
                  } else {  // if avgErr > 5.9321
                    return 0;
                  }
                }
              }
            } else {  // if avgShiftX > -1.4499
              if (height <= 121.9) {
                if (width <= 69.535) {
                  if (width <= 68.374) {
                    if (avgShift <= 0.40221) {
                      return 2;
                    } else {  // if avgShift > 0.40221
                      return 2;
                    }
                  } else {  // if width > 68.374
                    if (avgErr <= 12.332) {
                      return 1;
                    } else {  // if avgErr > 12.332
                      return 2;
                    }
                  }
                } else {  // if width > 69.535
                  if (avgShiftX <= 0.049766) {
                    if (avgShiftX <= -0.15231) {
                      return 2;
                    } else {  // if avgShiftX > -0.15231
                      return 2;
                    }
                  } else {  // if avgShiftX > 0.049766
                    if (avgShiftX <= 1.0089) {
                      return 2;
                    } else {  // if avgShiftX > 1.0089
                      return 2;
                    }
                  }
                }
              } else {  // if height > 121.9
                if (avgShift <= 1.1749) {
                  if (ncc <= 0.91603) {
                    if (avgShiftY <= -0.14802) {
                      return 1;
                    } else {  // if avgShiftY > -0.14802
                      return 0;
                    }
                  } else {  // if ncc > 0.91603
                    if (stdShiftX <= 0.23486) {
                      return 1;
                    } else {  // if stdShiftX > 0.23486
                      return 2;
                    }
                  }
                } else {  // if avgShift > 1.1749
                  if (width <= 68.054) {
                    if (avgErr <= 9.1317) {
                      return 2;
                    } else {  // if avgErr > 9.1317
                      return 1;
                    }
                  } else {  // if width > 68.054
                    if (avgShiftY <= -2.8814) {
                      return 0;
                    } else {  // if avgShiftY > -2.8814
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.6642
            if (height <= 61.715) {
              if (width <= 119) {
                if (xyRatio <= 1.5461) {
                  if (avgShift <= 65.25) {
                    return 1;
                  } else {  // if avgShift > 65.25
                    return 0;
                  }
                } else {  // if xyRatio > 1.5461
                  if (avgErr <= 23.858) {
                    if (avgErr <= 4.0686) {
                      return 2;
                    } else {  // if avgErr > 4.0686
                      return 2;
                    }
                  } else {  // if avgErr > 23.858
                    return 0;
                  }
                }
              } else {  // if width > 119
                if (height <= 49.958) {
                  return 0;
                } else {  // if height > 49.958
                  return 2;
                }
              }
            } else {  // if height > 61.715
              if (xyRatio <= 1.6928) {
                if (height <= 70.75) {
                  if (xyRatio <= 1.3159) {
                    if (xyRatio <= 1.2452) {
                      return 2;
                    } else {  // if xyRatio > 1.2452
                      return 1;
                    }
                  } else {  // if xyRatio > 1.3159
                    if (stdShiftX <= 0.13082) {
                      return 0;
                    } else {  // if stdShiftX > 0.13082
                      return 2;
                    }
                  }
                } else {  // if height > 70.75
                  if (avgShift <= 1.3531) {
                    if (xyRatio <= 1.3788) {
                      return 2;
                    } else {  // if xyRatio > 1.3788
                      return 0;
                    }
                  } else {  // if avgShift > 1.3531
                    if (avgErr <= 9.4744) {
                      return 2;
                    } else {  // if avgErr > 9.4744
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 1.6928
                if (ncc <= 0.94092) {
                  if (avgErr <= 7.8302) {
                    if (avgShiftY <= -2.7933) {
                      return 1;
                    } else {  // if avgShiftY > -2.7933
                      return 0;
                    }
                  } else {  // if avgErr > 7.8302
                    return 2;
                  }
                } else {  // if ncc > 0.94092
                  if (avgErr <= 5.4151) {
                    if (ncc <= 0.98969) {
                      return 2;
                    } else {  // if ncc > 0.98969
                      return 0;
                    }
                  } else {  // if avgErr > 5.4151
                    return 2;
                  }
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 124.54
        if (height <= 177.5) {
          if (height <= 58.926) {
            if (height <= 47.785) {
              if (width <= 170) {
                if (avgErr <= 13.793) {
                  if (stdShift <= 0.21542) {
                    if (stdShiftX <= 0.0093498) {
                      return 2;
                    } else {  // if stdShiftX > 0.0093498
                      return 1;
                    }
                  } else {  // if stdShift > 0.21542
                    return 2;
                  }
                } else {  // if avgErr > 13.793
                  if (avgShiftX <= -3.3087) {
                    return 0;
                  } else {  // if avgShiftX > -3.3087
                    if (stdShiftY <= 0.37973) {
                      return 0;
                    } else {  // if stdShiftY > 0.37973
                      return 1;
                    }
                  }
                }
              } else {  // if width > 170
                if (xyRatio <= 5.2442) {
                  if (xyRatio <= 5.0907) {
                    if (stdShiftX <= 1.2701) {
                      return 2;
                    } else {  // if stdShiftX > 1.2701
                      return 1;
                    }
                  } else {  // if xyRatio > 5.0907
                    return 2;
                  }
                } else {  // if xyRatio > 5.2442
                  if (avgShift <= 1.2314) {
                    return 2;
                  } else {  // if avgShift > 1.2314
                    if (ncc <= 0.97221) {
                      return 1;
                    } else {  // if ncc > 0.97221
                      return 1;
                    }
                  }
                }
              }
            } else {  // if height > 47.785
              if (width <= 135) {
                return 1;
              } else {  // if width > 135
                if (avgShiftX <= -0.10659) {
                  if (height <= 57.663) {
                    if (stdShiftY <= 0.18371) {
                      return 1;
                    } else {  // if stdShiftY > 0.18371
                      return 2;
                    }
                  } else {  // if height > 57.663
                    return 0;
                  }
                } else {  // if avgShiftX > -0.10659
                  return 0;
                }
              }
            }
          } else {  // if height > 58.926
            if (avgShiftY <= -4.3943) {
              if (stdShiftY <= 1.0349) {
                if (avgShift <= 90.682) {
                  if (avgErr <= 6.8158) {
                    return 0;
                  } else {  // if avgErr > 6.8158
                    return 1;
                  }
                } else {  // if avgShift > 90.682
                  return 1;
                }
              } else {  // if stdShiftY > 1.0349
                if (width <= 87.165) {
                  if (avgShift <= 44.385) {
                    if (avgShiftX <= -0.65722) {
                      return 1;
                    } else {  // if avgShiftX > -0.65722
                      return 2;
                    }
                  } else {  // if avgShift > 44.385
                    return 0;
                  }
                } else {  // if width > 87.165
                  if (xyRatio <= 0.76908) {
                    return 0;
                  } else {  // if xyRatio > 0.76908
                    if (stdShiftX <= 1.7518) {
                      return 2;
                    } else {  // if stdShiftX > 1.7518
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgShiftY > -4.3943
              if (xyRatio <= 0.48301) {
                if (height <= 125.21) {
                  if (stdShiftY <= 1.2685) {
                    return 1;
                  } else {  // if stdShiftY > 1.2685
                    return 2;
                  }
                } else {  // if height > 125.21
                  if (avgShift <= 38.329) {
                    if (maxEdgeLength <= 154.91) {
                      return 2;
                    } else {  // if maxEdgeLength > 154.91
                      return 2;
                    }
                  } else {  // if avgShift > 38.329
                    if (avgErr <= 6.8449) {
                      return 2;
                    } else {  // if avgErr > 6.8449
                      return 1;
                    }
                  }
                }
              } else {  // if xyRatio > 0.48301
                if (height <= 148.84) {
                  if (maxEdgeLength <= 130.96) {
                    if (stdShift <= 0.054032) {
                      return 1;
                    } else {  // if stdShift > 0.054032
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 130.96
                    if (xyRatio <= 1.6183) {
                      return 2;
                    } else {  // if xyRatio > 1.6183
                      return 2;
                    }
                  }
                } else {  // if height > 148.84
                  if (avgErr <= 4.6242) {
                    if (avgErr <= 2.3325) {
                      return 1;
                    } else {  // if avgErr > 2.3325
                      return 2;
                    }
                  } else {  // if avgErr > 4.6242
                    if (xyRatio <= 0.61202) {
                      return 2;
                    } else {  // if xyRatio > 0.61202
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if height > 177.5
          if (origin <= 2.5) {
            if (xyRatio <= 0.46788) {
              if (avgShiftX <= -4.1221) {
                if (stdShiftY <= 0.73427) {
                  return 1;
                } else {  // if stdShiftY > 0.73427
                  if (stdShiftX <= 1.9938) {
                    if (avgShiftX <= -5.3503) {
                      return 2;
                    } else {  // if avgShiftX > -5.3503
                      return 1;
                    }
                  } else {  // if stdShiftX > 1.9938
                    if (stdShiftX <= 3.0292) {
                      return 1;
                    } else {  // if stdShiftX > 3.0292
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftX > -4.1221
                if (height <= 180.2) {
                  if (ncc <= 0.95906) {
                    return 1;
                  } else {  // if ncc > 0.95906
                    if (avgErr <= 9.3795) {
                      return 2;
                    } else {  // if avgErr > 9.3795
                      return 0;
                    }
                  }
                } else {  // if height > 180.2
                  if (xyRatio <= 0.39838) {
                    if (avgShift <= 4.1819) {
                      return 2;
                    } else {  // if avgShift > 4.1819
                      return 1;
                    }
                  } else {  // if xyRatio > 0.39838
                    if (avgShiftX <= -2.4249) {
                      return 2;
                    } else {  // if avgShiftX > -2.4249
                      return 2;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.46788
              if (stdShiftY <= 1.654) {
                if (width <= 101.04) {
                  if (avgShiftX <= -3.342) {
                    if (ncc <= 0.98385) {
                      return 1;
                    } else {  // if ncc > 0.98385
                      return 2;
                    }
                  } else {  // if avgShiftX > -3.342
                    return 2;
                  }
                } else {  // if width > 101.04
                  if (xyRatio <= 0.52483) {
                    if (stdShiftY <= 0.32442) {
                      return 2;
                    } else {  // if stdShiftY > 0.32442
                      return 1;
                    }
                  } else {  // if xyRatio > 0.52483
                    if (width <= 124.54) {
                      return 1;
                    } else {  // if width > 124.54
                      return 1;
                    }
                  }
                }
              } else {  // if stdShiftY > 1.654
                if (stdShiftY <= 2.0128) {
                  if (avgShiftX <= 3.481) {
                    if (maxEdgeLength <= 204.76) {
                      return 2;
                    } else {  // if maxEdgeLength > 204.76
                      return 1;
                    }
                  } else {  // if avgShiftX > 3.481
                    if (stdShiftX <= 2.0209) {
                      return 1;
                    } else {  // if stdShiftX > 2.0209
                      return 0;
                    }
                  }
                } else {  // if stdShiftY > 2.0128
                  if (ncc <= 0.88095) {
                    if (height <= 203.41) {
                      return 2;
                    } else {  // if height > 203.41
                      return 1;
                    }
                  } else {  // if ncc > 0.88095
                    return 1;
                  }
                }
              }
            }
          } else {  // if origin > 2.5
            if (xyRatio <= 0.6986) {
              if (height <= 199) {
                if (avgShiftY <= 0.47995) {
                  if (avgShiftX <= -1.4987) {
                    return 1;
                  } else {  // if avgShiftX > -1.4987
                    if (avgShiftX <= 1.3859) {
                      return 2;
                    } else {  // if avgShiftX > 1.3859
                      return 2;
                    }
                  }
                } else {  // if avgShiftY > 0.47995
                  if (height <= 183) {
                    if (stdShiftY <= 0.6541) {
                      return 2;
                    } else {  // if stdShiftY > 0.6541
                      return 1;
                    }
                  } else {  // if height > 183
                    if (ncc <= 0.79986) {
                      return 0;
                    } else {  // if ncc > 0.79986
                      return 2;
                    }
                  }
                }
              } else {  // if height > 199
                if (avgShiftY <= 0.27274) {
                  if (stdShiftX <= 0.40872) {
                    if (avgShiftY <= 0.13096) {
                      return 1;
                    } else {  // if avgShiftY > 0.13096
                      return 0;
                    }
                  } else {  // if stdShiftX > 0.40872
                    if (avgShiftX <= 0.91644) {
                      return 1;
                    } else {  // if avgShiftX > 0.91644
                      return 2;
                    }
                  }
                } else {  // if avgShiftY > 0.27274
                  if (stdShiftY <= 0.25839) {
                    return 1;
                  } else {  // if stdShiftY > 0.25839
                    if (avgErr <= 7.9157) {
                      return 2;
                    } else {  // if avgErr > 7.9157
                      return 0;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.6986
              if (ncc <= 0.93703) {
                if (stdShiftX <= 2.0649) {
                  if (avgShiftY <= 0.71809) {
                    if (height <= 189) {
                      return 1;
                    } else {  // if height > 189
                      return 2;
                    }
                  } else {  // if avgShiftY > 0.71809
                    if (ncc <= 0.92976) {
                      return 0;
                    } else {  // if ncc > 0.92976
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 2.0649
                  if (ncc <= 0.43116) {
                    return 2;
                  } else {  // if ncc > 0.43116
                    if (avgErr <= 11.51) {
                      return 1;
                    } else {  // if avgErr > 11.51
                      return 0;
                    }
                  }
                }
              } else {  // if ncc > 0.93703
                if (stdShiftY <= 0.74544) {
                  return 0;
                } else {  // if stdShiftY > 0.74544
                  if (avgShift <= 18.029) {
                    return 1;
                  } else {  // if avgShift > 18.029
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 208.46
      if (height <= 290.76) {
        if (avgShiftY <= -1.2603) {
          if (width <= 131.44) {
            if (avgErr <= 13.136) {
              if (avgErr <= 8.1141) {
                if (stdShiftY <= 0.73615) {
                  if (height <= 235.06) {
                    return 1;
                  } else {  // if height > 235.06
                    if (avgErr <= 6.3684) {
                      return 1;
                    } else {  // if avgErr > 6.3684
                      return 0;
                    }
                  }
                } else {  // if stdShiftY > 0.73615
                  return 1;
                }
              } else {  // if avgErr > 8.1141
                if (stdShiftX <= 0.72946) {
                  if (stdShift <= 2.2306) {
                    return 1;
                  } else {  // if stdShift > 2.2306
                    if (maxEdgeLength <= 209.06) {
                      return 2;
                    } else {  // if maxEdgeLength > 209.06
                      return 0;
                    }
                  }
                } else {  // if stdShiftX > 0.72946
                  if (avgShiftX <= 4.7246) {
                    if (stdShiftX <= 1.5876) {
                      return 1;
                    } else {  // if stdShiftX > 1.5876
                      return 0;
                    }
                  } else {  // if avgShiftX > 4.7246
                    return 0;
                  }
                }
              }
            } else {  // if avgErr > 13.136
              return 0;
            }
          } else {  // if width > 131.44
            if (stdShift <= 4.8478) {
              if (xyRatio <= 0.57279) {
                return 0;
              } else {  // if xyRatio > 0.57279
                if (ncc <= 0.95176) {
                  return 1;
                } else {  // if ncc > 0.95176
                  if (avgShiftY <= -4.6585) {
                    if (stdShiftX <= 1.1141) {
                      return 2;
                    } else {  // if stdShiftX > 1.1141
                      return 0;
                    }
                  } else {  // if avgShiftY > -4.6585
                    if (stdShiftX <= 1.6463) {
                      return 0;
                    } else {  // if stdShiftX > 1.6463
                      return 2;
                    }
                  }
                }
              }
            } else {  // if stdShift > 4.8478
              if (stdShiftY <= 4.5186) {
                if (stdShiftX <= 2.1576) {
                  if (stdShift <= 6.6209) {
                    if (avgErr <= 10.073) {
                      return 1;
                    } else {  // if avgErr > 10.073
                      return 0;
                    }
                  } else {  // if stdShift > 6.6209
                    if (stdShiftY <= 2.3304) {
                      return 0;
                    } else {  // if stdShiftY > 2.3304
                      return 0;
                    }
                  }
                } else {  // if stdShiftX > 2.1576
                  if (stdShiftX <= 2.8295) {
                    return 1;
                  } else {  // if stdShiftX > 2.8295
                    if (stdShiftX <= 3.0367) {
                      return 0;
                    } else {  // if stdShiftX > 3.0367
                      return 1;
                    }
                  }
                }
              } else {  // if stdShiftY > 4.5186
                return 0;
              }
            }
          }
        } else {  // if avgShiftY > -1.2603
          if (maxEdgeLength <= 235.22) {
            if (stdShiftY <= 0.38559) {
              if (avgShiftX <= 0.096894) {
                if (avgShift <= 0.0084127) {
                  return 1;
                } else {  // if avgShift > 0.0084127
                  if (avgShiftX <= 0.071292) {
                    if (stdShiftY <= 0.19604) {
                      return 2;
                    } else {  // if stdShiftY > 0.19604
                      return 2;
                    }
                  } else {  // if avgShiftX > 0.071292
                    if (stdShiftX <= 0.17757) {
                      return 2;
                    } else {  // if stdShiftX > 0.17757
                      return 0;
                    }
                  }
                }
              } else {  // if avgShiftX > 0.096894
                if (avgShiftX <= 4.783) {
                  if (width <= 96) {
                    return 2;
                  } else {  // if width > 96
                    if (avgShiftY <= 0.15247) {
                      return 1;
                    } else {  // if avgShiftY > 0.15247
                      return 0;
                    }
                  }
                } else {  // if avgShiftX > 4.783
                  if (avgErr <= 3.0188) {
                    return 1;
                  } else {  // if avgErr > 3.0188
                    return 2;
                  }
                }
              }
            } else {  // if stdShiftY > 0.38559
              if (height <= 213.36) {
                if (avgShiftY <= -0.20887) {
                  if (avgErr <= 4.1116) {
                    if (avgErr <= 3.269) {
                      return 2;
                    } else {  // if avgErr > 3.269
                      return 1;
                    }
                  } else {  // if avgErr > 4.1116
                    if (stdShift <= 16.319) {
                      return 2;
                    } else {  // if stdShift > 16.319
                      return 1;
                    }
                  }
                } else {  // if avgShiftY > -0.20887
                  if (avgShift <= 0.65578) {
                    return 0;
                  } else {  // if avgShift > 0.65578
                    if (avgShiftY <= 2.2355) {
                      return 1;
                    } else {  // if avgShiftY > 2.2355
                      return 0;
                    }
                  }
                }
              } else {  // if height > 213.36
                if (avgShiftY <= 0.57013) {
                  if (avgShiftY <= -1.2506) {
                    return 2;
                  } else {  // if avgShiftY > -1.2506
                    if (avgErr <= 4.9839) {
                      return 1;
                    } else {  // if avgErr > 4.9839
                      return 1;
                    }
                  }
                } else {  // if avgShiftY > 0.57013
                  if (avgErr <= 6.1814) {
                    if (avgShiftX <= -0.67702) {
                      return 1;
                    } else {  // if avgShiftX > -0.67702
                      return 2;
                    }
                  } else {  // if avgErr > 6.1814
                    if (stdShiftX <= 0.33007) {
                      return 0;
                    } else {  // if stdShiftX > 0.33007
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 235.22
            if (height <= 269) {
              if (ncc <= 0.2395) {
                if (avgShiftY <= -0.2628) {
                  return 0;
                } else {  // if avgShiftY > -0.2628
                  if (ncc <= 0.23507) {
                    if (ncc <= 0.14138) {
                      return 1;
                    } else {  // if ncc > 0.14138
                      return 1;
                    }
                  } else {  // if ncc > 0.23507
                    return 0;
                  }
                }
              } else {  // if ncc > 0.2395
                if (ncc <= 0.42027) {
                  if (ncc <= 0.40761) {
                    return 1;
                  } else {  // if ncc > 0.40761
                    return 0;
                  }
                } else {  // if ncc > 0.42027
                  if (stdShiftX <= 0.23806) {
                    if (stdShift <= 0.28681) {
                      return 1;
                    } else {  // if stdShift > 0.28681
                      return 0;
                    }
                  } else {  // if stdShiftX > 0.23806
                    return 1;
                  }
                }
              }
            } else {  // if height > 269
              if (stdShift <= 0.46555) {
                return 0;
              } else {  // if stdShift > 0.46555
                if (ncc <= 0.30641) {
                  return 1;
                } else {  // if ncc > 0.30641
                  if (ncc <= 0.33072) {
                    return 0;
                  } else {  // if ncc > 0.33072
                    if (stdShiftX <= 0.60028) {
                      return 1;
                    } else {  // if stdShiftX > 0.60028
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if height > 290.76
        if (height <= 377) {
          if (avgShiftY <= 0.62779) {
            if (avgShift <= 161.2) {
              if (stdShiftX <= 5.5714) {
                if (height <= 348.7) {
                  if (height <= 297.74) {
                    if (height <= 295.06) {
                      return 0;
                    } else {  // if height > 295.06
                      return 0;
                    }
                  } else {  // if height > 297.74
                    if (xyRatio <= 0.42977) {
                      return 0;
                    } else {  // if xyRatio > 0.42977
                      return 0;
                    }
                  }
                } else {  // if height > 348.7
                  if (avgShiftY <= -2.5992) {
                    return 1;
                  } else {  // if avgShiftY > -2.5992
                    return 0;
                  }
                }
              } else {  // if stdShiftX > 5.5714
                if (maxEdgeLength <= 323.6) {
                  return 1;
                } else {  // if maxEdgeLength > 323.6
                  return 0;
                }
              }
            } else {  // if avgShift > 161.2
              return 1;
            }
          } else {  // if avgShiftY > 0.62779
            if (avgShift <= 119.2) {
              if (width <= 154) {
                if (ncc <= 0.82163) {
                  return 1;
                } else {  // if ncc > 0.82163
                  return 0;
                }
              } else {  // if width > 154
                if (stdShiftX <= 0.22238) {
                  if (avgShiftX <= 0.26199) {
                    return 0;
                  } else {  // if avgShiftX > 0.26199
                    return 1;
                  }
                } else {  // if stdShiftX > 0.22238
                  if (avgErr <= 6.3513) {
                    if (avgShiftY <= 1.3071) {
                      return 1;
                    } else {  // if avgShiftY > 1.3071
                      return 0;
                    }
                  } else {  // if avgErr > 6.3513
                    if (stdShiftX <= 0.27514) {
                      return 1;
                    } else {  // if stdShiftX > 0.27514
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avgShift > 119.2
              return 0;
            }
          }
        } else {  // if height > 377
          return 0;
        }
      }
    }
  }
}

} // namespace rm
