#include <image/PPM.hpp>
#include <math/interval.hpp>

namespace polaris::image {
PixelU8 PixelF64::ToU8() const {
  static const math::Interval_d intensity(0, 0.9999);

  return PixelU8(static_cast<int>(intensity.Clamp(r) * 256),
                 static_cast<int>(intensity.Clamp(g) * 256),
                 static_cast<int>(intensity.Clamp(b) * 256));
}

void PPM::Set(std::size_t x, std::size_t y, const PixelU8& p) {
  const auto index = (y * width_) + x;

#ifndef NDEBUG
  if (static_cast<std::size_t>(index) > data_.size()) [[unlikely]] {
    return;
  }
#endif

  data_[index] = p;
}

void PPM::Write(std::ofstream& fstream) {
  fstream << "P3\n" << width_ << ' ' << height_ << "\n255\n";
  for (size_t i = 0; i < data_.size(); i++) {
    fstream << data_[i] << '\n';
  }
}
}  // namespace polaris::image
