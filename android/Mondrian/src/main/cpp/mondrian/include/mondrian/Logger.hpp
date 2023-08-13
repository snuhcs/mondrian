#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>

#include "mondrian/Time.hpp"

namespace md {

class BoundingBox;

class Frame;

class ROI;

class Logger {
 public:
  Logger(const char* logPath);

  ~Logger();

  void logFrameHeader();

  void logFrame(const Frame* frame);

  void logROIHeader();

  void logROI(const ROI* roi);

  void logBoxesHeader();

  void logBoxes(int vid, int frameIndex, const std::vector<BoundingBox>& boxes);

 private:
  time_us fromBaseTime(const time_us& time) const;

  const char delim = '\t';

  std::ofstream logFile;
  std::mutex mtx;

  time_us baseTime;
};

} // namespace md

#endif // LOGGER_HPP
