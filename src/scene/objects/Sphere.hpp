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
  Sphere(const math::Vec3& _center, double _radius,
         std::shared_ptr<scene::material::Material> mat)
      : center_(_center), radius_(std::fmax(0, _radius)), material_(mat) {
    auto r = math::Vec3(radius_, radius_, radius_);
    bb_ = math::AABB(center_ - r, center_ + r);
  }

  [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval& t_interval,
                         HitInfo& rec) const override;

  [[nodiscard]] math::AABB GetBounds() const override { return bb_; }

 private:
  math::Vec3 center_{};
  double radius_ = 0.0;
  std::shared_ptr<material::Material> material_{};
  math::AABB bb_{};
};
}  // namespace polaris::scene::objects

#endif
