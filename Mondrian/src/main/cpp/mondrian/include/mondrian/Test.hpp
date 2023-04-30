#ifndef TEST_HPP_
#define TEST_HPP_

#include <vector>

namespace md {

class BoundingBox;
class ROI;

void testAssignedUniqueBoxID(const std::vector<std::unique_ptr<BoundingBox>>& boxes);

void testAssignedUniqueROIID(const std::vector<std::unique_ptr<ROI>>& rois);

void testParentChildrenIDsAndChildIDsSame(const std::vector<std::unique_ptr<ROI>>& childROIs,
                                          const std::vector<std::unique_ptr<ROI>>& parentROIs);

void testChildROIsFrameRelation(const std::vector<std::unique_ptr<ROI>>& childROIs);

void testParentROIsFrameRelation(const std::vector<std::unique_ptr<ROI>>& parentROIs);

} // namespace md

#endif // TEST_HPP_
