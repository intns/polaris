#include <scene/objects/Sphere.hpp>

namespace polaris::scene::objects {
bool Sphere::Hit(const math::Ray& r, const math::Interval& t_interval,
                 HitInfo& rec) const {
  math::Vec3 current_center = center_.at(r.Time());
  math::Vec3 oc = current_center - r.origin();
  auto a = r.direction().LengthSquared();
  auto h = r.direction().Dot(oc);
  auto c = oc.LengthSquared() - (radius_ * radius_);

  auto discriminant = (h * h) - (a * c);
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
  const math::Vec3 outward_normal = (rec.point_ - current_center) / radius_;
  rec.SetNormal(r, outward_normal);
  GetSphereUV(outward_normal, rec.u_, rec.v_);
  rec.material_ = material_;

  return true;
}

void Sphere::GetSphereUV(const math::Vec3& point, double& u, double& v) {
  auto theta = std::acos(-point.Y());
  auto phi = std::atan2(-point.Z(), point.X()) + std::numbers::pi;

  u = phi / (2 * std::numbers::pi);
  v = theta / std::numbers::pi;
}
}  // namespace polaris::scene::objects
