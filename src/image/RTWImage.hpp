#ifndef POLARIS_IMAGE_RTW_IMAGE
#define POLARIS_IMAGE_RTW_IMAGE

#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <external/stb_image.h>

#include <cstdlib>
#include <iostream>

namespace polaris::image {
class RTWImage {
public:
  RTWImage() = default; // seems wrong...

  explicit RTWImage(const char* image_filename) {
    auto filename = std::string(image_filename);
    auto *imagedir = getenv("RTW_IMAGES");

    if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
    if (load(filename)) return;
    if (load("images/" + filename)) return;
    if (load("../images/" + filename)) return;
    if (load("../../images/" + filename)) return;
    if (load("../../../images/" + filename)) return;
    if (load("../../../../images/" + filename)) return;
    if (load("../../../../../images/" + filename)) return;
    if (load("../../../../../../images/" + filename)) return;

    std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
  }

  ~RTWImage() {
    delete[] bdata_;
    STBI_FREE(fdata_);
  }

  [[nodiscard]] bool load(const std::string& filename) {
    auto n = bytes_per_pixel_;
    fdata_ = stbi_loadf(filename.c_str(), &image_width_, &image_height_, &n, bytes_per_pixel_);
    if (fdata_ == nullptr) return false;

    bytes_per_scanline_ = image_width_ * bytes_per_pixel_;
    ConvertToBytes();
    return true;
  }

  [[nodiscard]] int Width() const { return (fdata_ == nullptr) ? 0 : image_width_; }
  [[nodiscard]] int Height() const { return (fdata_ == nullptr) ? 0 : image_height_; }

  [[nodiscard]] const unsigned char* PixelData(int x, int y) const {
    static unsigned char magenta[] = { 255, 0, 255 };
    if (bdata_ == nullptr) return magenta;

    x = Clamp(x, 0, image_width_);
    y = Clamp(y, 0, image_height_);

    return bdata_ + (y * bytes_per_scanline_) + (x * bytes_per_pixel_);
  }

private:
  const int bytes_per_pixel_ = 3;
  float* fdata_ = nullptr;
  unsigned char* bdata_ = nullptr;
  int image_width_ = 0;
  int image_height_ = 0;
  int bytes_per_scanline_ = 0;

  [[nodiscard]] static int Clamp(int x, int low, int high) {
    if (x < low) return low;
    if (x < high) return x;
    return high - 1;
  }

  [[nodiscard]] static unsigned char FloatToByte(float value) {
    if (value <= 0.0)
      return 0;
    if (1.0 <= value)
      return 255;
    return static_cast<unsigned char>(256.0 * value);
  }

  void ConvertToBytes() {
    int total_bytes = image_width_ * image_height_ * bytes_per_pixel_;
    bdata_ = new unsigned char[total_bytes];

    auto *bptr = bdata_;
    auto *fptr = fdata_;
    for (auto i = 0; i < total_bytes; i++, fptr++, bptr++) {
      *bptr = FloatToByte(*fptr);
    }
  }
};
} // namespace polaris::image

#ifdef _MSC_VER
    #pragma warning (pop)
#endif
#endif
