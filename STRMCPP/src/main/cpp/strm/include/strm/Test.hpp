#ifndef TEST_HPP_
#define TEST_HPP_

#include "strm/DataType.hpp"

namespace rm {

void testAssignedUniqueBoxID(const std::vector<std::unique_ptr<BoundingBox>>& boxes) {
  std::set<idType> IDs;
  for (const std::unique_ptr<BoundingBox>& box : boxes) {
    assert(box->id != UNASSIGNED_ID);
    assert(IDs.find(box->id) == IDs.end());
    IDs.insert(box->id);
  }
}

void testAssignedUniqueRoIID(const std::vector<RoI>& rois) {
  std::set<idType> IDs;
  for (const RoI& roi : rois) {
    assert(IDs.find(roi.id) == IDs.end());
    IDs.insert(roi.id);
  }
}

void testParentChildrenIDsAndChildIDsSame(const std::vector<RoI>& childRoIs,
                                          const std::vector<std::unique_ptr<RoI>>& parentRoIs) {
  for (const auto& parentRoI : parentRoIs) {
    for (const RoI* childRoI : parentRoI->childRoIs) {
      assert(childRoI->parentRoI == parentRoI.get());
    }
  }
}

} // namespace rm

#endif // TEST_HPP_
