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
                                          const std::vector<RoI>& parentRoIs) {
  std::set<idType> childIDs;
  for (const RoI& childRoI : childRoIs) {
    assert(childIDs.find(childRoI.id) == childIDs.end());
    childIDs.insert(childRoI.id);
  }
  std::set<idType> childrenIDs;
  for (const RoI& parentRoI : parentRoIs) {
    for (const RoI* childRoI : parentRoI.childRoIs) {
      assert(childrenIDs.find(childRoI->id) == childrenIDs.end());
      childrenIDs.insert(childRoI->id);
    }
  }
  assert(childIDs == childrenIDs);
}

} // namespace rm

#endif // TEST_HPP_
