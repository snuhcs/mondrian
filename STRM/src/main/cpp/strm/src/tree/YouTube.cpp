#include "strm/tree/YouTube.hpp"

namespace rm {

float YouTube(float width, float height, float type, float xyRatio, float avgShiftX,
              float avgShiftY, float avgShift, float stdShiftX, float stdShiftY, float stdShift,
              float avgErr, float ncc) {
  if (height <= -0.75183) {
    if (height <= -0.79972) {
      if (width <= 0.18203) {
        return 0;
      } else {  // if width > 0.18203
        if (width <= 1.0007) {
          if (width <= 0.49948) {
            return 1;
          } else {  // if width > 0.49948
            return 2;
          }
        } else {  // if width > 1.0007
          return 0;
        }
      }
    } else {  // if height > -0.79972
      if (avgShift <= -0.32724) {
        if (stdShiftY <= -0.5774) {
          return 1;
        } else {  // if stdShiftY > -0.5774
          if (ncc <= -0.30592) {
            return 0;
          } else {  // if ncc > -0.30592
            return 1;
          }
        }
      } else {  // if avgShift > -0.32724
        if (avgShiftX <= -0.20645) {
          return 0;
        } else {  // if avgShiftX > -0.20645
          if (avgShiftY <= 0.296) {
            if (stdShiftX <= -0.47265) {
              return 0;
            } else {  // if stdShiftX > -0.47265
              if (stdShift <= -0.29993) {
                if (avgShiftY <= 0.28559) {
                  if (height <= -0.76669) {
                    return 1;
                  } else {  // if height > -0.76669
                    if (avgShiftX <= 0.021916) {
                      return 1;
                    } else {  // if avgShiftX > 0.021916
                      return 0;
                    }
                  }
                } else {  // if avgShiftY > 0.28559
                  return 0;
                }
              } else {  // if stdShift > -0.29993
                if (stdShiftX <= 0.058402) {
                  if (avgShiftY <= 0.13493) {
                    return 1;
                  } else {  // if avgShiftY > 0.13493
                    return 0;
                  }
                } else {  // if stdShiftX > 0.058402
                  return 1;
                }
              }
            }
          } else {  // if avgShiftY > 0.296
            if (avgShift <= 0.02697) {
              return 1;
            } else {  // if avgShift > 0.02697
              return 0;
            }
          }
        }
      }
    }
  } else {  // if height > -0.75183
    if (height <= -0.66351) {
      if (width <= -0.60544) {
        if (avgShiftX <= 0.89433) {
          if (ncc <= -0.19856) {
            if (stdShift <= -0.31871) {
              return 1;
            } else {  // if stdShift > -0.31871
              return 0;
            }
          } else {  // if ncc > -0.19856
            return 1;
          }
        } else {  // if avgShiftX > 0.89433
          return 0;
        }
      } else {  // if width > -0.60544
        if (height <= -0.69325) {
          if (width <= 0.18203) {
            if (avgShiftX <= 0.015963) {
              if (stdShift <= -0.16222) {
                return 0;
              } else {  // if stdShift > -0.16222
                return 1;
              }
            } else {  // if avgShiftX > 0.015963
              if (stdShiftY <= -0.54968) {
                return 1;
              } else {  // if stdShiftY > -0.54968
                if (stdShiftX <= -0.4399) {
                  return 0;
                } else {  // if stdShiftX > -0.4399
                  if (avgShiftY <= -0.51624) {
                    if (avgErr <= 0.65059) {
                      return 0;
                    } else {  // if avgErr > 0.65059
                      return 1;
                    }
                  } else {  // if avgShiftY > -0.51624
                    return 1;
                  }
                }
              }
            }
          } else {  // if width > 0.18203
            return 2;
          }
        } else {  // if height > -0.69325
          return 1;
        }
      }
    } else {  // if height > -0.66351
      if (xyRatio <= 1.7104) {
        if (height <= -0.4523) {
          if (width <= -0.87378) {
            if (avgErr <= -0.54085) {
              return 1;
            } else {  // if avgErr > -0.54085
              return 2;
            }
          } else {  // if width > -0.87378
            if (width <= -0.68073) {
              if (avgShiftY <= 0.17112) {
                return 0;
              } else {  // if avgShiftY > 0.17112
                if (height <= -0.45322) {
                  if (xyRatio <= -0.85921) {
                    if (avgErr <= -1.2803) {
                      return 1;
                    } else {  // if avgErr > -1.2803
                      return 2;
                    }
                  } else {  // if xyRatio > -0.85921
                    if (ncc <= -1.0432) {
                      return 2;
                    } else {  // if ncc > -1.0432
                      return 2;
                    }
                  }
                } else {  // if height > -0.45322
                  if (avgErr <= -0.56768) {
                    return 1;
                  } else {  // if avgErr > -0.56768
                    if (avgErr <= -0.32015) {
                      return 2;
                    } else {  // if avgErr > -0.32015
                      return 1;
                    }
                  }
                }
              }
            } else {  // if width > -0.68073
              if (xyRatio <= -0.28745) {
                if (height <= -0.454) {
                  if (avgErr <= 0.5358) {
                    if (stdShiftY <= -0.41444) {
                      return 2;
                    } else {  // if stdShiftY > -0.41444
                      return 1;
                    }
                  } else {  // if avgErr > 0.5358
                    if (ncc <= 1.0764) {
                      return 0;
                    } else {  // if ncc > 1.0764
                      return 2;
                    }
                  }
                } else {  // if height > -0.454
                  if (stdShiftX <= -0.62103) {
                    return 1;
                  } else {  // if stdShiftX > -0.62103
                    return 2;
                  }
                }
              } else {  // if xyRatio > -0.28745
                if (avgShiftX <= 0.78222) {
                  if (avgShiftY <= 0.30894) {
                    if (stdShiftX <= -0.39159) {
                      return 1;
                    } else {  // if stdShiftX > -0.39159
                      return 2;
                    }
                  } else {  // if avgShiftY > 0.30894
                    if (stdShiftY <= -0.29143) {
                      return 2;
                    } else {  // if stdShiftY > -0.29143
                      return 2;
                    }
                  }
                } else {  // if avgShiftX > 0.78222
                  return 1;
                }
              }
            }
          }
        } else {  // if height > -0.4523
          if (width <= 2.2244) {
            if (height <= 0.039549) {
              if (avgShiftX <= -0.33474) {
                if (width <= -0.3924) {
                  if (avgShiftY <= 0.56266) {
                    if (avgErr <= 0.68107) {
                      return 1;
                    } else {  // if avgErr > 0.68107
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.56266
                    return 0;
                  }
                } else {  // if width > -0.3924
                  if (avgShiftX <= -0.51034) {
                    if (stdShift <= -0.063034) {
                      return 2;
                    } else {  // if stdShift > -0.063034
                      return 0;
                    }
                  } else {  // if avgShiftX > -0.51034
                    if (ncc <= 0.19803) {
                      return 2;
                    } else {  // if ncc > 0.19803
                      return 1;
                    }
                  }
                }
              } else {  // if avgShiftX > -0.33474
                if (height <= -0.27259) {
                  if (avgShiftY <= 0.4794) {
                    if (stdShift <= -0.22044) {
                      return 2;
                    } else {  // if stdShift > -0.22044
                      return 1;
                    }
                  } else {  // if avgShiftY > 0.4794
                    if (ncc <= 0.98689) {
                      return 0;
                    } else {  // if ncc > 0.98689
                      return 1;
                    }
                  }
                } else {  // if height > -0.27259
                  if (ncc <= -0.24331) {
                    if (stdShiftX <= 0.63821) {
                      return 0;
                    } else {  // if stdShiftX > 0.63821
                      return 1;
                    }
                  } else {  // if ncc > -0.24331
                    if (stdShift <= 0.075974) {
                      return 2;
                    } else {  // if stdShift > 0.075974
                      return 0;
                    }
                  }
                }
              }
            } else {  // if height > 0.039549
              if (avgShift <= -0.051763) {
                if (ncc <= -1.2848) {
                  if (avgShiftY <= 0.37385) {
                    if (avgErr <= -1.2459) {
                      return 2;
                    } else {  // if avgErr > -1.2459
                      return 2;
                    }
                  } else {  // if avgShiftY > 0.37385
                    if (stdShift <= -0.30046) {
                      return 0;
                    } else {  // if stdShift > -0.30046
                      return 2;
                    }
                  }
                } else {  // if ncc > -1.2848
                  if (stdShiftY <= -0.17071) {
                    if (height <= 0.86139) {
                      return 2;
                    } else {  // if height > 0.86139
                      return 2;
                    }
                  } else {  // if stdShiftY > -0.17071
                    if (avgShift <= -0.32417) {
                      return 1;
                    } else {  // if avgShift > -0.32417
                      return 2;
                    }
                  }
                }
              } else {  // if avgShift > -0.051763
                if (stdShift <= 0.67469) {
                  if (stdShift <= -0.10575) {
                    if (stdShiftX <= -0.089127) {
                      return 1;
                    } else {  // if stdShiftX > -0.089127
                      return 0;
                    }
                  } else {  // if stdShift > -0.10575
                    if (stdShift <= 0.058113) {
                      return 2;
                    } else {  // if stdShift > 0.058113
                      return 1;
                    }
                  }
                } else {  // if stdShift > 0.67469
                  if (avgShiftX <= -0.32304) {
                    if (ncc <= 0.92137) {
                      return 2;
                    } else {  // if ncc > 0.92137
                      return 0;
                    }
                  } else {  // if avgShiftX > -0.32304
                    if (avgShiftX <= -0.27581) {
                      return 1;
                    } else {  // if avgShiftX > -0.27581
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if width > 2.2244
            if (ncc <= 1.1934) {
              if (avgShiftX <= 0.015354) {
                if (stdShiftX <= 3.8164) {
                  if (ncc <= -0.082792) {
                    if (avgShiftY <= -1.0199) {
                      return 0;
                    } else {  // if avgShiftY > -1.0199
                      return 0;
                    }
                  } else {  // if ncc > -0.082792
                    if (stdShiftX <= 3.3283) {
                      return 2;
                    } else {  // if stdShiftX > 3.3283
                      return 1;
                    }
                  }
                } else {  // if stdShiftX > 3.8164
                  if (stdShift <= 4.8501) {
                    if (avgErr <= 0.16395) {
                      return 2;
                    } else {  // if avgErr > 0.16395
                      return 1;
                    }
                  } else {  // if stdShift > 4.8501
                    if (avgShift <= 1.9442) {
                      return 2;
                    } else {  // if avgShift > 1.9442
                      return 0;
                    }
                  }
                }
              } else {  // if avgShiftX > 0.015354
                if (stdShiftX <= 2.8008) {
                  if (stdShiftX <= 2.1954) {
                    if (ncc <= 1.0739) {
                      return 1;
                    } else {  // if ncc > 1.0739
                      return 1;
                    }
                  } else {  // if stdShiftX > 2.1954
                    if (stdShiftX <= 2.427) {
                      return 0;
                    } else {  // if stdShiftX > 2.427
                      return 1;
                    }
                  }
                } else {  // if stdShiftX > 2.8008
                  return 1;
                }
              }
            } else {  // if ncc > 1.1934
              if (avgShiftX <= 3.7106) {
                return 0;
              } else {  // if avgShiftX > 3.7106
                if (avgShift <= 3.9567) {
                  return 1;
                } else {  // if avgShift > 3.9567
                  return 2;
                }
              }
            }
          }
        }
      } else {  // if xyRatio > 1.7104
        if (xyRatio <= 1.8777) {
          if (avgShiftY <= 0.35312) {
            if (stdShiftY <= 3.2037) {
              if (avgShiftX <= 0.088041) {
                if (ncc <= 0.26619) {
                  return 0;
                } else {  // if ncc > 0.26619
                  if (stdShiftY <= -0.52339) {
                    return 0;
                  } else {  // if stdShiftY > -0.52339
                    return 1;
                  }
                }
              } else {  // if avgShiftX > 0.088041
                return 1;
              }
            } else {  // if stdShiftY > 3.2037
              return 1;
            }
          } else {  // if avgShiftY > 0.35312
            return 1;
          }
        } else {  // if xyRatio > 1.8777
          if (ncc <= 1.2021) {
            if (stdShiftY <= -0.52502) {
              if (avgErr <= -1.1689) {
                if (ncc <= -1.1519) {
                  return 1;
                } else {  // if ncc > -1.1519
                  if (avgErr <= -1.2335) {
                    if (avgShiftX <= 0.009884) {
                      return 1;
                    } else {  // if avgShiftX > 0.009884
                      return 0;
                    }
                  } else {  // if avgErr > -1.2335
                    return 0;
                  }
                }
              } else {  // if avgErr > -1.1689
                return 1;
              }
            } else {  // if stdShiftY > -0.52502
              if (avgShiftY <= -4.5825) {
                return 1;
              } else {  // if avgShiftY > -4.5825
                if (stdShiftX <= 1.6353) {
                  return 0;
                } else {  // if stdShiftX > 1.6353
                  return 1;
                }
              }
            }
          } else {  // if ncc > 1.2021
            return 1;
          }
        }
      }
    }
  }
}

} // namespace rm
