#include "strm/tree/VIRAT.hpp"

namespace rm {

int VIRAT(float maxEdgeLength, float area, float HWRatio, float shiftAvg, float shiftStd,
          float shiftNcc, float avgErr, float confidence) {
  if (area <= 19502) {
    if (area <= 16425) {
      if (area <= 8600) {
        if (HWRatio <= 1.1786) {
          return 2;
        } else {  // if HWRatio > 1.1786
          if (area <= 7301.5) {
            if (shiftAvg <= 5.3618e-05) {
              return 4;
            } else {  // if shiftAvg > 5.3618e-05
              if (HWRatio <= 1.3379) {
                return 1;
              } else {  // if HWRatio > 1.3379
                if (shiftNcc <= 0.84158) {
                  if (avgErr <= 1.6077) {
                    if (shiftAvg <= 0.00039183) {
                      return 0;
                    } else {  // if shiftAvg > 0.00039183
                      return 1;
                    }
                  } else {  // if avgErr > 1.6077
                    if (HWRatio <= 1.8995) {
                      return 0;
                    } else {  // if HWRatio > 1.8995
                      return 0;
                    }
                  }
                } else {  // if shiftNcc > 0.84158
                  if (shiftAvg <= 0.021533) {
                    return 4;
                  } else {  // if shiftAvg > 0.021533
                    return 1;
                  }
                }
              }
            }
          } else {  // if area > 7301.5
            if (shiftNcc <= 0.23937) {
              if (shiftAvg <= 1.3277e-05) {
                return 2;
              } else {  // if shiftAvg > 1.3277e-05
                if (shiftNcc <= -0.0024471) {
                  if (area <= 7827.5) {
                    if (HWRatio <= 1.284) {
                      return 4;
                    } else {  // if HWRatio > 1.284
                      return 2;
                    }
                  } else {  // if area > 7827.5
                    if (shiftAvg <= 0.0040148) {
                      return 0;
                    } else {  // if shiftAvg > 0.0040148
                      return 4;
                    }
                  }
                } else {  // if shiftNcc > -0.0024471
                  if (area <= 8435) {
                    if (shiftAvg <= 0.02887) {
                      return 0;
                    } else {  // if shiftAvg > 0.02887
                      return 2;
                    }
                  } else {  // if area > 8435
                    if (HWRatio <= 1.2885) {
                      return 1;
                    } else {  // if HWRatio > 1.2885
                      return 0;
                    }
                  }
                }
              }
            } else {  // if shiftNcc > 0.23937
              if (HWRatio <= 1.4) {
                if (shiftAvg <= 0.013891) {
                  if (maxEdgeLength <= 107.5) {
                    if (avgErr <= 1.694) {
                      return 0;
                    } else {  // if avgErr > 1.694
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 107.5
                    return 1;
                  }
                } else {  // if shiftAvg > 0.013891
                  if (maxEdgeLength <= 103.5) {
                    if (shiftAvg <= 0.024238) {
                      return 0;
                    } else {  // if shiftAvg > 0.024238
                      return 4;
                    }
                  } else {  // if maxEdgeLength > 103.5
                    if (shiftAvg <= 0.019888) {
                      return 4;
                    } else {  // if shiftAvg > 0.019888
                      return 2;
                    }
                  }
                }
              } else {  // if HWRatio > 1.4
                if (avgErr <= 3.3453) {
                  if (shiftNcc <= 0.59209) {
                    if (HWRatio <= 1.6716) {
                      return 0;
                    } else {  // if HWRatio > 1.6716
                      return 0;
                    }
                  } else {  // if shiftNcc > 0.59209
                    if (shiftAvg <= 0.026754) {
                      return 2;
                    } else {  // if shiftAvg > 0.026754
                      return 0;
                    }
                  }
                } else {  // if avgErr > 3.3453
                  if (shiftNcc <= 0.64004) {
                    if (shiftAvg <= 0.010709) {
                      return 4;
                    } else {  // if shiftAvg > 0.010709
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.64004
                    if (avgErr <= 3.765) {
                      return 2;
                    } else {  // if avgErr > 3.765
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 8600
        if (avgErr <= 1.1801) {
          if (maxEdgeLength <= 132) {
            if (avgErr <= 1.1019) {
              if (shiftStd <= 0.006702) {
                return 1;
              } else {  // if shiftStd > 0.006702
                return 3;
              }
            } else {  // if avgErr > 1.1019
              return 3;
            }
          } else {  // if maxEdgeLength > 132
            if (area <= 15992) {
              if (shiftStd <= 0.0094481) {
                if (area <= 14626) {
                  return 4;
                } else {  // if area > 14626
                  if (shiftAvg <= 0.00022363) {
                    return 4;
                  } else {  // if shiftAvg > 0.00022363
                    if (maxEdgeLength <= 151.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 151.5
                      return 1;
                    }
                  }
                }
              } else {  // if shiftStd > 0.0094481
                if (shiftAvg <= 0.0032211) {
                  if (avgErr <= 0.058062) {
                    return 1;
                  } else {  // if avgErr > 0.058062
                    if (area <= 15948) {
                      return 4;
                    } else {  // if area > 15948
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.0032211
                  if (shiftAvg <= 0.015244) {
                    if (shiftStd <= 0.025404) {
                      return 4;
                    } else {  // if shiftStd > 0.025404
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.015244
                    return 4;
                  }
                }
              }
            } else {  // if area > 15992
              if (area <= 16095) {
                if (maxEdgeLength <= 173.5) {
                  return 1;
                } else {  // if maxEdgeLength > 173.5
                  return 4;
                }
              } else {  // if area > 16095
                if (shiftNcc <= 0.029433) {
                  if (shiftAvg <= 0.00017681) {
                    return 1;
                  } else {  // if shiftAvg > 0.00017681
                    return 4;
                  }
                } else {  // if shiftNcc > 0.029433
                  if (maxEdgeLength <= 162) {
                    if (shiftAvg <= 0.28247) {
                      return 1;
                    } else {  // if shiftAvg > 0.28247
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 162
                    if (shiftStd <= 0.006477) {
                      return 2;
                    } else {  // if shiftStd > 0.006477
                      return 4;
                    }
                  }
                }
              }
            }
          }
        } else {  // if avgErr > 1.1801
          if (shiftAvg <= 0.027854) {
            if (area <= 8805) {
              if (avgErr <= 4.0901) {
                if (shiftStd <= 0.0038614) {
                  if (avgErr <= 1.4493) {
                    return 2;
                  } else {  // if avgErr > 1.4493
                    return 1;
                  }
                } else {  // if shiftStd > 0.0038614
                  if (HWRatio <= 1.4675) {
                    if (avgErr <= 1.8922) {
                      return 0;
                    } else {  // if avgErr > 1.8922
                      return 0;
                    }
                  } else {  // if HWRatio > 1.4675
                    if (area <= 8693.5) {
                      return 1;
                    } else {  // if area > 8693.5
                      return 3;
                    }
                  }
                }
              } else {  // if avgErr > 4.0901
                return 1;
              }
            } else {  // if area > 8805
              if (area <= 10544) {
                if (shiftAvg <= 0.014104) {
                  if (shiftNcc <= 0.35174) {
                    if (shiftNcc <= 0.15434) {
                      return 1;
                    } else {  // if shiftNcc > 0.15434
                      return 1;
                    }
                  } else {  // if shiftNcc > 0.35174
                    return 1;
                  }
                } else {  // if shiftAvg > 0.014104
                  if (avgErr <= 3.3998) {
                    if (area <= 9917.5) {
                      return 1;
                    } else {  // if area > 9917.5
                      return 4;
                    }
                  } else {  // if avgErr > 3.3998
                    if (HWRatio <= 1.2711) {
                      return 3;
                    } else {  // if HWRatio > 1.2711
                      return 4;
                    }
                  }
                }
              } else {  // if area > 10544
                if (shiftAvg <= 0.0020136) {
                  if (shiftAvg <= 0.00049389) {
                    return 1;
                  } else {  // if shiftAvg > 0.00049389
                    if (shiftStd <= 0.013651) {
                      return 1;
                    } else {  // if shiftStd > 0.013651
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.0020136
                  if (maxEdgeLength <= 165.5) {
                    if (HWRatio <= 1.2393) {
                      return 1;
                    } else {  // if HWRatio > 1.2393
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 165.5
                    return 4;
                  }
                }
              }
            }
          } else {  // if shiftAvg > 0.027854
            if (shiftAvg <= 18.491) {
              if (shiftNcc <= 0.26028) {
                if (shiftNcc <= 0.25745) {
                  if (shiftAvg <= 0.076057) {
                    return 1;
                  } else {  // if shiftAvg > 0.076057
                    if (shiftStd <= 3.7665) {
                      return 1;
                    } else {  // if shiftStd > 3.7665
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.25745
                  return 4;
                }
              } else {  // if shiftNcc > 0.26028
                if (avgErr <= 6.7538) {
                  if (shiftAvg <= 0.31898) {
                    if (shiftStd <= 0.02364) {
                      return 1;
                    } else {  // if shiftStd > 0.02364
                      return 1;
                    }
                  } else {  // if shiftAvg > 0.31898
                    if (shiftNcc <= 0.44966) {
                      return 1;
                    } else {  // if shiftNcc > 0.44966
                      return 1;
                    }
                  }
                } else {  // if avgErr > 6.7538
                  if (area <= 16282) {
                    if (HWRatio <= 2.1696) {
                      return 1;
                    } else {  // if HWRatio > 2.1696
                      return 1;
                    }
                  } else {  // if area > 16282
                    if (shiftStd <= 1.315) {
                      return 1;
                    } else {  // if shiftStd > 1.315
                      return 2;
                    }
                  }
                }
              }
            } else {  // if shiftAvg > 18.491
              if (area <= 13629) {
                return 1;
              } else {  // if area > 13629
                if (HWRatio <= 1.614) {
                  return 4;
                } else {  // if HWRatio > 1.614
                  return 2;
                }
              }
            }
          }
        }
      }
    } else {  // if area > 16425
      if (avgErr <= 1.1394) {
        if (maxEdgeLength <= 179.5) {
          if (area <= 16724) {
            if (shiftStd <= 0.022475) {
              if (shiftStd <= 0.014035) {
                return 4;
              } else {  // if shiftStd > 0.014035
                if (area <= 16659) {
                  return 2;
                } else {  // if area > 16659
                  return 1;
                }
              }
            } else {  // if shiftStd > 0.022475
              return 4;
            }
          } else {  // if area > 16724
            if (maxEdgeLength <= 177.5) {
              if (shiftNcc <= 0.018275) {
                if (shiftNcc <= -0.051286) {
                  if (shiftNcc <= -0.10503) {
                    return 4;
                  } else {  // if shiftNcc > -0.10503
                    if (HWRatio <= 1.6067) {
                      return 2;
                    } else {  // if HWRatio > 1.6067
                      return 2;
                    }
                  }
                } else {  // if shiftNcc > -0.051286
                  if (shiftAvg <= 0.0071289) {
                    if (HWRatio <= 1.5602) {
                      return 2;
                    } else {  // if HWRatio > 1.5602
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.0071289
                    return 4;
                  }
                }
              } else {  // if shiftNcc > 0.018275
                if (shiftNcc <= 0.025566) {
                  return 2;
                } else {  // if shiftNcc > 0.025566
                  if (area <= 19194) {
                    if (avgErr <= 0.55064) {
                      return 2;
                    } else {  // if avgErr > 0.55064
                      return 4;
                    }
                  } else {  // if area > 19194
                    if (HWRatio <= 1.3087) {
                      return 2;
                    } else {  // if HWRatio > 1.3087
                      return 4;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 177.5
              if (HWRatio <= 1.6683) {
                if (shiftStd <= 0.013405) {
                  return 2;
                } else {  // if shiftStd > 0.013405
                  return 4;
                }
              } else {  // if HWRatio > 1.6683
                return 2;
              }
            }
          }
        } else {  // if maxEdgeLength > 179.5
          if (shiftNcc <= 0.21393) {
            if (shiftStd <= 0.021173) {
              if (shiftAvg <= 0.00057393) {
                return 4;
              } else {  // if shiftAvg > 0.00057393
                if (HWRatio <= 1.887) {
                  return 2;
                } else {  // if HWRatio > 1.887
                  return 4;
                }
              }
            } else {  // if shiftStd > 0.021173
              return 4;
            }
          } else {  // if shiftNcc > 0.21393
            return 2;
          }
        }
      } else {  // if avgErr > 1.1394
        if (maxEdgeLength <= 194.5) {
          if (area <= 19210) {
            if (area <= 16912) {
              if (avgErr <= 10.775) {
                if (avgErr <= 7.5435) {
                  if (area <= 16828) {
                    if (maxEdgeLength <= 143.5) {
                      return 4;
                    } else {  // if maxEdgeLength > 143.5
                      return 2;
                    }
                  } else {  // if area > 16828
                    if (shiftNcc <= 0.55771) {
                      return 2;
                    } else {  // if shiftNcc > 0.55771
                      return 1;
                    }
                  }
                } else {  // if avgErr > 7.5435
                  if (shiftAvg <= 0.53267) {
                    return 4;
                  } else {  // if shiftAvg > 0.53267
                    if (shiftStd <= 1.6142) {
                      return 1;
                    } else {  // if shiftStd > 1.6142
                      return 2;
                    }
                  }
                }
              } else {  // if avgErr > 10.775
                return 2;
              }
            } else {  // if area > 16912
              if (HWRatio <= 1.293) {
                if (maxEdgeLength <= 153.5) {
                  if (shiftNcc <= 0.11212) {
                    if (shiftStd <= 0.51226) {
                      return 2;
                    } else {  // if shiftStd > 0.51226
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.11212
                    return 2;
                  }
                } else {  // if maxEdgeLength > 153.5
                  return 4;
                }
              } else {  // if HWRatio > 1.293
                if (shiftNcc <= -0.025241) {
                  return 4;
                } else {  // if shiftNcc > -0.025241
                  if (HWRatio <= 2.1043) {
                    if (shiftAvg <= 0.028913) {
                      return 2;
                    } else {  // if shiftAvg > 0.028913
                      return 2;
                    }
                  } else {  // if HWRatio > 2.1043
                    return 4;
                  }
                }
              }
            }
          } else {  // if area > 19210
            if (area <= 19240) {
              if (avgErr <= 7.8013) {
                return 3;
              } else {  // if avgErr > 7.8013
                return 4;
              }
            } else {  // if area > 19240
              if (area <= 19362) {
                if (shiftNcc <= 0.91316) {
                  if (shiftNcc <= 0.12103) {
                    if (shiftNcc <= 0.053612) {
                      return 2;
                    } else {  // if shiftNcc > 0.053612
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.12103
                    return 2;
                  }
                } else {  // if shiftNcc > 0.91316
                  return 4;
                }
              } else {  // if area > 19362
                if (HWRatio <= 1.8676) {
                  if (avgErr <= 3.8244) {
                    return 2;
                  } else {  // if avgErr > 3.8244
                    if (shiftAvg <= 0.34206) {
                      return 3;
                    } else {  // if shiftAvg > 0.34206
                      return 2;
                    }
                  }
                } else {  // if HWRatio > 1.8676
                  return 4;
                }
              }
            }
          }
        } else {  // if maxEdgeLength > 194.5
          if (HWRatio <= 2.2841) {
            if (area <= 19294) {
              if (avgErr <= 5.4924) {
                if (shiftAvg <= 0.41067) {
                  return 0;
                } else {  // if shiftAvg > 0.41067
                  return 4;
                }
              } else {  // if avgErr > 5.4924
                return 0;
              }
            } else {  // if area > 19294
              if (shiftNcc <= 0.18708) {
                return 2;
              } else {  // if shiftNcc > 0.18708
                if (shiftStd <= 1.3909) {
                  if (maxEdgeLength <= 202) {
                    return 4;
                  } else {  // if maxEdgeLength > 202
                    return 0;
                  }
                } else {  // if shiftStd > 1.3909
                  return 0;
                }
              }
            }
          } else {  // if HWRatio > 2.2841
            if (shiftNcc <= 0.28449) {
              return 0;
            } else {  // if shiftNcc > 0.28449
              return 4;
            }
          }
        }
      }
    }
  } else {  // if area > 19502
    if (maxEdgeLength <= 194.5) {
      if (area <= 23307) {
        if (area <= 20034) {
          if (HWRatio <= 1.5644) {
            if (shiftAvg <= 0.0035025) {
              if (shiftAvg <= 0.00072785) {
                if (area <= 19644) {
                  if (avgErr <= 0.28281) {
                    return 2;
                  } else {  // if avgErr > 0.28281
                    return 4;
                  }
                } else {  // if area > 19644
                  if (shiftStd <= 0.0075712) {
                    return 3;
                  } else {  // if shiftStd > 0.0075712
                    if (maxEdgeLength <= 167.5) {
                      return 2;
                    } else {  // if maxEdgeLength > 167.5
                      return 3;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.00072785
                if (area <= 19602) {
                  return 3;
                } else {  // if area > 19602
                  if (area <= 19921) {
                    if (shiftAvg <= 0.0030111) {
                      return 4;
                    } else {  // if shiftAvg > 0.0030111
                      return 2;
                    }
                  } else {  // if area > 19921
                    return 2;
                  }
                }
              }
            } else {  // if shiftAvg > 0.0035025
              if (HWRatio <= 1.1784) {
                return 4;
              } else {  // if HWRatio > 1.1784
                if (shiftNcc <= -0.010031) {
                  return 4;
                } else {  // if shiftNcc > -0.010031
                  if (shiftNcc <= 0.62967) {
                    if (shiftNcc <= 0.44816) {
                      return 3;
                    } else {  // if shiftNcc > 0.44816
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.62967
                    if (shiftNcc <= 0.77304) {
                      return 4;
                    } else {  // if shiftNcc > 0.77304
                      return 3;
                    }
                  }
                }
              }
            }
          } else {  // if HWRatio > 1.5644
            if (maxEdgeLength <= 187.5) {
              if (avgErr <= 0.38738) {
                if (shiftNcc <= 0.019206) {
                  return 3;
                } else {  // if shiftNcc > 0.019206
                  return 4;
                }
              } else {  // if avgErr > 0.38738
                if (area <= 19628) {
                  if (avgErr <= 6.4896) {
                    return 3;
                  } else {  // if avgErr > 6.4896
                    return 2;
                  }
                } else {  // if area > 19628
                  if (shiftAvg <= 0.86172) {
                    if (avgErr <= 5.9678) {
                      return 2;
                    } else {  // if avgErr > 5.9678
                      return 2;
                    }
                  } else {  // if shiftAvg > 0.86172
                    if (shiftStd <= 2.3745) {
                      return 3;
                    } else {  // if shiftStd > 2.3745
                      return 2;
                    }
                  }
                }
              }
            } else {  // if maxEdgeLength > 187.5
              if (HWRatio <= 1.8406) {
                return 3;
              } else {  // if HWRatio > 1.8406
                if (HWRatio <= 1.8592) {
                  return 2;
                } else {  // if HWRatio > 1.8592
                  if (shiftNcc <= 0.46045) {
                    return 4;
                  } else {  // if shiftNcc > 0.46045
                    return 3;
                  }
                }
              }
            }
          }
        } else {  // if area > 20034
          if (maxEdgeLength <= 155.5) {
            if (shiftStd <= 0.22191) {
              return 3;
            } else {  // if shiftStd > 0.22191
              return 4;
            }
          } else {  // if maxEdgeLength > 155.5
            if (area <= 22907) {
              if (shiftNcc <= -0.018231) {
                if (shiftNcc <= -0.022076) {
                  if (area <= 20437) {
                    return 4;
                  } else {  // if area > 20437
                    return 3;
                  }
                } else {  // if shiftNcc > -0.022076
                  return 4;
                }
              } else {  // if shiftNcc > -0.018231
                if (shiftAvg <= 9.0037e-05) {
                  return 4;
                } else {  // if shiftAvg > 9.0037e-05
                  if (shiftNcc <= 0.99821) {
                    if (area <= 21186) {
                      return 3;
                    } else {  // if area > 21186
                      return 3;
                    }
                  } else {  // if shiftNcc > 0.99821
                    return 4;
                  }
                }
              }
            } else {  // if area > 22907
              if (HWRatio <= 1.6017) {
                if (shiftNcc <= 0.056259) {
                  return 4;
                } else {  // if shiftNcc > 0.056259
                  if (avgErr <= 9.8039) {
                    if (avgErr <= 5.9729) {
                      return 3;
                    } else {  // if avgErr > 5.9729
                      return 3;
                    }
                  } else {  // if avgErr > 9.8039
                    return 4;
                  }
                }
              } else {  // if HWRatio > 1.6017
                return 4;
              }
            }
          }
        }
      } else {  // if area > 23307
        if (area <= 23573) {
          if (area <= 23372) {
            return 4;
          } else {  // if area > 23372
            if (area <= 23416) {
              return 3;
            } else {  // if area > 23416
              if (shiftStd <= 0.44471) {
                return 3;
              } else {  // if shiftStd > 0.44471
                if (HWRatio <= 1.5426) {
                  return 4;
                } else {  // if HWRatio > 1.5426
                  if (avgErr <= 7.5879) {
                    return 3;
                  } else {  // if avgErr > 7.5879
                    return 4;
                  }
                }
              }
            }
          }
        } else {  // if area > 23573
          if (avgErr <= 0.44677) {
            return 3;
          } else {  // if avgErr > 0.44677
            if (shiftStd <= 0.15383) {
              if (HWRatio <= 1.3423) {
                return 3;
              } else {  // if HWRatio > 1.3423
                return 4;
              }
            } else {  // if shiftStd > 0.15383
              return 4;
            }
          }
        }
      }
    } else {  // if maxEdgeLength > 194.5
      if (area <= 31521) {
        if (shiftAvg <= 0.18112) {
          if (area <= 23637) {
            if (avgErr <= 9.2893) {
              if (shiftNcc <= -0.025753) {
                if (HWRatio <= 1.9251) {
                  return 3;
                } else {  // if HWRatio > 1.9251
                  return 0;
                }
              } else {  // if shiftNcc > -0.025753
                if (shiftNcc <= 0.073556) {
                  return 0;
                } else {  // if shiftNcc > 0.073556
                  if (shiftNcc <= 0.098986) {
                    return 3;
                  } else {  // if shiftNcc > 0.098986
                    if (avgErr <= 2.8355) {
                      return 0;
                    } else {  // if avgErr > 2.8355
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avgErr > 9.2893
              if (area <= 21824) {
                return 4;
              } else {  // if area > 21824
                return 3;
              }
            }
          } else {  // if area > 23637
            if (avgErr <= 9.5158) {
              if (area <= 30305) {
                if (shiftAvg <= 0.11423) {
                  if (shiftNcc <= 0.0082277) {
                    if (shiftNcc <= 0.0020603) {
                      return 0;
                    } else {  // if shiftNcc > 0.0020603
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.0082277
                    return 0;
                  }
                } else {  // if shiftAvg > 0.11423
                  if (shiftAvg <= 0.12315) {
                    if (area <= 24255) {
                      return 0;
                    } else {  // if area > 24255
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.12315
                    if (HWRatio <= 1.7061) {
                      return 0;
                    } else {  // if HWRatio > 1.7061
                      return 0;
                    }
                  }
                }
              } else {  // if area > 30305
                if (shiftAvg <= 0.020585) {
                  return 4;
                } else {  // if shiftAvg > 0.020585
                  return 0;
                }
              }
            } else {  // if avgErr > 9.5158
              if (avgErr <= 10.855) {
                return 4;
              } else {  // if avgErr > 10.855
                return 0;
              }
            }
          }
        } else {  // if shiftAvg > 0.18112
          if (shiftStd <= 0.19933) {
            if (avgErr <= 4.0836) {
              return 4;
            } else {  // if avgErr > 4.0836
              return 3;
            }
          } else {  // if shiftStd > 0.19933
            if (area <= 24010) {
              if (shiftNcc <= 0.49858) {
                if (shiftStd <= 1.8267) {
                  if (HWRatio <= 1.6923) {
                    if (avgErr <= 8.8369) {
                      return 3;
                    } else {  // if avgErr > 8.8369
                      return 0;
                    }
                  } else {  // if HWRatio > 1.6923
                    if (HWRatio <= 1.8028) {
                      return 0;
                    } else {  // if HWRatio > 1.8028
                      return 0;
                    }
                  }
                } else {  // if shiftStd > 1.8267
                  if (shiftStd <= 6.8706) {
                    if (maxEdgeLength <= 198) {
                      return 0;
                    } else {  // if maxEdgeLength > 198
                      return 4;
                    }
                  } else {  // if shiftStd > 6.8706
                    if (area <= 22110) {
                      return 0;
                    } else {  // if area > 22110
                      return 0;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.49858
                if (shiftNcc <= 0.98182) {
                  if (shiftStd <= 1.6409) {
                    if (shiftAvg <= 1.3752) {
                      return 0;
                    } else {  // if shiftAvg > 1.3752
                      return 0;
                    }
                  } else {  // if shiftStd > 1.6409
                    return 0;
                  }
                } else {  // if shiftNcc > 0.98182
                  return 4;
                }
              }
            } else {  // if area > 24010
              if (area <= 24170) {
                return 4;
              } else {  // if area > 24170
                if (shiftNcc <= 0.46389) {
                  if (shiftAvg <= 0.18258) {
                    return 4;
                  } else {  // if shiftAvg > 0.18258
                    if (area <= 24374) {
                      return 0;
                    } else {  // if area > 24374
                      return 0;
                    }
                  }
                } else {  // if shiftNcc > 0.46389
                  if (area <= 27936) {
                    if (avgErr <= 7.0567) {
                      return 0;
                    } else {  // if avgErr > 7.0567
                      return 4;
                    }
                  } else {  // if area > 27936
                    return 4;
                  }
                }
              }
            }
          }
        }
      } else {  // if area > 31521
        if (maxEdgeLength <= 242.5) {
          return 4;
        } else {  // if maxEdgeLength > 242.5
          if (area <= 36278) {
            if (maxEdgeLength <= 289.5) {
              return 0;
            } else {  // if maxEdgeLength > 289.5
              return 1;
            }
          } else {  // if area > 36278
            return 1;
          }
        }
      }
    }
  }
}

} // namespace rm
