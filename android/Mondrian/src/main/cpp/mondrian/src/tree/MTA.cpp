#include "mondrian/tree/MTA.hpp"

namespace md {

int MTA(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr) {
  if (xyRatio <= 1.0559) {
    if (maxEdgeLength <= 92.5) {
      if (area <= 5181.5) {
        if (area <= 4093.5) {
          if (area <= 2905.5) {
            if (area <= 2393) {
              if (area <= 2003) {
                if (maxEdgeLength <= 49.5) {
                  if (area <= 1848.5) {
                    if (avgErr <= 0.33517) {
                      return 1;
                    } else {  // if avgErr > 0.33517
                      return 3;
                    }
                  } else {  // if area > 1848.5
                    if (shiftStd <= 0.00021504) {
                      return 3;
                    } else {  // if shiftStd > 0.00021504
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 49.5
                  if (avgErr <= 5.0914) {
                    if (maxEdgeLength <= 55.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 55.5
                      return 1;
                    }
                  } else {  // if avgErr > 5.0914
                    if (area <= 1824) {
                      return 3;
                    } else {  // if area > 1824
                      return 3;
                    }
                  }
                }
              } else {  // if area > 2003
                if (maxEdgeLength <= 48.5) {
                  if (xyRatio <= 1.0329) {
                    if (shiftAvg <= 2.9246) {
                      return 2;
                    } else {  // if shiftAvg > 2.9246
                      return 4;
                    }
                  } else {  // if xyRatio > 1.0329
                    return 4;
                  }
                } else {  // if maxEdgeLength > 48.5
                  if (xyRatio <= 0.93939) {
                    if (shiftAvg <= 0.1173) {
                      return 0;
                    } else {  // if shiftAvg > 0.1173
                      return 3;
                    }
                  } else {  // if xyRatio > 0.93939
                    if (shiftStd <= 0.036566) {
                      return 4;
                    } else {  // if shiftStd > 0.036566
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 2393
              if (maxEdgeLength <= 64.5) {
                if (avgErr <= 14.058) {
                  if (avgErr <= 0.81724) {
                    if (shiftAvg <= 2.492e-05) {
                      return 1;
                    } else {  // if shiftAvg > 2.492e-05
                      return 1;
                    }
                  } else {  // if avgErr > 0.81724
                    if (avgErr <= 3.3252) {
                      return 4;
                    } else {  // if avgErr > 3.3252
                      return 1;
                    }
                  }
                } else {  // if avgErr > 14.058
                  if (shiftStd <= 0.15964) {
                    if (avgErr <= 16.149) {
                      return 3;
                    } else {  // if avgErr > 16.149
                      return 4;
                    }
                  } else {  // if shiftStd > 0.15964
                    if (maxEdgeLength <= 56.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 56.5
                      return 4;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 64.5
                if (shiftAvg <= 0.094184) {
                  if (shiftStd <= 0.00026917) {
                    if (area <= 2725) {
                      return 1;
                    } else {  // if area > 2725
                      return 4;
                    }
                  } else {  // if shiftStd > 0.00026917
                    if (avgErr <= 1.2611) {
                      return 0;
                    } else {  // if avgErr > 1.2611
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.094184
                  if (area <= 2821) {
                    if (shiftStd <= 0.061519) {
                      return 3;
                    } else {  // if shiftStd > 0.061519
                      return 1;
                    }
                  } else {  // if area > 2821
                    return 4;
                  }
                }
              }
            }
          } else {  // if area > 2905.5
            if (maxEdgeLength <= 64.5) {
              if (area <= 3714.5) {
                if (shiftStd <= 0.017472) {
                  if (avgErr <= 0.70404) {
                    if (shiftStd <= 0.00026565) {
                      return 4;
                    } else {  // if shiftStd > 0.00026565
                      return 2;
                    }
                  } else {  // if avgErr > 0.70404
                    if (avgErr <= 5.5391) {
                      return 4;
                    } else {  // if avgErr > 5.5391
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.017472
                  if (avgErr <= 9.1538) {
                    if (area <= 3024.5) {
                      return 4;
                    } else {  // if area > 3024.5
                      return 2;
                    }
                  } else {  // if avgErr > 9.1538
                    if (shiftStd <= 0.23749) {
                      return 4;
                    } else {  // if shiftStd > 0.23749
                      return 4;
                    }
                  }
                }
              } else {  // if area > 3714.5
                if (shiftAvg <= 0.00030314) {
                  if (xyRatio <= 0.96043) {
                    if (shiftStd <= 0.00016438) {
                      return 3;
                    } else {  // if shiftStd > 0.00016438
                      return 4;
                    }
                  } else {  // if xyRatio > 0.96043
                    return 4;
                  }
                } else {  // if shiftAvg > 0.00030314
                  if (shiftStd <= 3.302) {
                    if (maxEdgeLength <= 61.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 61.5
                      return 3;
                    }
                  } else {  // if shiftStd > 3.302
                    return 4;
                  }
                }
              }
            } else {  // if maxEdgeLength > 64.5
              if (avgErr <= 5.4834) {
                if (avgErr <= 1.1123) {
                  if (area <= 3623.5) {
                    if (maxEdgeLength <= 68.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 68.5
                      return 4;
                    }
                  } else {  // if area > 3623.5
                    if (shiftAvg <= 0.00057201) {
                      return 4;
                    } else {  // if shiftAvg > 0.00057201
                      return 0;
                    }
                  }
                } else {  // if avgErr > 1.1123
                  if (shiftStd <= 0.0064597) {
                    if (xyRatio <= 0.62676) {
                      return 4;
                    } else {  // if xyRatio > 0.62676
                      return 4;
                    }
                  } else {  // if shiftStd > 0.0064597
                    if (shiftStd <= 0.10552) {
                      return 4;
                    } else {  // if shiftStd > 0.10552
                      return 4;
                    }
                  }
                }
              } else {  // if avgErr > 5.4834
                if (shiftStd <= 0.29156) {
                  if (avgErr <= 9.2938) {
                    if (shiftAvg <= 0.11088) {
                      return 0;
                    } else {  // if shiftAvg > 0.11088
                      return 4;
                    }
                  } else {  // if avgErr > 9.2938
                    if (shiftStd <= 0.11142) {
                      return 3;
                    } else {  // if shiftStd > 0.11142
                      return 4;
                    }
                  }
                } else {  // if shiftStd > 0.29156
                  if (shiftAvg <= 0.017567) {
                    if (shiftNcc <= 0.035322) {
                      return 0;
                    } else {  // if shiftNcc > 0.035322
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.017567
                    if (area <= 4081.5) {
                      return 4;
                    } else {  // if area > 4081.5
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 4093.5
          if (shiftStd <= 0.0020171) {
            if (area <= 5125.5) {
              if (area <= 4167) {
                if (shiftStd <= 0.00024822) {
                  return 4;
                } else {  // if shiftStd > 0.00024822
                  return 1;
                }
              } else {  // if area > 4167
                if (avgErr <= 0.65754) {
                  if (avgErr <= 0.61913) {
                    if (shiftStd <= 0.00084958) {
                      return 4;
                    } else {  // if shiftStd > 0.00084958
                      return 1;
                    }
                  } else {  // if avgErr > 0.61913
                    return 1;
                  }
                } else {  // if avgErr > 0.65754
                  return 4;
                }
              }
            } else {  // if area > 5125.5
              return 1;
            }
          } else {  // if shiftStd > 0.0020171
            if (xyRatio <= 0.83225) {
              if (shiftAvg <= 0.072795) {
                if (avgErr <= 12.852) {
                  if (xyRatio <= 0.72614) {
                    if (xyRatio <= 0.65838) {
                      return 4;
                    } else {  // if xyRatio > 0.65838
                      return 4;
                    }
                  } else {  // if xyRatio > 0.72614
                    if (area <= 4270) {
                      return 4;
                    } else {  // if area > 4270
                      return 1;
                    }
                  }
                } else {  // if avgErr > 12.852
                  if (shiftAvg <= 0.059493) {
                    if (shiftAvg <= 0.022961) {
                      return 3;
                    } else {  // if shiftAvg > 0.022961
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.059493
                    return 1;
                  }
                }
              } else {  // if shiftAvg > 0.072795
                if (shiftStd <= 7.9978) {
                  if (avgErr <= 7.3039) {
                    return 4;
                  } else {  // if avgErr > 7.3039
                    if (area <= 5095.5) {
                      return 4;
                    } else {  // if area > 5095.5
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 7.9978
                  return 3;
                }
              }
            } else {  // if xyRatio > 0.83225
              if (shiftAvg <= 0.021244) {
                if (area <= 5038.5) {
                  if (area <= 4623.5) {
                    if (xyRatio <= 1.0154) {
                      return 4;
                    } else {  // if xyRatio > 1.0154
                      return 1;
                    }
                  } else {  // if area > 4623.5
                    if (avgErr <= 3.4799) {
                      return 4;
                    } else {  // if avgErr > 3.4799
                      return 1;
                    }
                  }
                } else {  // if area > 5038.5
                  if (maxEdgeLength <= 76.5) {
                    if (shiftAvg <= 0.016105) {
                      return 2;
                    } else {  // if shiftAvg > 0.016105
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 76.5
                    return 1;
                  }
                }
              } else {  // if shiftAvg > 0.021244
                if (shiftAvg <= 0.10596) {
                  if (area <= 4993) {
                    if (maxEdgeLength <= 64.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 64.5
                      return 1;
                    }
                  } else {  // if area > 4993
                    if (maxEdgeLength <= 75.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 75.5
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.10596
                  if (avgErr <= 6.2234) {
                    if (shiftStd <= 0.1344) {
                      return 1;
                    } else {  // if shiftStd > 0.1344
                      return 4;
                    }
                  } else {  // if avgErr > 6.2234
                    if (xyRatio <= 1.0288) {
                      return 1;
                    } else {  // if xyRatio > 1.0288
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 5181.5
        if (area <= 6554.5) {
          if (maxEdgeLength <= 81.5) {
            if (area <= 5255) {
              if (area <= 5252) {
                if (shiftAvg <= 0.020108) {
                  if (maxEdgeLength <= 79.5) {
                    if (avgErr <= 4.7021) {
                      return 4;
                    } else {  // if avgErr > 4.7021
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 79.5
                    return 1;
                  }
                } else {  // if shiftAvg > 0.020108
                  if (xyRatio <= 0.9863) {
                    if (shiftAvg <= 0.452) {
                      return 4;
                    } else {  // if shiftAvg > 0.452
                      return 2;
                    }
                  } else {  // if xyRatio > 0.9863
                    return 2;
                  }
                }
              } else {  // if area > 5252
                if (shiftAvg <= 0.096073) {
                  return 1;
                } else {  // if shiftAvg > 0.096073
                  if (avgErr <= 7.8502) {
                    return 3;
                  } else {  // if avgErr > 7.8502
                    return 4;
                  }
                }
              }
            } else {  // if area > 5255
              if (avgErr <= 2.42) {
                if (maxEdgeLength <= 75.5) {
                  if (shiftNcc <= 0.26119) {
                    if (area <= 5587.5) {
                      return 2;
                    } else {  // if area > 5587.5
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.26119
                    return 3;
                  }
                } else {  // if maxEdgeLength > 75.5
                  if (maxEdgeLength <= 78.5) {
                    if (shiftStd <= 0.063238) {
                      return 4;
                    } else {  // if shiftStd > 0.063238
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 78.5
                    if (shiftAvg <= 0.0020766) {
                      return 4;
                    } else {  // if shiftAvg > 0.0020766
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 2.42
                if (shiftAvg <= 0.47061) {
                  if (shiftNcc <= 0.45481) {
                    if (shiftNcc <= 0.34769) {
                      return 2;
                    } else {  // if shiftNcc > 0.34769
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.45481
                    if (shiftStd <= 0.058141) {
                      return 2;
                    } else {  // if shiftStd > 0.058141
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.47061
                  if (shiftNcc <= 0.74415) {
                    if (xyRatio <= 1.0523) {
                      return 4;
                    } else {  // if xyRatio > 1.0523
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.74415
                    if (area <= 5475.5) {
                      return 3;
                    } else {  // if area > 5475.5
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 81.5
            if (xyRatio <= 0.92785) {
              if (shiftStd <= 0.1115) {
                if (shiftAvg <= 0.054793) {
                  if (shiftStd <= 0.04499) {
                    if (shiftNcc <= -0.00083756) {
                      return 4;
                    } else {  // if shiftNcc > -0.00083756
                      return 2;
                    }
                  } else {  // if shiftStd > 0.04499
                    return 4;
                  }
                } else {  // if shiftAvg > 0.054793
                  return 2;
                }
              } else {  // if shiftStd > 0.1115
                if (shiftNcc <= 0.13222) {
                  if (avgErr <= 11.845) {
                    if (xyRatio <= 0.86477) {
                      return 2;
                    } else {  // if xyRatio > 0.86477
                      return 3;
                    }
                  } else {  // if avgErr > 11.845
                    if (avgErr <= 16.663) {
                      return 4;
                    } else {  // if avgErr > 16.663
                      return 2;
                    }
                  }
                } else {  // if shiftNcc > 0.13222
                  if (avgErr <= 14.622) {
                    if (shiftNcc <= 0.94718) {
                      return 4;
                    } else {  // if shiftNcc > 0.94718
                      return 2;
                    }
                  } else {  // if avgErr > 14.622
                    if (maxEdgeLength <= 90) {
                      return 2;
                    } else {  // if maxEdgeLength > 90
                      return 4;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.92785
              if (shiftStd <= 0.044499) {
                return 4;
              } else {  // if shiftStd > 0.044499
                return 3;
              }
            }
          }
        } else {  // if area > 6554.5
          if (xyRatio <= 1.0299) {
            if (shiftAvg <= 0.07435) {
              if (xyRatio <= 0.98823) {
                if (avgErr <= 4.4416) {
                  if (shiftStd <= 0.045483) {
                    if (maxEdgeLength <= 84) {
                      return 3;
                    } else {  // if maxEdgeLength > 84
                      return 4;
                    }
                  } else {  // if shiftStd > 0.045483
                    return 4;
                  }
                } else {  // if avgErr > 4.4416
                  if (area <= 6641) {
                    return 4;
                  } else {  // if area > 6641
                    return 3;
                  }
                }
              } else {  // if xyRatio > 0.98823
                return 3;
              }
            } else {  // if shiftAvg > 0.07435
              if (xyRatio <= 1.012) {
                if (shiftStd <= 0.062703) {
                  return 4;
                } else {  // if shiftStd > 0.062703
                  if (shiftStd <= 0.28351) {
                    if (shiftAvg <= 0.080508) {
                      return 4;
                    } else {  // if shiftAvg > 0.080508
                      return 3;
                    }
                  } else {  // if shiftStd > 0.28351
                    if (shiftStd <= 0.7239) {
                      return 4;
                    } else {  // if shiftStd > 0.7239
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 1.012
                if (shiftNcc <= 0.38481) {
                  if (avgErr <= 6.5038) {
                    return 4;
                  } else {  // if avgErr > 6.5038
                    return 3;
                  }
                } else {  // if shiftNcc > 0.38481
                  return 4;
                }
              }
            }
          } else {  // if xyRatio > 1.0299
            if (shiftAvg <= 1.0806) {
              return 3;
            } else {  // if shiftAvg > 1.0806
              return 4;
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 92.5
      if (xyRatio <= 0.89529) {
        if (shiftStd <= 50.636) {
          if (xyRatio <= 0.76988) {
            return 4;
          } else {  // if xyRatio > 0.76988
            if (avgErr <= 5.7468) {
              if (maxEdgeLength <= 107.5) {
                if (shiftStd <= 1.0527) {
                  return 3;
                } else {  // if shiftStd > 1.0527
                  return 0;
                }
              } else {  // if maxEdgeLength > 107.5
                return 1;
              }
            } else {  // if avgErr > 5.7468
              return 4;
            }
          }
        } else {  // if shiftStd > 50.636
          if (shiftAvg <= 72.113) {
            return 4;
          } else {  // if shiftAvg > 72.113
            return 0;
          }
        }
      } else {  // if xyRatio > 0.89529
        if (avgErr <= 5.7295) {
          if (area <= 11338) {
            if (shiftNcc <= 0.33263) {
              if (shiftAvg <= 0.009825) {
                return 4;
              } else {  // if shiftAvg > 0.009825
                return 3;
              }
            } else {  // if shiftNcc > 0.33263
              return 1;
            }
          } else {  // if area > 11338
            if (area <= 14518) {
              if (shiftNcc <= 0.92465) {
                return 2;
              } else {  // if shiftNcc > 0.92465
                if (avgErr <= 5.2883) {
                  return 2;
                } else {  // if avgErr > 5.2883
                  return 1;
                }
              }
            } else {  // if area > 14518
              return 3;
            }
          }
        } else {  // if avgErr > 5.7295
          if (maxEdgeLength <= 121.5) {
            if (area <= 9649.5) {
              if (shiftNcc <= 0.75817) {
                if (shiftAvg <= 0.10242) {
                  if (avgErr <= 13.207) {
                    if (shiftStd <= 0.29871) {
                      return 1;
                    } else {  // if shiftStd > 0.29871
                      return 0;
                    }
                  } else {  // if avgErr > 13.207
                    return 4;
                  }
                } else {  // if shiftAvg > 0.10242
                  if (maxEdgeLength <= 96.5) {
                    return 3;
                  } else {  // if maxEdgeLength > 96.5
                    if (shiftStd <= 0.45461) {
                      return 3;
                    } else {  // if shiftStd > 0.45461
                      return 4;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.75817
                return 4;
              }
            } else {  // if area > 9649.5
              if (shiftAvg <= 0.065435) {
                if (area <= 10773) {
                  return 4;
                } else {  // if area > 10773
                  return 2;
                }
              } else {  // if shiftAvg > 0.065435
                if (area <= 9799) {
                  if (shiftAvg <= 1.3126) {
                    return 1;
                  } else {  // if shiftAvg > 1.3126
                    return 4;
                  }
                } else {  // if area > 9799
                  return 4;
                }
              }
            }
          } else {  // if maxEdgeLength > 121.5
            if (avgErr <= 13.295) {
              if (maxEdgeLength <= 189.5) {
                if (xyRatio <= 0.97206) {
                  if (xyRatio <= 0.96138) {
                    if (shiftAvg <= 22.656) {
                      return 3;
                    } else {  // if shiftAvg > 22.656
                      return 4;
                    }
                  } else {  // if xyRatio > 0.96138
                    return 4;
                  }
                } else {  // if xyRatio > 0.97206
                  return 3;
                }
              } else {  // if maxEdgeLength > 189.5
                if (xyRatio <= 1.0316) {
                  return 4;
                } else {  // if xyRatio > 1.0316
                  return 1;
                }
              }
            } else {  // if avgErr > 13.295
              if (area <= 18432) {
                return 2;
              } else {  // if area > 18432
                return 4;
              }
            }
          }
        }
      }
    }
  } else {  // if xyRatio > 1.0559
    if (area <= 14583) {
      if (area <= 5211) {
        if (area <= 4160.5) {
          if (maxEdgeLength <= 64.5) {
            if (area <= 2994.5) {
              if (area <= 2334) {
                if (maxEdgeLength <= 49.5) {
                  if (shiftNcc <= 0.063132) {
                    if (area <= 1698) {
                      return 1;
                    } else {  // if area > 1698
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.063132
                    if (maxEdgeLength <= 48.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 48.5
                      return 0;
                    }
                  }
                } else {  // if maxEdgeLength > 49.5
                  if (shiftNcc <= 0.10831) {
                    if (shiftNcc <= 0.057499) {
                      return 4;
                    } else {  // if shiftNcc > 0.057499
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.10831
                    if (xyRatio <= 1.1117) {
                      return 2;
                    } else {  // if xyRatio > 1.1117
                      return 0;
                    }
                  }
                }
              } else {  // if area > 2334
                if (shiftAvg <= 0.00075446) {
                  if (shiftAvg <= 6.7486e-06) {
                    if (avgErr <= 0.89544) {
                      return 1;
                    } else {  // if avgErr > 0.89544
                      return 4;
                    }
                  } else {  // if shiftAvg > 6.7486e-06
                    if (avgErr <= 7.189) {
                      return 4;
                    } else {  // if avgErr > 7.189
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.00075446
                  if (area <= 2873.5) {
                    if (avgErr <= 9.2921) {
                      return 1;
                    } else {  // if avgErr > 9.2921
                      return 1;
                    }
                  } else {  // if area > 2873.5
                    if (area <= 2982.5) {
                      return 4;
                    } else {  // if area > 2982.5
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 2994.5
              if (area <= 3559) {
                if (shiftNcc <= 0.22883) {
                  if (area <= 3073) {
                    if (shiftAvg <= 0.011189) {
                      return 2;
                    } else {  // if shiftAvg > 0.011189
                      return 2;
                    }
                  } else {  // if area > 3073
                    if (area <= 3254.5) {
                      return 4;
                    } else {  // if area > 3254.5
                      return 2;
                    }
                  }
                } else {  // if shiftNcc > 0.22883
                  if (avgErr <= 1.0925) {
                    return 4;
                  } else {  // if avgErr > 1.0925
                    if (avgErr <= 16.078) {
                      return 2;
                    } else {  // if avgErr > 16.078
                      return 4;
                    }
                  }
                }
              } else {  // if area > 3559
                if (shiftStd <= 0.018028) {
                  if (avgErr <= 0.25328) {
                    if (area <= 3683) {
                      return 2;
                    } else {  // if area > 3683
                      return 3;
                    }
                  } else {  // if avgErr > 0.25328
                    return 4;
                  }
                } else {  // if shiftStd > 0.018028
                  if (maxEdgeLength <= 63.5) {
                    if (xyRatio <= 1.0776) {
                      return 3;
                    } else {  // if xyRatio > 1.0776
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 63.5
                    if (shiftStd <= 0.24155) {
                      return 3;
                    } else {  // if shiftStd > 0.24155
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 64.5
            if (shiftStd <= 0.0055602) {
              if (avgErr <= 0.58028) {
                if (avgErr <= 0.3427) {
                  if (shiftAvg <= 1.7602e-06) {
                    if (maxEdgeLength <= 75.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 75.5
                      return 0;
                    }
                  } else {  // if shiftAvg > 1.7602e-06
                    if (shiftStd <= 2.1413e-05) {
                      return 1;
                    } else {  // if shiftStd > 2.1413e-05
                      return 0;
                    }
                  }
                } else {  // if avgErr > 0.3427
                  if (xyRatio <= 1.3397) {
                    if (avgErr <= 0.47427) {
                      return 3;
                    } else {  // if avgErr > 0.47427
                      return 4;
                    }
                  } else {  // if xyRatio > 1.3397
                    if (avgErr <= 0.36169) {
                      return 3;
                    } else {  // if avgErr > 0.36169
                      return 0;
                    }
                  }
                }
              } else {  // if avgErr > 0.58028
                if (area <= 2173.5) {
                  return 0;
                } else {  // if area > 2173.5
                  if (shiftStd <= 0.00032118) {
                    if (shiftNcc <= 0.43775) {
                      return 4;
                    } else {  // if shiftNcc > 0.43775
                      return 2;
                    }
                  } else {  // if shiftStd > 0.00032118
                    if (avgErr <= 0.92797) {
                      return 0;
                    } else {  // if avgErr > 0.92797
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftStd > 0.0055602
              if (area <= 4030.5) {
                if (avgErr <= 12.834) {
                  if (xyRatio <= 2.1978) {
                    if (shiftAvg <= 0.029888) {
                      return 0;
                    } else {  // if shiftAvg > 0.029888
                      return 0;
                    }
                  } else {  // if xyRatio > 2.1978
                    if (maxEdgeLength <= 90) {
                      return 4;
                    } else {  // if maxEdgeLength > 90
                      return 0;
                    }
                  }
                } else {  // if avgErr > 12.834
                  if (area <= 3571) {
                    if (xyRatio <= 1.4947) {
                      return 0;
                    } else {  // if xyRatio > 1.4947
                      return 4;
                    }
                  } else {  // if area > 3571
                    if (shiftAvg <= 1.0236) {
                      return 0;
                    } else {  // if shiftAvg > 1.0236
                      return 4;
                    }
                  }
                }
              } else {  // if area > 4030.5
                if (area <= 4129) {
                  if (area <= 4093) {
                    if (maxEdgeLength <= 76) {
                      return 0;
                    } else {  // if maxEdgeLength > 76
                      return 0;
                    }
                  } else {  // if area > 4093
                    if (shiftAvg <= 0.35732) {
                      return 1;
                    } else {  // if shiftAvg > 0.35732
                      return 3;
                    }
                  }
                } else {  // if area > 4129
                  if (xyRatio <= 1.1682) {
                    if (maxEdgeLength <= 68.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 68.5
                      return 1;
                    }
                  } else {  // if xyRatio > 1.1682
                    if (maxEdgeLength <= 82) {
                      return 0;
                    } else {  // if maxEdgeLength > 82
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 4160.5
          if (maxEdgeLength <= 97.5) {
            if (area <= 5083) {
              if (shiftStd <= 0.010026) {
                if (avgErr <= 0.63985) {
                  if (xyRatio <= 1.1078) {
                    if (shiftStd <= 6.9385e-06) {
                      return 3;
                    } else {  // if shiftStd > 6.9385e-06
                      return 4;
                    }
                  } else {  // if xyRatio > 1.1078
                    if (shiftNcc <= -0.021653) {
                      return 1;
                    } else {  // if shiftNcc > -0.021653
                      return 1;
                    }
                  }
                } else {  // if avgErr > 0.63985
                  if (shiftStd <= 0.0003262) {
                    if (shiftNcc <= -0.021009) {
                      return 3;
                    } else {  // if shiftNcc > -0.021009
                      return 4;
                    }
                  } else {  // if shiftStd > 0.0003262
                    if (area <= 4676) {
                      return 4;
                    } else {  // if area > 4676
                      return 1;
                    }
                  }
                }
              } else {  // if shiftStd > 0.010026
                if (area <= 4291) {
                  if (area <= 4287) {
                    if (area <= 4231.5) {
                      return 1;
                    } else {  // if area > 4231.5
                      return 1;
                    }
                  } else {  // if area > 4287
                    if (shiftAvg <= 0.48993) {
                      return 0;
                    } else {  // if shiftAvg > 0.48993
                      return 4;
                    }
                  }
                } else {  // if area > 4291
                  if (xyRatio <= 1.3277) {
                    if (shiftNcc <= 0.060734) {
                      return 1;
                    } else {  // if shiftNcc > 0.060734
                      return 1;
                    }
                  } else {  // if xyRatio > 1.3277
                    if (shiftNcc <= -0.022573) {
                      return 1;
                    } else {  // if shiftNcc > -0.022573
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 5083
              if (xyRatio <= 1.2937) {
                if (maxEdgeLength <= 79.5) {
                  if (xyRatio <= 1.1986) {
                    if (avgErr <= 5.9972) {
                      return 1;
                    } else {  // if avgErr > 5.9972
                      return 1;
                    }
                  } else {  // if xyRatio > 1.1986
                    if (shiftStd <= 0.039247) {
                      return 4;
                    } else {  // if shiftStd > 0.039247
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 79.5
                  if (area <= 5111.5) {
                    if (shiftAvg <= 0.064174) {
                      return 1;
                    } else {  // if shiftAvg > 0.064174
                      return 1;
                    }
                  } else {  // if area > 5111.5
                    if (shiftNcc <= 0.8266) {
                      return 1;
                    } else {  // if shiftNcc > 0.8266
                      return 3;
                    }
                  }
                }
              } else {  // if xyRatio > 1.2937
                if (xyRatio <= 1.454) {
                  if (maxEdgeLength <= 84.5) {
                    if (xyRatio <= 1.3306) {
                      return 2;
                    } else {  // if xyRatio > 1.3306
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 84.5
                    if (shiftStd <= 0.022876) {
                      return 4;
                    } else {  // if shiftStd > 0.022876
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 1.454
                  if (xyRatio <= 1.6339) {
                    if (xyRatio <= 1.5567) {
                      return 1;
                    } else {  // if xyRatio > 1.5567
                      return 1;
                    }
                  } else {  // if xyRatio > 1.6339
                    if (area <= 5122.5) {
                      return 1;
                    } else {  // if area > 5122.5
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if maxEdgeLength > 97.5
            if (avgErr <= 10.836) {
              if (xyRatio <= 1.9604) {
                return 0;
              } else {  // if xyRatio > 1.9604
                if (shiftStd <= 0.068618) {
                  if (shiftStd <= 0.025417) {
                    if (shiftStd <= 0.00067747) {
                      return 0;
                    } else {  // if shiftStd > 0.00067747
                      return 2;
                    }
                  } else {  // if shiftStd > 0.025417
                    if (shiftStd <= 0.032571) {
                      return 0;
                    } else {  // if shiftStd > 0.032571
                      return 0;
                    }
                  }
                } else {  // if shiftStd > 0.068618
                  if (avgErr <= 4.9276) {
                    if (area <= 4946) {
                      return 1;
                    } else {  // if area > 4946
                      return 0;
                    }
                  } else {  // if avgErr > 4.9276
                    if (maxEdgeLength <= 105.5) {
                      return 0;
                    } else {  // if maxEdgeLength > 105.5
                      return 4;
                    }
                  }
                }
              }
            } else {  // if avgErr > 10.836
              if (shiftNcc <= 0.56103) {
                if (maxEdgeLength <= 100.5) {
                  if (shiftNcc <= 0.10985) {
                    return 1;
                  } else {  // if shiftNcc > 0.10985
                    return 0;
                  }
                } else {  // if maxEdgeLength > 100.5
                  if (area <= 4862) {
                    return 1;
                  } else {  // if area > 4862
                    if (shiftStd <= 0.7231) {
                      return 1;
                    } else {  // if shiftStd > 0.7231
                      return 4;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.56103
                if (shiftNcc <= 0.83907) {
                  return 4;
                } else {  // if shiftNcc > 0.83907
                  if (shiftNcc <= 0.91907) {
                    if (avgErr <= 14.432) {
                      return 3;
                    } else {  // if avgErr > 14.432
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.91907
                    if (maxEdgeLength <= 104.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 104.5
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 5211
        if (maxEdgeLength <= 97.5) {
          if (area <= 6439) {
            if (shiftStd <= 0.0025614) {
              if (maxEdgeLength <= 96.5) {
                if (xyRatio <= 1.0615) {
                  return 2;
                } else {  // if xyRatio > 1.0615
                  if (avgErr <= 1.8465) {
                    if (shiftNcc <= 0.12897) {
                      return 4;
                    } else {  // if shiftNcc > 0.12897
                      return 4;
                    }
                  } else {  // if avgErr > 1.8465
                    return 2;
                  }
                }
              } else {  // if maxEdgeLength > 96.5
                if (shiftAvg <= 3.1382e-06) {
                  return 4;
                } else {  // if shiftAvg > 3.1382e-06
                  if (avgErr <= 0.52504) {
                    return 1;
                  } else {  // if avgErr > 0.52504
                    return 2;
                  }
                }
              }
            } else {  // if shiftStd > 0.0025614
              if (area <= 5305.5) {
                if (area <= 5302.5) {
                  if (maxEdgeLength <= 91.5) {
                    if (maxEdgeLength <= 80.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 80.5
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 91.5
                    if (maxEdgeLength <= 96.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 96.5
                      return 1;
                    }
                  }
                } else {  // if area > 5302.5
                  if (avgErr <= 11.653) {
                    if (shiftAvg <= 0.036811) {
                      return 1;
                    } else {  // if shiftAvg > 0.036811
                      return 1;
                    }
                  } else {  // if avgErr > 11.653
                    return 3;
                  }
                }
              } else {  // if area > 5305.5
                if (area <= 6260) {
                  if (shiftAvg <= 0.049419) {
                    if (avgErr <= 0.82918) {
                      return 4;
                    } else {  // if avgErr > 0.82918
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.049419
                    if (avgErr <= 8.6051) {
                      return 2;
                    } else {  // if avgErr > 8.6051
                      return 2;
                    }
                  }
                } else {  // if area > 6260
                  if (maxEdgeLength <= 94.5) {
                    if (xyRatio <= 1.1689) {
                      return 3;
                    } else {  // if xyRatio > 1.1689
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 94.5
                    if (xyRatio <= 1.447) {
                      return 3;
                    } else {  // if xyRatio > 1.447
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 6439
            if (area <= 6601.5) {
              if (area <= 6598) {
                if (area <= 6562.5) {
                  if (area <= 6460.5) {
                    if (shiftNcc <= 0.81999) {
                      return 3;
                    } else {  // if shiftNcc > 0.81999
                      return 4;
                    }
                  } else {  // if area > 6460.5
                    if (avgErr <= 3.8067) {
                      return 4;
                    } else {  // if avgErr > 3.8067
                      return 3;
                    }
                  }
                } else {  // if area > 6562.5
                  return 3;
                }
              } else {  // if area > 6598
                if (avgErr <= 3.6649) {
                  if (shiftAvg <= 0.043123) {
                    return 2;
                  } else {  // if shiftAvg > 0.043123
                    return 4;
                  }
                } else {  // if avgErr > 3.6649
                  return 2;
                }
              }
            } else {  // if area > 6601.5
              if (shiftAvg <= 0.00018608) {
                if (area <= 7114.5) {
                  return 4;
                } else {  // if area > 7114.5
                  return 3;
                }
              } else {  // if shiftAvg > 0.00018608
                if (xyRatio <= 1.1677) {
                  if (avgErr <= 4.2988) {
                    if (xyRatio <= 1.1165) {
                      return 4;
                    } else {  // if xyRatio > 1.1165
                      return 3;
                    }
                  } else {  // if avgErr > 4.2988
                    if (area <= 7985) {
                      return 3;
                    } else {  // if area > 7985
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 1.1677
                  if (shiftAvg <= 0.41907) {
                    if (avgErr <= 5.0296) {
                      return 3;
                    } else {  // if avgErr > 5.0296
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.41907
                    if (avgErr <= 11.145) {
                      return 3;
                    } else {  // if avgErr > 11.145
                      return 3;
                    }
                  }
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 97.5
          if (area <= 9439) {
            if (area <= 9202) {
              if (shiftStd <= 0.00074185) {
                if (area <= 6707.5) {
                  if (shiftAvg <= 5.0816e-06) {
                    if (xyRatio <= 1.5842) {
                      return 2;
                    } else {  // if xyRatio > 1.5842
                      return 0;
                    }
                  } else {  // if shiftAvg > 5.0816e-06
                    if (avgErr <= 0.94806) {
                      return 4;
                    } else {  // if avgErr > 0.94806
                      return 4;
                    }
                  }
                } else {  // if area > 6707.5
                  if (xyRatio <= 1.3271) {
                    if (shiftAvg <= 3.3285e-05) {
                      return 3;
                    } else {  // if shiftAvg > 3.3285e-05
                      return 0;
                    }
                  } else {  // if xyRatio > 1.3271
                    if (shiftStd <= 6.9004e-05) {
                      return 4;
                    } else {  // if shiftStd > 6.9004e-05
                      return 0;
                    }
                  }
                }
              } else {  // if shiftStd > 0.00074185
                if (maxEdgeLength <= 109.5) {
                  if (area <= 9056) {
                    if (area <= 6371) {
                      return 0;
                    } else {  // if area > 6371
                      return 0;
                    }
                  } else {  // if area > 9056
                    return 1;
                  }
                } else {  // if maxEdgeLength > 109.5
                  if (shiftAvg <= 0.059737) {
                    if (shiftAvg <= 0.059482) {
                      return 0;
                    } else {  // if shiftAvg > 0.059482
                      return 3;
                    }
                  } else {  // if shiftAvg > 0.059737
                    if (shiftStd <= 3.9711) {
                      return 0;
                    } else {  // if shiftStd > 3.9711
                      return 0;
                    }
                  }
                }
              }
            } else {  // if area > 9202
              if (area <= 9237) {
                if (avgErr <= 7.5348) {
                  if (area <= 9214.5) {
                    return 3;
                  } else {  // if area > 9214.5
                    return 0;
                  }
                } else {  // if avgErr > 7.5348
                  return 1;
                }
              } else {  // if area > 9237
                if (area <= 9253) {
                  return 0;
                } else {  // if area > 9253
                  if (area <= 9349) {
                    if (area <= 9319.5) {
                      return 0;
                    } else {  // if area > 9319.5
                      return 1;
                    }
                  } else {  // if area > 9349
                    if (area <= 9377) {
                      return 0;
                    } else {  // if area > 9377
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if area > 9439
            if (area <= 11646) {
              if (area <= 9654) {
                if (shiftAvg <= 1.8457) {
                  if (area <= 9625) {
                    if (shiftAvg <= 0.0010058) {
                      return 3;
                    } else {  // if shiftAvg > 0.0010058
                      return 1;
                    }
                  } else {  // if area > 9625
                    if (area <= 9638.5) {
                      return 0;
                    } else {  // if area > 9638.5
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 1.8457
                  return 0;
                }
              } else {  // if area > 9654
                if (xyRatio <= 1.2753) {
                  if (avgErr <= 7.312) {
                    if (shiftNcc <= -0.0084861) {
                      return 4;
                    } else {  // if shiftNcc > -0.0084861
                      return 1;
                    }
                  } else {  // if avgErr > 7.312
                    if (shiftNcc <= 0.09766) {
                      return 1;
                    } else {  // if shiftNcc > 0.09766
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 1.2753
                  if (area <= 11434) {
                    if (xyRatio <= 1.4386) {
                      return 1;
                    } else {  // if xyRatio > 1.4386
                      return 1;
                    }
                  } else {  // if area > 11434
                    if (area <= 11458) {
                      return 2;
                    } else {  // if area > 11458
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 11646
              if (area <= 12067) {
                if (shiftAvg <= 0.88512) {
                  if (shiftNcc <= 0.61577) {
                    if (maxEdgeLength <= 137.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 137.5
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.61577
                    if (xyRatio <= 2.0329) {
                      return 2;
                    } else {  // if xyRatio > 2.0329
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.88512
                  if (xyRatio <= 1.5511) {
                    if (maxEdgeLength <= 127.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 127.5
                      return 2;
                    }
                  } else {  // if xyRatio > 1.5511
                    if (maxEdgeLength <= 137.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 137.5
                      return 1;
                    }
                  }
                }
              } else {  // if area > 12067
                if (area <= 14376) {
                  if (xyRatio <= 1.1442) {
                    if (shiftAvg <= 0.19504) {
                      return 4;
                    } else {  // if shiftAvg > 0.19504
                      return 2;
                    }
                  } else {  // if xyRatio > 1.1442
                    if (maxEdgeLength <= 120.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 120.5
                      return 2;
                    }
                  }
                } else {  // if area > 14376
                  if (maxEdgeLength <= 152.5) {
                    if (xyRatio <= 1.4502) {
                      return 2;
                    } else {  // if xyRatio > 1.4502
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 152.5
                    if (shiftNcc <= 0.76023) {
                      return 3;
                    } else {  // if shiftNcc > 0.76023
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if area > 14583
      if (maxEdgeLength <= 194.5) {
        if (area <= 14870) {
          if (xyRatio <= 1.1498) {
            return 4;
          } else {  // if xyRatio > 1.1498
            if (area <= 14856) {
              if (shiftAvg <= 8.9865) {
                if (xyRatio <= 1.3835) {
                  if (area <= 14800) {
                    if (area <= 14751) {
                      return 3;
                    } else {  // if area > 14751
                      return 2;
                    }
                  } else {  // if area > 14800
                    return 3;
                  }
                } else {  // if xyRatio > 1.3835
                  if (area <= 14759) {
                    if (area <= 14736) {
                      return 3;
                    } else {  // if area > 14736
                      return 2;
                    }
                  } else {  // if area > 14759
                    if (shiftNcc <= 0.92009) {
                      return 3;
                    } else {  // if shiftNcc > 0.92009
                      return 2;
                    }
                  }
                }
              } else {  // if shiftAvg > 8.9865
                return 2;
              }
            } else {  // if area > 14856
              return 2;
            }
          }
        } else {  // if area > 14870
          if (shiftAvg <= 92.025) {
            if (shiftNcc <= 0.22077) {
              if (area <= 19342) {
                if (xyRatio <= 2.1176) {
                  if (maxEdgeLength <= 181.5) {
                    if (xyRatio <= 1.2724) {
                      return 3;
                    } else {  // if xyRatio > 1.2724
                      return 3;
                    }
                  } else {  // if maxEdgeLength > 181.5
                    if (shiftStd <= 1.6278) {
                      return 4;
                    } else {  // if shiftStd > 1.6278
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 2.1176
                  return 4;
                }
              } else {  // if area > 19342
                if (shiftStd <= 4.3121) {
                  return 4;
                } else {  // if shiftStd > 4.3121
                  return 3;
                }
              }
            } else {  // if shiftNcc > 0.22077
              if (area <= 14920) {
                if (area <= 14907) {
                  if (shiftNcc <= 0.86978) {
                    return 3;
                  } else {  // if shiftNcc > 0.86978
                    if (shiftAvg <= 4.3951) {
                      return 2;
                    } else {  // if shiftAvg > 4.3951
                      return 3;
                    }
                  }
                } else {  // if area > 14907
                  return 2;
                }
              } else {  // if area > 14920
                if (xyRatio <= 1.1607) {
                  if (xyRatio <= 1.1568) {
                    if (shiftNcc <= 0.43207) {
                      return 3;
                    } else {  // if shiftNcc > 0.43207
                      return 3;
                    }
                  } else {  // if xyRatio > 1.1568
                    if (shiftAvg <= 0.23865) {
                      return 3;
                    } else {  // if shiftAvg > 0.23865
                      return 4;
                    }
                  }
                } else {  // if xyRatio > 1.1607
                  if (avgErr <= 7.9259) {
                    if (avgErr <= 7.914) {
                      return 3;
                    } else {  // if avgErr > 7.914
                      return 4;
                    }
                  } else {  // if avgErr > 7.9259
                    if (maxEdgeLength <= 188.5) {
                      return 3;
                    } else {  // if maxEdgeLength > 188.5
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if shiftAvg > 92.025
            return 4;
          }
        }
      } else {  // if maxEdgeLength > 194.5
        if (area <= 37288) {
          if (shiftNcc <= 0.39964) {
            if (avgErr <= 15.497) {
              if (shiftStd <= 22.781) {
                if (shiftStd <= 2.5721) {
                  return 0;
                } else {  // if shiftStd > 2.5721
                  return 3;
                }
              } else {  // if shiftStd > 22.781
                return 0;
              }
            } else {  // if avgErr > 15.497
              if (area <= 36375) {
                return 0;
              } else {  // if area > 36375
                return 3;
              }
            }
          } else {  // if shiftNcc > 0.39964
            if (shiftAvg <= 0.50376) {
              return 4;
            } else {  // if shiftAvg > 0.50376
              if (avgErr <= 17.462) {
                if (avgErr <= 13.913) {
                  if (avgErr <= 13.887) {
                    if (shiftAvg <= 45.551) {
                      return 0;
                    } else {  // if shiftAvg > 45.551
                      return 0;
                    }
                  } else {  // if avgErr > 13.887
                    return 3;
                  }
                } else {  // if avgErr > 13.913
                  return 0;
                }
              } else {  // if avgErr > 17.462
                if (avgErr <= 17.532) {
                  return 3;
                } else {  // if avgErr > 17.532
                  if (maxEdgeLength <= 263) {
                    if (area <= 22776) {
                      return 0;
                    } else {  // if area > 22776
                      return 0;
                    }
                  } else {  // if maxEdgeLength > 263
                    if (shiftNcc <= 0.73162) {
                      return 3;
                    } else {  // if shiftNcc > 0.73162
                      return 0;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 37288
          if (area <= 46285) {
            if (area <= 38560) {
              if (area <= 37714) {
                return 1;
              } else {  // if area > 37714
                if (xyRatio <= 1.7024) {
                  if (shiftStd <= 1.2992) {
                    return 1;
                  } else {  // if shiftStd > 1.2992
                    return 0;
                  }
                } else {  // if xyRatio > 1.7024
                  if (avgErr <= 11.354) {
                    return 0;
                  } else {  // if avgErr > 11.354
                    return 1;
                  }
                }
              }
            } else {  // if area > 38560
              if (shiftAvg <= 3.4989) {
                return 2;
              } else {  // if shiftAvg > 3.4989
                if (shiftNcc <= 0.49262) {
                  if (shiftNcc <= 0.47533) {
                    if (shiftAvg <= 10.969) {
                      return 1;
                    } else {  // if shiftAvg > 10.969
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.47533
                    if (shiftAvg <= 9.0002) {
                      return 3;
                    } else {  // if shiftAvg > 9.0002
                      return 0;
                    }
                  }
                } else {  // if shiftNcc > 0.49262
                  if (area <= 45885) {
                    if (avgErr <= 17.941) {
                      return 1;
                    } else {  // if avgErr > 17.941
                      return 1;
                    }
                  } else {  // if area > 45885
                    if (area <= 46045) {
                      return 2;
                    } else {  // if area > 46045
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if area > 46285
            if (area <= 58278) {
              if (area <= 47104) {
                if (shiftNcc <= 0.90679) {
                  return 2;
                } else {  // if shiftNcc > 0.90679
                  return 1;
                }
              } else {  // if area > 47104
                if (maxEdgeLength <= 305.5) {
                  if (area <= 47455) {
                    if (area <= 47416) {
                      return 2;
                    } else {  // if area > 47416
                      return 4;
                    }
                  } else {  // if area > 47455
                    return 2;
                  }
                } else {  // if maxEdgeLength > 305.5
                  if (xyRatio <= 1.6505) {
                    return 3;
                  } else {  // if xyRatio > 1.6505
                    return 2;
                  }
                }
              }
            } else {  // if area > 58278
              if (shiftAvg <= 28.686) {
                if (shiftAvg <= 27.819) {
                  return 3;
                } else {  // if shiftAvg > 27.819
                  return 2;
                }
              } else {  // if shiftAvg > 28.686
                return 3;
              }
            }
          }
        }
      }
    }
  }
}

} // namespace md
