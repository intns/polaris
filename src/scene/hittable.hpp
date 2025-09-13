#ifndef POLARIS_SCENE_HITTABLE_HPP
#define POLARIS_SCENE_HITTABLE_HPP

#include <math/ray.hpp>
#include <math/vec.hpp>

namespace polaris::scene {

struct HitRecord {
  math::Vec3 point_;
  math::Vec3 normal_;
  double t_ = 0.0;
  bool front_face_ = false;

  void SetNormal(const math::Ray& r, const math::Vec3& outward_normal) {
    front_face_ = r.direction().dot(outward_normal) < 0;
    normal_ = front_face_ ? outward_normal : -outward_normal;
  }
};

class Hittable {
 public:
  virtual ~Hittable() = default;

  [[nodiscard]] virtual bool Hit(const math::Ray& r, double t_min, double t_max,
                                 HitRecord& rec) const = 0;
};

}  // namespace polaris::scene

#endif