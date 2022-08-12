#include "strm/tree/MTA.hpp"

namespace rm {

float MTA(float width, float height, float label, float xyRatio, float avg_shift_x,
          float avg_shift_y, float avg_shift, float std_shift_x, float std_shift_y, float std_shift,
          float avg_err, float ncc) {
  if (height <= -0.54417) {
    if (height <= -0.68842) {
      if (width <= -0.23883) {
        if (height <= -0.71246) {
          return 0;
        } else {  // if height > -0.71246
          if (xyRatio <= -0.41536) {
            if (ncc <= -2.7826) {
              return 1;
            } else {  // if ncc > -2.7826
              if (ncc <= 1.4856) {
                if (avg_shift_x <= -0.030452) {
                  if (avg_shift_y <= 0.0092252) {
                    if (avg_shift <= -0.26243) {
                      return 1;
                    } else {  // if avg_shift > -0.26243
                      return 0;
                    }
                  } else {  // if avg_shift_y > 0.0092252
                    return 0;
                  }
                } else {  // if avg_shift_x > -0.030452
                  if (ncc <= -0.42446) {
                    return 0;
                  } else {  // if ncc > -0.42446
                    return 1;
                  }
                }
              } else {  // if ncc > 1.4856
                return 1;
              }
            }
          } else {  // if xyRatio > -0.41536
            if (ncc <= 1.0995) {
              if (avg_shift_x <= 0.49829) {
                if (std_shift_x <= -0.28881) {
                  if (avg_shift_x <= -0.066804) {
                    if (width <= -0.2992) {
                      return 1;
                    } else {  // if width > -0.2992
                      return 0;
                    }
                  } else {  // if avg_shift_x > -0.066804
                    if (std_shift_y <= -0.45892) {
                      return 1;
                    } else {  // if std_shift_y > -0.45892
                      return 0;
                    }
                  }
                } else {  // if std_shift_x > -0.28881
                  if (avg_shift_x <= 0.36706) {
                    return 1;
                  } else {  // if avg_shift_x > 0.36706
                    return 0;
                  }
                }
              } else {  // if avg_shift_x > 0.49829
                if (width <= -0.30725) {
                  if (avg_shift <= -0.21024) {
                    return 0;
                  } else {  // if avg_shift > -0.21024
                    return 1;
                  }
                } else {  // if width > -0.30725
                  return 0;
                }
              }
            } else {  // if ncc > 1.0995
              if (avg_shift_y <= 0.14877) {
                if (avg_shift_y <= 0.13152) {
                  if (std_shift_y <= -0.69791) {
                    if (std_shift_x <= -0.70892) {
                      return 0;
                    } else {  // if std_shift_x > -0.70892
                      return 1;
                    }
                  } else {  // if std_shift_y > -0.69791
                    if (std_shift <= -0.28677) {
                      return 0;
                    } else {  // if std_shift > -0.28677
                      return 0;
                    }
                  }
                } else {  // if avg_shift_y > 0.13152
                  return 1;
                }
              } else {  // if avg_shift_y > 0.14877
                if (ncc <= 1.4989) {
                  return 0;
                } else {  // if ncc > 1.4989
                  return 1;
                }
              }
            }
          }
        }
      } else {  // if width > -0.23883
        if (height <= -0.89276) {
          if (avg_shift <= -0.2604) {
            if (height <= -0.92882) {
              return 0;
            } else {  // if height > -0.92882
              return 1;
            }
          } else {  // if avg_shift > -0.2604
            return 1;
          }
        } else {  // if height > -0.89276
          if (avg_shift <= -0.26204) {
            if (avg_shift_y <= -0.027054) {
              if (avg_shift_y <= -0.042134) {
                return 1;
              } else {  // if avg_shift_y > -0.042134
                return 0;
              }
            } else {  // if avg_shift_y > -0.027054
              if (ncc <= -1.7601) {
                return 1;
              } else {  // if ncc > -1.7601
                if (avg_err <= -1.0928) {
                  if (avg_shift_x <= -0.039229) {
                    if (avg_err <= -1.6136) {
                      return 1;
                    } else {  // if avg_err > -1.6136
                      return 0;
                    }
                  } else {  // if avg_shift_x > -0.039229
                    return 1;
                  }
                } else {  // if avg_err > -1.0928
                  if (std_shift <= -0.28887) {
                    return 0;
                  } else {  // if std_shift > -0.28887
                    return 1;
                  }
                }
              }
            }
          } else {  // if avg_shift > -0.26204
            if (ncc <= 1.0069) {
              if (avg_shift_y <= 0.07282) {
                if (avg_shift_y <= 0.055009) {
                  if (avg_err <= 2.1885) {
                    if (avg_err <= -0.85936) {
                      return 1;
                    } else {  // if avg_err > -0.85936
                      return 0;
                    }
                  } else {  // if avg_err > 2.1885
                    return 1;
                  }
                } else {  // if avg_shift_y > 0.055009
                  if (std_shift_x <= -0.37111) {
                    return 1;
                  } else {  // if std_shift_x > -0.37111
                    return 0;
                  }
                }
              } else {  // if avg_shift_y > 0.07282
                if (avg_err <= 2.1858) {
                  return 0;
                } else {  // if avg_err > 2.1858
                  if (avg_shift_x <= -0.082888) {
                    return 1;
                  } else {  // if avg_shift_x > -0.082888
                    return 0;
                  }
                }
              }
            } else {  // if ncc > 1.0069
              if (avg_shift_y <= 0.0062752) {
                if (std_shift_x <= -0.30917) {
                  if (std_shift_y <= -0.62499) {
                    return 0;
                  } else {  // if std_shift_y > -0.62499
                    return 1;
                  }
                } else {  // if std_shift_x > -0.30917
                  return 0;
                }
              } else {  // if avg_shift_y > 0.0062752
                return 1;
              }
            }
          }
        }
      }
    } else {  // if height > -0.68842
      if (avg_shift <= -0.22188) {
        if (width <= -0.19859) {
          if (height <= -0.64033) {
            if (xyRatio <= 0.46479) {
              if (xyRatio <= -0.12397) {
                if (std_shift <= -0.28931) {
                  if (avg_shift_y <= -0.15339) {
                    if (avg_shift_x <= -0.31558) {
                      return 1;
                    } else {  // if avg_shift_x > -0.31558
                      return 0;
                    }
                  } else {  // if avg_shift_y > -0.15339
                    if (ncc <= 1.4936) {
                      return 1;
                    } else {  // if ncc > 1.4936
                      return 0;
                    }
                  }
                } else {  // if std_shift > -0.28931
                  if (avg_shift <= -0.2611) {
                    if (std_shift_x <= -0.56608) {
                      return 0;
                    } else {  // if std_shift_x > -0.56608
                      return 1;
                    }
                  } else {  // if avg_shift > -0.2611
                    if (avg_err <= 1.7382) {
                      return 1;
                    } else {  // if avg_err > 1.7382
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > -0.12397
                if (xyRatio <= 0.0058119) {
                  if (std_shift <= -0.27968) {
                    if (std_shift_y <= -0.73167) {
                      return 1;
                    } else {  // if std_shift_y > -0.73167
                      return 0;
                    }
                  } else {  // if std_shift > -0.27968
                    if (xyRatio <= -0.11488) {
                      return 0;
                    } else {  // if xyRatio > -0.11488
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.0058119
                  if (std_shift_x <= -0.41693) {
                    if (height <= -0.66437) {
                      return 0;
                    } else {  // if height > -0.66437
                      return 1;
                    }
                  } else {  // if std_shift_x > -0.41693
                    if (avg_shift_x <= -0.27107) {
                      return 1;
                    } else {  // if avg_shift_x > -0.27107
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.46479
              if (avg_shift_x <= -0.29047) {
                if (xyRatio <= 0.86445) {
                  if (avg_err <= -1.5652) {
                    return 1;
                  } else {  // if avg_err > -1.5652
                    return 0;
                  }
                } else {  // if xyRatio > 0.86445
                  return 1;
                }
              } else {  // if avg_shift_x > -0.29047
                if (std_shift_y <= -0.61976) {
                  if (avg_shift_x <= -0.14629) {
                    if (ncc <= -0.31829) {
                      return 1;
                    } else {  // if ncc > -0.31829
                      return 0;
                    }
                  } else {  // if avg_shift_x > -0.14629
                    return 1;
                  }
                } else {  // if std_shift_y > -0.61976
                  if (avg_shift <= -0.26158) {
                    if (avg_err <= 0.2009) {
                      return 0;
                    } else {  // if avg_err > 0.2009
                      return 1;
                    }
                  } else {  // if avg_shift > -0.26158
                    if (avg_shift_y <= -0.086579) {
                      return 1;
                    } else {  // if avg_shift_y > -0.086579
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if height > -0.64033
            if (avg_shift <= -0.24739) {
              if (height <= -0.55619) {
                if (avg_err <= -1.5024) {
                  if (ncc <= -3.1542) {
                    return 0;
                  } else {  // if ncc > -3.1542
                    if (std_shift_x <= -0.060458) {
                      return 1;
                    } else {  // if std_shift_x > -0.060458
                      return 0;
                    }
                  }
                } else {  // if avg_err > -1.5024
                  if (ncc <= 1.3055) {
                    if (std_shift_y <= -0.58322) {
                      return 1;
                    } else {  // if std_shift_y > -0.58322
                      return 1;
                    }
                  } else {  // if ncc > 1.3055
                    if (avg_shift <= -0.25112) {
                      return 0;
                    } else {  // if avg_shift > -0.25112
                      return 1;
                    }
                  }
                }
              } else {  // if height > -0.55619
                if (ncc <= -2.5491) {
                  return 0;
                } else {  // if ncc > -2.5491
                  if (avg_shift <= -0.25642) {
                    if (std_shift_y <= -0.69987) {
                      return 1;
                    } else {  // if std_shift_y > -0.69987
                      return 1;
                    }
                  } else {  // if avg_shift > -0.25642
                    if (ncc <= -1.4568) {
                      return 0;
                    } else {  // if ncc > -1.4568
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avg_shift > -0.24739
              if (width <= -0.21066) {
                if (height <= -0.58023) {
                  if (avg_err <= -0.061141) {
                    if (xyRatio <= 0.044321) {
                      return 1;
                    } else {  // if xyRatio > 0.044321
                      return 1;
                    }
                  } else {  // if avg_err > -0.061141
                    if (avg_shift_y <= 0.24703) {
                      return 1;
                    } else {  // if avg_shift_y > 0.24703
                      return 0;
                    }
                  }
                } else {  // if height > -0.58023
                  if (std_shift_y <= -0.61878) {
                    if (avg_shift_x <= 0.32155) {
                      return 1;
                    } else {  // if avg_shift_x > 0.32155
                      return 1;
                    }
                  } else {  // if std_shift_y > -0.61878
                    if (avg_shift_x <= 0.34076) {
                      return 1;
                    } else {  // if avg_shift_x > 0.34076
                      return 1;
                    }
                  }
                }
              } else {  // if width > -0.21066
                if (ncc <= -0.45967) {
                  return 1;
                } else {  // if ncc > -0.45967
                  if (avg_shift <= -0.2413) {
                    if (avg_err <= 0.35533) {
                      return 0;
                    } else {  // if avg_err > 0.35533
                      return 1;
                    }
                  } else {  // if avg_shift > -0.2413
                    return 0;
                  }
                }
              }
            }
          }
        } else {  // if width > -0.19859
          if (avg_shift <= -0.26018) {
            if (ncc <= -1.7924) {
              return 0;
            } else {  // if ncc > -1.7924
              return 1;
            }
          } else {  // if avg_shift > -0.26018
            if (xyRatio <= 0.65635) {
              return 1;
            } else {  // if xyRatio > 0.65635
              if (avg_err <= 1.8065) {
                if (avg_shift <= -0.25952) {
                  if (std_shift <= -0.27988) {
                    return 1;
                  } else {  // if std_shift > -0.27988
                    return 0;
                  }
                } else {  // if avg_shift > -0.25952
                  return 0;
                }
              } else {  // if avg_err > 1.8065
                if (avg_err <= 1.9274) {
                  return 1;
                } else {  // if avg_err > 1.9274
                  return 0;
                }
              }
            }
          }
        }
      } else {  // if avg_shift > -0.22188
        if (avg_shift_y <= 0.69965) {
          if (height <= -0.58023) {
            if (avg_shift_y <= -0.5375) {
              if (xyRatio <= 0.052647) {
                return 1;
              } else {  // if xyRatio > 0.052647
                if (xyRatio <= 0.39521) {
                  if (std_shift_y <= -0.70672) {
                    return 1;
                  } else {  // if std_shift_y > -0.70672
                    if (ncc <= 1.4883) {
                      return 0;
                    } else {  // if ncc > 1.4883
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.39521
                  return 1;
                }
              }
            } else {  // if avg_shift_y > -0.5375
              if (height <= -0.67639) {
                if (avg_shift <= -0.21733) {
                  return 1;
                } else {  // if avg_shift > -0.21733
                  if (avg_err <= 1.9381) {
                    if (ncc <= 1.3766) {
                      return 0;
                    } else {  // if ncc > 1.3766
                      return 0;
                    }
                  } else {  // if avg_err > 1.9381
                    if (avg_shift_y <= 0.58615) {
                      return 1;
                    } else {  // if avg_shift_y > 0.58615
                      return 0;
                    }
                  }
                }
              } else {  // if height > -0.67639
                if (std_shift_x <= -0.49093) {
                  if (ncc <= 1.4036) {
                    if (std_shift <= -0.28615) {
                      return 1;
                    } else {  // if std_shift > -0.28615
                      return 1;
                    }
                  } else {  // if ncc > 1.4036
                    if (std_shift_y <= -0.71698) {
                      return 1;
                    } else {  // if std_shift_y > -0.71698
                      return 0;
                    }
                  }
                } else {  // if std_shift_x > -0.49093
                  if (xyRatio <= 0.10138) {
                    if (avg_err <= -0.65129) {
                      return 1;
                    } else {  // if avg_err > -0.65129
                      return 1;
                    }
                  } else {  // if xyRatio > 0.10138
                    if (std_shift_y <= -0.3878) {
                      return 1;
                    } else {  // if std_shift_y > -0.3878
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if height > -0.58023
            if (xyRatio <= 0.77959) {
              if (avg_err <= 0.72741) {
                if (std_shift <= -0.28443) {
                  if (std_shift_y <= -0.55739) {
                    if (xyRatio <= -0.47048) {
                      return 0;
                    } else {  // if xyRatio > -0.47048
                      return 1;
                    }
                  } else {  // if std_shift_y > -0.55739
                    return 0;
                  }
                } else {  // if std_shift > -0.28443
                  if (xyRatio <= 0.58622) {
                    if (std_shift_y <= -0.51979) {
                      return 1;
                    } else {  // if std_shift_y > -0.51979
                      return 1;
                    }
                  } else {  // if xyRatio > 0.58622
                    return 0;
                  }
                }
              } else {  // if avg_err > 0.72741
                if (xyRatio <= -0.38001) {
                  return 1;
                } else {  // if xyRatio > -0.38001
                  if (width <= -0.25895) {
                    if (std_shift <= -0.23074) {
                      return 0;
                    } else {  // if std_shift > -0.23074
                      return 1;
                    }
                  } else {  // if width > -0.25895
                    return 1;
                  }
                }
              }
            } else {  // if xyRatio > 0.77959
              return 0;
            }
          }
        } else {  // if avg_shift_y > 0.69965
          if (xyRatio <= 1.447) {
            if (xyRatio <= -0.15235) {
              if (avg_shift_y <= 0.78773) {
                return 0;
              } else {  // if avg_shift_y > 0.78773
                return 1;
              }
            } else {  // if xyRatio > -0.15235
              if (std_shift <= -0.28881) {
                return 1;
              } else {  // if std_shift > -0.28881
                if (avg_err <= -0.68358) {
                  if (avg_err <= -0.90142) {
                    return 0;
                  } else {  // if avg_err > -0.90142
                    return 1;
                  }
                } else {  // if avg_err > -0.68358
                  if (std_shift_y <= -0.64845) {
                    if (std_shift_y <= -0.65869) {
                      return 0;
                    } else {  // if std_shift_y > -0.65869
                      return 1;
                    }
                  } else {  // if std_shift_y > -0.64845
                    if (ncc <= 0.20103) {
                      return 0;
                    } else {  // if ncc > 0.20103
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 1.447
            if (avg_shift_y <= 2.1751) {
              return 1;
            } else {  // if avg_shift_y > 2.1751
              if (avg_shift <= 4.4621) {
                return 2;
              } else {  // if avg_shift > 4.4621
                if (ncc <= 0.36662) {
                  return 0;
                } else {  // if ncc > 0.36662
                  return 2;
                }
              }
            }
          }
        }
      }
    }
  } else {  // if height > -0.54417
    if (label <= -1.1061) {
      if (ncc <= -1.0086) {
        if (width <= 0.67875) {
          if (height <= 0.092905) {
            if (std_shift_x <= -0.45621) {
              if (avg_shift_x <= 0.12448) {
                if (avg_shift_x <= -0.1358) {
                  if (avg_shift <= -0.26021) {
                    return 1;
                  } else {  // if avg_shift > -0.26021
                    if (avg_shift_x <= -0.15847) {
                      return 2;
                    } else {  // if avg_shift_x > -0.15847
                      return 2;
                    }
                  }
                } else {  // if avg_shift_x > -0.1358
                  return 2;
                }
              } else {  // if avg_shift_x > 0.12448
                return 1;
              }
            } else {  // if std_shift_x > -0.45621
              if (avg_shift_x <= -0.61605) {
                if (std_shift <= -0.094989) {
                  return 1;
                } else {  // if std_shift > -0.094989
                  if (avg_err <= -1.7087) {
                    return 1;
                  } else {  // if avg_err > -1.7087
                    return 2;
                  }
                }
              } else {  // if avg_shift_x > -0.61605
                if (ncc <= -1.641) {
                  if (ncc <= -1.642) {
                    if (std_shift_y <= -0.12618) {
                      return 2;
                    } else {  // if std_shift_y > -0.12618
                      return 2;
                    }
                  } else {  // if ncc > -1.642
                    return 1;
                  }
                } else {  // if ncc > -1.641
                  if (avg_shift_y <= 0.15104) {
                    if (xyRatio <= 1.0605) {
                      return 2;
                    } else {  // if xyRatio > 1.0605
                      return 2;
                    }
                  } else {  // if avg_shift_y > 0.15104
                    if (height <= -0.18356) {
                      return 1;
                    } else {  // if height > -0.18356
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if height > 0.092905
            if (width <= 0.18374) {
              if (ncc <= -1.1746) {
                if (avg_err <= -0.35314) {
                  if (avg_shift_x <= -0.14296) {
                    if (std_shift_x <= -0.36275) {
                      return 2;
                    } else {  // if std_shift_x > -0.36275
                      return 2;
                    }
                  } else {  // if avg_shift_x > -0.14296
                    if (avg_shift <= -0.19425) {
                      return 2;
                    } else {  // if avg_shift > -0.19425
                      return 2;
                    }
                  }
                } else {  // if avg_err > -0.35314
                  if (avg_err <= -0.3509) {
                    return 1;
                  } else {  // if avg_err > -0.3509
                    if (avg_err <= 0.2326) {
                      return 2;
                    } else {  // if avg_err > 0.2326
                      return 2;
                    }
                  }
                }
              } else {  // if ncc > -1.1746
                if (xyRatio <= 0.018654) {
                  if (height <= 0.81412) {
                    if (avg_shift_y <= -0.20855) {
                      return 0;
                    } else {  // if avg_shift_y > -0.20855
                      return 2;
                    }
                  } else {  // if height > 0.81412
                    if (avg_shift_y <= -0.18303) {
                      return 0;
                    } else {  // if avg_shift_y > -0.18303
                      return 1;
                    }
                  }
                } else {  // if xyRatio > 0.018654
                  if (std_shift_x <= 0.20968) {
                    if (std_shift_x <= -0.18319) {
                      return 2;
                    } else {  // if std_shift_x > -0.18319
                      return 1;
                    }
                  } else {  // if std_shift_x > 0.20968
                    return 2;
                  }
                }
              }
            } else {  // if width > 0.18374
              if (std_shift <= -0.24266) {
                if (avg_shift_x <= -0.0024531) {
                  if (xyRatio <= 1.1211) {
                    if (width <= 0.32862) {
                      return 2;
                    } else {  // if width > 0.32862
                      return 0;
                    }
                  } else {  // if xyRatio > 1.1211
                    return 2;
                  }
                } else {  // if avg_shift_x > -0.0024531
                  if (avg_shift_y <= 0.18522) {
                    if (avg_shift_x <= 0.27404) {
                      return 1;
                    } else {  // if avg_shift_x > 0.27404
                      return 0;
                    }
                  } else {  // if avg_shift_y > 0.18522
                    return 2;
                  }
                }
              } else {  // if std_shift > -0.24266
                if (avg_shift_y <= 2.2386) {
                  if (ncc <= -1.4407) {
                    if (avg_shift_y <= -0.65067) {
                      return 0;
                    } else {  // if avg_shift_y > -0.65067
                      return 2;
                    }
                  } else {  // if ncc > -1.4407
                    if (ncc <= -1.4364) {
                      return 1;
                    } else {  // if ncc > -1.4364
                      return 2;
                    }
                  }
                } else {  // if avg_shift_y > 2.2386
                  return 1;
                }
              }
            }
          }
        } else {  // if width > 0.67875
          if (width <= 0.69082) {
            if (avg_shift_x <= 0.49281) {
              return 1;
            } else {  // if avg_shift_x > 0.49281
              return 0;
            }
          } else {  // if width > 0.69082
            if (avg_shift_y <= 0.39858) {
              if (avg_shift <= -0.26192) {
                return 2;
              } else {  // if avg_shift > -0.26192
                if (std_shift_y <= -0.45192) {
                  if (avg_err <= 0.22019) {
                    if (avg_err <= -0.11795) {
                      return 2;
                    } else {  // if avg_err > -0.11795
                      return 2;
                    }
                  } else {  // if avg_err > 0.22019
                    return 0;
                  }
                } else {  // if std_shift_y > -0.45192
                  if (avg_err <= -1.7321) {
                    return 0;
                  } else {  // if avg_err > -1.7321
                    if (std_shift_x <= 1.091) {
                      return 2;
                    } else {  // if std_shift_x > 1.091
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avg_shift_y > 0.39858
              if (avg_shift_y <= 1.9217) {
                return 0;
              } else {  // if avg_shift_y > 1.9217
                return 1;
              }
            }
          }
        }
      } else {  // if ncc > -1.0086
        if (width <= 0.37691) {
          if (width <= 0.35277) {
            if (xyRatio <= 0.53752) {
              if (xyRatio <= 0.00058139) {
                if (avg_err <= -0.5514) {
                  if (width <= 0.24411) {
                    if (ncc <= 0.25853) {
                      return 2;
                    } else {  // if ncc > 0.25853
                      return 2;
                    }
                  } else {  // if width > 0.24411
                    return 1;
                  }
                } else {  // if avg_err > -0.5514
                  if (width <= 0.15959) {
                    if (avg_shift_y <= -0.47628) {
                      return 2;
                    } else {  // if avg_shift_y > -0.47628
                      return 1;
                    }
                  } else {  // if width > 0.15959
                    if (width <= 0.19984) {
                      return 2;
                    } else {  // if width > 0.19984
                      return 2;
                    }
                  }
                }
              } else {  // if xyRatio > 0.00058139
                if (width <= 0.083128) {
                  if (avg_shift_y <= 0.55978) {
                    if (avg_shift <= -0.16733) {
                      return 1;
                    } else {  // if avg_shift > -0.16733
                      return 2;
                    }
                  } else {  // if avg_shift_y > 0.55978
                    if (ncc <= -0.78532) {
                      return 2;
                    } else {  // if ncc > -0.78532
                      return 1;
                    }
                  }
                } else {  // if width > 0.083128
                  if (avg_err <= -0.77699) {
                    if (avg_shift_y <= -0.21082) {
                      return 1;
                    } else {  // if avg_shift_y > -0.21082
                      return 1;
                    }
                  } else {  // if avg_err > -0.77699
                    if (std_shift <= -0.22405) {
                      return 0;
                    } else {  // if std_shift > -0.22405
                      return 1;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.53752
              if (avg_shift <= 3.0214) {
                if (avg_shift_x <= -0.33051) {
                  if (avg_shift_y <= 0.068151) {
                    if (width <= 0.16362) {
                      return 1;
                    } else {  // if width > 0.16362
                      return 2;
                    }
                  } else {  // if avg_shift_y > 0.068151
                    if (height <= 0.026793) {
                      return 2;
                    } else {  // if height > 0.026793
                      return 2;
                    }
                  }
                } else {  // if avg_shift_x > -0.33051
                  if (height <= 0.20109) {
                    if (width <= 0.091177) {
                      return 2;
                    } else {  // if width > 0.091177
                      return 2;
                    }
                  } else {  // if height > 0.20109
                    if (avg_err <= 0.18761) {
                      return 2;
                    } else {  // if avg_err > 0.18761
                      return 1;
                    }
                  }
                }
              } else {  // if avg_shift > 3.0214
                if (avg_shift_y <= -8.7792) {
                  return 2;
                } else {  // if avg_shift_y > -8.7792
                  if (width <= 0.17569) {
                    if (std_shift_x <= 4.7771) {
                      return 1;
                    } else {  // if std_shift_x > 4.7771
                      return 0;
                    }
                  } else {  // if width > 0.17569
                    if (std_shift_y <= 0.17009) {
                      return 1;
                    } else {  // if std_shift_y > 0.17009
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if width > 0.35277
            if (avg_shift_y <= -0.013149) {
              if (avg_shift_x <= 3.1119) {
                if (std_shift <= -0.25988) {
                  return 2;
                } else {  // if std_shift > -0.25988
                  if (ncc <= -0.70342) {
                    if (avg_err <= -0.18378) {
                      return 1;
                    } else {  // if avg_err > -0.18378
                      return 2;
                    }
                  } else {  // if ncc > -0.70342
                    return 1;
                  }
                }
              } else {  // if avg_shift_x > 3.1119
                if (std_shift_y <= 1.2943) {
                  if (avg_err <= 0.17918) {
                    return 1;
                  } else {  // if avg_err > 0.17918
                    return 2;
                  }
                } else {  // if std_shift_y > 1.2943
                  return 1;
                }
              }
            } else {  // if avg_shift_y > -0.013149
              if (avg_shift_y <= 0.39626) {
                if (avg_shift <= -0.17886) {
                  return 1;
                } else {  // if avg_shift > -0.17886
                  if (std_shift_y <= -0.0096165) {
                    return 2;
                  } else {  // if std_shift_y > -0.0096165
                    if (ncc <= -0.79224) {
                      return 2;
                    } else {  // if ncc > -0.79224
                      return 0;
                    }
                  }
                }
              } else {  // if avg_shift_y > 0.39626
                if (ncc <= -0.47039) {
                  return 0;
                } else {  // if ncc > -0.47039
                  if (avg_shift_x <= -0.92605) {
                    return 0;
                  } else {  // if avg_shift_x > -0.92605
                    return 1;
                  }
                }
              }
            }
          }
        } else {  // if width > 0.37691
          if (width <= 0.67473) {
            if (avg_shift_x <= -0.68157) {
              if (avg_shift_x <= -1.1896) {
                if (avg_shift_y <= -4.4672) {
                  return 1;
                } else {  // if avg_shift_y > -4.4672
                  if (avg_err <= 1.3644) {
                    if (ncc <= -0.98376) {
                      return 0;
                    } else {  // if ncc > -0.98376
                      return 2;
                    }
                  } else {  // if avg_err > 1.3644
                    if (std_shift <= 12.448) {
                      return 1;
                    } else {  // if std_shift > 12.448
                      return 0;
                    }
                  }
                }
              } else {  // if avg_shift_x > -1.1896
                if (avg_shift_y <= 0.60048) {
                  if (avg_shift_x <= -0.97857) {
                    if (avg_shift_x <= -0.99456) {
                      return 1;
                    } else {  // if avg_shift_x > -0.99456
                      return 2;
                    }
                  } else {  // if avg_shift_x > -0.97857
                    if (ncc <= -0.67971) {
                      return 0;
                    } else {  // if ncc > -0.67971
                      return 1;
                    }
                  }
                } else {  // if avg_shift_y > 0.60048
                  return 2;
                }
              }
            } else {  // if avg_shift_x > -0.68157
              if (std_shift_y <= -0.31239) {
                return 2;
              } else {  // if std_shift_y > -0.31239
                if (xyRatio <= 2.8239) {
                  if (xyRatio <= 1.415) {
                    if (avg_shift_x <= -0.42897) {
                      return 1;
                    } else {  // if avg_shift_x > -0.42897
                      return 2;
                    }
                  } else {  // if xyRatio > 1.415
                    return 2;
                  }
                } else {  // if xyRatio > 2.8239
                  if (std_shift_y <= 1.9322) {
                    if (avg_shift_x <= 0.3423) {
                      return 2;
                    } else {  // if avg_shift_x > 0.3423
                      return 1;
                    }
                  } else {  // if std_shift_y > 1.9322
                    if (avg_err <= -0.67111) {
                      return 1;
                    } else {  // if avg_err > -0.67111
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if width > 0.67473
            if (xyRatio <= 0.47352) {
              if (avg_shift_y <= -0.82971) {
                if (avg_shift_y <= -4.706) {
                  if (avg_shift_x <= 3.2065) {
                    return 2;
                  } else {  // if avg_shift_x > 3.2065
                    return 1;
                  }
                } else {  // if avg_shift_y > -4.706
                  if (avg_shift_x <= -0.0043807) {
                    if (avg_shift_x <= -0.51628) {
                      return 2;
                    } else {  // if avg_shift_x > -0.51628
                      return 2;
                    }
                  } else {  // if avg_shift_x > -0.0043807
                    if (avg_err <= -0.2689) {
                      return 1;
                    } else {  // if avg_err > -0.2689
                      return 2;
                    }
                  }
                }
              } else {  // if avg_shift_y > -0.82971
                if (std_shift_x <= 1.3652) {
                  if (std_shift_y <= 0.28268) {
                    if (avg_shift_x <= 1.3865) {
                      return 1;
                    } else {  // if avg_shift_x > 1.3865
                      return 2;
                    }
                  } else {  // if std_shift_y > 0.28268
                    if (avg_err <= -0.11303) {
                      return 2;
                    } else {  // if avg_err > -0.11303
                      return 2;
                    }
                  }
                } else {  // if std_shift_x > 1.3652
                  if (std_shift <= 0.37038) {
                    if (avg_shift <= 0.10453) {
                      return 1;
                    } else {  // if avg_shift > 0.10453
                      return 2;
                    }
                  } else {  // if std_shift > 0.37038
                    return 1;
                  }
                }
              }
            } else {  // if xyRatio > 0.47352
              if (xyRatio <= 1.3238) {
                if (width <= 0.68277) {
                  return 1;
                } else {  // if width > 0.68277
                  if (avg_err <= 0.4772) {
                    if (ncc <= -0.43873) {
                      return 0;
                    } else {  // if ncc > -0.43873
                      return 1;
                    }
                  } else {  // if avg_err > 0.4772
                    return 0;
                  }
                }
              } else {  // if xyRatio > 1.3238
                if (avg_shift_x <= 2.5493) {
                  if (std_shift_y <= 2.1745) {
                    return 2;
                  } else {  // if std_shift_y > 2.1745
                    if (width <= 0.89607) {
                      return 0;
                    } else {  // if width > 0.89607
                      return 1;
                    }
                  }
                } else {  // if avg_shift_x > 2.5493
                  if (width <= 0.71497) {
                    return 0;
                  } else {  // if width > 0.71497
                    if (height <= 0.24316) {
                      return 2;
                    } else {  // if height > 0.24316
                      return 2;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if label > -1.1061
      if (ncc <= -0.6412) {
        if (width <= 6.1078) {
          if (width <= -0.20261) {
            if (height <= -0.47205) {
              if (std_shift <= -0.28099) {
                if (height <= -0.49609) {
                  if (avg_shift <= -0.26128) {
                    if (avg_shift_x <= 0.0023816) {
                      return 2;
                    } else {  // if avg_shift_x > 0.0023816
                      return 2;
                    }
                  } else {  // if avg_shift > -0.26128
                    if (std_shift <= -0.28252) {
                      return 1;
                    } else {  // if std_shift > -0.28252
                      return 2;
                    }
                  }
                } else {  // if height > -0.49609
                  if (ncc <= -2.0216) {
                    return 2;
                  } else {  // if ncc > -2.0216
                    if (avg_shift <= -0.25721) {
                      return 1;
                    } else {  // if avg_shift > -0.25721
                      return 2;
                    }
                  }
                }
              } else {  // if std_shift > -0.28099
                if (xyRatio <= -0.65969) {
                  if (avg_shift_y <= -0.10408) {
                    return 0;
                  } else {  // if avg_shift_y > -0.10408
                    if (std_shift_x <= -0.57813) {
                      return 1;
                    } else {  // if std_shift_x > -0.57813
                      return 1;
                    }
                  }
                } else {  // if xyRatio > -0.65969
                  if (avg_err <= -1.0022) {
                    if (ncc <= -1.8736) {
                      return 2;
                    } else {  // if ncc > -1.8736
                      return 2;
                    }
                  } else {  // if avg_err > -1.0022
                    if (avg_shift_y <= 0.0606) {
                      return 1;
                    } else {  // if avg_shift_y > 0.0606
                      return 1;
                    }
                  }
                }
              }
            } else {  // if height > -0.47205
              if (height <= -0.44801) {
                if (avg_err <= 0.70917) {
                  if (avg_err <= -1.2451) {
                    if (std_shift <= -0.27317) {
                      return 1;
                    } else {  // if std_shift > -0.27317
                      return 2;
                    }
                  } else {  // if avg_err > -1.2451
                    if (std_shift <= 1.3111) {
                      return 2;
                    } else {  // if std_shift > 1.3111
                      return 1;
                    }
                  }
                } else {  // if avg_err > 0.70917
                  if (std_shift_x <= -0.17495) {
                    if (avg_shift_x <= 0.11355) {
                      return 1;
                    } else {  // if avg_shift_x > 0.11355
                      return 2;
                    }
                  } else {  // if std_shift_x > -0.17495
                    return 2;
                  }
                }
              } else {  // if height > -0.44801
                if (std_shift_x <= -0.54419) {
                  if (xyRatio <= -0.94293) {
                    if (width <= -0.29115) {
                      return 0;
                    } else {  // if width > -0.29115
                      return 2;
                    }
                  } else {  // if xyRatio > -0.94293
                    if (avg_err <= -1.8521) {
                      return 2;
                    } else {  // if avg_err > -1.8521
                      return 1;
                    }
                  }
                } else {  // if std_shift_x > -0.54419
                  if (avg_err <= -1.8535) {
                    return 2;
                  } else {  // if avg_err > -1.8535
                    if (xyRatio <= -0.3051) {
                      return 2;
                    } else {  // if xyRatio > -0.3051
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if width > -0.20261
            if (ncc <= -1.2116) {
              if (avg_err <= -0.76514) {
                if (width <= -0.18651) {
                  if (ncc <= -1.545) {
                    if (ncc <= -1.7732) {
                      return 0;
                    } else {  // if ncc > -1.7732
                      return 2;
                    }
                  } else {  // if ncc > -1.545
                    if (avg_err <= -1.2211) {
                      return 1;
                    } else {  // if avg_err > -1.2211
                      return 2;
                    }
                  }
                } else {  // if width > -0.18651
                  if (std_shift_y <= 3.1034) {
                    if (avg_shift_x <= 0.33944) {
                      return 2;
                    } else {  // if avg_shift_x > 0.33944
                      return 2;
                    }
                  } else {  // if std_shift_y > 3.1034
                    if (width <= 0.16764) {
                      return 0;
                    } else {  // if width > 0.16764
                      return 2;
                    }
                  }
                }
              } else {  // if avg_err > -0.76514
                if (std_shift_x <= -0.44914) {
                  if (xyRatio <= -0.54825) {
                    if (avg_shift_y <= -0.16347) {
                      return 0;
                    } else {  // if avg_shift_y > -0.16347
                      return 2;
                    }
                  } else {  // if xyRatio > -0.54825
                    if (height <= -0.34584) {
                      return 2;
                    } else {  // if height > -0.34584
                      return 0;
                    }
                  }
                } else {  // if std_shift_x > -0.44914
                  if (height <= 0.33331) {
                    if (std_shift_x <= 0.6958) {
                      return 2;
                    } else {  // if std_shift_x > 0.6958
                      return 2;
                    }
                  } else {  // if height > 0.33331
                    if (height <= 0.53165) {
                      return 2;
                    } else {  // if height > 0.53165
                      return 2;
                    }
                  }
                }
              }
            } else {  // if ncc > -1.2116
              if (std_shift <= -0.19714) {
                if (avg_err <= -1.5991) {
                  if (width <= 0.050932) {
                    if (avg_shift <= -0.26224) {
                      return 1;
                    } else {  // if avg_shift > -0.26224
                      return 2;
                    }
                  } else {  // if width > 0.050932
                    if (ncc <= -0.96049) {
                      return 2;
                    } else {  // if ncc > -0.96049
                      return 0;
                    }
                  }
                } else {  // if avg_err > -1.5991
                  if (avg_err <= 0.87854) {
                    if (avg_err <= -0.76908) {
                      return 2;
                    } else {  // if avg_err > -0.76908
                      return 0;
                    }
                  } else {  // if avg_err > 0.87854
                    if (xyRatio <= -0.44064) {
                      return 2;
                    } else {  // if xyRatio > -0.44064
                      return 2;
                    }
                  }
                }
              } else {  // if std_shift > -0.19714
                if (avg_shift_y <= -0.84889) {
                  if (avg_err <= -0.18906) {
                    if (std_shift_x <= 2.567) {
                      return 2;
                    } else {  // if std_shift_x > 2.567
                      return 0;
                    }
                  } else {  // if avg_err > -0.18906
                    if (width <= -0.0859) {
                      return 1;
                    } else {  // if width > -0.0859
                      return 2;
                    }
                  }
                } else {  // if avg_shift_y > -0.84889
                  if (std_shift_x <= 0.96882) {
                    if (height <= 0.67589) {
                      return 2;
                    } else {  // if height > 0.67589
                      return 0;
                    }
                  } else {  // if std_shift_x > 0.96882
                    if (height <= 0.40543) {
                      return 2;
                    } else {  // if height > 0.40543
                      return 2;
                    }
                  }
                }
              }
            }
          }
        } else {  // if width > 6.1078
          if (avg_shift <= -0.26236) {
            if (avg_shift <= -0.26238) {
              if (avg_err <= -1.6684) {
                return 0;
              } else {  // if avg_err > -1.6684
                if (avg_shift <= -0.26242) {
                  return 0;
                } else {  // if avg_shift > -0.26242
                  return 2;
                }
              }
            } else {  // if avg_shift > -0.26238
              return 2;
            }
          } else {  // if avg_shift > -0.26236
            if (ncc <= -1.6356) {
              if (ncc <= -1.6406) {
                return 0;
              } else {  // if ncc > -1.6406
                if (std_shift_x <= -0.43516) {
                  return 2;
                } else {  // if std_shift_x > -0.43516
                  if (std_shift_y <= -0.11051) {
                    return 0;
                  } else {  // if std_shift_y > -0.11051
                    return 2;
                  }
                }
              }
            } else {  // if ncc > -1.6356
              if (avg_shift_x <= -0.20645) {
                if (std_shift_y <= -0.084961) {
                  if (std_shift_y <= -0.12474) {
                    return 0;
                  } else {  // if std_shift_y > -0.12474
                    return 2;
                  }
                } else {  // if std_shift_y > -0.084961
                  return 0;
                }
              } else {  // if avg_shift_x > -0.20645
                if (width <= 12.583) {
                  if (ncc <= -1.5635) {
                    return 0;
                  } else {  // if ncc > -1.5635
                    if (std_shift_x <= -0.50534) {
                      return 2;
                    } else {  // if std_shift_x > -0.50534
                      return 0;
                    }
                  }
                } else {  // if width > 12.583
                  return 0;
                }
              }
            }
          }
        }
      } else {  // if ncc > -0.6412
        if (height <= -0.25568) {
          if (xyRatio <= 0.79285) {
            if (std_shift_x <= -0.33847) {
              if (height <= -0.44801) {
                if (avg_shift <= -0.25346) {
                  if (width <= -0.25895) {
                    if (height <= -0.47205) {
                      return 1;
                    } else {  // if height > -0.47205
                      return 2;
                    }
                  } else {  // if width > -0.25895
                    if (avg_shift_y <= 0.10231) {
                      return 1;
                    } else {  // if avg_shift_y > 0.10231
                      return 1;
                    }
                  }
                } else {  // if avg_shift > -0.25346
                  if (xyRatio <= 0.32685) {
                    if (avg_shift_y <= 0.17902) {
                      return 1;
                    } else {  // if avg_shift_y > 0.17902
                      return 1;
                    }
                  } else {  // if xyRatio > 0.32685
                    if (avg_shift_x <= 0.42063) {
                      return 2;
                    } else {  // if avg_shift_x > 0.42063
                      return 1;
                    }
                  }
                }
              } else {  // if height > -0.44801
                if (width <= -0.25895) {
                  if (xyRatio <= -0.90244) {
                    if (avg_shift_y <= 0.55646) {
                      return 2;
                    } else {  // if avg_shift_y > 0.55646
                      return 1;
                    }
                  } else {  // if xyRatio > -0.90244
                    if (xyRatio <= -0.79583) {
                      return 0;
                    } else {  // if xyRatio > -0.79583
                      return 0;
                    }
                  }
                } else {  // if width > -0.25895
                  if (height <= -0.31579) {
                    if (xyRatio <= -0.37988) {
                      return 1;
                    } else {  // if xyRatio > -0.37988
                      return 2;
                    }
                  } else {  // if height > -0.31579
                    if (width <= -0.20261) {
                      return 1;
                    } else {  // if width > -0.20261
                      return 0;
                    }
                  }
                }
              }
            } else {  // if std_shift_x > -0.33847
              if (height <= -0.46003) {
                if (height <= -0.50811) {
                  if (avg_shift_y <= 0.062021) {
                    if (avg_shift_x <= 0.39217) {
                      return 2;
                    } else {  // if avg_shift_x > 0.39217
                      return 1;
                    }
                  } else {  // if avg_shift_y > 0.062021
                    if (avg_shift <= -0.19518) {
                      return 1;
                    } else {  // if avg_shift > -0.19518
                      return 1;
                    }
                  }
                } else {  // if height > -0.50811
                  if (width <= -0.2509) {
                    if (xyRatio <= -0.3553) {
                      return 1;
                    } else {  // if xyRatio > -0.3553
                      return 1;
                    }
                  } else {  // if width > -0.2509
                    if (xyRatio <= 0.10375) {
                      return 2;
                    } else {  // if xyRatio > 0.10375
                      return 1;
                    }
                  }
                }
              } else {  // if height > -0.46003
                if (avg_shift_x <= -0.7333) {
                  if (avg_err <= 0.45968) {
                    if (ncc <= -0.44529) {
                      return 1;
                    } else {  // if ncc > -0.44529
                      return 2;
                    }
                  } else {  // if avg_err > 0.45968
                    if (std_shift_x <= 0.43706) {
                      return 1;
                    } else {  // if std_shift_x > 0.43706
                      return 2;
                    }
                  }
                } else {  // if avg_shift_x > -0.7333
                  if (avg_shift <= -0.044323) {
                    if (std_shift_x <= -0.12701) {
                      return 2;
                    } else {  // if std_shift_x > -0.12701
                      return 2;
                    }
                  } else {  // if avg_shift > -0.044323
                    if (std_shift_y <= -0.35175) {
                      return 2;
                    } else {  // if std_shift_y > -0.35175
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.79285
            if (width <= 0.058981) {
              if (width <= -0.093949) {
                if (avg_shift <= -0.032185) {
                  if (avg_shift <= -0.13699) {
                    if (std_shift <= -0.27965) {
                      return 2;
                    } else {  // if std_shift > -0.27965
                      return 1;
                    }
                  } else {  // if avg_shift > -0.13699
                    if (avg_shift <= -0.072714) {
                      return 0;
                    } else {  // if avg_shift > -0.072714
                      return 1;
                    }
                  }
                } else {  // if avg_shift > -0.032185
                  if (avg_shift_y <= -0.81614) {
                    if (width <= -0.11407) {
                      return 2;
                    } else {  // if width > -0.11407
                      return 0;
                    }
                  } else {  // if avg_shift_y > -0.81614
                    if (avg_shift <= 0.25397) {
                      return 0;
                    } else {  // if avg_shift > 0.25397
                      return 1;
                    }
                  }
                }
              } else {  // if width > -0.093949
                if (width <= -0.029558) {
                  if (avg_shift_x <= -0.9491) {
                    if (std_shift_x <= -0.31568) {
                      return 0;
                    } else {  // if std_shift_x > -0.31568
                      return 0;
                    }
                  } else {  // if avg_shift_x > -0.9491
                    if (avg_shift_y <= -0.40555) {
                      return 0;
                    } else {  // if avg_shift_y > -0.40555
                      return 0;
                    }
                  }
                } else {  // if width > -0.029558
                  if (avg_shift <= -0.041295) {
                    if (ncc <= -0.12946) {
                      return 0;
                    } else {  // if ncc > -0.12946
                      return 0;
                    }
                  } else {  // if avg_shift > -0.041295
                    if (std_shift <= -0.23018) {
                      return 1;
                    } else {  // if std_shift > -0.23018
                      return 0;
                    }
                  }
                }
              }
            } else {  // if width > 0.058981
              if (avg_shift_x <= 1.4127) {
                if (avg_shift_x <= 0.7736) {
                  if (ncc <= -0.21615) {
                    if (std_shift_x <= 0.8667) {
                      return 1;
                    } else {  // if std_shift_x > 0.8667
                      return 2;
                    }
                  } else {  // if ncc > -0.21615
                    return 2;
                  }
                } else {  // if avg_shift_x > 0.7736
                  if (avg_err <= 0.020782) {
                    return 1;
                  } else {  // if avg_err > 0.020782
                    if (avg_shift_x <= 1.1359) {
                      return 1;
                    } else {  // if avg_shift_x > 1.1359
                      return 2;
                    }
                  }
                }
              } else {  // if avg_shift_x > 1.4127
                return 0;
              }
            }
          }
        } else {  // if height > -0.25568
          if (avg_shift_y <= -0.54817) {
            if (ncc <= -0.28434) {
              if (width <= -0.13822) {
                if (xyRatio <= -0.59983) {
                  if (avg_shift <= -0.18325) {
                    return 2;
                  } else {  // if avg_shift > -0.18325
                    if (avg_shift_x <= -0.2331) {
                      return 2;
                    } else {  // if avg_shift_x > -0.2331
                      return 1;
                    }
                  }
                } else {  // if xyRatio > -0.59983
                  if (std_shift_x <= 0.43645) {
                    if (std_shift <= -0.1712) {
                      return 0;
                    } else {  // if std_shift > -0.1712
                      return 0;
                    }
                  } else {  // if std_shift_x > 0.43645
                    return 2;
                  }
                }
              } else {  // if width > -0.13822
                if (avg_shift_x <= 0.76064) {
                  if (xyRatio <= 0.13195) {
                    if (avg_err <= 1.399) {
                      return 2;
                    } else {  // if avg_err > 1.399
                      return 1;
                    }
                  } else {  // if xyRatio > 0.13195
                    if (avg_err <= -0.86217) {
                      return 0;
                    } else {  // if avg_err > -0.86217
                      return 2;
                    }
                  }
                } else {  // if avg_shift_x > 0.76064
                  if (std_shift <= 0.44959) {
                    if (avg_shift <= -0.095991) {
                      return 2;
                    } else {  // if avg_shift > -0.095991
                      return 1;
                    }
                  } else {  // if std_shift > 0.44959
                    if (avg_shift <= 1.1573) {
                      return 2;
                    } else {  // if avg_shift > 1.1573
                      return 1;
                    }
                  }
                }
              }
            } else {  // if ncc > -0.28434
              if (xyRatio <= -0.62183) {
                if (xyRatio <= -0.77137) {
                  if (avg_shift <= -0.21798) {
                    if (std_shift_x <= -0.36131) {
                      return 1;
                    } else {  // if std_shift_x > -0.36131
                      return 2;
                    }
                  } else {  // if avg_shift > -0.21798
                    if (avg_shift <= 0.79046) {
                      return 2;
                    } else {  // if avg_shift > 0.79046
                      return 0;
                    }
                  }
                } else {  // if xyRatio > -0.77137
                  if (std_shift <= -0.1416) {
                    if (avg_shift <= -0.19369) {
                      return 2;
                    } else {  // if avg_shift > -0.19369
                      return 1;
                    }
                  } else {  // if std_shift > -0.1416
                    if (avg_shift_y <= -0.57823) {
                      return 2;
                    } else {  // if avg_shift_y > -0.57823
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > -0.62183
                if (width <= -0.13822) {
                  if (width <= -0.17444) {
                    if (xyRatio <= -0.43933) {
                      return 2;
                    } else {  // if xyRatio > -0.43933
                      return 0;
                    }
                  } else {  // if width > -0.17444
                    if (std_shift_x <= 2.0033) {
                      return 0;
                    } else {  // if std_shift_x > 2.0033
                      return 2;
                    }
                  }
                } else {  // if width > -0.13822
                  if (height <= 0.79008) {
                    if (xyRatio <= -0.098103) {
                      return 1;
                    } else {  // if xyRatio > -0.098103
                      return 2;
                    }
                  } else {  // if height > 0.79008
                    if (xyRatio <= 0.18588) {
                      return 0;
                    } else {  // if xyRatio > 0.18588
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if avg_shift_y > -0.54817
            if (height <= 0.35735) {
              if (avg_err <= -0.39523) {
                if (width <= -0.057729) {
                  if (height <= -0.18356) {
                    if (avg_err <= -0.81841) {
                      return 2;
                    } else {  // if avg_err > -0.81841
                      return 2;
                    }
                  } else {  // if height > -0.18356
                    if (std_shift_x <= -0.5705) {
                      return 0;
                    } else {  // if std_shift_x > -0.5705
                      return 2;
                    }
                  }
                } else {  // if width > -0.057729
                  if (avg_err <= -1.1258) {
                    if (width <= 0.16764) {
                      return 0;
                    } else {  // if width > 0.16764
                      return 2;
                    }
                  } else {  // if avg_err > -1.1258
                    if (std_shift <= -0.12945) {
                      return 0;
                    } else {  // if std_shift > -0.12945
                      return 0;
                    }
                  }
                }
              } else {  // if avg_err > -0.39523
                if (width <= -0.18651) {
                  if (xyRatio <= -0.54058) {
                    if (width <= -0.21066) {
                      return 1;
                    } else {  // if width > -0.21066
                      return 0;
                    }
                  } else {  // if xyRatio > -0.54058
                    if (xyRatio <= -0.43398) {
                      return 2;
                    } else {  // if xyRatio > -0.43398
                      return 0;
                    }
                  }
                } else {  // if width > -0.18651
                  if (avg_shift_y <= 1.3204) {
                    if (avg_shift_x <= 0.041404) {
                      return 0;
                    } else {  // if avg_shift_x > 0.041404
                      return 0;
                    }
                  } else {  // if avg_shift_y > 1.3204
                    if (avg_shift <= 2.9919) {
                      return 0;
                    } else {  // if avg_shift > 2.9919
                      return 2;
                    }
                  }
                }
              }
            } else {  // if height > 0.35735
              if (avg_shift_x <= -4.313) {
                if (avg_err <= 1.193) {
                  if (avg_shift_x <= -6.2308) {
                    if (avg_shift_y <= 1.2955) {
                      return 2;
                    } else {  // if avg_shift_y > 1.2955
                      return 0;
                    }
                  } else {  // if avg_shift_x > -6.2308
                    if (width <= 0.33667) {
                      return 2;
                    } else {  // if width > 0.33667
                      return 1;
                    }
                  }
                } else {  // if avg_err > 1.193
                  if (std_shift <= 8.0809) {
                    if (std_shift_y <= 1.6715) {
                      return 2;
                    } else {  // if std_shift_y > 1.6715
                      return 1;
                    }
                  } else {  // if std_shift > 8.0809
                    return 2;
                  }
                }
              } else {  // if avg_shift_x > -4.313
                if (avg_shift_x <= -1.4545) {
                  if (avg_shift_y <= 2.5373) {
                    if (height <= 0.6218) {
                      return 2;
                    } else {  // if height > 0.6218
                      return 1;
                    }
                  } else {  // if avg_shift_y > 2.5373
                    if (std_shift_x <= 0.33773) {
                      return 0;
                    } else {  // if std_shift_x > 0.33773
                      return 1;
                    }
                  }
                } else {  // if avg_shift_x > -1.4545
                  if (width <= 0.10325) {
                    if (height <= 0.5016) {
                      return 2;
                    } else {  // if height > 0.5016
                      return 0;
                    }
                  } else {  // if width > 0.10325
                    if (xyRatio <= -0.36332) {
                      return 0;
                    } else {  // if xyRatio > -0.36332
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

} // namespace rm
