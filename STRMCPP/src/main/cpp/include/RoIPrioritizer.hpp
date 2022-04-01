#ifndef ROI_PRIORITIZER_HPP_
#define ROI_PRIORITIZER_HPP_

namespace rm {

class RoIPrioritizer {
public:
    virtual bool priority(int const&, int const&) = 0;
};

}

#endif // ROI_PRIORITIZER_HPP_
