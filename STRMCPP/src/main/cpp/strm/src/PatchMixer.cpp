#include "strm/PatchMixer.hpp"

namespace rm {

int PatchMixer::mixedFrameIndex = 0;

std::vector<MixedFrame> PatchMixer::pack(const std::vector<Frame*>& frames, const cv::Size& size) {
  LOGD("PatchMixer::pack() %lu", frames.size());

  std::vector<Rect> freeRects({Rect(0, 0, size.width, size.height)});

  std::vector<RoI*> rois;
  for (auto it = frames.rbegin(); it != frames.rend(); it++) {
    for (RoI& roi : (*it)->rois) {
      rois.push_back(&roi);
    }
  }

  time_us mixingStartTime = NowMicros();
  for (RoI* roi : rois) {
    std::pair<int, int> wh = roi->getResizedWidthHeight();
    for (auto it = freeRects.begin(); it != freeRects.end(); it++) {
      const Rect freeRect = *it;
      if (canFit(wh, freeRect)) {
        freeRects.erase(it);
        roi->packedLocation = std::make_pair(freeRect.left, freeRect.top);
        std::pair<Rect, Rect> newFreeRectPair = splitFreeRect(wh, freeRect);
        freeRects.push_back(newFreeRectPair.first);
        freeRects.push_back(newFreeRectPair.second);
        break;
      }
    }
  }
  time_us mixingEndTime = NowMicros();

  for (auto& frame : frames) {
    frame->mixingStartTime = mixingStartTime;
    frame->mixingEndTime = mixingEndTime;
  }

  MixedFrame mixedFrame(mixedFrameIndex++, frames, size);
  return {mixedFrame};
}

bool PatchMixer::canFit(std::pair<int, int> wh, const Rect& rect) {
  return wh.first <= rect.width() && wh.second <= rect.height();
}

std::pair<Rect, Rect> PatchMixer::splitFreeRect(std::pair<int, int> wh, const Rect& rect) {
  int w = wh.first;
  int h = wh.second;
  if (rect.width() > rect.height()) {
    return std::make_pair(Rect(rect.left + w, rect.top, rect.right, rect.bottom),
                          Rect(rect.left, rect.top + h, rect.left + w, rect.bottom));
  } else {
    return std::make_pair(Rect(rect.left, rect.top + h, rect.right, rect.bottom),
                          Rect(rect.left + w, rect.top, rect.right, rect.top + h));
  }
}

} // namespace rm
