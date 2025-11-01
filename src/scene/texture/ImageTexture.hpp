#ifndef POLARIS_SCENE_IMAGE_TEXTURE_HPP
#define POLARIS_SCENE_IMAGE_TEXTURE_HPP

#include <image/RTWImage.hpp>
#include <scene/texture/Texture.hpp>
#include <math/Interval.hpp>

namespace polaris::scene::texture {
class ImageTexture : public Texture {
public:
  explicit ImageTexture(const char* filename) : image_(filename) {}

  image::PixelF64 Value(double u, double v,
                        const math::Vec3  /*p*/) const noexcept override {
    if (image_.Height() <= 0) { return image::PixelF64{0, 1, 1}; }

    u = math::Interval{0, 1}.Clamp(u);
    v = 1.0 - math::Interval{0, 1}.Clamp(v);

    auto i = static_cast<int>(u * image_.Width());
    auto j = static_cast<int>(v * image_.Height());
    const auto *pixel = image_.PixelData(i, j);

    auto color_scale = 1.0 / 255.0;
    return {color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]};
  }

private:
  image::RTWImage image_;
};
} // namespace polaris::scene::texture

#endif
