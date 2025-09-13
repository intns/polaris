#include <image/PPM.hpp>

namespace polaris::image {
PixelU8 PixelF64::ToU8() const {
  return PixelU8(static_cast<int>(r * 255.999), static_cast<int>(g * 255.999),
                 static_cast<int>(b * 255.999));
}

void PPM::Set(std::size_t x, std::size_t y, const PixelU8& p) {
  // transform into 1d index
  const auto index = (y * width_) + x;

#ifndef NDEBUG
  if (static_cast<std::size_t>(index) > data_.size()) [[unlikely]] {
    return;
  }
#endif

  data_[index] = p;
}

void PPM::Write(std::ofstream& fstream) {
  fstream << "P3" << std::endl;
  fstream << width_ << ' ' << height_ << std::endl;
  fstream << "255" << std::endl;

  for (int y = 0; y < height_; y++) {
    for (int x = 0; x < width_; x++) {
      const auto idx = (y * width_) + x;
      fstream << data_[idx] << '\n';
    }
  }

  fstream.flush();
}
}  // namespace polaris::image
