#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <map>

namespace md {

class Frame;
class PackedCanvas;
enum class Device;

class ROIExtractor {
 public:
  virtual ~ROIExtractor() = default;

  virtual void enqueue(Frame* frame) = 0;

  virtual void notify() = 0;

  virtual std::list<Frame*> collectFrames(int currID) = 0;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
