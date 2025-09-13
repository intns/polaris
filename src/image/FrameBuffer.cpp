#include <array>
#include <cstdint>
#include <image/FrameBuffer.hpp>

namespace polaris::image {

// Generic helpers used for BMP file writing
namespace {
void WriteLE32(std::ofstream& out, std::uint32_t value) {
  std::array<std::uint8_t, 4> buf{
      static_cast<std::uint8_t>(value & 0xFF),
      static_cast<std::uint8_t>((value >> 8) & 0xFF),
      static_cast<std::uint8_t>((value >> 16) & 0xFF),
      static_cast<std::uint8_t>((value >> 24) & 0xFF)};
  out.write(reinterpret_cast<const char*>(buf.data()), buf.size());
}

void WriteLE16(std::ofstream& out, std::uint16_t value) {
  std::array<std::uint8_t, 2> buf{
      static_cast<std::uint8_t>(value & 0xFF),
      static_cast<std::uint8_t>((value >> 8) & 0xFF)};
  out.write(reinterpret_cast<char*>(buf.data()), buf.size());
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
    case FileFormat::PPM:
      WriteAsPPM(out);
      break;
    case FileFormat::BMP:
    default:
      WriteAsBMP(out);
      break;
  }
}

void FrameBuffer::WriteAsPPM(std::ofstream& out) {
  out << "P3\n" << width_ << ' ' << height_ << "\n255\n";

  for (size_t y = 0; y < height_; ++y) {
    for (size_t x = 0; x < width_; ++x) {
      out << data_[y * width_ + x];
    }
  }

  out.flush();
}

void FrameBuffer::WriteAsBMP(std::ofstream& out) {
  // 14-byte file header + 40-byte DIB header = 54 bytes
  size_t pixel_offset = 54;
  size_t row_bytes = width_ * 3;
  size_t padding = (4 - (row_bytes % 4)) % 4;
  size_t file_size = pixel_offset + (row_bytes + padding) * height_;

  // Write BMP file header
  out.seekp(0, std::ios::beg);
  out.write("BM", 2);                                   // signature
  WriteLE32(out, static_cast<uint32_t>(file_size));     // file size
  WriteLE32(out, 0);                                    // reserved
  WriteLE32(out, static_cast<uint32_t>(pixel_offset));  // pixel data offset

  // Write DIB header (BITMAPINFOHEADER, 40 bytes)
  WriteLE32(out, 40);                                   // DIB header size
  WriteLE32(out, static_cast<std::uint32_t>(width_));   // width
  WriteLE32(out, static_cast<std::uint32_t>(height_));  // height
  WriteLE16(out, 1);                                    // planes
  WriteLE16(out, 24);                                   // bits per pixel
  WriteLE32(out, 0);  // compression (0 = BI_RGB)
  WriteLE32(out, static_cast<uint32_t>((row_bytes + padding) *
                                       height_));  // image size
  WriteLE32(out, 2835);                            // X pixels per meter
  WriteLE32(out, 2835);                            // Y pixels per meter
  WriteLE32(out, 0);                               // colors in palette
  WriteLE32(out, 0);                               // important colors

  // Write pixel data bottom-up
  std::vector<std::uint8_t> pad(padding, 0);
  for (int y = static_cast<int>(height_ - 1); y >= 0; --y) {
    for (size_t x = 0; x < width_; ++x) {
      const PixelU8& p = data_[y * width_ + x];
      std::array<std::uint8_t, 3> bgr{p.B(), p.G(), p.R()};
      out.write(reinterpret_cast<char*>(bgr.data()), bgr.size());
    }

    if (padding) {
      out.write(reinterpret_cast<const char*>(pad.data()), padding);
    }
  }
}
}  // namespace polaris::image