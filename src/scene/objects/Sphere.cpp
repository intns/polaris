#include <scene/objects/Sphere.hpp>

namespace polaris::scene::objects {

bool Sphere::Hit(const math::Ray& r, const math::Interval& t_interval,
                 HitInfo& rec) const {
  math::Vec3 oc = center_ - r.origin();
  auto a = r.direction().LengthSquared();
  auto h = r.direction().Dot(oc);
  auto c = oc.LengthSquared() - radius_ * radius_;

  auto discriminant = h * h - a * c;
  if (discriminant < 0) {
    return false;
  }

  auto sqrtd = std::sqrt(discriminant);

  auto root_times_a = h - sqrtd;
  auto t = root_times_a / a;
  if (!t_interval.Surrounds(t)) {
    root_times_a = h + sqrtd;
    t = root_times_a / a;
    if (!t_interval.Contains(t)) {
      return false;
    }
  }

  rec.t_ = t;
  rec.point_ = r.at(t);
  const auto inv_radius = 1.0f / radius_;
  rec.SetNormal(r, (rec.point_ - center_) * inv_radius);
  rec.material_ = material_;

  return true;
}
}  // namespace polaris::scene::objects
