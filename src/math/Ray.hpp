#ifndef POLARIS_MATH_RAY_HPP
#define POLARIS_MATH_RAY_HPP

#include <math/Vec.hpp>

namespace polaris::math {

class Ray {
 public:
  Ray() = default;
  Ray(const Vec3& origin, const Vec3&direction, double time)
      : origin_(origin),
        direction_(direction),
        inv_direction_(1.0 / direction.X(), 1.0 / direction.Y(),
                       1.0 / direction.Z()),
        tm_(time) {}
  Ray(const Vec3& origin, const Vec3& direction)
      : Ray(origin, direction, 0) {}

  [[nodiscard]] const Vec3& origin() const { return origin_; }
  [[nodiscard]] const Vec3& direction() const { return direction_; }
  [[nodiscard]] double Time() const { return tm_; }
  [[nodiscard]] const Vec3& InverseDirection() const { return inv_direction_; }

  [[nodiscard]] Vec3 at(double t) const { return origin_ + t * direction_; }

 private:
  Vec3 origin_;
  Vec3 direction_;
  Vec3 inv_direction_;
  double tm_;
};
}  // namespace polaris::math

#endif
