#ifndef IMPL_STATIC_RESIZE_PROFILE_HPP_
#define IMPL_STATIC_RESIZE_PROFILE_HPP_

namespace rm {

class StaticResizeProfile : public ResizeProfile {
 public:
  StaticResizeProfile(int staticTargetSize)
      : STATIC_TARGET_SIZE(staticTargetSize) {}

  int getTargetSize(const idType id, const RoI::Features& features) const override {
    return STATIC_TARGET_SIZE;
  }

  void updateTable(RoI& roi) override {
    return;
  }

 private:
  int STATIC_TARGET_SIZE;
};

} // namespace rm

#endif // IMPL_STATIC_RESIZE_PROFILE_HPP_
