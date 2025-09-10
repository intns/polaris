#ifndef POLARIS_MATH_VEC_HPP
#define POLARIS_MATH_VEC_HPP

#include <cmath>
#include <concepts>
#include <iostream>
#include <type_traits>

namespace polaris::math {

template <typename T>
concept is_numeric = std::integral<T> or std::floating_point<T>;

template <typename T>
  requires is_numeric<T>
class Vec3 {
 public:
  Vec3() : x_(0), y_(0), z_(0) {}
  explicit Vec3(T _x, T _y, T _z) : x_(_x), y_(_y), z_(_z) {}

  Vec3 operator-() const { return Vec3(-x_, -y_, -z_); }
  Vec3 operator*(T s) const { return Vec3(x_ * s, y_ * s, z_ * s); }
  Vec3 operator/(T s) const { return Vec3(x_ / s, y_ / s, z_ / s); }

  // clang-format off
  Vec3 operator+(const Vec3& v) const { return Vec3(x_ + v.x_, y_ + v.y_, z_ + v.z_); }
  Vec3 operator-(const Vec3& v) const { return Vec3(x_ - v.x_, y_ - v.y_, z_ - v.z_); }
  Vec3 operator*(const Vec3& v) const { return Vec3(x_ * v.x_, y_ * v.y_, z_ * v.z_); }
  Vec3 operator/(const Vec3& v) const { return Vec3(x_ / v.x_, y_ / v.y_, z_ / v.z_); }

  Vec3& operator+=(const Vec3& v) { x_ += v.x_; y_ += v.y_; z_ += v.z_; return *this; }
  Vec3& operator-=(const Vec3& v) { x_ -= v.x_; y_ -= v.y_; z_ -= v.z_; return *this; }
  Vec3& operator*=(const Vec3& v) { x_ *= v.x_; y_ *= v.y_; z_ *= v.z_; return *this; }
  Vec3& operator/=(const Vec3& v) { x_ /= v.x_; y_ /= v.y_; z_ /= v.z_; return *this; }
  Vec3& operator*=(T s) { x_ *= s; y_ *= s; z_ *= s; return *this; }
  Vec3& operator/=(T s) { x_ /= s; y_ /= s; z_ /= s; return *this; }
  // clang-format on

  T length() const { return std::sqrt(length_sqr()); }
  T length_sqr() const { return x_ * x_ + y_ * y_ + z_ * z_; }

  T dot(const Vec3& v) const { return x_ * v.x_ + y_ * v.y_ + z_ * v.z_; }
  Vec3 cross(const Vec3& v) const {
    return Vec3(y_ * v.z_ - z_ * v.y_, z_ * v.x_ - x_ * v.z_,
                x_ * v.y_ - y_ * v.x_);
  }
  Vec3 unit() const { return *this / length(); }

  T x() const { return x_; }
  T y() const { return y_; }
  T z() const { return z_; }

  friend std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    if constexpr (std::is_integral_v<T> && sizeof(T) == 1) {
      // Print byte-sized integers as numbers, not characters
      return out << static_cast<int>(v.x()) << ' ' << static_cast<int>(v.y())
                 << ' ' << static_cast<int>(v.z());
    } else {
      return out << v.x() << ' ' << v.y() << ' ' << v.z();
    }
  }

 private:
  T x_, y_, z_;
};

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<int>;

// Allow scalar on the left: s * v
template <typename T>
  requires is_numeric<T>
inline Vec3<T> operator*(T s, const Vec3<T>& v) {
  return v * s;
}

}  // namespace polaris::math

#endif
