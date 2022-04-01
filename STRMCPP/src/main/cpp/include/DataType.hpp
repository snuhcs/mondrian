#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <string>
#include <utility>
#include <vector>

#include "opencv2/core/mat.hpp"

namespace rm {

struct RoI;

struct Rect {
    const int left;
    const int top;
    const int right;
    const int bottom;

    Rect() : left(0), top(0), right(0), bottom(0) {};

    Rect(int left, int top, int right, int bottom)
    : left(left), top(top), right(right), bottom(bottom) {};

    int width() const {
        return right - left;
    }

    int height() const {
        return bottom - top;
    }
};

struct BoundingBox {
    const Rect location;
    const float confidence;
    const std::string labelName;

    BoundingBox(Rect location, float confidence, std::string labelName)
    : location(location), confidence(confidence), labelName(labelName) {};
};

struct Frame {
    const std::string key;
    const int frameIndex;
    const cv::Mat* mat;

    std::vector<RoI> rois;
    std::vector<RoI> opticalFlowRoIs;

    std::vector<BoundingBox> boxes;

    Frame(const Frame& frame)
    : key(frame.key), frameIndex(frame.frameIndex), mat(frame.mat) {}

    Frame(const std::string& key, const int frameIndex, const cv::Mat* mat)
    : key(key), frameIndex(frameIndex), mat(mat) {}

    void setRoIs(std::vector<RoI> rs) {
        rois = std::move(rs);
    }

    std::vector<RoI> getRoIs() const {
        return rois;
    }

    void setOpticalFlowRoIs(std::vector<RoI> ofRoIs) {
        opticalFlowRoIs = std::move(ofRoIs);
    }

    void addResult(const BoundingBox& box) {
        boxes.push_back(box);
    }

    void addResults(const std::vector<BoundingBox>& boxesToAdd) {
        boxes.insert(boxes.begin(), boxesToAdd.begin(), boxesToAdd.end());
    }

    const std::vector<BoundingBox>& getResults() const {
        return boxes;
    }
};

struct RoI {
    enum Type {
        OF = 1,
        PD = 2,
    };

    Frame frame;
    Rect location;

    int handle;
    std::vector<BoundingBox> boxes;

    int minOriginLength;
    float scale;
    std::pair<int, int> packedLocation;

    Type type;
    std::string labelName;

    RoI(Frame frame, Rect location, Type type, std::string labelName)
    : frame(frame), location(location), type(type), labelName(labelName),
      packedLocation(std::make_pair(-1, -1)) {};

    RoI(Frame frame, Rect location, Type type, std::string labelName, int minOriginLength)
    : frame(frame), location(location), type(type), labelName(labelName), minOriginLength(minOriginLength),
      packedLocation(std::make_pair(-1, -1)) {};

    bool isPacked() const {
        return packedLocation.first == -1 && packedLocation.second == -1;
    }

    int getArea() const {
        return location.width() * location.height();
    }

    std::pair<int, int> getResizedWidthHeight() {
        return std::pair<int, int>();
    }

    const cv::Mat* getMat() {
        return frame.mat;
    }

    const cv::Mat* getResizedMat() {
        return frame.mat;
    }

    void setHandle(int h) {
        handle = h;
    }

    std::vector<BoundingBox> getResults() {
        return boxes;
    }

    float getScale() {
        return scale;
    }
};

struct MixedFrame {
    cv::Mat packedMat;
    std::vector<Frame*> packedFrames;

    int handle;
    std::vector<BoundingBox> boxes;

    MixedFrame(cv::Mat packedMat, std::vector<Frame*> packedFrames)
    : packedMat(std::move(packedMat)), packedFrames(packedFrames) {};

    void setHandle(int h) {
        handle = h;
    }

    std::vector<BoundingBox> getResults() {
        return boxes;
    }
};

} // namespace rm

#endif // DATA_TYPE_HPP_
