#ifndef POLARIS_SCENE_OBJECTS_SPHERE_HPP
#define POLARIS_SCENE_OBJECTS_SPHERE_HPP

#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <scene/Hittable.hpp>

namespace polaris::scene::objects {
class Sphere : public Hittable {
 public:
  Sphere(const math::Vec3& _center, double _radius)
      : center_(_center), radius_(std::fmax(0, _radius)) {}

  [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval& t_interval,
                         HitInfo& rec) const override {
    math::Vec3 oc = center_ - r.origin();
    auto a = r.direction().LengthSquared();
    auto h = r.direction().Dot(oc);
    auto c = oc.LengthSquared() - radius_ * radius_;

    auto discriminant = h * h - a * c;
    if (discriminant < 0) return false;

    auto sqrtd = std::sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (!t_interval.Surrounds(root)) {
      root = (h + sqrtd) / a;
      if (!t_interval.Contains(root)) {
        return false;
      }
    }

    rec.t_ = root;
    rec.point_ = r.at(rec.t_);
    rec.SetNormal(r, (rec.point_ - center_) / radius_);

    return true;
  }

 private:
  math::Vec3 center_;
  double radius_;
};
}  // namespace polaris::scene::objects

#endif
