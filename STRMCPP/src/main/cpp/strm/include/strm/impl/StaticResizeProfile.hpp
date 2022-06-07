#ifndef IMPL_STATIC_RESIZE_PROFILE_HPP_
#define IMPL_STATIC_RESIZE_PROFILE_HPP_

namespace rm {

class StaticResizeProfile : public ResizeProfile {
 public:
  StaticResizeProfile(int staticTargetSize)
      : STATIC_TARGET_SIZE(staticTargetSize) {}

  int getTargetSize(const RoI::Features& features) const {
    return STATIC_TARGET_SIZE;
  }

 private:
  const int STATIC_TARGET_SIZE;
};

} // namespace rm

#endif // IMPL_STATIC_RESIZE_PROFILE_HPP_
