#ifndef POLARIS_IMAGE_PPM_HPP
#define POLARIS_IMAGE_PPM_HPP

#include <cstdint>
#include <fstream>
#include <math/vec.hpp>
#include <string>
#include <vector>

namespace polaris::image {
class PixelU8;

// Pixel is stored as floating point values in the range [0.0, 1.0]
class PixelF64 {
 public:
  PixelF64() : r(0.0f), g(0.0f), b(0.0f) {}
  PixelF64(double _r, double _g, double _b) : r(_r), g(_g), b(_b) {}

  double R() const { return r; }
  double G() const { return g; }
  double B() const { return b; }

  PixelU8 ToU8() const;

  // clang-format off
  PixelF64& operator+=(const PixelF64& o) { r += o.r; g += o.g; b += o.b; return *this; }
  PixelF64& operator-=(const PixelF64& o) { r -= o.r; g -= o.g; b -= o.b; return *this; }
  PixelF64& operator*=(const PixelF64& o) { r *= o.r; g *= o.g; b *= o.b; return *this; }
  PixelF64& operator/=(const PixelF64& o) { r /= o.r; g /= o.g; b /= o.b; return *this; }

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

  PixelF64 operator+(double s) const { PixelF64 t = *this; t += s; return t; }
  PixelF64 operator-(double s) const { PixelF64 t = *this; t -= s; return t; }
  PixelF64 operator*(double s) const { PixelF64 t = *this; t *= s; return t; }
  PixelF64 operator/(double s) const { PixelF64 t = *this; t /= s; return t; }

  friend PixelF64 operator*(double s, const PixelF64& p) { return p * s; }
  friend PixelF64 operator+(double s, const PixelF64& p) { return p + s; }

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

  std::uint8_t R() const { return r; }
  std::uint8_t G() const { return g; }
  std::uint8_t B() const { return b; }

  PixelF64 ToF32() const { return {r / 255.999f, g / 255.999f, b / 255.999f}; }

  friend std::ofstream& operator<<(std::ofstream& os, const PixelU8& p) {
    os << int(p.r) << ' ' << int(p.g) << ' ' << int(p.b);
    return os;
  }

 private:
  std::uint8_t r, g, b;
};

class PPM {
 public:
  PPM(std::size_t _width, std::size_t _height)
      : width_(_width), height_(_height) {
    data_.resize(width_ * height_);
  }

  void Set(std::size_t x, std::size_t y, const PixelU8& p);
  void Set(std::size_t x, std::size_t y, const PixelF64& p) {
    Set(x, y, p.ToU8());
  }

  void Write(std::ofstream& fstream);

  std::size_t Width() const { return width_; }
  std::size_t Height() const { return height_; }

 private:
  std::size_t width_;
  std::size_t height_;

  std::vector<PixelU8> data_;
};
}  // namespace polaris::image

#endif
