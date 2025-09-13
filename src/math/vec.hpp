#ifndef POLARIS_MATH_VEC_HPP
#define POLARIS_MATH_VEC_HPP

#include <array>
#include <cmath>
#include <iostream>

namespace polaris::math {

class Vec3 {
 public:
  std::array<double, 3> e;

  constexpr Vec3() noexcept : e{0, 0, 0} {}
  constexpr Vec3(double e0, double e1, double e2) noexcept : e{e0, e1, e2} {}

  [[nodiscard]] constexpr double x() const noexcept { return e[0]; }
  [[nodiscard]] constexpr double y() const noexcept { return e[1]; }
  [[nodiscard]] constexpr double z() const noexcept { return e[2]; }

  [[nodiscard]] constexpr Vec3 operator-() const noexcept {
    return Vec3(-e[0], -e[1], -e[2]);
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

  [[nodiscard]] double length() const { return std::sqrt(length_squared()); }
  [[nodiscard]] constexpr double length_squared() const noexcept {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }

  [[nodiscard]] constexpr double dot(const Vec3& v) const noexcept {
    return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];
  }

  [[nodiscard]] constexpr Vec3 cross(const Vec3& v) const noexcept {
    return Vec3(e[1] * v.e[2] - e[2] * v.e[1], e[2] * v.e[0] - e[0] * v.e[2],
                e[0] * v.e[1] - e[1] * v.e[0]);
  }

  [[nodiscard]] inline Vec3 unit_vector() const { return *this / length(); }

  friend inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
  }

  // clang-format off
  friend inline Vec3 operator+(const Vec3& u, const Vec3& v) noexcept { return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]); }
  friend inline Vec3 operator-(const Vec3& u, const Vec3& v) noexcept { return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]); }
  friend inline Vec3 operator*(const Vec3& u, const Vec3& v) noexcept { return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]); }
  friend inline Vec3 operator*(double t, const Vec3& v) noexcept { return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]); }
  friend inline Vec3 operator*(const Vec3& v, double t) noexcept { return t * v; }
  friend inline Vec3 operator/(const Vec3& v, double t) noexcept { return (1 / t) * v; }
  // clang-format on
};
}  // namespace polaris::math

#endif
