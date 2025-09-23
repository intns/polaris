#ifndef POLARIS_SCENE_OBJECTS_SPHERE_HPP
#define POLARIS_SCENE_OBJECTS_SPHERE_HPP

#include <math/AABB.hpp>
#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Material.hpp>

namespace polaris::scene::objects {
class Sphere : public Hittable {
 public:
  // Stationary Sphere
  Sphere(const math::Vec3& static_centre, const double radius,
          std::shared_ptr<material::Material> mat)
            : center_(static_centre, math::Vec3(0, 0, 0)),
              radius_(std::fmax(0, radius)),
              material_(mat) {
    auto r = math::Vec3(radius, radius, radius);
    bb_ = math::AABB(static_centre - r, static_centre + r);
  }

  Sphere(const math::Vec3& center1, const math::Vec3& center2,
          const double radius, std::shared_ptr<material::Material> mat)
            : center_(center1, center2 - center1),
              radius_(std::fmax(0, radius)),
              material_(mat) {
    auto r = math::Vec3(radius, radius, radius);
    const math::AABB box1(center_.at(0) - r, center_.at(0) + r);
    const math::AABB box2(center_.at(1) - r, center_.at(1) + r);
    bb_ = math::AABB(box1, box2);
  }

  [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval& t_interval,
                         HitInfo& rec) const override;

  [[nodiscard]] math::AABB GetBounds() const override { return bb_; }

  static void GetSphereUV(const math::Vec3& point, double& u, double& v);

 private:
  math::Ray center_{};
  double radius_ = 0.0;
  std::shared_ptr<material::Material> material_{};
  math::AABB bb_{};
};
}  // namespace polaris::scene::objects

#endif
