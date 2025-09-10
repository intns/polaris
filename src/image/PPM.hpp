#ifndef POLARIS_IMAGE_PPM_HPP
#define POLARIS_IMAGE_PPM_HPP

#include <cstdint>
#include <fstream>
#include <math/vec.hpp>
#include <vector>

namespace polaris::image {
using Pixel = math::Vec3<std::uint8_t>;

class PPM {
 public:
  PPM(std::size_t _width, std::size_t _height)
      : width_(_width), height_(_height) {
    data_.resize(width_ * height_);
  }

  void Set(std::uint16_t x, std::uint16_t y, const Pixel& p);

  void Write(std::ofstream& fstream);

  std::size_t Width() const { return width_; }
  std::size_t Height() const { return height_; }

 private:
  std::size_t width_;
  std::size_t height_;

  std::vector<Pixel> data_;
};
}  // namespace polaris::image

#endif
