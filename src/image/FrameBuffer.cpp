#include <array>
#include <cstdint>
#include <image/FrameBuffer.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <external/stb_image_write.h>

namespace polaris::image {

namespace {
void WriteToStream(void* context, void* data, int size) {
  auto* out = static_cast<std::ofstream*>(context);
  out->write(static_cast<const char*>(data), size);
}
}  // namespace

void FrameBuffer::Set(std::size_t x, std::size_t y, const PixelU8& p) {
  const auto index = (y * width_) + x;

#ifndef NDEBUG
  // Bounds check (index must be strictly less than data_.size())
  if (index >= data_.size()) [[unlikely]] {
    return;
  }
#endif

  data_[index] = p;
}

void FrameBuffer::Write(std::ofstream& out) {
  switch (format_) {
    case FileFormat::BMP:
      WriteAsBMP(out);
      break;
    case FileFormat::PNG:
      WriteAsPNG(out);
      break;
    case FileFormat::JPG:
      WriteAsJPG(out);
      break;
    default:
      WriteAsBMP(out);
      break;
  }
}

void FrameBuffer::WriteAsBMP(std::ofstream& out) {
  if (!out.good()) {
    return;
  }

  const int width = static_cast<int>(width_);
  const int height = static_cast<int>(height_);

  std::vector<std::uint8_t> rgb;
  rgb.resize(width_ * height_ * 3);

  for (std::size_t y = 0; y < height_; ++y) {
    for (std::size_t x = 0; x < width_; ++x) {
      const PixelU8& p = data_[(y * width_) + x];
      const std::size_t offset = (y * width_ + x) * 3;
      rgb[offset + 0] = p.R();
      rgb[offset + 1] = p.G();
      rgb[offset + 2] = p.B();
    }
  }

  stbi_write_bmp_to_func(WriteToStream, &out, width, height, 3, rgb.data());
}

void FrameBuffer::WriteAsPNG(std::ofstream& out) {
  if (!out.good()) {
    return;
  }

  const int width = static_cast<int>(width_);
  const int height = static_cast<int>(height_);

  std::vector<std::uint8_t> rgb;
  rgb.resize(width_ * height_ * 3);

  for (std::size_t y = 0; y < height_; ++y) {
    for (std::size_t x = 0; x < width_; ++x) {
      const PixelU8& p = data_[(y * width_) + x];
      const std::size_t offset = (y * width_ + x) * 3;
      rgb[offset + 0] = p.R();
      rgb[offset + 1] = p.G();
      rgb[offset + 2] = p.B();
    }
  }

  const int stride = width * 3;
  stbi_write_png_to_func(WriteToStream, &out, width, height, 3, rgb.data(),
                         stride);
}

void FrameBuffer::WriteAsJPG(std::ofstream& out) {
  if (!out.good()) {
    return;
  }

  const int width = static_cast<int>(width_);
  const int height = static_cast<int>(height_);

  std::vector<std::uint8_t> rgb;
  rgb.resize(width_ * height_ * 3);

  for (std::size_t y = 0; y < height_; ++y) {
    for (std::size_t x = 0; x < width_; ++x) {
      const PixelU8& p = data_[(y * width_) + x];
      const std::size_t offset = (y * width_ + x) * 3;
      rgb[offset + 0] = p.R();
      rgb[offset + 1] = p.G();
      rgb[offset + 2] = p.B();
    }
  }

  constexpr int kJpegQuality = 90;
  stbi_write_jpg_to_func(WriteToStream, &out, width, height, 3, rgb.data(),
                         kJpegQuality);
}
}  // namespace polaris::image
