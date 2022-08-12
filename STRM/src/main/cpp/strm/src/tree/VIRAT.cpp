#include "strm/tree/VIRAT.hpp"

namespace rm {

float VIRAT(float width, float height, float label, float xyRatio, float avg_shift_x,
            float avg_shift_y, float avg_shift, float std_shift_x, float std_shift_y,
            float std_shift, float avg_err, float ncc) {
  if (avg_err <= -1.0569) {
    if (height <= -0.68335) {
      if (xyRatio <= -0.10731) {
        if (height <= -0.82686) {
          if (avg_err <= -1.1073) {
            return 0;
          } else {  // if avg_err > -1.1073
            if (avg_shift_y <= -0.012412) {
              return 1;
            } else {  // if avg_shift_y > -0.012412
              return 0;
            }
          }
        } else {  // if height > -0.82686
          if (height <= -0.71205) {
            if (xyRatio <= -0.60329) {
              return 0;
            } else {  // if xyRatio > -0.60329
              if (std_shift_x <= -0.74711) {
                return 1;
              } else {  // if std_shift_x > -0.74711
                if (ncc <= -0.51879) {
                  return 1;
                } else {  // if ncc > -0.51879
                  return 0;
                }
              }
            }
          } else {  // if height > -0.71205
            if (ncc <= -0.71703) {
              if (avg_err <= -1.3724) {
                if (std_shift <= -0.30121) {
                  return 0;
                } else {  // if std_shift > -0.30121
                  if (avg_shift_y <= -0.26084) {
                    return 0;
                  } else {  // if avg_shift_y > -0.26084
                    return 1;
                  }
                }
              } else {  // if avg_err > -1.3724
                return 0;
              }
            } else {  // if ncc > -0.71703
              if (avg_shift_y <= -0.21417) {
                return 1;
              } else {  // if avg_shift_y > -0.21417
                if (std_shift <= -0.30053) {
                  return 0;
                } else {  // if std_shift > -0.30053
                  return 1;
                }
              }
            }
          }
        }
      } else {  // if xyRatio > -0.10731
        if (width <= -0.25006) {
          return 0;
        } else {  // if width > -0.25006
          if (std_shift_y <= -0.75882) {
            if (avg_shift <= -0.50355) {
              return 1;
            } else {  // if avg_shift > -0.50355
              if (std_shift_y <= -0.77392) {
                return 0;
              } else {  // if std_shift_y > -0.77392
                if (ncc <= -0.61536) {
                  if (avg_shift <= -0.50347) {
                    if (std_shift_y <= -0.7712) {
                      return 1;
                    } else {  // if std_shift_y > -0.7712
                      return 0;
                    }
                  } else {  // if avg_shift > -0.50347
                    return 1;
                  }
                } else {  // if ncc > -0.61536
                  if (height <= -0.71205) {
                    if (avg_shift_x <= -0.029592) {
                      return 1;
                    } else {  // if avg_shift_x > -0.029592
                      return 0;
                    }
                  } else {  // if height > -0.71205
                    return 2;
                  }
                }
              }
            }
          } else {  // if std_shift_y > -0.75882
            if (avg_shift_y <= -0.16611) {
              if (std_shift_x <= -0.34872) {
                if (std_shift_x <= -1.105) {
                  return 0;
                } else {  // if std_shift_x > -1.105
                  if (ncc <= 1.0635) {
                    if (avg_shift <= -0.50336) {
                      return 1;
                    } else {  // if avg_shift > -0.50336
                      return 1;
                    }
                  } else {  // if ncc > 1.0635
                    return 0;
                  }
                }
              } else {  // if std_shift_x > -0.34872
                return 2;
              }
            } else {  // if avg_shift_y > -0.16611
              return 2;
            }
          }
        }
      }
    } else {  // if height > -0.68335
      if (xyRatio <= 0.23885) {
        if (height <= 2.0864) {
          if (width <= -0.15775) {
            if (width <= -0.24567) {
              if (xyRatio <= -0.6947) {
                if (std_shift_x <= -1.0292) {
                  if (avg_shift_x <= 0.0070714) {
                    if (std_shift <= -0.2983) {
                      return 1;
                    } else {  // if std_shift > -0.2983
                      return 0;
                    }
                  } else {  // if avg_shift_x > 0.0070714
                    return 0;
                  }
                } else {  // if std_shift_x > -1.0292
                  if (avg_shift <= -0.50261) {
                    if (std_shift_x <= -0.88398) {
                      return 0;
                    } else {  // if std_shift_x > -0.88398
                      return 1;
                    }
                  } else {  // if avg_shift > -0.50261
                    if (ncc <= 0.08928) {
                      return 2;
                    } else {  // if ncc > 0.08928
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > -0.6947
                if (avg_err <= -1.1399) {
                  return 1;
                } else {  // if avg_err > -1.1399
                  return 2;
                }
              }
            } else {  // if width > -0.24567
              if (std_shift_y <= 0.17149) {
                if (xyRatio <= -0.59186) {
                  return 0;
                } else {  // if xyRatio > -0.59186
                  if (height <= -0.43938) {
                    if (std_shift_y <= -0.75966) {
                      return 2;
                    } else {  // if std_shift_y > -0.75966
                      return 1;
                    }
                  } else {  // if height > -0.43938
                    if (avg_shift_x <= -0.38365) {
                      return 2;
                    } else {  // if avg_shift_x > -0.38365
                      return 2;
                    }
                  }
                }
              } else {  // if std_shift_y > 0.17149
                return 1;
              }
            }
          } else {  // if width > -0.15775
            if (height <= -0.15953) {
              if (avg_err <= -1.4503) {
                if (avg_err <= -1.4657) {
                  if (ncc <= -1.4207) {
                    return 2;
                  } else {  // if ncc > -1.4207
                    if (std_shift_x <= -0.56902) {
                      return 1;
                    } else {  // if std_shift_x > -0.56902
                      return 0;
                    }
                  }
                } else {  // if avg_err > -1.4657
                  return 1;
                }
              } else {  // if avg_err > -1.4503
                if (xyRatio <= 0.029765) {
                  if (avg_shift <= -0.43291) {
                    return 0;
                  } else {  // if avg_shift > -0.43291
                    return 2;
                  }
                } else {  // if xyRatio > 0.029765
                  if (avg_err <= -1.0741) {
                    return 1;
                  } else {  // if avg_err > -1.0741
                    return 0;
                  }
                }
              }
            } else {  // if height > -0.15953
              if (std_shift_y <= -0.73459) {
                if (width <= 0.0093008) {
                  if (height <= 0.54368) {
                    if (xyRatio <= -0.29024) {
                      return 0;
                    } else {  // if xyRatio > -0.29024
                      return 2;
                    }
                  } else {  // if height > 0.54368
                    return 1;
                  }
                } else {  // if width > 0.0093008
                  if (xyRatio <= 0.039293) {
                    if (std_shift_y <= -0.73787) {
                      return 2;
                    } else {  // if std_shift_y > -0.73787
                      return 0;
                    }
                  } else {  // if xyRatio > 0.039293
                    if (avg_shift <= -0.50326) {
                      return 1;
                    } else {  // if avg_shift > -0.50326
                      return 2;
                    }
                  }
                }
              } else {  // if std_shift_y > -0.73459
                if (xyRatio <= -0.32358) {
                  if (xyRatio <= -0.33462) {
                    if (avg_err <= -1.0944) {
                      return 2;
                    } else {  // if avg_err > -1.0944
                      return 2;
                    }
                  } else {  // if xyRatio > -0.33462
                    return 0;
                  }
                } else {  // if xyRatio > -0.32358
                  if (height <= 1.1106) {
                    if (height <= 0.36429) {
                      return 2;
                    } else {  // if height > 0.36429
                      return 2;
                    }
                  } else {  // if height > 1.1106
                    return 1;
                  }
                }
              }
            }
          }
        } else {  // if height > 2.0864
          if (xyRatio <= -0.62292) {
            return 0;
          } else {  // if xyRatio > -0.62292
            if (std_shift <= -0.29721) {
              if (ncc <= -0.56807) {
                if (avg_shift_y <= -0.23425) {
                  if (std_shift_x <= -0.94168) {
                    if (avg_shift <= -0.50318) {
                      return 0;
                    } else {  // if avg_shift > -0.50318
                      return 2;
                    }
                  } else {  // if std_shift_x > -0.94168
                    return 0;
                  }
                } else {  // if avg_shift_y > -0.23425
                  return 2;
                }
              } else {  // if ncc > -0.56807
                return 0;
              }
            } else {  // if std_shift > -0.29721
              return 0;
            }
          }
        }
      } else {  // if xyRatio > 0.23885
        if (label <= -0.93519) {
          if (std_shift_y <= -0.70256) {
            return 0;
          } else {  // if std_shift_y > -0.70256
            if (xyRatio <= 1.4105) {
              if (std_shift <= -0.28806) {
                if (avg_shift_y <= -0.35096) {
                  return 1;
                } else {  // if avg_shift_y > -0.35096
                  if (avg_shift_y <= -0.08793) {
                    return 2;
                  } else {  // if avg_shift_y > -0.08793
                    return 1;
                  }
                }
              } else {  // if std_shift > -0.28806
                return 0;
              }
            } else {  // if xyRatio > 1.4105
              return 2;
            }
          }
        } else {  // if label > -0.93519
          if (width <= 0.013697) {
            if (avg_shift_x <= 0.087816) {
              if (std_shift_x <= -0.87246) {
                if (avg_shift_y <= -0.24578) {
                  if (std_shift_x <= -0.93477) {
                    if (ncc <= -1.6753) {
                      return 0;
                    } else {  // if ncc > -1.6753
                      return 1;
                    }
                  } else {  // if std_shift_x > -0.93477
                    return 2;
                  }
                } else {  // if avg_shift_y > -0.24578
                  if (avg_err <= -1.1391) {
                    if (ncc <= -1.5436) {
                      return 0;
                    } else {  // if ncc > -1.5436
                      return 2;
                    }
                  } else {  // if avg_err > -1.1391
                    if (std_shift_x <= -0.89322) {
                      return 0;
                    } else {  // if std_shift_x > -0.89322
                      return 1;
                    }
                  }
                }
              } else {  // if std_shift_x > -0.87246
                if (avg_shift_y <= -0.5777) {
                  return 1;
                } else {  // if avg_shift_y > -0.5777
                  if (avg_err <= -1.5633) {
                    return 1;
                  } else {  // if avg_err > -1.5633
                    if (avg_shift_x <= 0.037018) {
                      return 2;
                    } else {  // if avg_shift_x > 0.037018
                      return 2;
                    }
                  }
                }
              }
            } else {  // if avg_shift_x > 0.087816
              if (std_shift_y <= -0.37834) {
                if (avg_shift <= -0.49782) {
                  return 0;
                } else {  // if avg_shift > -0.49782
                  if (ncc <= 0.3347) {
                    if (avg_shift_x <= 0.14091) {
                      return 1;
                    } else {  // if avg_shift_x > 0.14091
                      return 1;
                    }
                  } else {  // if ncc > 0.3347
                    return 2;
                  }
                }
              } else {  // if std_shift_y > -0.37834
                if (avg_shift <= -0.46603) {
                  return 2;
                } else {  // if avg_shift > -0.46603
                  if (avg_shift <= -0.3631) {
                    return 0;
                  } else {  // if avg_shift > -0.3631
                    return 2;
                  }
                }
              }
            }
          } else {  // if width > 0.013697
            return 1;
          }
        }
      }
    }
  } else {  // if avg_err > -1.0569
    if (height <= -0.33892) {
      if (label <= -0.93519) {
        if (height <= -0.59724) {
          if (std_shift <= -0.24727) {
            if (std_shift_y <= -0.39725) {
              if (avg_shift_y <= -0.54997) {
                if (avg_shift <= -0.44053) {
                  return 2;
                } else {  // if avg_shift > -0.44053
                  return 1;
                }
              } else {  // if avg_shift_y > -0.54997
                if (avg_shift_x <= -0.63763) {
                  return 2;
                } else {  // if avg_shift_x > -0.63763
                  if (std_shift_x <= -0.35094) {
                    if (avg_shift_y <= -0.025901) {
                      return 1;
                    } else {  // if avg_shift_y > -0.025901
                      return 1;
                    }
                  } else {  // if std_shift_x > -0.35094
                    if (std_shift <= -0.26324) {
                      return 0;
                    } else {  // if std_shift > -0.26324
                      return 1;
                    }
                  }
                }
              }
            } else {  // if std_shift_y > -0.39725
              if (avg_err <= -0.065703) {
                return 2;
              } else {  // if avg_err > -0.065703
                return 1;
              }
            }
          } else {  // if std_shift > -0.24727
            if (avg_shift <= 0.11704) {
              return 1;
            } else {  // if avg_shift > 0.11704
              if (xyRatio <= 3.4759) {
                return 1;
              } else {  // if xyRatio > 3.4759
                return 2;
              }
            }
          }
        } else {  // if height > -0.59724
          if (height <= -0.38197) {
            if (xyRatio <= 0.97681) {
              if (xyRatio <= 0.60323) {
                if (ncc <= -1.5914) {
                  return 1;
                } else {  // if ncc > -1.5914
                  if (avg_shift_y <= 0.38103) {
                    if (std_shift <= -0.26395) {
                      return 2;
                    } else {  // if std_shift > -0.26395
                      return 2;
                    }
                  } else {  // if avg_shift_y > 0.38103
                    return 1;
                  }
                }
              } else {  // if xyRatio > 0.60323
                if (std_shift_x <= -0.11736) {
                  if (std_shift <= -0.24909) {
                    if (avg_shift_y <= -0.52203) {
                      return 0;
                    } else {  // if avg_shift_y > -0.52203
                      return 1;
                    }
                  } else {  // if std_shift > -0.24909
                    return 1;
                  }
                } else {  // if std_shift_x > -0.11736
                  return 2;
                }
              }
            } else {  // if xyRatio > 0.97681
              if (avg_shift <= 0.38789) {
                if (std_shift_y <= -0.65274) {
                  return 1;
                } else {  // if std_shift_y > -0.65274
                  if (ncc <= 1.2804) {
                    if (avg_shift_y <= 0.30235) {
                      return 2;
                    } else {  // if avg_shift_y > 0.30235
                      return 1;
                    }
                  } else {  // if ncc > 1.2804
                    return 1;
                  }
                }
              } else {  // if avg_shift > 0.38789
                return 1;
              }
            }
          } else {  // if height > -0.38197
            if (std_shift <= -0.27977) {
              return 1;
            } else {  // if std_shift > -0.27977
              if (std_shift_y <= -0.46938) {
                if (std_shift_y <= -0.54749) {
                  return 1;
                } else {  // if std_shift_y > -0.54749
                  if (avg_shift_y <= 0.17825) {
                    return 0;
                  } else {  // if avg_shift_y > 0.17825
                    return 1;
                  }
                }
              } else {  // if std_shift_y > -0.46938
                if (std_shift_x <= -0.52151) {
                  return 0;
                } else {  // if std_shift_x > -0.52151
                  return 1;
                }
              }
            }
          }
        }
      } else {  // if label > -0.93519
        if (height <= -0.91297) {
          if (width <= -0.33359) {
            if (std_shift_x <= -0.93654) {
              if (ncc <= 1.2877) {
                return 0;
              } else {  // if ncc > 1.2877
                if (avg_shift <= -0.43242) {
                  return 1;
                } else {  // if avg_shift > -0.43242
                  return 0;
                }
              }
            } else {  // if std_shift_x > -0.93654
              if (std_shift_y <= -0.57601) {
                if (avg_shift_y <= 0.2881) {
                  return 1;
                } else {  // if avg_shift_y > 0.2881
                  return 0;
                }
              } else {  // if std_shift_y > -0.57601
                return 0;
              }
            }
          } else {  // if width > -0.33359
            if (avg_shift_y <= 0.4407) {
              if (avg_shift_x <= 0.41494) {
                if (avg_shift_x <= -2.1215) {
                  if (avg_shift_y <= -0.49046) {
                    return 0;
                  } else {  // if avg_shift_y > -0.49046
                    return 1;
                  }
                } else {  // if avg_shift_x > -2.1215
                  if (avg_shift_y <= 0.0096052) {
                    return 0;
                  } else {  // if avg_shift_y > 0.0096052
                    if (ncc <= -0.14787) {
                      return 1;
                    } else {  // if ncc > -0.14787
                      return 0;
                    }
                  }
                }
              } else {  // if avg_shift_x > 0.41494
                if (avg_shift_y <= -0.60923) {
                  return 1;
                } else {  // if avg_shift_y > -0.60923
                  return 0;
                }
              }
            } else {  // if avg_shift_y > 0.4407
              return 1;
            }
          }
        } else {  // if height > -0.91297
          if (avg_shift_x <= 0.7885) {
            if (height <= -0.7551) {
              if (xyRatio <= -0.38379) {
                if (height <= -0.76946) {
                  if (xyRatio <= -0.60329) {
                    if (std_shift <= -0.29418) {
                      return 0;
                    } else {  // if std_shift > -0.29418
                      return 0;
                    }
                  } else {  // if xyRatio > -0.60329
                    if (avg_err <= 0.28322) {
                      return 1;
                    } else {  // if avg_err > 0.28322
                      return 0;
                    }
                  }
                } else {  // if height > -0.76946
                  if (std_shift_x <= -0.40817) {
                    if (avg_shift_y <= -0.019079) {
                      return 0;
                    } else {  // if avg_shift_y > -0.019079
                      return 0;
                    }
                  } else {  // if std_shift_x > -0.40817
                    if (avg_err <= 0.21745) {
                      return 1;
                    } else {  // if avg_err > 0.21745
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > -0.38379
                if (height <= -0.84121) {
                  if (avg_shift_y <= 0.5655) {
                    if (std_shift_x <= -0.33218) {
                      return 1;
                    } else {  // if std_shift_x > -0.33218
                      return 1;
                    }
                  } else {  // if avg_shift_y > 0.5655
                    return 0;
                  }
                } else {  // if height > -0.84121
                  if (std_shift <= -0.29785) {
                    if (xyRatio <= -0.10444) {
                      return 1;
                    } else {  // if xyRatio > -0.10444
                      return 0;
                    }
                  } else {  // if std_shift > -0.29785
                    if (avg_shift_x <= -0.56633) {
                      return 1;
                    } else {  // if avg_shift_x > -0.56633
                      return 0;
                    }
                  }
                }
              }
            } else {  // if height > -0.7551
              if (height <= -0.62594) {
                if (avg_shift_x <= -0.11567) {
                  if (height <= -0.6403) {
                    if (std_shift_x <= -0.20759) {
                      return 0;
                    } else {  // if std_shift_x > -0.20759
                      return 1;
                    }
                  } else {  // if height > -0.6403
                    if (xyRatio <= -0.023075) {
                      return 1;
                    } else {  // if xyRatio > -0.023075
                      return 1;
                    }
                  }
                } else {  // if avg_shift_x > -0.11567
                  if (avg_shift_y <= 0.069193) {
                    if (std_shift <= -0.30012) {
                      return 0;
                    } else {  // if std_shift > -0.30012
                      return 1;
                    }
                  } else {  // if avg_shift_y > 0.069193
                    if (avg_shift_x <= 0.23853) {
                      return 1;
                    } else {  // if avg_shift_x > 0.23853
                      return 0;
                    }
                  }
                }
              } else {  // if height > -0.62594
                if (std_shift_x <= -0.55257) {
                  if (width <= -0.26325) {
                    if (height <= -0.54701) {
                      return 0;
                    } else {  // if height > -0.54701
                      return 1;
                    }
                  } else {  // if width > -0.26325
                    if (xyRatio <= -0.39736) {
                      return 0;
                    } else {  // if xyRatio > -0.39736
                      return 0;
                    }
                  }
                } else {  // if std_shift_x > -0.55257
                  if (avg_err <= -0.54692) {
                    if (avg_shift_y <= -0.62159) {
                      return 0;
                    } else {  // if avg_shift_y > -0.62159
                      return 1;
                    }
                  } else {  // if avg_err > -0.54692
                    if (height <= -0.55419) {
                      return 0;
                    } else {  // if height > -0.55419
                      return 1;
                    }
                  }
                }
              }
            }
          } else {  // if avg_shift_x > 0.7885
            if (height <= -0.41068) {
              if (std_shift <= -0.21642) {
                if (xyRatio <= -0.13612) {
                  if (std_shift_x <= -0.6504) {
                    return 0;
                  } else {  // if std_shift_x > -0.6504
                    return 1;
                  }
                } else {  // if xyRatio > -0.13612
                  if (ncc <= 0.36094) {
                    return 1;
                  } else {  // if ncc > 0.36094
                    if (std_shift_y <= -0.55827) {
                      return 2;
                    } else {  // if std_shift_y > -0.55827
                      return 0;
                    }
                  }
                }
              } else {  // if std_shift > -0.21642
                if (xyRatio <= 0.15297) {
                  if (avg_shift <= 1.708) {
                    if (avg_shift_y <= 0.59242) {
                      return 0;
                    } else {  // if avg_shift_y > 0.59242
                      return 0;
                    }
                  } else {  // if avg_shift > 1.708
                    if (std_shift_y <= 0.61312) {
                      return 1;
                    } else {  // if std_shift_y > 0.61312
                      return 0;
                    }
                  }
                } else {  // if xyRatio > 0.15297
                  if (std_shift_y <= -0.13655) {
                    if (std_shift_y <= -0.19001) {
                      return 0;
                    } else {  // if std_shift_y > -0.19001
                      return 2;
                    }
                  } else {  // if std_shift_y > -0.13655
                    return 0;
                  }
                }
              }
            } else {  // if height > -0.41068
              if (width <= -0.14016) {
                if (width <= -0.2105) {
                  return 2;
                } else {  // if width > -0.2105
                  if (std_shift_x <= 0.2711) {
                    if (avg_err <= 0.25511) {
                      return 2;
                    } else {  // if avg_err > 0.25511
                      return 0;
                    }
                  } else {  // if std_shift_x > 0.2711
                    if (xyRatio <= -0.023342) {
                      return 2;
                    } else {  // if xyRatio > -0.023342
                      return 0;
                    }
                  }
                }
              } else {  // if width > -0.14016
                if (std_shift <= -0.20418) {
                  return 2;
                } else {  // if std_shift > -0.20418
                  return 1;
                }
              }
            }
          }
        }
      }
    } else {  // if height > -0.33892
      if (height <= 0.16337) {
        if (avg_shift_x <= 0.67886) {
          if (height <= -0.23846) {
            if (label <= -0.93519) {
              return 2;
            } else {  // if label > -0.93519
              if (xyRatio <= -0.86967) {
                if (std_shift_y <= 0.39746) {
                  if (avg_shift <= -0.41079) {
                    return 1;
                  } else {  // if avg_shift > -0.41079
                    if (std_shift_y <= -0.44172) {
                      return 0;
                    } else {  // if std_shift_y > -0.44172
                      return 0;
                    }
                  }
                } else {  // if std_shift_y > 0.39746
                  if (avg_err <= 2.1578) {
                    if (avg_err <= 0.74063) {
                      return 1;
                    } else {  // if avg_err > 0.74063
                      return 1;
                    }
                  } else {  // if avg_err > 2.1578
                    if (std_shift_x <= -0.11437) {
                      return 1;
                    } else {  // if std_shift_x > -0.11437
                      return 0;
                    }
                  }
                }
              } else {  // if xyRatio > -0.86967
                if (xyRatio <= -0.6581) {
                  if (ncc <= 1.8338) {
                    if (avg_shift_x <= -1.1596) {
                      return 1;
                    } else {  // if avg_shift_x > -1.1596
                      return 1;
                    }
                  } else {  // if ncc > 1.8338
                    if (std_shift_x <= -0.15081) {
                      return 1;
                    } else {  // if std_shift_x > -0.15081
                      return 0;
                    }
                  }
                } else {  // if xyRatio > -0.6581
                  if (avg_shift_x <= 0.43203) {
                    if (xyRatio <= -0.071357) {
                      return 1;
                    } else {  // if xyRatio > -0.071357
                      return 1;
                    }
                  } else {  // if avg_shift_x > 0.43203
                    return 2;
                  }
                }
              }
            }
          } else {  // if height > -0.23846
            if (width <= 0.25987) {
              if (width <= -0.18412) {
                if (xyRatio <= -0.79936) {
                  if (xyRatio <= -0.90294) {
                    if (height <= -0.059072) {
                      return 2;
                    } else {  // if height > -0.059072
                      return 1;
                    }
                  } else {  // if xyRatio > -0.90294
                    if (avg_shift_x <= -0.15442) {
                      return 2;
                    } else {  // if avg_shift_x > -0.15442
                      return 0;
                    }
                  }
                } else {  // if xyRatio > -0.79936
                  if (width <= -0.2105) {
                    if (width <= -0.22808) {
                      return 2;
                    } else {  // if width > -0.22808
                      return 1;
                    }
                  } else {  // if width > -0.2105
                    if (width <= -0.20171) {
                      return 2;
                    } else {  // if width > -0.20171
                      return 1;
                    }
                  }
                }
              } else {  // if width > -0.18412
                if (height <= 0.14902) {
                  if (height <= 0.12032) {
                    if (xyRatio <= -0.50777) {
                      return 2;
                    } else {  // if xyRatio > -0.50777
                      return 0;
                    }
                  } else {  // if height > 0.12032
                    if (avg_shift_y <= -0.52672) {
                      return 1;
                    } else {  // if avg_shift_y > -0.52672
                      return 2;
                    }
                  }
                } else {  // if height > 0.14902
                  return 0;
                }
              }
            } else {  // if width > 0.25987
              if (width <= 0.35659) {
                if (avg_err <= -0.41012) {
                  return 2;
                } else {  // if avg_err > -0.41012
                  if (std_shift <= -0.093357) {
                    if (ncc <= 0.087987) {
                      return 1;
                    } else {  // if ncc > 0.087987
                      return 1;
                    }
                  } else {  // if std_shift > -0.093357
                    if (ncc <= -0.4982) {
                      return 1;
                    } else {  // if ncc > -0.4982
                      return 2;
                    }
                  }
                }
              } else {  // if width > 0.35659
                if (avg_err <= -0.058446) {
                  if (ncc <= -0.85086) {
                    if (avg_shift <= -0.49385) {
                      return 2;
                    } else {  // if avg_shift > -0.49385
                      return 1;
                    }
                  } else {  // if ncc > -0.85086
                    if (ncc <= -0.66406) {
                      return 2;
                    } else {  // if ncc > -0.66406
                      return 2;
                    }
                  }
                } else {  // if avg_err > -0.058446
                  if (ncc <= 0.77435) {
                    if (height <= 0.077265) {
                      return 0;
                    } else {  // if height > 0.077265
                      return 1;
                    }
                  } else {  // if ncc > 0.77435
                    if (std_shift <= -0.1512) {
                      return 1;
                    } else {  // if std_shift > -0.1512
                      return 1;
                    }
                  }
                }
              }
            }
          }
        } else {  // if avg_shift_x > 0.67886
          if (xyRatio <= 0.3991) {
            if (avg_err <= 1.4076) {
              if (ncc <= -0.72711) {
                if (width <= -0.2061) {
                  if (std_shift_x <= 1.9022) {
                    if (ncc <= -1.2936) {
                      return 0;
                    } else {  // if ncc > -1.2936
                      return 2;
                    }
                  } else {  // if std_shift_x > 1.9022
                    if (std_shift <= 0.88202) {
                      return 1;
                    } else {  // if std_shift > 0.88202
                      return 0;
                    }
                  }
                } else {  // if width > -0.2061
                  if (avg_shift <= -0.29686) {
                    return 2;
                  } else {  // if avg_shift > -0.29686
                    if (height <= -0.14518) {
                      return 0;
                    } else {  // if height > -0.14518
                      return 1;
                    }
                  }
                }
              } else {  // if ncc > -0.72711
                if (height <= 0.077265) {
                  if (height <= -0.12365) {
                    if (xyRatio <= -0.091329) {
                      return 0;
                    } else {  // if xyRatio > -0.091329
                      return 2;
                    }
                  } else {  // if height > -0.12365
                    if (std_shift_x <= 0.7643) {
                      return 0;
                    } else {  // if std_shift_x > 0.7643
                      return 0;
                    }
                  }
                } else {  // if height > 0.077265
                  if (std_shift_y <= -0.18087) {
                    if (width <= -0.12258) {
                      return 2;
                    } else {  // if width > -0.12258
                      return 1;
                    }
                  } else {  // if std_shift_y > -0.18087
                    if (xyRatio <= -0.21311) {
                      return 1;
                    } else {  // if xyRatio > -0.21311
                      return 0;
                    }
                  }
                }
              }
            } else {  // if avg_err > 1.4076
              if (std_shift_x <= 1.2955) {
                if (xyRatio <= 0.050317) {
                  if (xyRatio <= -0.13307) {
                    if (height <= -0.037545) {
                      return 1;
                    } else {  // if height > -0.037545
                      return 0;
                    }
                  } else {  // if xyRatio > -0.13307
                    if (ncc <= -0.27749) {
                      return 0;
                    } else {  // if ncc > -0.27749
                      return 2;
                    }
                  }
                } else {  // if xyRatio > 0.050317
                  if (avg_err <= 1.7241) {
                    if (std_shift_x <= 0.38887) {
                      return 1;
                    } else {  // if std_shift_x > 0.38887
                      return 2;
                    }
                  } else {  // if avg_err > 1.7241
                    if (std_shift_x <= 1.1801) {
                      return 1;
                    } else {  // if std_shift_x > 1.1801
                      return 0;
                    }
                  }
                }
              } else {  // if std_shift_x > 1.2955
                if (avg_err <= 2.1209) {
                  if (xyRatio <= -0.18673) {
                    if (ncc <= 0.31486) {
                      return 0;
                    } else {  // if ncc > 0.31486
                      return 1;
                    }
                  } else {  // if xyRatio > -0.18673
                    if (std_shift_x <= 1.4691) {
                      return 1;
                    } else {  // if std_shift_x > 1.4691
                      return 0;
                    }
                  }
                } else {  // if avg_err > 2.1209
                  if (avg_shift_y <= -1.2039) {
                    if (avg_shift_x <= 1.9152) {
                      return 0;
                    } else {  // if avg_shift_x > 1.9152
                      return 1;
                    }
                  } else {  // if avg_shift_y > -1.2039
                    if (std_shift_y <= 2.984) {
                      return 0;
                    } else {  // if std_shift_y > 2.984
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 0.3991
            if (avg_shift <= -0.30352) {
              return 1;
            } else {  // if avg_shift > -0.30352
              if (avg_shift_x <= 1.3565) {
                if (avg_err <= 0.67431) {
                  if (ncc <= -1.0731) {
                    return 1;
                  } else {  // if ncc > -1.0731
                    if (avg_err <= 0.40485) {
                      return 2;
                    } else {  // if avg_err > 0.40485
                      return 2;
                    }
                  }
                } else {  // if avg_err > 0.67431
                  if (avg_shift <= 0.17334) {
                    if (std_shift_y <= 0.56176) {
                      return 1;
                    } else {  // if std_shift_y > 0.56176
                      return 2;
                    }
                  } else {  // if avg_shift > 0.17334
                    return 0;
                  }
                }
              } else {  // if avg_shift_x > 1.3565
                return 0;
              }
            }
          }
        }
      } else {  // if height > 0.16337
        if (avg_shift_x <= -1.9256) {
          if (height <= 0.85223) {
            if (avg_shift <= 5.7237) {
              if (avg_shift_y <= 2.1587) {
                if (avg_shift_y <= 1.3765) {
                  if (height <= 0.69437) {
                    return 1;
                  } else {  // if height > 0.69437
                    if (avg_shift_x <= -2.8964) {
                      return 1;
                    } else {  // if avg_shift_x > -2.8964
                      return 2;
                    }
                  }
                } else {  // if avg_shift_y > 1.3765
                  if (avg_err <= 0.86825) {
                    if (avg_err <= 0.7934) {
                      return 1;
                    } else {  // if avg_err > 0.7934
                      return 0;
                    }
                  } else {  // if avg_err > 0.86825
                    return 1;
                  }
                }
              } else {  // if avg_shift_y > 2.1587
                if (avg_shift_y <= 2.2139) {
                  if (avg_shift_x <= -2.29) {
                    return 0;
                  } else {  // if avg_shift_x > -2.29
                    if (avg_shift_y <= 2.1857) {
                      return 2;
                    } else {  // if avg_shift_y > 2.1857
                      return 1;
                    }
                  }
                } else {  // if avg_shift_y > 2.2139
                  if (avg_shift_x <= -3.6912) {
                    if (ncc <= 1.4315) {
                      return 1;
                    } else {  // if ncc > 1.4315
                      return 2;
                    }
                  } else {  // if avg_shift_x > -3.6912
                    if (avg_shift_x <= -3.6033) {
                      return 1;
                    } else {  // if avg_shift_x > -3.6033
                      return 1;
                    }
                  }
                }
              }
            } else {  // if avg_shift > 5.7237
              if (avg_err <= 1.2771) {
                if (width <= 0.6731) {
                  return 1;
                } else {  // if width > 0.6731
                  if (avg_shift_y <= 1.7388) {
                    return 1;
                  } else {  // if avg_shift_y > 1.7388
                    if (avg_shift_y <= 2.8119) {
                      return 0;
                    } else {  // if avg_shift_y > 2.8119
                      return 1;
                    }
                  }
                }
              } else {  // if avg_err > 1.2771
                return 1;
              }
            }
          } else {  // if height > 0.85223
            if (std_shift_y <= 0.57809) {
              if (ncc <= -0.15647) {
                return 2;
              } else {  // if ncc > -0.15647
                return 0;
              }
            } else {  // if std_shift_y > 0.57809
              if (avg_shift_x <= -2.4072) {
                return 2;
              } else {  // if avg_shift_x > -2.4072
                if (ncc <= 0.48877) {
                  return 1;
                } else {  // if ncc > 0.48877
                  return 2;
                }
              }
            }
          }
        } else {  // if avg_shift_x > -1.9256
          if (xyRatio <= 6.126) {
            if (width <= 0.46649) {
              if (std_shift_x <= -0.74114) {
                if (width <= 0.16316) {
                  if (avg_shift_x <= 0.31928) {
                    if (width <= -0.025867) {
                      return 2;
                    } else {  // if width > -0.025867
                      return 2;
                    }
                  } else {  // if avg_shift_x > 0.31928
                    if (avg_shift_x <= 0.46318) {
                      return 0;
                    } else {  // if avg_shift_x > 0.46318
                      return 1;
                    }
                  }
                } else {  // if width > 0.16316
                  if (height <= 1.577) {
                    if (avg_shift <= -0.50268) {
                      return 2;
                    } else {  // if avg_shift > -0.50268
                      return 1;
                    }
                  } else {  // if height > 1.577
                    if (xyRatio <= -0.49508) {
                      return 0;
                    } else {  // if xyRatio > -0.49508
                      return 2;
                    }
                  }
                }
              } else {  // if std_shift_x > -0.74114
                if (avg_shift_y <= -0.04762) {
                  if (width <= 0.15437) {
                    if (avg_shift_x <= 0.45508) {
                      return 2;
                    } else {  // if avg_shift_x > 0.45508
                      return 1;
                    }
                  } else {  // if width > 0.15437
                    if (avg_shift_x <= -0.42936) {
                      return 2;
                    } else {  // if avg_shift_x > -0.42936
                      return 1;
                    }
                  }
                } else {  // if avg_shift_y > -0.04762
                  if (avg_shift_x <= -0.45937) {
                    if (std_shift_x <= 0.70735) {
                      return 2;
                    } else {  // if std_shift_x > 0.70735
                      return 1;
                    }
                  } else {  // if avg_shift_x > -0.45937
                    if (xyRatio <= -0.94632) {
                      return 2;
                    } else {  // if xyRatio > -0.94632
                      return 0;
                    }
                  }
                }
              }
            } else {  // if width > 0.46649
              if (std_shift <= -0.14875) {
                if (ncc <= -1.1452) {
                  if (ncc <= -1.5583) {
                    if (height <= 0.34276) {
                      return 1;
                    } else {  // if height > 0.34276
                      return 2;
                    }
                  } else {  // if ncc > -1.5583
                    if (avg_shift_x <= -0.24726) {
                      return 2;
                    } else {  // if avg_shift_x > -0.24726
                      return 2;
                    }
                  }
                } else {  // if ncc > -1.1452
                  if (avg_err <= 0.45216) {
                    if (avg_shift_x <= -0.56554) {
                      return 2;
                    } else {  // if avg_shift_x > -0.56554
                      return 2;
                    }
                  } else {  // if avg_err > 0.45216
                    if (std_shift_x <= 0.17866) {
                      return 0;
                    } else {  // if std_shift_x > 0.17866
                      return 1;
                    }
                  }
                }
              } else {  // if std_shift > -0.14875
                if (std_shift_x <= 1.5143) {
                  if (avg_shift <= 0.98975) {
                    if (height <= 0.78765) {
                      return 2;
                    } else {  // if height > 0.78765
                      return 2;
                    }
                  } else {  // if avg_shift > 0.98975
                    if (std_shift_y <= 0.85662) {
                      return 2;
                    } else {  // if std_shift_y > 0.85662
                      return 1;
                    }
                  }
                } else {  // if std_shift_x > 1.5143
                  if (std_shift <= 0.37422) {
                    if (xyRatio <= 0.86337) {
                      return 0;
                    } else {  // if xyRatio > 0.86337
                      return 0;
                    }
                  } else {  // if std_shift > 0.37422
                    if (std_shift <= 5.1921) {
                      return 2;
                    } else {  // if std_shift > 5.1921
                      return 0;
                    }
                  }
                }
              }
            }
          } else {  // if xyRatio > 6.126
            if (avg_shift_x <= -0.23747) {
              if (ncc <= -1.5199) {
                if (avg_shift <= -0.43489) {
                  return 2;
                } else {  // if avg_shift > -0.43489
                  return 0;
                }
              } else {  // if ncc > -1.5199
                return 0;
              }
            } else {  // if avg_shift_x > -0.23747
              return 0;
            }
          }
        }
      }
    }
  }
}

} // namespace rm
