#include "mondrian/tree/VIRAT.hpp"

namespace md {

int VIRAT(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
          float shiftNcc, float avgErr) {
  if (area <= 17972) {
    if (area <= 15154) {
      if (area <= 8133) {
        if (maxEdgeLength <= 97.5) {
          if (area <= 6693) {
            return 1;
          } else {  // if area > 6693
            return 2;
          }
        } else {  // if maxEdgeLength > 97.5
          if (area <= 7774) {
            if (xyRatio <= 1.3356) {
              if (maxEdgeLength <= 99.5) {
                if (shiftAvg <= 0.0006793) {
                  return 2;
                } else {  // if shiftAvg > 0.0006793
                  return 0;
                }
              } else {  // if maxEdgeLength > 99.5
                return 2;
              }
            } else {  // if xyRatio > 1.3356
              if (area <= 7702) {
                if (avgErr <= 5.5644) {
                  if (xyRatio <= 1.3586) {
                    if (shiftAvg <= 0.00066769) {
                      return 0;
                    } else {  // if shiftAvg > 0.00066769
                      return 0;
                    }
                  } else {  // if xyRatio > 1.3586
                    if (avgErr <= 1.3588) {
                      return 0;
                    } else {  // if avgErr > 1.3588
                      return 0;
                    }
                  }
                } else {  // if avgErr > 5.5644
                  if (shiftStd <= 0.16008) {
                    if (shiftAvg <= 0.026076) {
                      return 2;
                    } else {  // if shiftAvg > 0.026076
                      return 4;
                    }
                  } else {  // if shiftStd > 0.16008
                    return 0;
                  }
                }
              } else {  // if area > 7702
                if (maxEdgeLength <= 110) {
                  if (avgErr <= 1.9511) {
                    return 0;
                  } else {  // if avgErr > 1.9511
                    if (avgErr <= 2.2948) {
                      return 1;
                    } else {  // if avgErr > 2.2948
                      return 2;
                    }
                  }
                } else {  // if maxEdgeLength > 110
                  return 0;
                }
              }
            }
          } else {  // if area > 7774
            if (shiftNcc <= 0.26988) {
              if (xyRatio <= 1.2516) {
                return 1;
              } else {  // if xyRatio > 1.2516
                if (area <= 7776.5) {
                  return 1;
                } else {  // if area > 7776.5
                  if (xyRatio <= 1.6071) {
                    if (area <= 8065) {
                      return 0;
                    } else {  // if area > 8065
                      return 0;
                    }
                  } else {  // if xyRatio > 1.6071
                    if (maxEdgeLength <= 114.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 114.5
                      return 0;
                    }
                  }
                }
              }
            } else {  // if shiftNcc > 0.26988
              if (area <= 7824.5) {
                if (avgErr <= 2.4199) {
                  return 1;
                } else {  // if avgErr > 2.4199
                  return 0;
                }
              } else {  // if area > 7824.5
                return 1;
              }
            }
          }
        }
      } else {  // if area > 8133
        if (xyRatio <= 1.3077) {
          if (area <= 9300.5) {
            if (avgErr <= 3.222) {
              if (area <= 8433.5) {
                if (shiftNcc <= 0.068128) {
                  return 3;
                } else {  // if shiftNcc > 0.068128
                  if (maxEdgeLength <= 101.5) {
                    if (area <= 8308) {
                      return 1;
                    } else {  // if area > 8308
                      return 1;
                    }
                  } else {  // if maxEdgeLength > 101.5
                    return 0;
                  }
                }
              } else {  // if area > 8433.5
                if (maxEdgeLength <= 106.5) {
                  return 1;
                } else {  // if maxEdgeLength > 106.5
                  if (shiftAvg <= 0.00029077) {
                    return 3;
                  } else {  // if shiftAvg > 0.00029077
                    if (shiftStd <= 0.018692) {
                      return 1;
                    } else {  // if shiftStd > 0.018692
                      return 3;
                    }
                  }
                }
              }
            } else {  // if avgErr > 3.222
              if (shiftNcc <= 0.034989) {
                return 1;
              } else {  // if shiftNcc > 0.034989
                if (shiftStd <= 0.09221) {
                  return 3;
                } else {  // if shiftStd > 0.09221
                  if (shiftAvg <= 0.0071388) {
                    return 3;
                  } else {  // if shiftAvg > 0.0071388
                    return 4;
                  }
                }
              }
            }
          } else {  // if area > 9300.5
            if (area <= 11280) {
              if (area <= 10852) {
                if (shiftAvg <= 0.001293) {
                  if (shiftAvg <= 0.00019377) {
                    return 1;
                  } else {  // if shiftAvg > 0.00019377
                    if (area <= 10314) {
                      return 4;
                    } else {  // if area > 10314
                      return 4;
                    }
                  }
                } else {  // if shiftAvg > 0.001293
                  if (xyRatio <= 1.2241) {
                    if (maxEdgeLength <= 104) {
                      return 1;
                    } else {  // if maxEdgeLength > 104
                      return 1;
                    }
                  } else {  // if xyRatio > 1.2241
                    if (xyRatio <= 1.2644) {
                      return 4;
                    } else {  // if xyRatio > 1.2644
                      return 1;
                    }
                  }
                }
              } else {  // if area > 10852
                if (shiftAvg <= 0.0016751) {
                  return 1;
                } else {  // if shiftAvg > 0.0016751
                  return 4;
                }
              }
            } else {  // if area > 11280
              return 1;
            }
          }
        } else {  // if xyRatio > 1.3077
          if (area <= 15048) {
            if (shiftStd <= 0.15993) {
              if (shiftAvg <= 0.00094788) {
                if (shiftNcc <= 0.14899) {
                  if (avgErr <= 2.0876) {
                    return 1;
                  } else {  // if avgErr > 2.0876
                    if (avgErr <= 2.1122) {
                      return 3;
                    } else {  // if avgErr > 2.1122
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.14899
                  if (xyRatio <= 1.4333) {
                    return 1;
                  } else {  // if xyRatio > 1.4333
                    return 3;
                  }
                }
              } else {  // if shiftAvg > 0.00094788
                if (shiftAvg <= 0.0016553) {
                  if (area <= 8602) {
                    if (maxEdgeLength <= 108.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 108.5
                      return 3;
                    }
                  } else {  // if area > 8602
                    if (xyRatio <= 1.7238) {
                      return 4;
                    } else {  // if xyRatio > 1.7238
                      return 1;
                    }
                  }
                } else {  // if shiftAvg > 0.0016553
                  if (area <= 9079) {
                    if (shiftAvg <= 0.034409) {
                      return 1;
                    } else {  // if shiftAvg > 0.034409
                      return 1;
                    }
                  } else {  // if area > 9079
                    if (maxEdgeLength <= 119.5) {
                      return 1;
                    } else {  // if maxEdgeLength > 119.5
                      return 1;
                    }
                  }
                }
              }
            } else {  // if shiftStd > 0.15993
              if (area <= 14947) {
                if (shiftNcc <= 0.94625) {
                  if (shiftAvg <= 0.010848) {
                    if (shiftAvg <= 0.0093056) {
                      return 1;
                    } else {  // if shiftAvg > 0.0093056
                      return 4;
                    }
                  } else {  // if shiftAvg > 0.010848
                    if (avgErr <= 4.6329) {
                      return 1;
                    } else {  // if avgErr > 4.6329
                      return 1;
                    }
                  }
                } else {  // if shiftNcc > 0.94625
                  if (shiftAvg <= 18.345) {
                    if (area <= 14468) {
                      return 4;
                    } else {  // if area > 14468
                      return 2;
                    }
                  } else {  // if shiftAvg > 18.345
                    return 1;
                  }
                }
              } else {  // if area > 14947
                if (area <= 14998) {
                  if (xyRatio <= 1.7515) {
                    return 4;
                  } else {  // if xyRatio > 1.7515
                    return 2;
                  }
                } else {  // if area > 14998
                  return 1;
                }
              }
            }
          } else {  // if area > 15048
            if (avgErr <= 5.6186) {
              if (shiftAvg <= 0.76883) {
                if (area <= 15058) {
                  return 2;
                } else {  // if area > 15058
                  if (shiftStd <= 0.26353) {
                    return 1;
                  } else {  // if shiftStd > 0.26353
                    if (shiftStd <= 0.33115) {
                      return 2;
                    } else {  // if shiftStd > 0.33115
                      return 1;
                    }
                  }
                }
              } else {  // if shiftAvg > 0.76883
                return 4;
              }
            } else {  // if avgErr > 5.6186
              return 2;
            }
          }
        }
      }
    } else {  // if area > 15154
      if (area <= 17602) {
        if (area <= 15553) {
          if (maxEdgeLength <= 147.5) {
            if (xyRatio <= 1.3857) {
              return 2;
            } else {  // if xyRatio > 1.3857
              return 1;
            }
          } else {  // if maxEdgeLength > 147.5
            if (area <= 15531) {
              if (maxEdgeLength <= 176.5) {
                if (shiftNcc <= -0.010212) {
                  if (shiftNcc <= -0.0354) {
                    return 2;
                  } else {  // if shiftNcc > -0.0354
                    return 1;
                  }
                } else {  // if shiftNcc > -0.010212
                  if (shiftNcc <= 0.60099) {
                    if (shiftAvg <= 0.00029104) {
                      return 1;
                    } else {  // if shiftAvg > 0.00029104
                      return 2;
                    }
                  } else {  // if shiftNcc > 0.60099
                    if (shiftAvg <= 0.75721) {
                      return 1;
                    } else {  // if shiftAvg > 0.75721
                      return 2;
                    }
                  }
                }
              } else {  // if maxEdgeLength > 176.5
                return 1;
              }
            } else {  // if area > 15531
              return 1;
            }
          }
        } else {  // if area > 15553
          if (shiftAvg <= 25.229) {
            if (area <= 15992) {
              if (shiftNcc <= 0.92595) {
                if (xyRatio <= 1.4076) {
                  return 4;
                } else {  // if xyRatio > 1.4076
                  if (area <= 15948) {
                    if (shiftAvg <= 8.5203e-05) {
                      return 2;
                    } else {  // if shiftAvg > 8.5203e-05
                      return 2;
                    }
                  } else {  // if area > 15948
                    if (shiftNcc <= 0.21083) {
                      return 4;
                    } else {  // if shiftNcc > 0.21083
                      return 2;
                    }
                  }
                }
              } else {  // if shiftNcc > 0.92595
                return 4;
              }
            } else {  // if area > 15992
              if (maxEdgeLength <= 145.5) {
                if (shiftStd <= 0.85181) {
                  if (avgErr <= 6.7933) {
                    return 2;
                  } else {  // if avgErr > 6.7933
                    return 4;
                  }
                } else {  // if shiftStd > 0.85181
                  return 2;
                }
              } else {  // if maxEdgeLength > 145.5
                if (area <= 17562) {
                  if (avgErr <= 7.1843) {
                    return 2;
                  } else {  // if avgErr > 7.1843
                    if (avgErr <= 7.2648) {
                      return 4;
                    } else {  // if avgErr > 7.2648
                      return 2;
                    }
                  }
                } else {  // if area > 17562
                  if (area <= 17571) {
                    return 3;
                  } else {  // if area > 17571
                    return 2;
                  }
                }
              }
            }
          } else {  // if shiftAvg > 25.229
            if (area <= 16662) {
              return 2;
            } else {  // if area > 16662
              return 4;
            }
          }
        }
      } else {  // if area > 17602
        if (area <= 17935) {
          if (area <= 17889) {
            if (xyRatio <= 1.9689) {
              if (area <= 17608) {
                return 3;
              } else {  // if area > 17608
                if (shiftAvg <= 0.00023857) {
                  if (shiftNcc <= -0.0034383) {
                    return 3;
                  } else {  // if shiftNcc > -0.0034383
                    return 2;
                  }
                } else {  // if shiftAvg > 0.00023857
                  if (xyRatio <= 1.1447) {
                    return 4;
                  } else {  // if xyRatio > 1.1447
                    if (shiftNcc <= 0.99073) {
                      return 2;
                    } else {  // if shiftNcc > 0.99073
                      return 3;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 1.9689
              return 3;
            }
          } else {  // if area > 17889
            if (maxEdgeLength <= 167.5) {
              if (shiftStd <= 0.030145) {
                if (shiftAvg <= 0.0008794) {
                  return 2;
                } else {  // if shiftAvg > 0.0008794
                  return 3;
                }
              } else {  // if shiftStd > 0.030145
                return 2;
              }
            } else {  // if maxEdgeLength > 167.5
              return 3;
            }
          }
        } else {  // if area > 17935
          return 2;
        }
      }
    }
  } else {  // if area > 17972
    if (maxEdgeLength <= 194.5) {
      if (area <= 20407) {
        if (area <= 20066) {
          if (area <= 18372) {
            if (area <= 18369) {
              if (shiftAvg <= 1.4592) {
                if (shiftNcc <= 0.58475) {
                  if (avgErr <= 0.50033) {
                    if (avgErr <= 0.43281) {
                      return 3;
                    } else {  // if avgErr > 0.43281
                      return 2;
                    }
                  } else {  // if avgErr > 0.50033
                    if (shiftNcc <= 0.19648) {
                      return 3;
                    } else {  // if shiftNcc > 0.19648
                      return 3;
                    }
                  }
                } else {  // if shiftNcc > 0.58475
                  if (shiftAvg <= 0.1657) {
                    return 3;
                  } else {  // if shiftAvg > 0.1657
                    return 2;
                  }
                }
              } else {  // if shiftAvg > 1.4592
                if (shiftNcc <= 0.66637) {
                  if (xyRatio <= 1.9511) {
                    if (shiftAvg <= 1.5954) {
                      return 4;
                    } else {  // if shiftAvg > 1.5954
                      return 2;
                    }
                  } else {  // if xyRatio > 1.9511
                    return 3;
                  }
                } else {  // if shiftNcc > 0.66637
                  if (maxEdgeLength <= 173) {
                    if (shiftAvg <= 8.6549) {
                      return 3;
                    } else {  // if shiftAvg > 8.6549
                      return 2;
                    }
                  } else {  // if maxEdgeLength > 173
                    return 3;
                  }
                }
              }
            } else {  // if area > 18369
              return 2;
            }
          } else {  // if area > 18372
            if (shiftAvg <= 25.926) {
              if (shiftStd <= 77.457) {
                if (area <= 18391) {
                  if (area <= 18378) {
                    return 3;
                  } else {  // if area > 18378
                    return 4;
                  }
                } else {  // if area > 18391
                  if (avgErr <= 9.0266) {
                    if (shiftNcc <= 0.76189) {
                      return 3;
                    } else {  // if shiftNcc > 0.76189
                      return 3;
                    }
                  } else {  // if avgErr > 9.0266
                    if (avgErr <= 9.1331) {
                      return 4;
                    } else {  // if avgErr > 9.1331
                      return 3;
                    }
                  }
                }
              } else {  // if shiftStd > 77.457
                return 4;
              }
            } else {  // if shiftAvg > 25.926
              if (shiftNcc <= 0.94952) {
                return 3;
              } else {  // if shiftNcc > 0.94952
                return 4;
              }
            }
          }
        } else {  // if area > 20066
          if (avgErr <= 7.0255) {
            if (area <= 20096) {
              return 4;
            } else {  // if area > 20096
              if (shiftNcc <= 0.87446) {
                if (shiftStd <= 1.7377) {
                  if (shiftNcc <= 0.17151) {
                    if (shiftAvg <= 0.21922) {
                      return 3;
                    } else {  // if shiftAvg > 0.21922
                      return 4;
                    }
                  } else {  // if shiftNcc > 0.17151
                    return 3;
                  }
                } else {  // if shiftStd > 1.7377
                  if (shiftNcc <= 0.60279) {
                    return 4;
                  } else {  // if shiftNcc > 0.60279
                    return 3;
                  }
                }
              } else {  // if shiftNcc > 0.87446
                if (shiftStd <= 3.8629) {
                  return 4;
                } else {  // if shiftStd > 3.8629
                  return 3;
                }
              }
            }
          } else {  // if avgErr > 7.0255
            if (area <= 20204) {
              if (xyRatio <= 1.7442) {
                if (shiftStd <= 9.8313) {
                  return 3;
                } else {  // if shiftStd > 9.8313
                  if (shiftAvg <= 1.5908) {
                    return 4;
                  } else {  // if shiftAvg > 1.5908
                    return 3;
                  }
                }
              } else {  // if xyRatio > 1.7442
                return 4;
              }
            } else {  // if area > 20204
              if (area <= 20372) {
                return 4;
              } else {  // if area > 20372
                return 3;
              }
            }
          }
        }
      } else {  // if area > 20407
        if (area <= 20774) {
          if (area <= 20728) {
            if (area <= 20584) {
              if (area <= 20530) {
                if (avgErr <= 11.145) {
                  if (area <= 20456) {
                    if (area <= 20438) {
                      return 4;
                    } else {  // if area > 20438
                      return 3;
                    }
                  } else {  // if area > 20456
                    return 4;
                  }
                } else {  // if avgErr > 11.145
                  return 3;
                }
              } else {  // if area > 20530
                return 3;
              }
            } else {  // if area > 20584
              if (shiftAvg <= 6.4421) {
                if (area <= 20708) {
                  return 4;
                } else {  // if area > 20708
                  if (maxEdgeLength <= 181) {
                    return 3;
                  } else {  // if maxEdgeLength > 181
                    return 4;
                  }
                }
              } else {  // if shiftAvg > 6.4421
                if (area <= 20660) {
                  return 3;
                } else {  // if area > 20660
                  return 4;
                }
              }
            }
          } else {  // if area > 20728
            if (xyRatio <= 1.495) {
              return 4;
            } else {  // if xyRatio > 1.495
              return 3;
            }
          }
        } else {  // if area > 20774
          return 4;
        }
      }
    } else {  // if maxEdgeLength > 194.5
      if (area <= 30753) {
        if (xyRatio <= 1.4352) {
          if (area <= 28681) {
            return 4;
          } else {  // if area > 28681
            return 0;
          }
        } else {  // if xyRatio > 1.4352
          if (area <= 30047) {
            if (maxEdgeLength <= 201.5) {
              if (shiftAvg <= 12.869) {
                return 0;
              } else {  // if shiftAvg > 12.869
                if (shiftAvg <= 13.308) {
                  return 3;
                } else {  // if shiftAvg > 13.308
                  return 0;
                }
              }
            } else {  // if maxEdgeLength > 201.5
              if (maxEdgeLength <= 204.5) {
                if (shiftNcc <= 0.28529) {
                  if (shiftAvg <= 0.087105) {
                    if (avgErr <= 2.6938) {
                      return 0;
                    } else {  // if avgErr > 2.6938
                      return 0;
                    }
                  } else {  // if shiftAvg > 0.087105
                    if (shiftStd <= 0.47902) {
                      return 0;
                    } else {  // if shiftStd > 0.47902
                      return 4;
                    }
                  }
                } else {  // if shiftNcc > 0.28529
                  return 0;
                }
              } else {  // if maxEdgeLength > 204.5
                if (area <= 22354) {
                  if (area <= 22302) {
                    if (xyRatio <= 1.9715) {
                      return 0;
                    } else {  // if xyRatio > 1.9715
                      return 0;
                    }
                  } else {  // if area > 22302
                    return 4;
                  }
                } else {  // if area > 22354
                  return 0;
                }
              }
            }
          } else {  // if area > 30047
            if (area <= 30198) {
              return 4;
            } else {  // if area > 30198
              return 0;
            }
          }
        }
      } else {  // if area > 30753
        if (shiftAvg <= 0.034315) {
          if (shiftStd <= 0.027402) {
            if (shiftStd <= 0.014159) {
              return 1;
            } else {  // if shiftStd > 0.014159
              return 4;
            }
          } else {  // if shiftStd > 0.027402
            return 1;
          }
        } else {  // if shiftAvg > 0.034315
          return 4;
        }
      }
    }
  }
}

} // namespace md
