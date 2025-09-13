#ifndef POLARIS_MATH_RAY_HPP
#define POLARIS_MATH_RAY_HPP

#include <math/Vec.hpp>

namespace polaris::math {

class Ray {
 public:
  Ray() = default;
  Ray(const Vec3& origin, const Vec3& direction)
      : origin_(origin), direction_(direction) {}

  [[nodiscard]] const Vec3& origin() const { return origin_; }
  [[nodiscard]] const Vec3& direction() const { return direction_; }

  [[nodiscard]] Vec3 at(double t) const { return origin_ + t * direction_; }

 private:
  Vec3 origin_;
  Vec3 direction_;
};
}  // namespace polaris::math

#endif
