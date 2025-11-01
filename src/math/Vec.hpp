#ifndef POLARIS_MATH_VEC_HPP
#define POLARIS_MATH_VEC_HPP

#include <array>
#include <cmath>
#include <iostream>
#include <math/Common.hpp>
#include <type_traits>


namespace polaris::math {

template<NumericType T, std::size_t N>
requires (N >=2 && N <= 4)
class Vector {
  std::array<T, N> data{};

 public:
  constexpr explicit Vector() noexcept = default;

  // can further constrain by std::convertible_to and have this accept types other than NumericType
  // but let's let the callers do the casts instead :D
  template<NumericType... Args>
  requires (sizeof...(Args) == N)
  constexpr explicit Vector(Args... args) noexcept : data{T(args)...} {}

  constexpr T& operator[](std::size_t i) noexcept { return data[i]; }
  constexpr const T& operator[](std::size_t i) const noexcept { return data[i]; }

  constexpr T X() const noexcept { return data[0]; }
  constexpr T Y() const noexcept { return data[1]; }
  constexpr T Z() const noexcept requires (N >= 3) { return data[2]; }
  constexpr T W() const noexcept requires (N >= 4) { return data[3]; }
  constexpr auto XY() const noexcept { return Vector<T, 2>{data[0], data[1]}; }
  constexpr auto XZ() const noexcept requires (N >= 3) { return Vector<T, 2>{data[0], data[2]}; }
  constexpr auto YZ() const noexcept requires (N >= 3) { return Vector<T, 2>{data[1], data[2]}; }
  constexpr auto XYZ() const noexcept requires (N >= 3) { return Vector<T, 3>{data[0], data[1], data[2]}; }
  constexpr auto XYW() const noexcept requires (N >= 4) { return Vector<T, 3>{data[0], data[1], data[3]}; }
  constexpr auto XZW() const noexcept requires (N >= 4) { return Vector<T, 3>{data[0], data[2], data[3]}; }
  constexpr auto XYZW() const noexcept requires (N >= 4) { return Vector<T, 4>{data[0], data[1], data[2], data[3]}; }

  friend constexpr Vector operator+(const Vector& a, const Vector& b) noexcept {
    Vector result;
    for (std::size_t i = 0; i < N; ++i) { result.data[i] = a.data[i] + b.data[i]; }
    return result;
  }

  friend constexpr Vector operator-(const Vector& a, const Vector& b) noexcept {
    Vector result;
    for (std::size_t i = 0; i < N; ++i) { result.data[i] = a.data[i] - b.data[i]; }
    return result;
  }

  friend constexpr Vector operator-(const Vector& v) noexcept {
    Vector result;
    for (std::size_t i = 0; i < N; ++i) { result.data[i] = -v.data[i]; }
    return result;
  }

  friend constexpr Vector operator*(const Vector& v, T scalar) noexcept {
    Vector result;
    for (std::size_t i = 0; i < N; ++i) { result.data[i] = v.data[i] * scalar; }
    return result;
  }

  friend constexpr Vector operator*(T scalar, const Vector& v) noexcept {
    return v * scalar;
  }

  friend constexpr Vector operator/(const Vector& v, T scalar) noexcept {
    Vector result;
    for (std::size_t i = 0; i < N; ++i) { result.data[i] = v.data[i] / scalar; }
    return result;
  }

  friend constexpr bool operator==(const Vector& a, const Vector& b) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      if (a.data[i] != b.data[i]) { return false; }
    }
    return true;
  }

  friend constexpr bool operator!=(const Vector& a, const Vector& b) noexcept {
    return !(a == b);
  }

  constexpr Vector& operator+=(const Vector& rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) { data[i] += rhs.data[i]; }
    return *this;
  }

  constexpr Vector& operator-=(const Vector& rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) { data[i] -= rhs.data[i]; }
    return *this;
  }

  constexpr Vector& operator*=(T scalar) noexcept {
    for (std::size_t i = 0; i < N; ++i) { data[i] *= scalar; }
    return *this;
  }

  constexpr Vector& operator/=(T scalar) noexcept {
    for (std::size_t i = 0; i < N; ++i) { data[i] /= scalar; }
    return *this;
  }

  constexpr T Dot(const Vector& rhs) const noexcept {
    T sum = 0;
    for (std::size_t i = 0; i < N; ++i) { sum += data[i] * rhs.data[i]; }
    return sum;
  }

  constexpr auto Cross(const Vector& rhs) const noexcept requires (N == 3) {
    return Vector {
      (data[1] * rhs.data[2]) - (data[2] * rhs.data[1]),
      (data[2] * rhs.data[0]) - (data[0] * rhs.data[2]),
      (data[0] * rhs.data[1]) - (data[1] * rhs.data[0])
    };
  }

  constexpr T Length() const noexcept {
    return std::sqrt(Dot(*this));
  }

  constexpr Vector Normalized() const noexcept {
    T len = Length();
    if (len == T(0)) { return *this; }
    return *this / len;
  }

  friend std::ostream& operator<<(std::ostream& out, const Vector& v) {
    for (std::size_t i = 0; i < N; ++i) {
      out << v.data[i];
      if (i + 1 < N) { out << ' '; }
    }
    return out;
  }

  constexpr T LengthSquared() const noexcept {
    return Dot(*this);
  }

  static Vector Random(T min = T(0.0), T max = T(1.0)) {
    return MakeRandom(std::make_index_sequence<N>{}, min, max);
  }

  static Vector RandomInUnitDisk() requires (std::is_floating_point_v<T> && N == 3) {
    while (true) {
      auto p = Vector(RandomValue<T>(-1, 1), RandomValue<T>(-1, 1), 0);
      if (p.LengthSquared() < 1) { return p; }
    }
  }

  // Thanks GPT
  static Vector RandomUnitVector() requires (std::is_floating_point_v<T> && N == 3)  {
    auto Z = RandomValue<T>(-1, 1);
    auto a = RandomValue<T>(0, 2 * std::numbers::pi);
    auto r = std::sqrt(1 - (Z * Z));
    return Vector(r * std::cos(a), r * std::sin(a), Z);
  }

  static Vector RandomOnHemisphere(const Vector& normal) requires (std::is_floating_point_v<T> && N == 3) {
    const auto on_unit_sphere = RandomUnitVector();
    if (on_unit_sphere.Dot(normal) > 0.0) {
      return on_unit_sphere;
    }

    return -on_unit_sphere;
  }

  Vector Reflect(const Vector& n) const requires (std::is_floating_point_v<T> && N == 3) {
    return *this - (2 * Dot(n) * n);
  }

  Vector Refract(const Vector& n, T etai_over_etat) const requires (std::is_floating_point_v<T> && N == 3) {
    const auto& uv = *this;
    auto cos_theta = std::fmin((-uv).Dot(n), 1.0);
    Vector r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vector r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.LengthSquared())) * n;
    return r_out_perp + r_out_parallel;
  }

  bool NearZero() const requires (std::is_floating_point_v<T>) {
    constexpr auto s = 1e-8;
    return LengthSquared() < (s * s);
  }

 private:
  template <std::size_t... I>
  static constexpr Vector MakeRandom(std::index_sequence<I...>, T min, T max) noexcept { // NOLINT
      return Vector{ (static_cast<void>(I), RandomValue<T>(min, max))... };
  }
};

// old class compatibility
using Vec3 = Vector<double, 3>;
// GLSL aliases
using vec2 = Vector<float, 2>;
using vec3 = Vector<float, 3>;
using vec4 = Vector<float, 4>;

} // namespace polaris::math

#endif
