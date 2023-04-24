#ifndef MERGED_ROI_HPP_
#define MERGED_ROI_HPP_

#include <memory>
#include <vector>

#include "mondrian/ROI.hpp"

namespace md {

class MergedROI {
 public:
  MergedROI(const std::vector<ROI*>& rois, int border, float targetScale, Type type);

  static std::unique_ptr<MergedROI> merge(const MergedROI* m0, const MergedROI* m1);

  static std::vector<std::unique_ptr<MergedROI>> mergeROIs(
      const std::vector<std::unique_ptr<ROI>>& rois, int maxSize, int border);

 private:
  static Rect locOf(const std::vector<ROI*>& rois);

  const std::vector<ROI*> rois;
  const int border;
  const float targetScale;
  const Type type;
  const Rect loc;
};

} // namespace md

#endif // MERGED_ROI_HPP_
