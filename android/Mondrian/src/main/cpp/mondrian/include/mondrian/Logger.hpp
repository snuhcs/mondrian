#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>

#include "mondrian/DataType.hpp"

namespace md {

class BoundingBox;
class Frame;
class MergedROI;
class ROI;

class Logger {
 public:
  Logger(const char* logPath);

  ~Logger();

  void logFrameHeader();

  void logROIHeader();

  void logBoxesHeader();

  void logFrame(const Frame* frame);

  void logROI(const ROI* roi);

  void logBoxes(VID vid, FID fid, const std::vector<BoundingBox>& boxes);

 private:
  std::ofstream logFile;
  std::mutex mtx;

  time_us baseTime;
};

} // namespace md

#endif // LOGGER_HPP
