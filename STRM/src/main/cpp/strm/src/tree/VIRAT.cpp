#include "strm/tree/VIRAT.hpp"

namespace rm {

int VIRAT(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
          float shiftNcc, float avgErr, float confidence) {
  if (maxEdgeLength <= 194.5) {
    if (area <= 15950) {
      if (area <= 8356) {
        if (area <= 7986) {
          if (xyRatio <= 1.4789) {
            if (shiftStd <= 0.07049) {
              if (maxEdgeLength <= 97) {
                return 1;
              } else {  // if maxEdgeLength > 97
                if (xyRatio <= 1.4748) {
                  if (area <= 7420.5) {
                    if (xyRatio <= 1.3334) {
                      return 1;
                    } else {  // if xyRatio > 1.3334
                      return 0;
                    }
                  } else {  // if area > 7420.5
                    if (shiftStd <= 0.015482) {
                      return 0;
                    } else {  // if shiftStd > 0.015482
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 1.4748
                  return 2;
                }
              }
            } else {  // if shiftStd > 0.07049
              return 2;
            }
          } else {  // if xyRatio > 1.4789
            if (shiftNcc <= 0.88358) {
              if (shiftAvg <= 5.6607e-05) {
                if (avgErr <= 0.35093) {
                  return 0;
                } else {  // if avgErr > 0.35093
                  if (maxEdgeLength <= 108) {
                    return 4;
                  } else {  // if maxEdgeLength > 108
                    return 2;
                  }
                }
              } else {  // if shiftAvg > 5.6607e-05
                if (shiftNcc <= 0.19324) {
                  return 0;
                } else {  // if shiftNcc > 0.19324
                  if (xyRatio <= 1.8641) {
                    if (shiftNcc <= 0.19519) {
                      return 4;
                    } else {  // if shiftNcc > 0.19519
                      return 0;
                    }
                  } else {  // if xyRatio > 1.8641
                    if (shiftAvg <= 0.0051627) {
                      return 2;
                    } else {  // if shiftAvg > 0.0051627
                      return 4;
                    }
                  }
                }
              }
            } else {  // if shiftNcc > 0.88358
              return 1;
            }
          }
        } else {  // if area > 7986
          if (area <= 8308) {
            if (shiftAvg <= 0.0010504) {
              if (shiftNcc <= 0.01243) {
                if (area <= 8100) {
                  if (area <= 8050) {
                    return 1;
                  } else {  // if area > 8050
                    return 0;
                  }
                } else {  // if area > 8100
                  return 2;
                }
              } else {  // if shiftNcc > 0.01243
                if (avgErr <= 2.2111) {
                  return 0;
                } else {  // if avgErr > 2.2111
                  if (area <= 8090) {
                    return 0;
                  } else {  // if area > 8090
                    return 1;
                  }
                }
              }
            } else {  // if shiftAvg > 0.0010504
              if (avgErr <= 2.2385) {
                if (area <= 8150.5) {
                  return 1;
                } else {  // if area > 8150.5
                  if (shiftAvg <= 0.0025386) {
                    if (avgErr <= 1.9616) {
                      return 1;
                    } else {  // if avgErr > 1.9616
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.0025386
                    return 0;
                  }
                }
              } else {  // if avgErr > 2.2385
                if (shiftStd <= 0.012716) {
                  if (area <= 8249) {
                    return 2;
                  } else {  // if area > 8249
                    return 1;
                  }
                } else {  // if shiftStd > 0.012716
                  if (area <= 8227.5) {
                    if (avgErr <= 3.7708) {
                      return 0;
                    } else {  // if avgErr > 3.7708
                      return 2;
                    }
                  } else {  // if area > 8227.5
                    if (shiftNcc <= 0.3157) {
                      return 1;
                    } else {  // if shiftNcc > 0.3157
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if area > 8308
            if (maxEdgeLength <= 107.5) {
              if (shiftNcc <= 0.054421) {
                return 4;
              } else {  // if shiftNcc > 0.054421
                return 2;
              }
            } else {  // if maxEdgeLength > 107.5
              if (shiftAvg <= 0.00013104) {
                return 2;
              } else {  // if shiftAvg > 0.00013104
                if (shiftNcc <= 0.56739) {
                  return 1;
                } else {  // if shiftNcc > 0.56739
                  return 0;
                }
              }
            }
          }
        }
      } else {  // if area > 8356
        if (avgErr <= 0.57643) {
          if (shiftNcc <= 0.16621) {
            if (shiftStd <= 0.019599) {
              if (xyRatio <= 1.84) {
                if (maxEdgeLength <= 128.5) {
                  return 1;
                } else {  // if maxEdgeLength > 128.5
                  if (shiftNcc <= -0.0047944) {
                    if (xyRatio <= 1.5026) {
                      return 4;
                    } else {  // if xyRatio > 1.5026
                      return 1;
                    }
                  } else {  // if shiftNcc > -0.0047944
                    if (avgErr <= 0.48578) {
                      return 4;
                    } else {  // if avgErr > 0.48578
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 1.84
                return 1;
              }
            } else {  // if shiftStd > 0.019599
              if (area <= 15800) {
                if (xyRatio <= 2.0297) {
                  return 4;
                } else {  // if xyRatio > 2.0297
                  if (shiftStd <= 0.039729) {
                    return 4;
                  } else {  // if shiftStd > 0.039729
                    return 1;
                  }
                }
              } else {  // if area > 15800
                return 1;
              }
            }
          } else {  // if shiftNcc > 0.16621
            if (avgErr <= 0.44156) {
              return 1;
            } else {  // if avgErr > 0.44156
              if (shiftStd <= 0.028785) {
                return 4;
              } else {  // if shiftStd > 0.028785
                if (xyRatio <= 1.7747) {
                  return 1;
                } else {  // if xyRatio > 1.7747
                  return 2;
                }
              }
            }
          }
        } else {  // if avgErr > 0.57643
          if (avgErr <= 6.2811) {
            if (area <= 9646) {
              if (shiftStd <= 0.017851) {
                if (shiftStd <= 0.017273) {
                  if (shiftAvg <= 8.9992e-05) {
                    if (avgErr <= 1.5482) {
                      return 3;
                    } else {  // if avgErr > 1.5482
                      return 0;
                    }
                  } else {  // if shiftAvg > 8.9992e-05
                    if (xyRatio <= 1.3196) {
                      return 1;
                    } else {  // if xyRatio > 1.3196
                      return 1;
                    }
                  }
                } else {  // if shiftStd > 0.017273
                  return 3;
                }
              } else {  // if shiftStd > 0.017851
                if (area <= 8700.5) {
                  if (area <= 8688.5) {
                    if (shiftStd <= 0.059514) {
                      return 1;
                    } else {  // if shiftStd > 0.059514
                      return 1;
                    }
                  } else {  // if area > 8688.5
                    if (area <= 8695) {
                      return 4;
                    } else {  // if area > 8695
                      return 3;
                    }
                  }
                } else {  // if area > 8700.5
                  if (xyRatio <= 1.1819) {
                    if (maxEdgeLength <= 104) {
                      return 1;
                    } else {  // if maxEdgeLength > 104
                      return 3;
                    }
                  } else {  // if xyRatio > 1.1819
                    if (maxEdgeLength <= 123) {
                      return 1;
                    } else {  // if maxEdgeLength > 123
                      return 1;
                    }
                  }
                }
              }
            } else {  // if area > 9646
              if (shiftNcc <= 0.92224) {
                if (shiftNcc <= 0.0033007) {
                  if (shiftNcc <= -0.0059011) {
                    if (maxEdgeLength <= 151) {
                      return 1;
                    } else {  // if maxEdgeLength > 151
                      return 4;
                    }
                  } else {  // if shiftNcc > -0.0059011
                    return 4;
                  }
                } else {  // if shiftNcc > 0.0033007
                  if (shiftStd <= 0.56643) {
                    if (shiftAvg <= 0.031398) {
                      return 1;
                    } else {  // if shiftAvg > 0.031398
                      return 1;
                    }
                  } else {  // if shiftStd > 0.56643
                    if (avgErr <= 4.6309) {
                      return 1;
                    } else {  // if avgErr > 4.6309
                      return 4;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.92224
                if (shiftStd <= 0.001337) {
                  return 1;
                } else {  // if shiftStd > 0.001337
                  if (area <= 12958) {
                    return 1;
                  } else {  // if area > 12958
                    return 4;
                  }
                }
              }
            }
          } else {  // if avgErr > 6.2811
            if (shiftStd <= 0.11598) {
              return 4;
            } else {  // if shiftStd > 0.11598
              if (xyRatio <= 1.239) {
                return 4;
              } else {  // if xyRatio > 1.239
                if (xyRatio <= 2.3846) {
                  if (shiftAvg <= 0.23461) {
                    if (maxEdgeLength <= 139.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 139.5
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.23461
                    if (shiftStd <= 0.32981) {
                      return 1;
                    } else {  // if shiftStd > 0.32981
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 2.3846
                  return 4;
                }
              }
            }
          }
        }
      }
    } else {  // if area > 15950
      if (area <= 19128) {
        if (area <= 18684) {
          if (area <= 16353) {
            if (avgErr <= 4.6501) {
              if (shiftNcc <= -0.024982) {
                return 4;
              } else {  // if shiftNcc > -0.024982
                if (shiftAvg <= 0.25888) {
                  if (shiftAvg <= 0.01088) {
                    if (area <= 16324) {
                      return 4;
                    } else {  // if area > 16324
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.01088
                    if (area <= 15992) {
                      return 2;
                    } else {  // if area > 15992
                      return 2;
                    }
                  }
                } else {  // if shiftAvg > 0.25888
                  if (area <= 16194) {
                    return 1;
                  } else {  // if area > 16194
                    return 4;
                  }
                }
              }
            } else {  // if avgErr > 4.6501
              if (avgErr <= 8.5407) {
                if (shiftStd <= 4.9934) {
                  if (avgErr <= 5.0305) {
                    if (area <= 16168) {
                      return 1;
                    } else {  // if area > 16168
                      return 2;
                    }
                  } else {  // if avgErr > 5.0305
                    return 2;
                  }
                } else {  // if shiftStd > 4.9934
                  if (maxEdgeLength <= 176) {
                    return 4;
                  } else {  // if maxEdgeLength > 176
                    return 1;
                  }
                }
              } else {  // if avgErr > 8.5407
                if (xyRatio <= 1.8176) {
                  return 1;
                } else {  // if xyRatio > 1.8176
                  if (avgErr <= 10.205) {
                    return 4;
                  } else {  // if avgErr > 10.205
                    return 2;
                  }
                }
              }
            }
          } else {  // if area > 16353
            if (avgErr <= 1.4723) {
              if (xyRatio <= 1.8113) {
                if (shiftStd <= 0.0045983) {
                  if (shiftNcc <= -0.060189) {
                    if (shiftNcc <= -0.082275) {
                      return 2;
                    } else {  // if shiftNcc > -0.082275
                      return 4;
                    }
                  } else {  // if shiftNcc > -0.060189
                    if (shiftStd <= 0.0012779) {
                      return 2;
                    } else {  // if shiftStd > 0.0012779
                      return 2;
                    }
                  }
                } else {  // if shiftStd > 0.0045983
                  if (avgErr <= 0.20468) {
                    return 4;
                  } else {  // if avgErr > 0.20468
                    if (maxEdgeLength <= 174.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 174.5
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 1.8113
                if (shiftNcc <= 0.21393) {
                  if (shiftNcc <= 0.038506) {
                    return 4;
                  } else {  // if shiftNcc > 0.038506
                    if (area <= 17057) {
                      return 4;
                    } else {  // if area > 17057
                      return 2;
                    }
                  }
                } else {  // if shiftNcc > 0.21393
                  if (area <= 16470) {
                    return 4;
                  } else {  // if area > 16470
                    return 2;
                  }
                }
              }
            } else {  // if avgErr > 1.4723
              if (xyRatio <= 1.2755) {
                if (maxEdgeLength <= 149.5) {
                  if (shiftNcc <= 0.056683) {
                    return 4;
                  } else {  // if shiftNcc > 0.056683
                    if (area <= 16644) {
                      return 4;
                    } else {  // if area > 16644
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 149.5
                  return 4;
                }
              } else {  // if xyRatio > 1.2755
                if (shiftNcc <= 0.61765) {
                  if (shiftNcc <= 0.61685) {
                    if (shiftNcc <= 0.44688) {
                      return 2;
                    } else {  // if shiftNcc > 0.44688
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.61685
                    return 4;
                  }
                } else {  // if shiftNcc > 0.61765
                  if (avgErr <= 11.49) {
                    if (area <= 16594) {
                      return 2;
                    } else {  // if area > 16594
                      return 2;
                    }
                  } else {  // if avgErr > 11.49
                    if (shiftStd <= 1.7699) {
                      return 4;
                    } else {  // if shiftStd > 1.7699
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 18684
          if (shiftNcc <= 0.14834) {
            if (shiftNcc <= 0.11028) {
              if (shiftNcc <= 0.01986) {
                if (xyRatio <= 1.4608) {
                  if (shiftAvg <= 0.00027786) {
                    return 4;
                  } else {  // if shiftAvg > 0.00027786
                    return 2;
                  }
                } else {  // if xyRatio > 1.4608
                  if (shiftAvg <= 0.0081844) {
                    return 3;
                  } else {  // if shiftAvg > 0.0081844
                    if (area <= 19002) {
                      return 2;
                    } else {  // if area > 19002
                      return 3;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.01986
                if (xyRatio <= 1.8106) {
                  if (avgErr <= 0.2478) {
                    return 3;
                  } else {  // if avgErr > 0.2478
                    if (shiftNcc <= 0.068087) {
                      return 2;
                    } else {  // if shiftNcc > 0.068087
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 1.8106
                  return 4;
                }
              }
            } else {  // if shiftNcc > 0.11028
              if (shiftStd <= 1.2548) {
                return 3;
              } else {  // if shiftStd > 1.2548
                if (shiftNcc <= 0.14542) {
                  return 3;
                } else {  // if shiftNcc > 0.14542
                  return 2;
                }
              }
            }
          } else {  // if shiftNcc > 0.14834
            if (shiftAvg <= 8.5928) {
              if (shiftStd <= 0.0043819) {
                return 4;
              } else {  // if shiftStd > 0.0043819
                if (area <= 18687) {
                  return 3;
                } else {  // if area > 18687
                  if (shiftStd <= 2.0469) {
                    if (shiftStd <= 1.7373) {
                      return 2;
                    } else {  // if shiftStd > 1.7373
                      return 4;
                    }
                  } else {  // if shiftStd > 2.0469
                    return 2;
                  }
                }
              }
            } else {  // if shiftAvg > 8.5928
              if (maxEdgeLength <= 156.5) {
                return 4;
              } else {  // if maxEdgeLength > 156.5
                if (area <= 19034) {
                  return 3;
                } else {  // if area > 19034
                  return 4;
                }
              }
            }
          }
        }
      } else {  // if area > 19128
        if (area <= 22069) {
          if (area <= 19303) {
            if (xyRatio <= 1.5588) {
              if (avgErr <= 11.451) {
                if (maxEdgeLength <= 160.5) {
                  if (xyRatio <= 1.3005) {
                    return 4;
                  } else {  // if xyRatio > 1.3005
                    return 3;
                  }
                } else {  // if maxEdgeLength > 160.5
                  if (area <= 19146) {
                    return 4;
                  } else {  // if area > 19146
                    if (shiftAvg <= 0.094396) {
                      return 2;
                    } else {  // if shiftAvg > 0.094396
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 11.451
                return 3;
              }
            } else {  // if xyRatio > 1.5588
              if (area <= 19260) {
                if (shiftStd <= 0.01279) {
                  if (maxEdgeLength <= 175) {
                    return 4;
                  } else {  // if maxEdgeLength > 175
                    return 2;
                  }
                } else {  // if shiftStd > 0.01279
                  if (avgErr <= 6.303) {
                    return 3;
                  } else {  // if avgErr > 6.303
                    if (area <= 19167) {
                      return 2;
                    } else {  // if area > 19167
                      return 3;
                    }
                  }
                }
              } else {  // if area > 19260
                if (shiftNcc <= 0.42294) {
                  if (avgErr <= 1.4794) {
                    return 4;
                  } else {  // if avgErr > 1.4794
                    return 2;
                  }
                } else {  // if shiftNcc > 0.42294
                  return 3;
                }
              }
            }
          } else {  // if area > 19303
            if (area <= 21592) {
              if (xyRatio <= 1.1641) {
                if (shiftStd <= 0.47991) {
                  return 3;
                } else {  // if shiftStd > 0.47991
                  return 4;
                }
              } else {  // if xyRatio > 1.1641
                if (xyRatio <= 1.8732) {
                  if (area <= 19491) {
                    if (maxEdgeLength <= 158) {
                      return 2;
                    } else {  // if maxEdgeLength > 158
                      return 3;
                    }
                  } else {  // if area > 19491
                    if (shiftNcc <= -0.01067) {
                      return 3;
                    } else {  // if shiftNcc > -0.01067
                      return 3;
                    }
                  }
                } else {  // if xyRatio > 1.8732
                  if (shiftStd <= 9.5931) {
                    return 4;
                  } else {  // if shiftStd > 9.5931
                    return 3;
                  }
                }
              }
            } else {  // if area > 21592
              if (maxEdgeLength <= 182.5) {
                if (shiftAvg <= 11.59) {
                  if (xyRatio <= 1.2756) {
                    if (shiftStd <= 0.56221) {
                      return 4;
                    } else {  // if shiftStd > 0.56221
                      return 3;
                    }
                  } else {  // if xyRatio > 1.2756
                    if (shiftStd <= 2.9459) {
                      return 3;
                    } else {  // if shiftStd > 2.9459
                      return 3;
                    }
                  }
                } else {  // if shiftAvg > 11.59
                  return 4;
                }
              } else {  // if maxEdgeLength > 182.5
                if (xyRatio <= 1.557) {
                  return 4;
                } else {  // if xyRatio > 1.557
                  if (area <= 21632) {
                    return 4;
                  } else {  // if area > 21632
                    if (shiftAvg <= 1.2568) {
                      return 3;
                    } else {  // if shiftAvg > 1.2568
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if area > 22069
          if (avgErr <= 10.412) {
            if (area <= 22478) {
              if (shiftNcc <= 0.74301) {
                if (area <= 22456) {
                  if (shiftAvg <= 0.13538) {
                    return 4;
                  } else {  // if shiftAvg > 0.13538
                    if (shiftAvg <= 0.16186) {
                      return 3;
                    } else {  // if shiftAvg > 0.16186
                      return 4;
                    }
                  }
                } else {  // if area > 22456
                  return 3;
                }
              } else {  // if shiftNcc > 0.74301
                if (maxEdgeLength <= 186) {
                  if (area <= 22347) {
                    return 4;
                  } else {  // if area > 22347
                    return 3;
                  }
                } else {  // if maxEdgeLength > 186
                  return 3;
                }
              }
            } else {  // if area > 22478
              return 4;
            }
          } else {  // if avgErr > 10.412
            return 3;
          }
        }
      }
    }
  } else {  // if maxEdgeLength > 194.5
    if (area <= 31521) {
      if (avgErr <= 5.2088) {
        if (area <= 23637) {
          if (area <= 23428) {
            if (xyRatio <= 1.9024) {
              if (xyRatio <= 1.885) {
                if (maxEdgeLength <= 200.5) {
                  return 0;
                } else {  // if maxEdgeLength > 200.5
                  if (area <= 22634) {
                    if (shiftAvg <= 0.026565) {
                      return 0;
                    } else {  // if shiftAvg > 0.026565
                      return 4;
                    }
                  } else {  // if area > 22634
                    return 0;
                  }
                }
              } else {  // if xyRatio > 1.885
                return 4;
              }
            } else {  // if xyRatio > 1.9024
              if (maxEdgeLength <= 196) {
                if (shiftStd <= 0.34439) {
                  return 3;
                } else {  // if shiftStd > 0.34439
                  return 0;
                }
              } else {  // if maxEdgeLength > 196
                return 0;
              }
            }
          } else {  // if area > 23428
            if (avgErr <= 3.826) {
              if (area <= 23432) {
                return 4;
              } else {  // if area > 23432
                return 0;
              }
            } else {  // if avgErr > 3.826
              return 4;
            }
          }
        } else {  // if area > 23637
          if (area <= 30305) {
            if (shiftAvg <= 0.11949) {
              return 0;
            } else {  // if shiftAvg > 0.11949
              if (shiftAvg <= 0.12665) {
                return 4;
              } else {  // if shiftAvg > 0.12665
                return 0;
              }
            }
          } else {  // if area > 30305
            if (shiftAvg <= 0.020585) {
              return 4;
            } else {  // if shiftAvg > 0.020585
              return 0;
            }
          }
        }
      } else {  // if avgErr > 5.2088
        if (avgErr <= 5.619) {
          if (shiftAvg <= 0.08813) {
            if (xyRatio <= 1.8363) {
              return 0;
            } else {  // if xyRatio > 1.8363
              return 3;
            }
          } else {  // if shiftAvg > 0.08813
            if (shiftStd <= 0.45151) {
              return 4;
            } else {  // if shiftStd > 0.45151
              if (xyRatio <= 1.862) {
                return 0;
              } else {  // if xyRatio > 1.862
                if (shiftAvg <= 0.77562) {
                  return 4;
                } else {  // if shiftAvg > 0.77562
                  return 0;
                }
              }
            }
          }
        } else {  // if avgErr > 5.619
          if (shiftStd <= 6.3943) {
            if (avgErr <= 9.7726) {
              if (maxEdgeLength <= 222) {
                if (area <= 25886) {
                  if (area <= 25750) {
                    if (shiftStd <= 2.2085) {
                      return 0;
                    } else {  // if shiftStd > 2.2085
                      return 0;
                    }
                  } else {  // if area > 25750
                    return 4;
                  }
                } else {  // if area > 25886
                  return 0;
                }
              } else {  // if maxEdgeLength > 222
                if (maxEdgeLength <= 225) {
                  return 4;
                } else {  // if maxEdgeLength > 225
                  return 0;
                }
              }
            } else {  // if avgErr > 9.7726
              if (shiftNcc <= 0.51852) {
                if (shiftAvg <= 0.3704) {
                  if (avgErr <= 10.011) {
                    if (area <= 21210) {
                      return 3;
                    } else {  // if area > 21210
                      return 4;
                    }
                  } else {  // if avgErr > 10.011
                    return 0;
                  }
                } else {  // if shiftAvg > 0.3704
                  return 4;
                }
              } else {  // if shiftNcc > 0.51852
                return 0;
              }
            }
          } else {  // if shiftStd > 6.3943
            return 0;
          }
        }
      }
    } else {  // if area > 31521
      if (maxEdgeLength <= 259) {
        if (shiftAvg <= 0.0018585) {
          return 1;
        } else {  // if shiftAvg > 0.0018585
          return 4;
        }
      } else {  // if maxEdgeLength > 259
        if (shiftAvg <= 0.26531) {
          return 1;
        } else {  // if shiftAvg > 0.26531
          return 0;
        }
      }
    }
  }
}

} // namespace rm
