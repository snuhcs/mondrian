#include "strm/tree/YouTube.hpp"

namespace rm {

float
YouTube(float width, float height, float maxEdgeLength, float type, float origin, float xyRatio,
        float avgShiftX, float avgShiftY, float avgShift, float stdShiftX, float stdShiftY,
        float stdShift, float avgErr, float ncc) {
  if (maxEdgeLength <= 115.75) {
    if (maxEdgeLength <= 111.01) {
      return 0;
    } else {  // if maxEdgeLength > 111.01
      if (avgShift <= 0.042149) {
        if (ncc <= 0.29423) {
          if (width <= 64.049) {
            return 0;
          } else {  // if width > 64.049
            return 1;
          }
        } else {  // if ncc > 0.29423
          return 1;
        }
      } else {  // if avgShift > 0.042149
        if (avgShiftX <= -0.50927) {
          return 0;
        } else {  // if avgShiftX > -0.50927
          if (avgShiftY <= -0.16271) {
            if (stdShiftX <= 0.16022) {
              return 0;
            } else {  // if stdShiftX > 0.16022
              if (stdShift <= 0.31919) {
                if (avgShiftY <= -0.18621) {
                  if (stdShiftX <= 0.39304) {
                    return 1;
                  } else {  // if stdShiftX > 0.39304
                    if (stdShift <= 0.17986) {
                      return 0;
                    } else {  // if stdShift > 0.17986
                      return 1;
                    }
                  }
                } else {  // if avgShiftY > -0.18621
                  return 0;
                }
              } else {  // if stdShift > 0.31919
                if (stdShift <= 0.76695) {
                  if (avgShiftY <= -0.52639) {
                    return 1;
                  } else {  // if avgShiftY > -0.52639
                    return 0;
                  }
                } else {  // if stdShift > 0.76695
                  return 1;
                }
              }
            }
          } else {  // if avgShiftY > -0.16271
            if (avgShift <= 11.958) {
              return 1;
            } else {  // if avgShift > 11.958
              return 0;
            }
          }
        }
      }
    }
  } else {  // if maxEdgeLength > 115.75
    if (maxEdgeLength <= 124.49) {
      if (avgShiftX <= -0.87948) {
        if (stdShiftX <= 1.235) {
          return 0;
        } else {  // if stdShiftX > 1.235
          return 1;
        }
      } else {  // if avgShiftX > -0.87948
        if (avgShiftY <= -1.9315) {
          if (avgShiftY <= -2.2516) {
            return 1;
          } else {  // if avgShiftY > -2.2516
            return 0;
          }
        } else {  // if avgShiftY > -1.9315
          if (avgShiftY <= 0.037053) {
            if (avgShiftY <= 0.032429) {
              if (avgErr <= 4.3584) {
                if (avgErr <= 0.028766) {
                  return 0;
                } else {  // if avgErr > 0.028766
                  if (width <= 84.138) {
                    return 1;
                  } else {  // if width > 84.138
                    if (width <= 105) {
                      return 0;
                    } else {  // if width > 105
                      return 1;
                    }
                  }
                }
              } else {  // if avgErr > 4.3584
                if (avgShiftY <= -0.14575) {
                  if (stdShiftX <= 0.18621) {
                    if (stdShift <= 0.073346) {
                      return 1;
                    } else {  // if stdShift > 0.073346
                      return 0;
                    }
                  } else {  // if stdShiftX > 0.18621
                    if (stdShiftY <= 0.57242) {
                      return 1;
                    } else {  // if stdShiftY > 0.57242
                      return 0;
                    }
                  }
                } else {  // if avgShiftY > -0.14575
                  return 0;
                }
              }
            } else {  // if avgShiftY > 0.032429
              return 0;
            }
          } else {  // if avgShiftY > 0.037053
            return 1;
          }
        }
      }
    } else {  // if maxEdgeLength > 124.49
      if (xyRatio <= 0.87368) {
        if (maxEdgeLength <= 145.38) {
          if (width <= 48.806) {
            if (avgErr <= 2.9524) {
              return 1;
            } else {  // if avgErr > 2.9524
              return 2;
            }
          } else {  // if width > 48.806
            if (width <= 60.361) {
              if (avgShiftY <= -0.44466) {
                return 0;
              } else {  // if avgShiftY > -0.44466
                if (height <= 145.29) {
                  if (xyRatio <= 0.28777) {
                    if (avgErr <= 0.08185) {
                      return 1;
                    } else {  // if avgErr > 0.08185
                      return 2;
                    }
                  } else {  // if xyRatio > 0.28777
                    if (ncc <= 0.12331) {
                      return 2;
                    } else {  // if ncc > 0.12331
                      return 2;
                    }
                  }
                } else {  // if height > 145.29
                  if (avgErr <= 2.8483) {
                    return 1;
                  } else {  // if avgErr > 2.8483
                    if (avgErr <= 3.8092) {
                      return 2;
                    } else {  // if avgErr > 3.8092
                      return 1;
                    }
                  }
                }
              }
            } else {  // if width > 60.361
              if (xyRatio <= 0.41814) {
                if (maxEdgeLength <= 145.21) {
                  if (avgErr <= 7.1321) {
                    if (stdShiftY <= 0.21556) {
                      return 2;
                    } else {  // if stdShiftY > 0.21556
                      return 1;
                    }
                  } else {  // if avgErr > 7.1321
                    if (ncc <= 0.91406) {
                      return 0;
                    } else {  // if ncc > 0.91406
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 145.21
                  if (stdShift <= 0.00054217) {
                    return 1;
                  } else {  // if stdShift > 0.00054217
                    return 2;
                  }
                }
              } else {  // if xyRatio > 0.41814
                if (stdShiftX <= 1.3122) {
                  if (avgShiftY <= -0.13348) {
                    if (avgErr <= 6.0418) {
                      return 1;
                    } else {  // if avgErr > 6.0418
                      return 2;
                    }
                  } else {  // if avgShiftY > -0.13348
                    if (stdShiftY <= 0.37763) {
                      return 2;
                    } else {  // if stdShiftY > 0.37763
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 1.3122
                  return 1;
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 145.38
          if (width <= 234.24) {
            if (height <= 194.04) {
              if (avgShiftX <= -0.80351) {
                if (width <= 77.618) {
                  if (avgShiftY <= 0.43939) {
                    if (avgErr <= 7.696) {
                      return 1;
                    } else {  // if avgErr > 7.696
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.43939
                    return 0;
                  }
                } else {  // if width > 77.618
                  if (avgShiftX <= -1.2063) {
                    if (stdShift <= 3.0432) {
                      return 2;
                    } else {  // if stdShift > 3.0432
                      return 0;
                    }
                  } else {  // if avgShiftX > -1.2063
                    if (ncc <= 0.58638) {
                      return 2;
                    } else {  // if ncc > 0.58638
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftX > -0.80351
                if (maxEdgeLength <= 163.16) {
                  if (avgShiftY <= 0.25139) {
                    if (stdShift <= 1.2333) {
                      return 2;
                    } else {  // if stdShift > 1.2333
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.25139
                    if (ncc <= 0.88068) {
                      return 0;
                    } else {  // if ncc > 0.88068
                      return 1;
                    }
                  }
                } else {  // if maxEdgeLength > 163.16
                  if (ncc <= 0.42173) {
                    if (stdShiftX <= 1.2929) {
                      return 0;
                    } else {  // if stdShiftX > 1.2929
                      return 1;
                    }
                  } else {  // if ncc > 0.42173
                    if (stdShift <= 4.6417) {
                      return 2;
                    } else {  // if stdShift > 4.6417
                      return 0;
                    }
                  }
                }
              }
            } else {  // if height > 194.04
              if (avgShift <= 9.3093) {
                if (origin <= 2.5) {
                  if (ncc <= 0.033162) {
                    if (avgShiftY <= 0.013076) {
                      return 2;
                    } else {  // if avgShiftY > 0.013076
                      return 0;
                    }
                  } else {  // if ncc > 0.033162
                    if (stdShiftY <= 0.53669) {
                      return 2;
                    } else {  // if stdShiftY > 0.53669
                      return 2;
                    }
                  }
                } else {  // if origin > 2.5
                  if (avgShiftY <= 0.26651) {
                    if (avgErr <= 3.841) {
                      return 2;
                    } else {  // if avgErr > 3.841
                      return 0;
                    }
                  } else {  // if avgShiftY > 0.26651
                    if (stdShiftY <= 0.87567) {
                      return 2;
                    } else {  // if stdShiftY > 0.87567
                      return 1;
                    }
                  }
                }
              } else {  // if avgShift > 9.3093
                if (stdShift <= 11.526) {
                  if (stdShift <= 2.5521) {
                    if (stdShiftX <= 0.55128) {
                      return 1;
                    } else {  // if stdShiftX > 0.55128
                      return 0;
                    }
                  } else {  // if stdShift > 2.5521
                    if (stdShift <= 4.4363) {
                      return 2;
                    } else {  // if stdShift > 4.4363
                      return 1;
                    }
                  }
                } else {  // if stdShift > 11.526
                  if (avgShiftX <= -0.77668) {
                    if (ncc <= 0.85624) {
                      return 2;
                    } else {  // if ncc > 0.85624
                      return 0;
                    }
                  } else {  // if avgShiftX > -0.77668
                    if (avgShiftX <= -0.66834) {
                      return 1;
                    } else {  // if avgShiftX > -0.66834
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if width > 234.24
            if (ncc <= 0.95774) {
              if (avgShiftX <= -0.00053414) {
                if (stdShiftX <= 4.5336) {
                  if (ncc <= 0.48161) {
                    if (avgShiftY <= -3.1338) {
                      return 0;
                    } else {  // if avgShiftY > -3.1338
                      return 0;
                    }
                  } else {  // if ncc > 0.48161
                    if (avgShift <= 26.379) {
                      return 0;
                    } else {  // if avgShift > 26.379
                      return 2;
                    }
                  }
                } else {  // if stdShiftX > 4.5336
                  if (stdShift <= 59.539) {
                    if (avgShiftY <= -2.5732) {
                      return 1;
                    } else {  // if avgShiftY > -2.5732
                      return 2;
                    }
                  } else {  // if stdShift > 59.539
                    if (avgShiftY <= -4.9919) {
                      return 1;
                    } else {  // if avgShiftY > -4.9919
                      return 2;
                    }
                  }
                }
              } else {  // if avgShiftX > -0.00053414
                if (stdShiftX <= 3.498) {
                  if (stdShiftX <= 2.8808) {
                    if (ncc <= 0.91314) {
                      return 1;
                    } else {  // if ncc > 0.91314
                      return 1;
                    }
                  } else {  // if stdShiftX > 2.8808
                    if (stdShiftX <= 3.1168) {
                      return 0;
                    } else {  // if stdShiftX > 3.1168
                      return 1;
                    }
                  }
                } else {  // if stdShiftX > 3.498
                  return 1;
                }
              }
            } else {  // if ncc > 0.95774
              if (avgShiftX <= 8.4749) {
                return 0;
              } else {  // if avgShiftX > 8.4749
                if (stdShift <= 6.7217) {
                  return 1;
                } else {  // if stdShift > 6.7217
                  return 2;
                }
              }
            }
          }
        }
      } else {  // if xyRatio > 0.87368
        if (xyRatio <= 0.92308) {
          if (avgShiftY <= 0.058943) {
            if (ncc <= 0.08275) {
              return 1;
            } else {  // if ncc > 0.08275
              if (avgShiftX <= 0.010789) {
                if (avgShiftX <= -0.036791) {
                  if (ncc <= 0.099107) {
                    return 1;
                  } else {  // if ncc > 0.099107
                    if (avgShiftY <= 0.042091) {
                      return 0;
                    } else {  // if avgShiftY > 0.042091
                      return 1;
                    }
                  }
                } else {  // if avgShiftX > -0.036791
                  if (avgShiftY <= 0.02048) {
                    return 0;
                  } else {  // if avgShiftY > 0.02048
                    if (stdShift <= 0.0032355) {
                      return 1;
                    } else {  // if stdShift > 0.0032355
                      return 0;
                    }
                  }
                }
              } else {  // if avgShiftX > 0.010789
                if (stdShift <= 12.512) {
                  if (avgShift <= 0.015791) {
                    return 1;
                  } else {  // if avgShift > 0.015791
                    if (avgShift <= 0.022441) {
                      return 0;
                    } else {  // if avgShift > 0.022441
                      return 1;
                    }
                  }
                } else {  // if stdShift > 12.512
                  if (stdShiftX <= 2.2556) {
                    return 0;
                  } else {  // if stdShiftX > 2.2556
                    return 1;
                  }
                }
              }
            }
          } else {  // if avgShiftY > 0.058943
            return 0;
          }
        } else {  // if xyRatio > 0.92308
          if (width <= 152.39) {
            return 2;
          } else {  // if width > 152.39
            return 0;
          }
        }
      }
    }
  }
}

} // namespace rm
