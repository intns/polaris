#ifndef POLARIS_SCENE_OBJECTS_SPHERE_HPP
#define POLARIS_SCENE_OBJECTS_SPHERE_HPP

#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Material.hpp>

namespace polaris::scene::objects {
class Sphere : public Hittable {
 public:
  Sphere(const math::Vec3& _center, double _radius,
         std::shared_ptr<scene::material::Material> mat)
      : center_(_center), radius_(std::fmax(0, _radius)), material_(mat) {}

  [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval& t_interval,
                         HitInfo& rec) const override;

 private:
  math::Vec3 center_ {};
  double radius_ = 0.0;
  std::shared_ptr<material::Material> material_ {};
};
}  // namespace polaris::scene::objects

#endif
