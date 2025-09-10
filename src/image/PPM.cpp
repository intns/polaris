#include <image/PPM.hpp>

namespace polaris::image {
void PPM::Set(std::uint16_t x, std::uint16_t y, const Pixel& p) {
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

  for (int x = 0; x < width_; x++) {
    for (int y = 0; y < height_; y++) {
      fstream << data_[(x * width_) + y] << std::endl;
    }
  }
}
}  // namespace polaris::image
