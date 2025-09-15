#ifndef POLARIS_MATH_VEC_HPP
#define POLARIS_MATH_VEC_HPP

#include <array>
#include <cmath>
#include <iostream>
#include <math/Common.hpp>

namespace polaris::math {

class Vec3 {
 public:
  std::array<double, 3> e;

  constexpr Vec3() noexcept : e{0, 0, 0} {}
  constexpr Vec3(double e0, double e1, double e2) noexcept : e{e0, e1, e2} {}

  [[nodiscard]] constexpr double X() const noexcept { return e[0]; }
  [[nodiscard]] constexpr double Y() const noexcept { return e[1]; }
  [[nodiscard]] constexpr double Z() const noexcept { return e[2]; }

  [[nodiscard]] constexpr Vec3 operator-() const noexcept {
    return {-e[0], -e[1], -e[2]};
  }
  [[nodiscard]] constexpr double operator[](int i) const noexcept {
    return e[i];
  }
  constexpr double& operator[](int i) noexcept { return e[i]; }

  Vec3& operator+=(const Vec3& v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }

  Vec3& operator-=(const Vec3& v) {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
  }

  Vec3& operator*=(double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }

  Vec3& operator*=(const Vec3& v) {
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
  }

  Vec3& operator/=(double t) { return *this *= 1 / t; }

  [[nodiscard]] double Length() const { return std::sqrt(LengthSquared()); }
  [[nodiscard]] constexpr double LengthSquared() const noexcept {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }

  [[nodiscard]] static Vec3 Random() {
    return {RandomDouble(), RandomDouble(), RandomDouble()};
  }

  [[nodiscard]] static Vec3 Random(double min, double max) {
    return {RandomDouble(min, max), RandomDouble(min, max),
            RandomDouble(min, max)};
  }

  [[nodiscard]] constexpr double Dot(const Vec3& v) const noexcept {
    return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];
  }

  [[nodiscard]] constexpr Vec3 Cross(const Vec3& v) const noexcept {
    return {e[1] * v.e[2] - e[2] * v.e[1], e[2] * v.e[0] - e[0] * v.e[2],
            e[0] * v.e[1] - e[1] * v.e[0]};
  }

  [[nodiscard]] Vec3 Unit() const { return *this / Length(); }

  [[nodiscard]] static Vec3 RandomInUnitDisk() {
    while (true) {
      auto p = Vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
      if (p.LengthSquared() < 1)
        return p;
    }
  }

  // Thanks GPT
  [[nodiscard]] static Vec3 RandomUnitVector() {
    auto Z = RandomDouble(-1, 1);
    auto a = RandomDouble(0, 2 * std::numbers::pi);
    auto r = std::sqrt(1 - Z * Z);
    return {r * std::cos(a), r * std::sin(a), Z};
  }

  [[nodiscard]] static Vec3 RandomOnHemisphere(const Vec3& normal) {
    const auto on_unit_sphere = RandomUnitVector();
    if (on_unit_sphere.Dot(normal) > 0.0) {
      return on_unit_sphere;
    }

      return -on_unit_sphere;
    }

  [[nodiscard]] Vec3 Reflect(const Vec3& n) const {
    return *this - 2 * Dot(n) * n;
  }

  [[nodiscard]] Vec3 Refract(const Vec3& n, double etai_over_etat) const {
    const auto& uv = *this;
    auto cos_theta = std::fmin((-uv).Dot(n), 1.0);
    Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.LengthSquared())) * n;
    return r_out_perp + r_out_parallel;
  }

  [[nodiscard]] bool NearZero() const {
    constexpr auto s = 1e-8;
    return LengthSquared() < (s * s);
  }

  friend std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
  }

  // clang-format off
  friend  Vec3 operator+(const Vec3& u, const Vec3& v) noexcept { return {u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]}; }
  friend  Vec3 operator-(const Vec3& u, const Vec3& v) noexcept { return {u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]}; }
  friend  Vec3 operator*(const Vec3& u, const Vec3& v) noexcept { return {u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]}; }
  friend  Vec3 operator*(double t, const Vec3& v) noexcept { return {t * v.e[0], t * v.e[1], t * v.e[2]}; }
  friend  Vec3 operator*(const Vec3& v, double t) noexcept { return t * v; }
  friend  Vec3 operator/(const Vec3& v, double t) noexcept { return (1 / t) * v; }
  // clang-format on
};
}  // namespace polaris::math

#endif
