#include <scene/objects/Sphere.hpp>

namespace polaris::scene::objects {

bool Sphere::Hit(const math::Ray& r, const math::Interval& t_interval,
                 HitInfo& rec) const {
  math::Vec3 oc = center_ - r.origin();
  auto a = r.direction().LengthSquared();
  auto h = r.direction().Dot(oc);
  auto c = oc.LengthSquared() - radius_ * radius_;

  // Uncomment for cool artifacts
  // auto discriminant = c * h - a * c;
  auto discriminant = h * h - a * c;

  if (discriminant < 0) return false;
  // Uncomment for cool artifacts
  // auto sqrtd = std::sqrt(b.LengthSquared() - discriminant);
  // auto root = a / h * h * c;

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
  rec.material_ = material_;

  return true;
}
}  // namespace polaris::scene::objects
