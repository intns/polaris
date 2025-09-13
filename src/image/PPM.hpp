#ifndef POLARIS_IMAGE_PPM_HPP
#define POLARIS_IMAGE_PPM_HPP

#include <cstdint>
#include <fstream>
#include <math/vec.hpp>
#include <vector>

namespace polaris::image {
class PixelU8;

// Pixel is stored as floating point values in the range [0.0, 1.0]
class PixelF64 {
 public:
  PixelF64() : r(0.0f), g(0.0f), b(0.0f) {}
  PixelF64(double _r, double _g, double _b) : r(_r), g(_g), b(_b) {}
  explicit PixelF64(const polaris::math::Vec3& v)
      : r(v.x()), g(v.y()), b(v.z()) {}

  [[nodiscard]] double R() const { return r; }
  [[nodiscard]] double G() const { return g; }
  [[nodiscard]] double B() const { return b; }

  [[nodiscard]] PixelU8 ToU8() const;

  // clang-format off
  PixelF64& operator+=(const PixelF64& o) { r += o.r; g += o.g; b += o.b; return *this; }
  PixelF64& operator-=(const PixelF64& o) { r -= o.r; g -= o.g; b -= o.b; return *this; }
  PixelF64& operator*=(const PixelF64& o) { r *= o.r; g *= o.g; b *= o.b; return *this; }
  PixelF64& operator/=(const PixelF64& o) { r /= o.r; g /= o.g; b /= o.b; return *this; }

  // Interop with math::Vec3 (component-wise operations)
  PixelF64& operator+=(const polaris::math::Vec3& v) { r += v.x(); g += v.y(); b += v.z(); return *this; }
  PixelF64& operator-=(const polaris::math::Vec3& v) { r -= v.x(); g -= v.y(); b -= v.z(); return *this; }
  PixelF64& operator*=(const polaris::math::Vec3& v) { r *= v.x(); g *= v.y(); b *= v.z(); return *this; }
  PixelF64& operator/=(const polaris::math::Vec3& v) { r /= v.x(); g /= v.y(); b /= v.z(); return *this; }

  PixelF64& operator+=(double s) { r += s; g += s; b += s; return *this; }
  PixelF64& operator-=(double s) { r -= s; g -= s; b -= s; return *this; }
  PixelF64& operator*=(double s) { r *= s; g *= s; b *= s; return *this; }
  PixelF64& operator/=(double s) { double inv = 1.0f / s; r *= inv; g *= inv; b *= inv; return *this; }

  PixelF64 operator+() const { return *this; }
  PixelF64 operator-() const { return PixelF64{-r, -g, -b}; }

  PixelF64 operator+(const PixelF64& o) const { PixelF64 t = *this; t += o; return t; }
  PixelF64 operator-(const PixelF64& o) const { PixelF64 t = *this; t -= o; return t; }
  PixelF64 operator*(const PixelF64& o) const { PixelF64 t = *this; t *= o; return t; }
  PixelF64 operator/(const PixelF64& o) const { PixelF64 t = *this; t /= o; return t; }

  PixelF64 operator+(const polaris::math::Vec3& v) const { PixelF64 t = *this; t += v; return t; }
  PixelF64 operator-(const polaris::math::Vec3& v) const { PixelF64 t = *this; t -= v; return t; }
  PixelF64 operator*(const polaris::math::Vec3& v) const { PixelF64 t = *this; t *= v; return t; }
  PixelF64 operator/(const polaris::math::Vec3& v) const { PixelF64 t = *this; t /= v; return t; }

  PixelF64 operator+(double s) const { PixelF64 t = *this; t += s; return t; }
  PixelF64 operator-(double s) const { PixelF64 t = *this; t -= s; return t; }
  PixelF64 operator*(double s) const { PixelF64 t = *this; t *= s; return t; }
  PixelF64 operator/(double s) const { PixelF64 t = *this; t /= s; return t; }

  friend PixelF64 operator*(double s, const PixelF64& p) { return p * s; }
  friend PixelF64 operator+(double s, const PixelF64& p) { return p + s; }

  // Symmetric free operators with Vec3 on the left-hand side
  friend PixelF64 operator+(const polaris::math::Vec3& v, const PixelF64& p) { return p + v; }
  friend PixelF64 operator*(const polaris::math::Vec3& v, const PixelF64& p) { return p * v; }
  friend PixelF64 operator-(const polaris::math::Vec3& v, const PixelF64& p) { return {v.x() - p.r, v.y() - p.g, v.z() - p.b}; }
  friend PixelF64 operator/(const polaris::math::Vec3& v, const PixelF64& p) { return {v.x() / p.r, v.y() / p.g, v.z() / p.b}; }

  bool operator==(const PixelF64& o) const { return r == o.r && g == o.g && b == o.b; }
  bool operator!=(const PixelF64& o) const { return !(*this == o); }
  // clang-format on

 private:
  double r, g, b;
};

// Pixel is stored as 8-bit unsigned integer values in the range [0, 255]
class PixelU8 {
 public:
  PixelU8() : r(0), g(0), b(0) {}
  explicit PixelU8(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b)
      : r(_r), g(_g), b(_b) {}
  explicit PixelU8(int _r, int _g, int _b)
      : r(static_cast<std::uint8_t>(_r)),
        g(static_cast<std::uint8_t>(_g)),
        b(static_cast<std::uint8_t>(_b)) {}

  [[nodiscard]] std::uint8_t R() const { return r; }
  [[nodiscard]] std::uint8_t G() const { return g; }
  [[nodiscard]] std::uint8_t B() const { return b; }

  friend std::ofstream& operator<<(std::ofstream& os, const PixelU8& p) {
    os << static_cast<int>(p.r) << ' ' << static_cast<int>(p.g) << ' '
       << static_cast<int>(p.b);
    return os;
  }

 private:
  std::uint8_t r, g, b;
};

class PPM {
 public:
  PPM() = default;

  PPM(std::size_t _width, std::size_t _height)
      : width_(_width), height_(_height) {
    data_.resize(width_ * height_);
  }

  void Resize(std::size_t _width, std::size_t _height) {
    width_ = _width;
    height_ = _height;
    data_.resize(width_ * height_);
  }

  void Set(std::size_t x, std::size_t y, const PixelU8& p);
  void Set(std::size_t x, std::size_t y, const PixelF64& p) {
    Set(x, y, p.ToU8());
  }

  void Write(std::ofstream& fstream);

  [[nodiscard]] std::size_t Width() const { return width_; }
  [[nodiscard]] std::size_t Height() const { return height_; }

 private:
  std::size_t width_ = 0;
  std::size_t height_ = 0;

  std::vector<PixelU8> data_;
};
}  // namespace polaris::image

#endif
