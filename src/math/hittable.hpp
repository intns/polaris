#ifndef POLARIS_MATH_HITTABLE_HPP
#define POLARIS_MATH_HITTABLE_HPP

#include <math/vec.hpp>

namespace polaris::math {

struct HitRecord {
  Vec3 point_;
  Vec3 normal_;
  double t_ = 0.0;
};

class Ray;
class AABB;

class Hittable {
 public:
  virtual ~Hittable() = default;

  [[nodiscard]] virtual bool Hit(const Ray& r, double t_min, double t_max,
                                 HitRecord& rec) const = 0;
};

}  // namespace polaris::math

#endif