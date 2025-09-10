#ifndef POLARIS_MATH_RAY_HPP
#define POLARIS_MATH_RAY_HPP

#include <math/vec.hpp>

namespace polaris::math {

class Ray {
 public:
  Ray() = default;
  explicit Ray(const Vec3d& origin, const Vec3d& direction)
      : origin_(origin), direction_(direction) {}

  const Vec3d& Origin() const { return origin_; }
  const Vec3d& Direction() const { return direction_; }
  Vec3d At(double t) const { return origin_ + direction_ * t; }

 private:
  Vec3d origin_;
  Vec3d direction_;
};

}  // namespace polaris::math

#endif
