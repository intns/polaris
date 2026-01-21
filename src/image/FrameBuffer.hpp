#ifndef POLARIS_IMAGE_FRAMEBUFFER_HPP
#define POLARIS_IMAGE_FRAMEBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <image/Pixel.hpp>
#include <vector>

namespace polaris::image {
enum class FileFormat : std::uint8_t {
  BMP = 0,  // Windows Bitmap (stb_image_write)
  PNG,      // Portable Network Graphics (stb_image_write)
  JPG,      // JPEG (stb_image_write)
};

class FrameBuffer {
 public:
  FrameBuffer() = default;

  void Set(std::size_t x, std::size_t y, const PixelU8& p);

  void Write(std::ofstream& out);

  void Assign(FileFormat fmt, std::size_t _width, std::size_t _height) {
    format_ = fmt;
    width_ = _width;
    height_ = _height;
    data_.resize(width_ * height_);
  }

  [[nodiscard]] std::size_t Width() const { return width_; }
  [[nodiscard]] std::size_t Height() const { return height_; }

 private:
  void WriteAsPPM(std::ofstream& out);
  void WriteAsBMP(std::ofstream& out);
  void WriteAsPNG(std::ofstream& out);
  void WriteAsJPG(std::ofstream& out);

  FileFormat format_ = FileFormat::BMP;
  std::size_t width_ = 0, height_ = 0;
  std::vector<PixelU8> data_;  // RGB byte data
};
}  // namespace polaris::image

#endif
