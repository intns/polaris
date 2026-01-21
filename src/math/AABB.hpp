#ifndef POLARIS_MATH_AABB_HPP
#define POLARIS_MATH_AABB_HPP

#include <math/Interval.hpp>
#include <math/Ray.hpp>

namespace polaris::math {

class AABB {
 public:
  AABB() = default;

  AABB(const Interval& x, const Interval& y, const Interval& z) noexcept
      : x_(x), y_(y), z_(z) {
        PadToMinimums();
      }

  AABB(const Vec3& a, const Vec3& b) noexcept
      : x_(std::min(a.X(), b.X()), std::max(a.X(), b.X())),
        y_(std::min(a.Y(), b.Y()), std::max(a.Y(), b.Y())),
        z_(std::min(a.Z(), b.Z()), std::max(a.Z(), b.Z())) {
          PadToMinimums();
        }

  AABB(const AABB& box0, const AABB& box1) noexcept
      : x_(std::min(box0.x_.Min(), box1.x_.Min()),
           std::max(box0.x_.Max(), box1.x_.Max())),
        y_(std::min(box0.y_.Min(), box1.y_.Min()),
           std::max(box0.y_.Max(), box1.y_.Max())),
        z_(std::min(box0.z_.Min(), box1.z_.Min()),
           std::max(box0.z_.Max(), box1.z_.Max())) {}

  [[nodiscard]] const Interval& X() const noexcept { return x_; }
  [[nodiscard]] const Interval& Y() const noexcept { return y_; }
  [[nodiscard]] const Interval& Z() const noexcept { return z_; }

  [[nodiscard]] const Interval& Axis(int axis) const {
    switch (axis) {
      case 0:
        return x_;
      case 1:
        return y_;
      case 2:
        return z_;
      default:
        [[unlikely]] throw std::out_of_range("Invalid axis");
    }
  }

  [[nodiscard]] bool Hit(const Ray& r, Interval t_interval) const {
    const auto& origin = r.Origin();
    const auto& inv_direction = r.InverseDirection();

    auto tmin = t_interval.Min();
    auto tmax = t_interval.Max();

    for (int a = 0; a < 3; ++a) {
      const auto& ax = Axis(a);
      auto t0 = (ax.Min() - origin[a]) * inv_direction[a];
      auto t1 = (ax.Max() - origin[a]) * inv_direction[a];

      if (t0 > t1) {
        std::swap(t0, t1);
      }

      tmin = std::max(t0, tmin);
      tmax = std::min(t1, tmax);

      if (tmax <= tmin) {
        return false;
      }
    }

    t_interval.SetMin(tmin);
    t_interval.SetMax(tmax);
    return true;
  }

 private:
  void PadToMinimums() {
    double delta = 0.0001;
    if(x_.Size() < delta) {
      x_.Expand(delta);
    }
    if(y_.Size() < delta) {
      y_.Expand(delta);
    }
    if(z_.Size() < delta) {
      z_.Expand(delta);
    }
  }

  Interval x_, y_, z_;
};

}  // namespace polaris::math

#endif
