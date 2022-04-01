#ifndef RESIZE_PROFILE_HPP_
#define RESIZE_PROFILE_HPP_

#include <string>

namespace rm {

class ResizeProfile {
public:
    virtual float getScale(std::string labelName, int width, int height, int minOriginLength) = 0;
};

}

#endif // RESIZE_PROFILE_HPP_
