#include "strm/tree/YouTube.hpp"

namespace rm {

float YouTube(float width, float height, float label, float xyRatio, float avg_shift_x,
              float avg_shift_y, float avg_shift, float std_shift_x, float std_shift_y,
              float std_shift, float avg_err, float ncc) {
  if (height <= -0.84787) {
    if (width <= -0.46933) {
      if (height <= -1.0813) {
        return 0;
      } else {  // if height > -1.0813
        if (xyRatio <= 0.02004) {
          if (avg_shift_x <= -0.24486) {
            if (avg_err <= 0.6735) {
              return 1;
            } else {  // if avg_err > 0.6735
              if (avg_shift_x <= -0.27815) {
                return 0;
              } else {  // if avg_shift_x > -0.27815
                return 1;
              }
            }
          } else {  // if avg_shift_x > -0.24486
            if (std_shift_x <= -0.14601) {
              if (xyRatio <= -0.072677) {
                if (ncc <= -0.504) {
                  if (ncc <= -0.75949) {
                    if (avg_shift_x <= 0.070433) {
                      return 0;
                    } else {  // if avg_shift_x > 0.070433
                      return 0;
                    }
                  } else {  // if ncc > -0.75949
                    if (avg_shift_y <= 0.14455) {
                      return 0;
                    } else {  // if avg_shift_y > 0.14455
                      return 1;
                    }
                  }
                } else {  // if ncc > -0.504
                  if (ncc <= 0.40911) {
                    if (ncc <= 0.33403) {
                      return 0;
                    } else {  // if ncc > 0.33403
                      return 1;
                    }
                  } else {  // if ncc > 0.40911
                    return 0;
                  }
                }
              } else {  // if xyRatio > -0.072677
                return 0;
              }
            } else {  // if std_shift_x > -0.14601
              if (std_shift <= -0.11986) {
                return 1;
              } else {  // if std_shift > -0.11986
                return 0;
              }
            }
          }
        } else {  // if xyRatio > 0.02004
          if (avg_shift_x <= -0.38436) {
            if (std_shift_x <= -0.21164) {
              return 0;
            } else {  // if std_shift_x > -0.21164
              if (ncc <= 2.3381) {
                if (avg_shift_y <= -0.064541) {
                  return 1;
                } else {  // if avg_shift_y > -0.064541
                  if (ncc <= 1.6143) {
                    if (std_shift <= 0.3363) {
                      return 1;
                    } else {  // if std_shift > 0.3363
                      return 0;
                    }
                  } else {  // if ncc > 1.6143
                    return 0;
                  }
                }
              } else {  // if ncc > 2.3381
                return 1;
              }
            }
          } else {  // if avg_shift_x > -0.38436
            if (std_shift <= -0.1249) {
              if (ncc <= -0.17238) {
                if (avg_shift_x <= 0.073187) {
                  if (std_shift_x <= -0.27784) {
                    return 1;
                  } else {  // if std_shift_x > -0.27784
                    return 0;
                  }
                } else {  // if avg_shift_x > 0.073187
                  if (height <= -1.0371) {
                    return 1;
                  } else {  // if height > -1.0371
                    if (std_shift_y <= -0.42251) {
                      return 0;
                    } else {  // if std_shift_y > -0.42251
                      return 0;
                    }
                  }
                }
              } else {  // if ncc > -0.17238
                if (width <= -0.51239) {
                  return 0;
                } else {  // if width > -0.51239
                  if (std_shift_x <= -0.3777) {
                    return 1;
                  } else {  // if std_shift_x > -0.3777
                    return 0;
                  }
                }
              }
            } else {  // if std_shift > -0.1249
              if (std_shift <= -0.1118) {
                if (ncc <= 1.5245) {
                  if (ncc <= -0.74135) {
                    if (avg_err <= 0.66475) {
                      return 1;
                    } else {  // if avg_err > 0.66475
                      return 0;
                    }
                  } else {  // if ncc > -0.74135
                    return 1;
                  }
                } else {  // if ncc > 1.5245
                  if (ncc <= 1.6804) {
                    return 0;
                  } else {  // if ncc > 1.6804
                    if (std_shift_y <= -0.28326) {
                      return 1;
                    } else {  // if std_shift_y > -0.28326
                      return 0;
                    }
                  }
                }
              } else {  // if std_shift > -0.1118
                if (avg_err <= 0.93244) {
                  if (ncc <= 1.5765) {
                    return 0;
                  } else {  // if ncc > 1.5765
                    return 1;
                  }
                } else {  // if avg_err > 0.93244
                  if (avg_err <= 2.1644) {
                    return 1;
                  } else {  // if avg_err > 2.1644
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    } else {  // if width > -0.46933
      if (avg_shift_y <= -0.78537) {
        if (avg_shift_x <= 2.3733) {
          return 0;
        } else {  // if avg_shift_x > 2.3733
          if (std_shift_x <= 3.2257) {
            return 1;
          } else {  // if std_shift_x > 3.2257
            return 2;
          }
        }
      } else {  // if avg_shift_y > -0.78537
        if (avg_shift_y <= 0.1593) {
          if (std_shift <= -0.096753) {
            if (std_shift_x <= -0.30219) {
              if (std_shift_y <= -0.26711) {
                if (avg_err <= -1.1155) {
                  if (avg_shift_x <= 0.073905) {
                    return 1;
                  } else {  // if avg_shift_x > 0.073905
                    return 0;
                  }
                } else {  // if avg_err > -1.1155
                  return 1;
                }
              } else {  // if std_shift_y > -0.26711
                if (avg_shift_y <= -0.36056) {
                  return 0;
                } else {  // if avg_shift_y > -0.36056
                  return 1;
                }
              }
            } else {  // if std_shift_x > -0.30219
              return 1;
            }
          } else {  // if std_shift > -0.096753
            return 0;
          }
        } else {  // if avg_shift_y > 0.1593
          return 0;
        }
      }
    }
  } else {  // if height > -0.84787
    if (avg_err <= 0.36637) {
      if (height <= 0.36331) {
        if (height <= 0.054206) {
          if (height <= -0.57662) {
            if (xyRatio <= -0.41098) {
              if (height <= -0.60816) {
                if (avg_shift <= -0.17201) {
                  if (avg_shift_y <= 0.18332) {
                    return 1;
                  } else {  // if avg_shift_y > 0.18332
                    if (avg_shift_y <= 0.18371) {
                      return 2;
                    } else {  // if avg_shift_y > 0.18371
                      return 1;
                    }
                  }
                } else {  // if avg_shift > -0.17201
                  if (avg_shift <= -0.16953) {
                    if (std_shift_x <= -0.35194) {
                      return 1;
                    } else {  // if std_shift_x > -0.35194
                      return 2;
                    }
                  } else {  // if avg_shift > -0.16953
                    return 1;
                  }
                }
              } else {  // if height > -0.60816
                if (height <= -0.58924) {
                  if (std_shift_x <= -0.33729) {
                    if (avg_shift_x <= 0.073633) {
                      return 1;
                    } else {  // if avg_shift_x > 0.073633
                      return 2;
                    }
                  } else {  // if std_shift_x > -0.33729
                    if (std_shift_x <= -0.25823) {
                      return 1;
                    } else {  // if std_shift_x > -0.25823
                      return 2;
                    }
                  }
                } else {  // if height > -0.58924
                  if (std_shift_y <= -0.24193) {
                    if (std_shift <= -0.12346) {
                      return 1;
                    } else {  // if std_shift > -0.12346
                      return 2;
                    }
                  } else {  // if std_shift_y > -0.24193
                    return 2;
                  }
                }
              }
            } else {  // if xyRatio > -0.41098
              if (height <= -0.7911) {
                if (width <= 0.19802) {
                  return 1;
                } else {  // if width > 0.19802
                  return 2;
                }
              } else {  // if height > -0.7911
                if (avg_shift_y <= 0.15928) {
                  if (ncc <= 0.79463) {
                    if (avg_shift_y <= 0.088047) {
                      return 0;
                    } else {  // if avg_shift_y > 0.088047
                      return 0;
                    }
                  } else {  // if ncc > 0.79463
                    return 1;
                  }
                } else {  // if avg_shift_y > 0.15928
                  if (avg_err <= -0.39925) {
                    if (std_shift_y <= -0.40336) {
                      return 1;
                    } else {  // if std_shift_y > -0.40336
                      return 0;
                    }
                  } else {  // if avg_err > -0.39925
                    if (avg_shift_y <= 0.16627) {
                      return 0;
                    } else {  // if avg_shift_y > 0.16627
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if height > -0.57662
            if (label <= -1.1801) {
              if (avg_shift_x <= 0.070629) {
                if (avg_shift_y <= 0.26366) {
                  if (std_shift <= -0.12078) {
                    if (std_shift_x <= -0.19949) {
                      return 2;
                    } else {  // if std_shift_x > -0.19949
                      return 1;
                    }
                  } else {  // if std_shift > -0.12078
                    if (avg_shift_x <= -0.40884) {
                      return 2;
                    } else {  // if avg_shift_x > -0.40884
                      return 2;
                    }
                  }
                } else {  // if avg_shift_y > 0.26366
                  if (avg_err <= -0.00083721) {
                    return 2;
                  } else {  // if avg_err > -0.00083721
                    if (avg_shift_y <= 0.30653) {
                      return 1;
                    } else {  // if avg_shift_y > 0.30653
                      return 0;
                    }
                  }
                }
              } else {  // if avg_shift_x > 0.070629
                if (avg_shift_x <= 3.8146) {
                  if (xyRatio <= -0.13071) {
                    if (avg_shift_y <= 0.15941) {
                      return 1;
                    } else {  // if avg_shift_y > 0.15941
                      return 0;
                    }
                  } else {  // if xyRatio > -0.13071
                    if (avg_err <= -1.078) {
                      return 1;
                    } else {  // if avg_err > -1.078
                      return 2;
                    }
                  }
                } else {  // if avg_shift_x > 3.8146
                  if (avg_shift_x <= 3.9692) {
                    if (avg_shift <= 3.3859) {
                      return 1;
                    } else {  // if avg_shift > 3.3859
                      return 2;
                    }
                  } else {  // if avg_shift_x > 3.9692
                    return 1;
                  }
                }
              }
            } else {  // if label > -1.1801
              if (height <= -0.43153) {
                if (height <= -0.4883) {
                  if (xyRatio <= -0.56331) {
                    return 0;
                  } else {  // if xyRatio > -0.56331
                    if (height <= -0.51354) {
                      return 1;
                    } else {  // if height > -0.51354
                      return 0;
                    }
                  }
                } else {  // if height > -0.4883
                  if (std_shift_x <= -0.43969) {
                    if (xyRatio <= -0.69835) {
                      return 1;
                    } else {  // if xyRatio > -0.69835
                      return 0;
                    }
                  } else {  // if std_shift_x > -0.43969
                    if (xyRatio <= 0.15004) {
                      return 1;
                    } else {  // if xyRatio > 0.15004
                      return 1;
                    }
                  }
                }
              } else {  // if height > -0.43153
                if (height <= -0.19812) {
                  if (height <= -0.27382) {
                    if (height <= -0.31167) {
                      return 1;
                    } else {  // if height > -0.31167
                      return 2;
                    }
                  } else {  // if height > -0.27382
                    if (xyRatio <= -0.39519) {
                      return 1;
                    } else {  // if xyRatio > -0.39519
                      return 2;
                    }
                  }
                } else {  // if height > -0.19812
                  if (width <= -0.37246) {
                    if (ncc <= -0.51889) {
                      return 2;
                    } else {  // if ncc > -0.51889
                      return 2;
                    }
                  } else {  // if width > -0.37246
                    if (xyRatio <= -0.51482) {
                      return 0;
                    } else {  // if xyRatio > -0.51482
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if height > 0.054206
          if (xyRatio <= -0.35447) {
            if (xyRatio <= -0.58742) {
              if (avg_err <= 0.31719) {
                if (std_shift_x <= -0.44443) {
                  return 0;
                } else {  // if std_shift_x > -0.44443
                  if (avg_shift_y <= 0.16616) {
                    return 2;
                  } else {  // if avg_shift_y > 0.16616
                    if (std_shift_y <= -0.29176) {
                      return 0;
                    } else {  // if std_shift_y > -0.29176
                      return 2;
                    }
                  }
                }
              } else {  // if avg_err > 0.31719
                return 0;
              }
            } else {  // if xyRatio > -0.58742
              if (ncc <= -0.82952) {
                if (std_shift_y <= -0.40769) {
                  if (std_shift_x <= -0.44696) {
                    if (std_shift_y <= -0.42198) {
                      return 0;
                    } else {  // if std_shift_y > -0.42198
                      return 1;
                    }
                  } else {  // if std_shift_x > -0.44696
                    if (avg_err <= -1.0841) {
                      return 0;
                    } else {  // if avg_err > -1.0841
                      return 0;
                    }
                  }
                } else {  // if std_shift_y > -0.40769
                  if (avg_shift <= -0.17301) {
                    if (std_shift_x <= -0.43747) {
                      return 0;
                    } else {  // if std_shift_x > -0.43747
                      return 1;
                    }
                  } else {  // if avg_shift > -0.17301
                    if (avg_shift_y <= 0.10387) {
                      return 1;
                    } else {  // if avg_shift_y > 0.10387
                      return 0;
                    }
                  }
                }
              } else {  // if ncc > -0.82952
                if (std_shift_x <= 0.10139) {
                  if (xyRatio <= -0.52483) {
                    if (avg_shift_x <= -0.22092) {
                      return 1;
                    } else {  // if avg_shift_x > -0.22092
                      return 0;
                    }
                  } else {  // if xyRatio > -0.52483
                    if (avg_shift_x <= 0.045555) {
                      return 0;
                    } else {  // if avg_shift_x > 0.045555
                      return 0;
                    }
                  }
                } else {  // if std_shift_x > 0.10139
                  if (xyRatio <= -0.46781) {
                    return 0;
                  } else {  // if xyRatio > -0.46781
                    return 2;
                  }
                }
              }
            }
          } else {  // if xyRatio > -0.35447
            if (label <= -1.1801) {
              if (height <= 0.29392) {
                if (avg_shift_y <= 0.25582) {
                  if (ncc <= -1.0027) {
                    return 1;
                  } else {  // if ncc > -1.0027
                    if (avg_shift_x <= 1.0349) {
                      return 2;
                    } else {  // if avg_shift_x > 1.0349
                      return 2;
                    }
                  }
                } else {  // if avg_shift_y > 0.25582
                  if (avg_shift_y <= 0.34306) {
                    if (ncc <= -0.55178) {
                      return 1;
                    } else {  // if ncc > -0.55178
                      return 2;
                    }
                  } else {  // if avg_shift_y > 0.34306
                    return 2;
                  }
                }
              } else {  // if height > 0.29392
                if (avg_shift_x <= -0.062118) {
                  if (avg_shift_x <= -0.22455) {
                    if (std_shift <= -0.094189) {
                      return 2;
                    } else {  // if std_shift > -0.094189
                      return 1;
                    }
                  } else {  // if avg_shift_x > -0.22455
                    return 1;
                  }
                } else {  // if avg_shift_x > -0.062118
                  if (ncc <= -0.76829) {
                    if (avg_shift_y <= 0.16757) {
                      return 1;
                    } else {  // if avg_shift_y > 0.16757
                      return 2;
                    }
                  } else {  // if ncc > -0.76829
                    return 2;
                  }
                }
              }
            } else {  // if label > -1.1801
              if (avg_shift_x <= -0.10515) {
                return 0;
              } else {  // if avg_shift_x > -0.10515
                if (avg_err <= -1.0684) {
                  if (xyRatio <= 0.22169) {
                    if (xyRatio <= -0.34324) {
                      return 0;
                    } else {  // if xyRatio > -0.34324
                      return 0;
                    }
                  } else {  // if xyRatio > 0.22169
                    if (avg_shift <= -0.17306) {
                      return 2;
                    } else {  // if avg_shift > -0.17306
                      return 1;
                    }
                  }
                } else {  // if avg_err > -1.0684
                  if (std_shift <= -0.044032) {
                    if (ncc <= -1.0018) {
                      return 0;
                    } else {  // if ncc > -1.0018
                      return 1;
                    }
                  } else {  // if std_shift > -0.044032
                    return 2;
                  }
                }
              }
            }
          }
        }
      } else {  // if height > 0.36331
        if (xyRatio <= 0.34877) {
          if (width <= -0.0064932) {
            if (ncc <= -1.2425) {
              return 0;
            } else {  // if ncc > -1.2425
              if (xyRatio <= -0.47866) {
                if (xyRatio <= -1.4993) {
                  if (avg_shift_y <= 0.16349) {
                    return 2;
                  } else {  // if avg_shift_y > 0.16349
                    return 0;
                  }
                } else {  // if xyRatio > -1.4993
                  return 2;
                }
              } else {  // if xyRatio > -0.47866
                if (avg_shift_x <= 0.073224) {
                  if (avg_shift_x <= -0.069925) {
                    return 1;
                  } else {  // if avg_shift_x > -0.069925
                    if (avg_err <= -0.53178) {
                      return 2;
                    } else {  // if avg_err > -0.53178
                      return 2;
                    }
                  }
                } else {  // if avg_shift_x > 0.073224
                  if (avg_shift_y <= 0.15485) {
                    return 1;
                  } else {  // if avg_shift_y > 0.15485
                    if (avg_err <= -0.83029) {
                      return 1;
                    } else {  // if avg_err > -0.83029
                      return 2;
                    }
                  }
                }
              }
            }
          } else {  // if width > -0.0064932
            if (xyRatio <= 0.20401) {
              if (avg_shift_y <= -0.38633) {
                if (avg_shift_y <= -2.1036) {
                  return 2;
                } else {  // if avg_shift_y > -2.1036
                  if (std_shift <= 0.02348) {
                    return 1;
                  } else {  // if std_shift > 0.02348
                    if (ncc <= -0.59826) {
                      return 1;
                    } else {  // if ncc > -0.59826
                      return 0;
                    }
                  }
                }
              } else {  // if avg_shift_y > -0.38633
                if (height <= 0.42639) {
                  if (ncc <= -0.70234) {
                    if (avg_err <= -1.0428) {
                      return 0;
                    } else {  // if avg_err > -1.0428
                      return 2;
                    }
                  } else {  // if ncc > -0.70234
                    if (std_shift_y <= -0.028869) {
                      return 0;
                    } else {  // if std_shift_y > -0.028869
                      return 1;
                    }
                  }
                } else {  // if height > 0.42639
                  if (xyRatio <= -0.18437) {
                    if (xyRatio <= -0.45109) {
                      return 0;
                    } else {  // if xyRatio > -0.45109
                      return 0;
                    }
                  } else {  // if xyRatio > -0.18437
                    if (width <= 2.4692) {
                      return 1;
                    } else {  // if width > 2.4692
                      return 0;
                    }
                  }
                }
              }
            } else {  // if xyRatio > 0.20401
              if (height <= 3.5427) {
                if (avg_shift <= -0.16593) {
                  if (ncc <= -1.0017) {
                    return 1;
                  } else {  // if ncc > -1.0017
                    return 2;
                  }
                } else {  // if avg_shift > -0.16593
                  if (ncc <= -0.84761) {
                    return 2;
                  } else {  // if ncc > -0.84761
                    return 1;
                  }
                }
              } else {  // if height > 3.5427
                if (avg_shift <= -0.13519) {
                  if (std_shift_y <= -0.41443) {
                    if (avg_err <= -1.1186) {
                      return 2;
                    } else {  // if avg_err > -1.1186
                      return 1;
                    }
                  } else {  // if std_shift_y > -0.41443
                    if (avg_shift_x <= -0.20968) {
                      return 0;
                    } else {  // if avg_shift_x > -0.20968
                      return 2;
                    }
                  }
                } else {  // if avg_shift > -0.13519
                  if (avg_shift_x <= -0.9045) {
                    return 0;
                  } else {  // if avg_shift_x > -0.9045
                    return 1;
                  }
                }
              }
            }
          }
        } else {  // if xyRatio > 0.34877
          if (label <= -1.1801) {
            if (std_shift <= -0.10256) {
              if (avg_shift_x <= -0.089327) {
                if (avg_err <= -0.15855) {
                  if (std_shift <= -0.12121) {
                    return 1;
                  } else {  // if std_shift > -0.12121
                    return 2;
                  }
                } else {  // if avg_err > -0.15855
                  return 1;
                }
              } else {  // if avg_shift_x > -0.089327
                if (avg_shift_y <= 0.15971) {
                  if (height <= 2.4513) {
                    if (avg_err <= 0.020594) {
                      return 2;
                    } else {  // if avg_err > 0.020594
                      return 1;
                    }
                  } else {  // if height > 2.4513
                    return 1;
                  }
                } else {  // if avg_shift_y > 0.15971
                  if (avg_shift_x <= 0.073428) {
                    return 2;
                  } else {  // if avg_shift_x > 0.073428
                    return 0;
                  }
                }
              }
            } else {  // if std_shift > -0.10256
              if (avg_shift_y <= 0.31964) {
                if (std_shift_y <= -0.12816) {
                  if (std_shift <= -0.05823) {
                    return 2;
                  } else {  // if std_shift > -0.05823
                    return 0;
                  }
                } else {  // if std_shift_y > -0.12816
                  if (avg_err <= 0.32967) {
                    return 2;
                  } else {  // if avg_err > 0.32967
                    if (avg_shift <= -0.12488) {
                      return 0;
                    } else {  // if avg_shift > -0.12488
                      return 2;
                    }
                  }
                }
              } else {  // if avg_shift_y > 0.31964
                return 0;
              }
            }
          } else {  // if label > -1.1801
            if (width <= 3.9761) {
              if (height <= 3.4228) {
                if (ncc <= -0.97182) {
                  if (std_shift_y <= -0.35506) {
                    return 2;
                  } else {  // if std_shift_y > -0.35506
                    return 0;
                  }
                } else {  // if ncc > -0.97182
                  return 2;
                }
              } else {  // if height > 3.4228
                if (std_shift_x <= -0.35921) {
                  if (avg_shift_x <= 0.10013) {
                    return 2;
                  } else {  // if avg_shift_x > 0.10013
                    if (avg_err <= -0.94331) {
                      return 1;
                    } else {  // if avg_err > -0.94331
                      return 2;
                    }
                  }
                } else {  // if std_shift_x > -0.35921
                  if (std_shift_x <= -0.35209) {
                    return 0;
                  } else {  // if std_shift_x > -0.35209
                    return 2;
                  }
                }
              }
            } else {  // if width > 3.9761
              if (avg_shift <= -0.17304) {
                if (std_shift_x <= -0.4177) {
                  return 2;
                } else {  // if std_shift_x > -0.4177
                  return 0;
                }
              } else {  // if avg_shift > -0.17304
                return 0;
              }
            }
          }
        }
      }
    } else {  // if avg_err > 0.36637
      if (height <= 0.054206) {
        if (avg_shift_y <= 0.12977) {
          if (height <= -0.37476) {
            if (width <= 0.32718) {
              if (height <= -0.38737) {
                if (height <= -0.45045) {
                  if (avg_shift <= -0.16872) {
                    if (std_shift_x <= -0.1838) {
                      return 0;
                    } else {  // if std_shift_x > -0.1838
                      return 1;
                    }
                  } else {  // if avg_shift > -0.16872
                    if (height <= -0.74063) {
                      return 1;
                    } else {  // if height > -0.74063
                      return 1;
                    }
                  }
                } else {  // if height > -0.45045
                  if (std_shift <= -0.079798) {
                    if (avg_err <= 1.7233) {
                      return 1;
                    } else {  // if avg_err > 1.7233
                      return 2;
                    }
                  } else {  // if std_shift > -0.079798
                    if (std_shift <= -0.060188) {
                      return 2;
                    } else {  // if std_shift > -0.060188
                      return 2;
                    }
                  }
                }
              } else {  // if height > -0.38737
                if (std_shift_x <= -0.12946) {
                  return 1;
                } else {  // if std_shift_x > -0.12946
                  if (std_shift <= -0.11986) {
                    return 1;
                  } else {  // if std_shift > -0.11986
                    if (avg_shift <= -0.16747) {
                      return 0;
                    } else {  // if avg_shift > -0.16747
                      return 0;
                    }
                  }
                }
              }
            } else {  // if width > 0.32718
              if (ncc <= 1.719) {
                return 2;
              } else {  // if ncc > 1.719
                return 1;
              }
            }
          } else {  // if height > -0.37476
            if (avg_err <= 1.3153) {
              if (height <= -0.14766) {
                if (width <= -0.38322) {
                  if (avg_shift_y <= -0.010325) {
                    if (std_shift_y <= 0.11402) {
                      return 1;
                    } else {  // if std_shift_y > 0.11402
                      return 0;
                    }
                  } else {  // if avg_shift_y > -0.010325
                    if (avg_shift <= -0.17278) {
                      return 1;
                    } else {  // if avg_shift > -0.17278
                      return 2;
                    }
                  }
                } else {  // if width > -0.38322
                  if (width <= 0.70391) {
                    if (avg_shift_y <= 0.12166) {
                      return 2;
                    } else {  // if avg_shift_y > 0.12166
                      return 0;
                    }
                  } else {  // if width > 0.70391
                    if (avg_err <= 1.1612) {
                      return 1;
                    } else {  // if avg_err > 1.1612
                      return 2;
                    }
                  }
                }
              } else {  // if height > -0.14766
                if (ncc <= -0.2089) {
                  if (height <= -0.046726) {
                    if (width <= -0.12489) {
                      return 2;
                    } else {  // if width > -0.12489
                      return 1;
                    }
                  } else {  // if height > -0.046726
                    return 2;
                  }
                } else {  // if ncc > -0.2089
                  if (ncc <= 0.068046) {
                    if (xyRatio <= -0.43322) {
                      return 0;
                    } else {  // if xyRatio > -0.43322
                      return 1;
                    }
                  } else {  // if ncc > 0.068046
                    if (avg_err <= 0.685) {
                      return 0;
                    } else {  // if avg_err > 0.685
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avg_err > 1.3153
              if (std_shift_x <= 8.6692) {
                if (width <= -0.54468) {
                  if (avg_shift <= 0.084255) {
                    if (std_shift_x <= -0.15766) {
                      return 2;
                    } else {  // if std_shift_x > -0.15766
                      return 0;
                    }
                  } else {  // if avg_shift > 0.084255
                    return 1;
                  }
                } else {  // if width > -0.54468
                  if (width <= 0.7362) {
                    if (ncc <= 1.5953) {
                      return 1;
                    } else {  // if ncc > 1.5953
                      return 1;
                    }
                  } else {  // if width > 0.7362
                    if (std_shift_x <= 5.2416) {
                      return 2;
                    } else {  // if std_shift_x > 5.2416
                      return 1;
                    }
                  }
                }
              } else {  // if std_shift_x > 8.6692
                return 2;
              }
            }
          }
        } else {  // if avg_shift_y > 0.12977
          if (height <= -0.37476) {
            if (height <= -0.4883) {
              if (std_shift <= -0.1239) {
                if (std_shift_y <= -0.33746) {
                  return 1;
                } else {  // if std_shift_y > -0.33746
                  if (avg_shift <= -0.16572) {
                    if (avg_shift_y <= 0.14391) {
                      return 0;
                    } else {  // if avg_shift_y > 0.14391
                      return 0;
                    }
                  } else {  // if avg_shift > -0.16572
                    return 1;
                  }
                }
              } else {  // if std_shift > -0.1239
                if (avg_shift_x <= -9.8535) {
                  if (avg_err <= 2.6726) {
                    return 0;
                  } else {  // if avg_err > 2.6726
                    return 1;
                  }
                } else {  // if avg_shift_x > -9.8535
                  if (height <= -0.60185) {
                    if (avg_shift_y <= 5.9197) {
                      return 1;
                    } else {  // if avg_shift_y > 5.9197
                      return 0;
                    }
                  } else {  // if height > -0.60185
                    if (avg_shift <= -0.17115) {
                      return 2;
                    } else {  // if avg_shift > -0.17115
                      return 1;
                    }
                  }
                }
              }
            } else {  // if height > -0.4883
              if (avg_err <= 0.67478) {
                if (height <= -0.46938) {
                  if (avg_shift <= -0.15687) {
                    return 0;
                  } else {  // if avg_shift > -0.15687
                    return 2;
                  }
                } else {  // if height > -0.46938
                  if (xyRatio <= -0.552) {
                    return 1;
                  } else {  // if xyRatio > -0.552
                    if (height <= -0.42522) {
                      return 1;
                    } else {  // if height > -0.42522
                      return 2;
                    }
                  }
                }
              } else {  // if avg_err > 0.67478
                if (std_shift_x <= -0.032788) {
                  if (avg_shift_y <= 0.13571) {
                    if (std_shift_y <= -0.17673) {
                      return 1;
                    } else {  // if std_shift_y > -0.17673
                      return 0;
                    }
                  } else {  // if avg_shift_y > 0.13571
                    if (avg_shift_y <= 0.57517) {
                      return 0;
                    } else {  // if avg_shift_y > 0.57517
                      return 0;
                    }
                  }
                } else {  // if std_shift_x > -0.032788
                  if (avg_shift <= -0.15075) {
                    if (ncc <= -0.80194) {
                      return 1;
                    } else {  // if ncc > -0.80194
                      return 2;
                    }
                  } else {  // if avg_shift > -0.15075
                    if (avg_err <= 2.9801) {
                      return 0;
                    } else {  // if avg_err > 2.9801
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if height > -0.37476
            if (avg_shift <= -0.15655) {
              if (avg_shift_x <= -0.025298) {
                if (width <= -0.60926) {
                  return 1;
                } else {  // if width > -0.60926
                  if (ncc <= -0.71349) {
                    if (std_shift_y <= 0.056296) {
                      return 1;
                    } else {  // if std_shift_y > 0.056296
                      return 2;
                    }
                  } else {  // if ncc > -0.71349
                    if (std_shift_y <= -0.29262) {
                      return 1;
                    } else {  // if std_shift_y > -0.29262
                      return 2;
                    }
                  }
                }
              } else {  // if avg_shift_x > -0.025298
                if (xyRatio <= -0.65179) {
                  if (avg_shift <= -0.17115) {
                    return 1;
                  } else {  // if avg_shift > -0.17115
                    if (std_shift <= -0.1225) {
                      return 2;
                    } else {  // if std_shift > -0.1225
                      return 2;
                    }
                  }
                } else {  // if xyRatio > -0.65179
                  if (xyRatio <= -0.26425) {
                    if (height <= -0.059343) {
                      return 0;
                    } else {  // if height > -0.059343
                      return 2;
                    }
                  } else {  // if xyRatio > -0.26425
                    if (height <= -0.10981) {
                      return 2;
                    } else {  // if height > -0.10981
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avg_shift > -0.15655
              if (xyRatio <= -0.51016) {
                if (xyRatio <= -0.68506) {
                  if (width <= -0.70614) {
                    return 2;
                  } else {  // if width > -0.70614
                    if (avg_err <= 1.8484) {
                      return 1;
                    } else {  // if avg_err > 1.8484
                      return 2;
                    }
                  }
                } else {  // if xyRatio > -0.68506
                  if (xyRatio <= -0.59437) {
                    return 0;
                  } else {  // if xyRatio > -0.59437
                    if (xyRatio <= -0.57881) {
                      return 2;
                    } else {  // if xyRatio > -0.57881
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > -0.51016
                if (xyRatio <= -0.045063) {
                  if (avg_shift_x <= -0.97011) {
                    if (std_shift_x <= 1.0109) {
                      return 2;
                    } else {  // if std_shift_x > 1.0109
                      return 1;
                    }
                  } else {  // if avg_shift_x > -0.97011
                    if (std_shift_x <= 0.45568) {
                      return 1;
                    } else {  // if std_shift_x > 0.45568
                      return 0;
                    }
                  }
                } else {  // if xyRatio > -0.045063
                  if (ncc <= 0.33054) {
                    if (std_shift_x <= 0.82121) {
                      return 1;
                    } else {  // if std_shift_x > 0.82121
                      return 2;
                    }
                  } else {  // if ncc > 0.33054
                    if (width <= 0.79002) {
                      return 0;
                    } else {  // if width > 0.79002
                      return 1;
                    }
                  }
                }
              }
            }
          }
        }
      } else {  // if height > 0.054206
        if (label <= -1.1801) {
          if (width <= 3.1258) {
            if (height <= 0.47055) {
              if (avg_shift_x <= 0.53559) {
                if (ncc <= -0.33871) {
                  return 2;
                } else {  // if ncc > -0.33871
                  if (avg_shift <= -0.019407) {
                    if (avg_shift <= -0.070816) {
                      return 1;
                    } else {  // if avg_shift > -0.070816
                      return 2;
                    }
                  } else {  // if avg_shift > -0.019407
                    if (avg_shift_x <= -2.0131) {
                      return 0;
                    } else {  // if avg_shift_x > -2.0131
                      return 1;
                    }
                  }
                }
              } else {  // if avg_shift_x > 0.53559
                if (avg_shift <= 2.5102) {
                  if (avg_shift_y <= 0.29033) {
                    if (std_shift_y <= 0.056035) {
                      return 0;
                    } else {  // if std_shift_y > 0.056035
                      return 0;
                    }
                  } else {  // if avg_shift_y > 0.29033
                    if (std_shift <= -0.05423) {
                      return 0;
                    } else {  // if std_shift > -0.05423
                      return 1;
                    }
                  }
                } else {  // if avg_shift > 2.5102
                  return 1;
                }
              }
            } else {  // if height > 0.47055
              if (avg_shift_x <= 1.9349) {
                if (xyRatio <= -0.59384) {
                  return 0;
                } else {  // if xyRatio > -0.59384
                  if (avg_shift_x <= -1.1484) {
                    if (avg_shift_x <= -1.3864) {
                      return 2;
                    } else {  // if avg_shift_x > -1.3864
                      return 0;
                    }
                  } else {  // if avg_shift_x > -1.1484
                    if (avg_err <= 1.7563) {
                      return 2;
                    } else {  // if avg_err > 1.7563
                      return 1;
                    }
                  }
                }
              } else {  // if avg_shift_x > 1.9349
                if (avg_shift_y <= -3.029) {
                  if (std_shift_x <= 2.3618) {
                    return 2;
                  } else {  // if std_shift_x > 2.3618
                    if (avg_shift_y <= -5.7222) {
                      return 2;
                    } else {  // if avg_shift_y > -5.7222
                      return 1;
                    }
                  }
                } else {  // if avg_shift_y > -3.029
                  return 1;
                }
              }
            }
          } else {  // if width > 3.1258
            if (width <= 3.6317) {
              return 0;
            } else {  // if width > 3.6317
              if (ncc <= -0.48562) {
                return 1;
              } else {  // if ncc > -0.48562
                if (avg_shift_y <= -1.7876) {
                  return 1;
                } else {  // if avg_shift_y > -1.7876
                  return 0;
                }
              }
            }
          }
        } else {  // if label > -1.1801
          if (avg_shift_x <= -5.0328) {
            if (ncc <= 2.8063) {
              if (avg_err <= 3.2327) {
                return 2;
              } else {  // if avg_err > 3.2327
                if (xyRatio <= -0.1817) {
                  return 2;
                } else {  // if xyRatio > -0.1817
                  return 0;
                }
              }
            } else {  // if ncc > 2.8063
              return 0;
            }
          } else {  // if avg_shift_x > -5.0328
            if (height <= 3.2336) {
              if (avg_shift_x <= -0.5669) {
                if (height <= 1.0509) {
                  if (std_shift_y <= 0.0065735) {
                    if (avg_err <= 1.1137) {
                      return 2;
                    } else {  // if avg_err > 1.1137
                      return 0;
                    }
                  } else {  // if std_shift_y > 0.0065735
                    if (std_shift <= 0.013715) {
                      return 0;
                    } else {  // if std_shift > 0.013715
                      return 0;
                    }
                  }
                } else {  // if height > 1.0509
                  if (ncc <= 1.4147) {
                    if (avg_shift_x <= -0.69164) {
                      return 1;
                    } else {  // if avg_shift_x > -0.69164
                      return 0;
                    }
                  } else {  // if ncc > 1.4147
                    if (ncc <= 2.1109) {
                      return 1;
                    } else {  // if ncc > 2.1109
                      return 0;
                    }
                  }
                }
              } else {  // if avg_shift_x > -0.5669
                if (avg_shift_y <= 0.032313) {
                  if (xyRatio <= 0.089815) {
                    if (xyRatio <= -0.52483) {
                      return 0;
                    } else {  // if xyRatio > -0.52483
                      return 0;
                    }
                  } else {  // if xyRatio > 0.089815
                    if (avg_shift <= -0.16449) {
                      return 1;
                    } else {  // if avg_shift > -0.16449
                      return 0;
                    }
                  }
                } else {  // if avg_shift_y > 0.032313
                  if (xyRatio <= -0.58742) {
                    if (std_shift_y <= 0.0092781) {
                      return 2;
                    } else {  // if std_shift_y > 0.0092781
                      return 0;
                    }
                  } else {  // if xyRatio > -0.58742
                    if (height <= 0.39485) {
                      return 0;
                    } else {  // if height > 0.39485
                      return 0;
                    }
                  }
                }
              }
            } else {  // if height > 3.2336
              if (avg_shift_x <= 3.2259) {
                if (avg_shift_x <= -0.99005) {
                  if (avg_shift_y <= -1.6876) {
                    if (height <= 4.0663) {
                      return 0;
                    } else {  // if height > 4.0663
                      return 1;
                    }
                  } else {  // if avg_shift_y > -1.6876
                    if (ncc <= -0.8421) {
                      return 0;
                    } else {  // if ncc > -0.8421
                      return 2;
                    }
                  }
                } else {  // if avg_shift_x > -0.99005
                  if (std_shift_x <= 1.434) {
                    if (avg_err <= 0.74345) {
                      return 1;
                    } else {  // if avg_err > 0.74345
                      return 0;
                    }
                  } else {  // if std_shift_x > 1.434
                    if (avg_shift_x <= 2.6515) {
                      return 1;
                    } else {  // if avg_shift_x > 2.6515
                      return 1;
                    }
                  }
                }
              } else {  // if avg_shift_x > 3.2259
                if (avg_err <= 1.4065) {
                  return 0;
                } else {  // if avg_err > 1.4065
                  if (ncc <= 0.24066) {
                    return 1;
                  } else {  // if ncc > 0.24066
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

} // namespace rm
