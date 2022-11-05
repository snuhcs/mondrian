#ifndef TEST_HPP_
#define TEST_HPP_

#include <vector>

namespace rm {

class BoundingBox;
class RoI;

void testAssignedUniqueBoxID(const std::vector<std::unique_ptr<BoundingBox>>& boxes);

void testAssignedUniqueRoIID(const std::vector<std::unique_ptr<RoI>>& rois);

void testParentChildrenIDsAndChildIDsSame(const std::vector<std::unique_ptr<RoI>>& childRoIs,
                                          const std::vector<std::unique_ptr<RoI>>& parentRoIs);

void testChildRoIsFrameRelation(const std::vector<std::unique_ptr<RoI>>& childRoIs);

void testParentRoIsFrameRelation(const std::vector<std::unique_ptr<RoI>>& parentRoIs);

} // namespace rm

#endif // TEST_HPP_
