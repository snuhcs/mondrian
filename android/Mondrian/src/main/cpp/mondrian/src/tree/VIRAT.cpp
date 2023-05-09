#include "mondrian/tree/VIRAT.hpp"

namespace md {

int VIRAT(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
          float shiftNcc, float avgErr) {
  if (area <= 17874) {
    if (area <= 14762) {
      if (area <= 8467) {
        if (maxEdgeLength <= 97.5) {
          if (area <= 6845.5) {
            if (shiftStd <= 1.7619) {
              if (xyRatio <= 1.1133) {
                if (avgErr <= 7.9081) {
                  return 4;
                } else {  // if avgErr > 7.9081
                  return 1;
                }
              } else {  // if xyRatio > 1.1133
                if (area <= 3571.5) {
                  return 0;
                } else {  // if area > 3571.5
                  return 1;
                }
              }
            } else {  // if shiftStd > 1.7619
              return 4;
            }
          } else {  // if area > 6845.5
            if (area <= 8078) {
              return 2;
            } else {  // if area > 8078
              return 3;
            }
          }
        } else {  // if maxEdgeLength > 97.5
          if (area <= 8133) {
            if (xyRatio <= 2.2453) {
              if (xyRatio <= 1.2012) {
                return 1;
              } else {  // if xyRatio > 1.2012
                if (xyRatio <= 1.3355) {
                  if (maxEdgeLength <= 102.5) {
                    if (avgErr <= 1.4261) {
                      return 0;
                    } else {  // if avgErr > 1.4261
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 102.5
                    return 1;
                  }
                } else {  // if xyRatio > 1.3355
                  if (avgErr <= 5.6593) {
                    if (area <= 7690) {
                      return 0;
                    } else {  // if area > 7690
                      return 0;
                    }
                  } else {  // if avgErr > 5.6593
                    if (shiftStd <= 0.13416) {
                      return 2;
                    } else {  // if shiftStd > 0.13416
                      return 0;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 2.2453
              return 4;
            }
          } else {  // if area > 8133
            if (shiftStd <= 0.13053) {
              if (maxEdgeLength <= 99.5) {
                if (shiftStd <= 0.015939) {
                  if (shiftStd <= 0.0042741) {
                    return 1;
                  } else {  // if shiftStd > 0.0042741
                    return 3;
                  }
                } else {  // if shiftStd > 0.015939
                  return 1;
                }
              } else {  // if maxEdgeLength > 99.5
                if (maxEdgeLength <= 107.5) {
                  if (shiftNcc <= 0.010014) {
                    if (area <= 8353.5) {
                      return 1;
                    } else {  // if area > 8353.5
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.010014
                    if (shiftStd <= 0.073762) {
                      return 0;
                    } else {  // if shiftStd > 0.073762
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 107.5
                  if (xyRatio <= 1.4569) {
                    if (area <= 8376.5) {
                      return 1;
                    } else {  // if area > 8376.5
                      return 1;
                    }
                  } else {  // if xyRatio > 1.4569
                    if (shiftStd <= 0.023772) {
                      return 0;
                    } else {  // if shiftStd > 0.023772
                      return 0;
                    }
                  }
                }
              }
            } else {  // if shiftStd > 0.13053
              if (avgErr <= 7.259) {
                if (maxEdgeLength <= 105) {
                  return 4;
                } else {  // if maxEdgeLength > 105
                  return 1;
                }
              } else {  // if avgErr > 7.259
                if (shiftAvg <= 4.8281) {
                  return 0;
                } else {  // if shiftAvg > 4.8281
                  return 4;
                }
              }
            }
          }
        }
      } else {  // if area > 8467
        if (xyRatio <= 2.3154) {
          if (area <= 14528) {
            if (maxEdgeLength <= 124.5) {
              if (area <= 8624.5) {
                if (area <= 8619) {
                  if (shiftStd <= 0.060795) {
                    if (shiftAvg <= 0.028754) {
                      return 1;
                    } else {  // if shiftAvg > 0.028754
                      return 3;
                    }
                  } else {  // if shiftStd > 0.060795
                    if (area <= 8576.5) {
                      return 0;
                    } else {  // if area > 8576.5
                      return 1;
                    }
                  }
                } else {  // if area > 8619
                  return 0;
                }
              } else {  // if area > 8624.5
                if (area <= 10110) {
                  if (xyRatio <= 1.0591) {
                    if (shiftAvg <= 0.0038564) {
                      return 4;
                    } else {  // if shiftAvg > 0.0038564
                      return 3;
                    }
                  } else {  // if xyRatio > 1.0591
                    if (avgErr <= 3.1727) {
                      return 1;
                    } else {  // if avgErr > 3.1727
                      return 1;
                    }
                  }
                } else {  // if area > 10110
                  if (xyRatio <= 1.2674) {
                    if (xyRatio <= 1.2514) {
                      return 1;
                    } else {  // if xyRatio > 1.2514
                      return 4;
                    }
                  } else {  // if xyRatio > 1.2674
                    if (area <= 10164) {
                      return 4;
                    } else {  // if area > 10164
                      return 1;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 124.5
              if (shiftStd <= 59.075) {
                if (shiftStd <= 0.001337) {
                  if (shiftAvg <= 0.027239) {
                    return 1;
                  } else {  // if shiftAvg > 0.027239
                    return 4;
                  }
                } else {  // if shiftStd > 0.001337
                  if (shiftNcc <= 0.99894) {
                    if (shiftNcc <= 0.42713) {
                      return 1;
                    } else {  // if shiftNcc > 0.42713
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.99894
                    if (shiftAvg <= 31.76) {
                      return 4;
                    } else {  // if shiftAvg > 31.76
                      return 1;
                    }
                  }
                }
              } else {  // if shiftStd > 59.075
                return 4;
              }
            }
          } else {  // if area > 14528
            if (area <= 14540) {
              return 2;
            } else {  // if area > 14540
              if (area <= 14612) {
                if (xyRatio <= 1.3748) {
                  return 2;
                } else {  // if xyRatio > 1.3748
                  return 1;
                }
              } else {  // if area > 14612
                if (area <= 14697) {
                  if (maxEdgeLength <= 152) {
                    if (shiftNcc <= 0.43683) {
                      return 2;
                    } else {  // if shiftNcc > 0.43683
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 152
                    if (maxEdgeLength <= 170) {
                      return 2;
                    } else {  // if maxEdgeLength > 170
                      return 4;
                    }
                  }
                } else {  // if area > 14697
                  if (avgErr <= 1.0784) {
                    return 4;
                  } else {  // if avgErr > 1.0784
                    if (avgErr <= 10.417) {
                      return 1;
                    } else {  // if avgErr > 10.417
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > 2.3154
          if (shiftNcc <= 0.55479) {
            return 4;
          } else {  // if shiftNcc > 0.55479
            if (maxEdgeLength <= 170) {
              if (avgErr <= 6.4711) {
                return 1;
              } else {  // if avgErr > 6.4711
                return 4;
              }
            } else {  // if maxEdgeLength > 170
              if (shiftAvg <= 13.216) {
                return 1;
              } else {  // if shiftAvg > 13.216
                return 0;
              }
            }
          }
        }
      }
    } else {  // if area > 14762
      if (area <= 17590) {
        if (area <= 15144) {
          if (xyRatio <= 1.4604) {
            if (xyRatio <= 1.4244) {
              if (shiftNcc <= 0.312) {
                return 1;
              } else {  // if shiftNcc > 0.312
                return 2;
              }
            } else {  // if xyRatio > 1.4244
              return 1;
            }
          } else {  // if xyRatio > 1.4604
            if (shiftNcc <= 0.61236) {
              if (avgErr <= 10.682) {
                if (shiftStd <= 4.1831) {
                  if (area <= 14777) {
                    return 4;
                  } else {  // if area > 14777
                    if (shiftNcc <= -0.061241) {
                      return 1;
                    } else {  // if shiftNcc > -0.061241
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 4.1831
                  if (avgErr <= 7.645) {
                    return 4;
                  } else {  // if avgErr > 7.645
                    return 2;
                  }
                }
              } else {  // if avgErr > 10.682
                return 1;
              }
            } else {  // if shiftNcc > 0.61236
              return 2;
            }
          }
        } else {  // if area > 15144
          if (shiftNcc <= 0.92545) {
            if (area <= 17536) {
              if (area <= 15962) {
                if (area <= 15948) {
                  if (avgErr <= 0.28038) {
                    if (avgErr <= 0.27573) {
                      return 2;
                    } else {  // if avgErr > 0.27573
                      return 4;
                    }
                  } else {  // if avgErr > 0.28038
                    if (xyRatio <= 1.3981) {
                      return 2;
                    } else {  // if xyRatio > 1.3981
                      return 2;
                    }
                  }
                } else {  // if area > 15948
                  if (shiftNcc <= 0.18897) {
                    if (shiftStd <= 0.62791) {
                      return 2;
                    } else {  // if shiftStd > 0.62791
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.18897
                    return 2;
                  }
                }
              } else {  // if area > 15962
                if (maxEdgeLength <= 145.5) {
                  if (shiftStd <= 0.85181) {
                    if (shiftNcc <= 0.42867) {
                      return 2;
                    } else {  // if shiftNcc > 0.42867
                      return 4;
                    }
                  } else {  // if shiftStd > 0.85181
                    return 2;
                  }
                } else {  // if maxEdgeLength > 145.5
                  if (avgErr <= 7.1977) {
                    return 2;
                  } else {  // if avgErr > 7.1977
                    if (shiftAvg <= 0.080848) {
                      return 4;
                    } else {  // if shiftAvg > 0.080848
                      return 2;
                    }
                  }
                }
              }
            } else {  // if area > 17536
              if (area <= 17540) {
                return 3;
              } else {  // if area > 17540
                if (xyRatio <= 1.8861) {
                  return 2;
                } else {  // if xyRatio > 1.8861
                  if (maxEdgeLength <= 184) {
                    return 3;
                  } else {  // if maxEdgeLength > 184
                    return 2;
                  }
                }
              }
            }
          } else {  // if shiftNcc > 0.92545
            if (shiftNcc <= 0.93184) {
              return 4;
            } else {  // if shiftNcc > 0.93184
              if (area <= 17520) {
                if (xyRatio <= 0.76616) {
                  if (maxEdgeLength <= 153) {
                    return 4;
                  } else {  // if maxEdgeLength > 153
                    return 2;
                  }
                } else {  // if xyRatio > 0.76616
                  if (xyRatio <= 1.9497) {
                    if (shiftNcc <= 0.9357) {
                      return 2;
                    } else {  // if shiftNcc > 0.9357
                      return 2;
                    }
                  } else {  // if xyRatio > 1.9497
                    if (shiftStd <= 0.011102) {
                      return 2;
                    } else {  // if shiftStd > 0.011102
                      return 4;
                    }
                  }
                }
              } else {  // if area > 17520
                return 4;
              }
            }
          }
        }
      } else {  // if area > 17590
        if (area <= 17820) {
          if (area <= 17792) {
            if (maxEdgeLength <= 164.5) {
              if (xyRatio <= 1.4819) {
                if (xyRatio <= 1.0388) {
                  return 4;
                } else {  // if xyRatio > 1.0388
                  if (shiftNcc <= 0.99206) {
                    if (area <= 17681) {
                      return 2;
                    } else {  // if area > 17681
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.99206
                    return 3;
                  }
                }
              } else {  // if xyRatio > 1.4819
                return 3;
              }
            } else {  // if maxEdgeLength > 164.5
              if (maxEdgeLength <= 187.5) {
                if (area <= 17650) {
                  if (area <= 17618) {
                    return 2;
                  } else {  // if area > 17618
                    if (xyRatio <= 1.8174) {
                      return 3;
                    } else {  // if xyRatio > 1.8174
                      return 2;
                    }
                  }
                } else {  // if area > 17650
                  if (area <= 17742) {
                    return 2;
                  } else {  // if area > 17742
                    if (area <= 17756) {
                      return 3;
                    } else {  // if area > 17756
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 187.5
                return 3;
              }
            }
          } else {  // if area > 17792
            return 3;
          }
        } else {  // if area > 17820
          if (xyRatio <= 1.8575) {
            if (shiftAvg <= 2.3121) {
              return 2;
            } else {  // if shiftAvg > 2.3121
              return 3;
            }
          } else {  // if xyRatio > 1.8575
            return 3;
          }
        }
      }
    }
  } else {  // if area > 17874
    if (area <= 20276) {
      if (maxEdgeLength <= 194.5) {
        if (area <= 18246) {
          if (xyRatio <= 1.7574) {
            if (xyRatio <= 1.7136) {
              if (xyRatio <= 0.98008) {
                return 4;
              } else {  // if xyRatio > 0.98008
                if (maxEdgeLength <= 172.5) {
                  if (maxEdgeLength <= 170.5) {
                    if (maxEdgeLength <= 167.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 167.5
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 170.5
                    return 2;
                  }
                } else {  // if maxEdgeLength > 172.5
                  if (shiftStd <= 2.3856) {
                    return 3;
                  } else {  // if shiftStd > 2.3856
                    if (shiftStd <= 8.9852) {
                      return 4;
                    } else {  // if shiftStd > 8.9852
                      return 3;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 1.7136
              if (shiftNcc <= 0.60212) {
                return 2;
              } else {  // if shiftNcc > 0.60212
                if (shiftAvg <= 0.88021) {
                  return 4;
                } else {  // if shiftAvg > 0.88021
                  return 2;
                }
              }
            }
          } else {  // if xyRatio > 1.7574
            if (shiftNcc <= 0.69807) {
              if (xyRatio <= 1.81) {
                return 3;
              } else {  // if xyRatio > 1.81
                if (xyRatio <= 1.8434) {
                  return 2;
                } else {  // if xyRatio > 1.8434
                  if (area <= 17993) {
                    if (xyRatio <= 1.9076) {
                      return 3;
                    } else {  // if xyRatio > 1.9076
                      return 2;
                    }
                  } else {  // if area > 17993
                    return 3;
                  }
                }
              }
            } else {  // if shiftNcc > 0.69807
              return 2;
            }
          }
        } else {  // if area > 18246
          if (area <= 19892) {
            if (xyRatio <= 0.83352) {
              if (area <= 19616) {
                if (shiftNcc <= 0.98867) {
                  return 4;
                } else {  // if shiftNcc > 0.98867
                  if (avgErr <= 3.5714) {
                    return 4;
                  } else {  // if avgErr > 3.5714
                    if (area <= 18382) {
                      return 4;
                    } else {  // if area > 18382
                      return 3;
                    }
                  }
                }
              } else {  // if area > 19616
                return 4;
              }
            } else {  // if xyRatio > 0.83352
              if (avgErr <= 18.099) {
                if (maxEdgeLength <= 162.5) {
                  if (xyRatio <= 1.4273) {
                    if (shiftStd <= 5.5256) {
                      return 3;
                    } else {  // if shiftStd > 5.5256
                      return 4;
                    }
                  } else {  // if xyRatio > 1.4273
                    return 2;
                  }
                } else {  // if maxEdgeLength > 162.5
                  if (avgErr <= 5.2243) {
                    return 3;
                  } else {  // if avgErr > 5.2243
                    if (avgErr <= 5.2399) {
                      return 4;
                    } else {  // if avgErr > 5.2399
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 18.099
                return 4;
              }
            }
          } else {  // if area > 19892
            if (area <= 19939) {
              if (shiftNcc <= 0.38601) {
                if (maxEdgeLength <= 179.5) {
                  return 4;
                } else {  // if maxEdgeLength > 179.5
                  if (area <= 19919) {
                    return 3;
                  } else {  // if area > 19919
                    return 4;
                  }
                }
              } else {  // if shiftNcc > 0.38601
                if (avgErr <= 8.9077) {
                  return 3;
                } else {  // if avgErr > 8.9077
                  return 4;
                }
              }
            } else {  // if area > 19939
              if (area <= 20062) {
                return 3;
              } else {  // if area > 20062
                if (maxEdgeLength <= 193) {
                  if (shiftAvg <= 0.56332) {
                    if (area <= 20120) {
                      return 4;
                    } else {  // if area > 20120
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.56332
                    if (area <= 20181) {
                      return 3;
                    } else {  // if area > 20181
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 193
                  return 4;
                }
              }
            }
          }
        }
      } else {  // if maxEdgeLength > 194.5
        if (shiftAvg <= 12.836) {
          return 0;
        } else {  // if shiftAvg > 12.836
          if (shiftNcc <= 0.82096) {
            return 3;
          } else {  // if shiftNcc > 0.82096
            return 0;
          }
        }
      }
    } else {  // if area > 20276
      if (maxEdgeLength <= 194.5) {
        if (area <= 20577) {
          if (area <= 20536) {
            if (shiftAvg <= 0.00095232) {
              if (area <= 20466) {
                return 3;
              } else {  // if area > 20466
                return 4;
              }
            } else {  // if shiftAvg > 0.00095232
              if (shiftStd <= 0.78631) {
                if (shiftNcc <= -0.024411) {
                  return 3;
                } else {  // if shiftNcc > -0.024411
                  if (avgErr <= 0.42737) {
                    if (shiftAvg <= 0.0069696) {
                      return 4;
                    } else {  // if shiftAvg > 0.0069696
                      return 3;
                    }
                  } else {  // if avgErr > 0.42737
                    if (maxEdgeLength <= 185.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 185.5
                      return 4;
                    }
                  }
                }
              } else {  // if shiftStd > 0.78631
                if (xyRatio <= 1.4619) {
                  if (area <= 20334) {
                    if (maxEdgeLength <= 162.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 162.5
                      return 3;
                    }
                  } else {  // if area > 20334
                    return 3;
                  }
                } else {  // if xyRatio > 1.4619
                  if (avgErr <= 5.1343) {
                    return 3;
                  } else {  // if avgErr > 5.1343
                    if (shiftStd <= 6.7906) {
                      return 4;
                    } else {  // if shiftStd > 6.7906
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if area > 20536
            if (xyRatio <= 1.8123) {
              return 3;
            } else {  // if xyRatio > 1.8123
              return 4;
            }
          }
        } else {  // if area > 20577
          if (area <= 20752) {
            if (area <= 20744) {
              if (xyRatio <= 1.5237) {
                if (maxEdgeLength <= 176.5) {
                  if (shiftAvg <= 6.4421) {
                    if (shiftNcc <= 0.40071) {
                      return 4;
                    } else {  // if shiftNcc > 0.40071
                      return 4;
                    }
                  } else {  // if shiftAvg > 6.4421
                    if (shiftNcc <= 0.91027) {
                      return 3;
                    } else {  // if shiftNcc > 0.91027
                      return 4;
                    }
                  }
                } else {  // if maxEdgeLength > 176.5
                  return 3;
                }
              } else {  // if xyRatio > 1.5237
                return 4;
              }
            } else {  // if area > 20744
              return 3;
            }
          } else {  // if area > 20752
            return 4;
          }
        }
      } else {  // if maxEdgeLength > 194.5
        if (area <= 28934) {
          if (xyRatio <= 1.4352) {
            if (area <= 28681) {
              return 4;
            } else {  // if area > 28681
              return 0;
            }
          } else {  // if xyRatio > 1.4352
            if (maxEdgeLength <= 201.5) {
              if (shiftAvg <= 0.0011676) {
                if (shiftAvg <= 0.00091068) {
                  return 0;
                } else {  // if shiftAvg > 0.00091068
                  return 4;
                }
              } else {  // if shiftAvg > 0.0011676
                return 0;
              }
            } else {  // if maxEdgeLength > 201.5
              if (area <= 20401) {
                if (shiftNcc <= 0.27367) {
                  return 4;
                } else {  // if shiftNcc > 0.27367
                  return 0;
                }
              } else {  // if area > 20401
                if (maxEdgeLength <= 207.5) {
                  if (shiftAvg <= 0.093766) {
                    if (xyRatio <= 1.4945) {
                      return 0;
                    } else {  // if xyRatio > 1.4945
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.093766
                    if (shiftNcc <= 0.29966) {
                      return 0;
                    } else {  // if shiftNcc > 0.29966
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 207.5
                  if (area <= 27728) {
                    return 0;
                  } else {  // if area > 27728
                    if (area <= 27756) {
                      return 4;
                    } else {  // if area > 27756
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 28934
          if (xyRatio <= 1.5295) {
            if (area <= 33192) {
              if (shiftAvg <= 0.0014351) {
                if (avgErr <= 1.7443) {
                  return 0;
                } else {  // if avgErr > 1.7443
                  return 4;
                }
              } else {  // if shiftAvg > 0.0014351
                if (area <= 32384) {
                  return 4;
                } else {  // if area > 32384
                  if (shiftAvg <= 0.035937) {
                    if (xyRatio <= 1.4496) {
                      return 0;
                    } else {  // if xyRatio > 1.4496
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.035937
                    return 4;
                  }
                }
              }
            } else {  // if area > 33192
              if (xyRatio <= 1.3846) {
                if (xyRatio <= 1.3263) {
                  if (maxEdgeLength <= 213) {
                    if (area <= 33298) {
                      return 1;
                    } else {  // if area > 33298
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 213
                    return 1;
                  }
                } else {  // if xyRatio > 1.3263
                  return 4;
                }
              } else {  // if xyRatio > 1.3846
                if (xyRatio <= 1.4655) {
                  return 1;
                } else {  // if xyRatio > 1.4655
                  return 0;
                }
              }
            }
          } else {  // if xyRatio > 1.5295
            if (shiftStd <= 0.051718) {
              return 4;
            } else {  // if shiftStd > 0.051718
              return 0;
            }
          }
        }
      }
    }
  }
}

} // namespace md
