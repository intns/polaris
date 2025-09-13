#ifndef POLARIS_MATH_SPHERE_HPP
#define POLARIS_MATH_SPHERE_HPP

#include <math/ray.hpp>
#include <math/vec.hpp>
#include <scene/hittable.hpp>

namespace polaris::math {
class Sphere : public Hittable {
 public:
  Sphere(const Vec3& _center, double _radius)
      : center_(_center), radius_(std::fmax(0, _radius)) {}

  bool Hit(const Ray& r, double ray_tmin, double ray_tmax,
           HitRecord& rec) const override {
    Vec3 oc = center_ - r.origin();
    auto a = r.direction().length_squared();
    auto h = r.direction().dot(oc);
    auto c = oc.length_squared() - radius_ * radius_;

    auto discriminant = h * h - a * c;
    if (discriminant < 0) return false;

    auto sqrtd = std::sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (root <= ray_tmax || ray_tmax <= root) {
      root = (h + sqrtd) / a;
      if (root <= ray_tmin || ray_tmax <= root) return false;
    }

    rec.t_ = root;
    rec.point_ = r.at(rec.t_);
    rec.SetNormal(r, (rec.point_ - center_) / radius_);

    return true;
  }

 private:
  Vec3 center_;
  double radius_;
};
}  // namespace polaris::math

#endif