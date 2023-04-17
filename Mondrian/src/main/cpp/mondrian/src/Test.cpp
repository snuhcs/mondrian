#include "mondrian/Test.hpp"

#include <set>

#include "mondrian/Frame.hpp"

namespace md {

void testAssignedUniqueBoxID(const std::vector<std::unique_ptr<BoundingBox>>& boxes) {
  std::set<idType> IDs;
  for (const std::unique_ptr<BoundingBox>& box : boxes) {
    assert(box->id != UNASSIGNED_ID);
    assert(IDs.find(box->id) == IDs.end());
    IDs.insert(box->id);
  }
}

void testAssignedUniqueROIID(const std::vector<std::unique_ptr<ROI>>& rois) {
  std::set<idType> IDs;
  for (const auto& roi : rois) {
    assert(IDs.find(roi->id) == IDs.end());
    IDs.insert(roi->id);
  }
}

void testParentChildrenIDsAndChildIDsSame(const std::vector<std::unique_ptr<ROI>>& childROIs,
                                          const std::vector<std::unique_ptr<ROI>>& parentROIs) {
  for (const auto& pROI : parentROIs) {
    for (const ROI* cROI : pROI->childROIs) {
      assert(cROI->parentROI == pROI.get());
    }
  }
}

void testChildROIsFrameRelation(const std::vector<std::unique_ptr<ROI>>& childROIs) {
  for (const auto& cROI : childROIs) {
    assert(std::any_of(cROI->frame->childROIs.begin(), cROI->frame->childROIs.end(),
                       [&cROI](auto& cROICandidate) { return cROICandidate.get() == cROI.get(); }));
  }
}

void testParentROIsFrameRelation(const std::vector<std::unique_ptr<ROI>>& parentROIs) {
  for (const auto& pROI : parentROIs) {
    assert(std::any_of(pROI->frame->parentROIs.begin(), pROI->frame->parentROIs.end(),
                       [&pROI](const auto& pROICandidate) {
                         return pROICandidate.get() == pROI.get();
                       }));
  }
}

} // namespace md
