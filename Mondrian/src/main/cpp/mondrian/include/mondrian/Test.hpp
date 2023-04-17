#ifndef TEST_HPP_
#define TEST_HPP_

#include <vector>

namespace md {

class BoundingBox;
class RoI;

void testAssignedUniqueBoxID(const std::vector<std::unique_ptr<BoundingBox>>& boxes);

void testAssignedUniqueRoIID(const std::vector<std::unique_ptr<RoI>>& rois);

void testParentChildrenIDsAndChildIDsSame(const std::vector<std::unique_ptr<RoI>>& childRoIs,
                                          const std::vector<std::unique_ptr<RoI>>& parentRoIs);

void testChildRoIsFrameRelation(const std::vector<std::unique_ptr<RoI>>& childRoIs);

void testParentRoIsFrameRelation(const std::vector<std::unique_ptr<RoI>>& parentRoIs);

} // namespace md

#endif // TEST_HPP_
