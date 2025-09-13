#include <image/Pixel.hpp>
#include <math/Interval.hpp>

namespace polaris::image {
namespace {
double LinearToGamma(double v) { return v > 0 ? std::sqrt(v) : 0; }
}  // anonymous namespace

PixelU8 PixelF64::AsU8() const {
  double gR = LinearToGamma(r);
  double gG = LinearToGamma(g);
  double gB = LinearToGamma(b);

  static const math::Interval intensity(0, 0.9999);
  return PixelU8(static_cast<int>(intensity.Clamp(gR) * 256),
                 static_cast<int>(intensity.Clamp(gG) * 256),
                 static_cast<int>(intensity.Clamp(gB) * 256));
}
}  // namespace polaris::image