#ifndef POLARIS_MATH_RAY_HPP
#define POLARIS_MATH_RAY_HPP

#include <math/vec.hpp>

namespace polaris::math {

class Ray {
 public:
  Ray() = default;
  Ray(const Vec3& origin, const Vec3& direction)
      : origin_(origin), direction_(direction) {}

  const Vec3& origin() const { return origin_; }
  const Vec3& direction() const { return direction_; }

  Vec3 at(double t) const { return origin_ + t * direction_; }

 private:
  Vec3 origin_;
  Vec3 direction_;
};
}  // namespace polaris::math

#endif
