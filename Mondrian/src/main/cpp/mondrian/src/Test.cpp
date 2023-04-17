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

void testAssignedUniqueRoIID(const std::vector<std::unique_ptr<RoI>>& rois) {
  std::set<idType> IDs;
  for (const auto& roi : rois) {
    assert(IDs.find(roi->id) == IDs.end());
    IDs.insert(roi->id);
  }
}

void testParentChildrenIDsAndChildIDsSame(const std::vector<std::unique_ptr<RoI>>& childRoIs,
                                          const std::vector<std::unique_ptr<RoI>>& parentRoIs) {
  for (const auto& pRoI : parentRoIs) {
    for (const RoI* cRoI : pRoI->childRoIs) {
      assert(cRoI->parentRoI == pRoI.get());
    }
  }
}

void testChildRoIsFrameRelation(const std::vector<std::unique_ptr<RoI>>& childRoIs) {
  for (const auto& cRoI : childRoIs) {
    assert(std::any_of(cRoI->frame->childRoIs.begin(), cRoI->frame->childRoIs.end(),
                       [&cRoI](auto& cRoICandidate) { return cRoICandidate.get() == cRoI.get(); }));
  }
}

void testParentRoIsFrameRelation(const std::vector<std::unique_ptr<RoI>>& parentRoIs) {
  for (const auto& pRoI : parentRoIs) {
    assert(std::any_of(pRoI->frame->parentRoIs.begin(), pRoI->frame->parentRoIs.end(),
                       [&pRoI](const auto& pRoICandidate) {
                         return pRoICandidate.get() == pRoI.get();
                       }));
  }
}

} // namespace md
