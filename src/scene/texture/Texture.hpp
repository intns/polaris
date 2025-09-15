#ifndef POLARIS_SCENE_TEXTURE_HPP
#define POLARIS_SCENE_TEXTURE_HPP

#include <image/Pixel.hpp>

namespace polaris::scene::texture {
class Texture {
public:
  Texture() = default;
  Texture(const Texture&) = default;
  Texture& operator=(const Texture&) = default;
  Texture(Texture&&) = default;
  Texture& operator=(Texture&&) = default;
  virtual ~Texture() = default;

  virtual image::PixelF64 Value(double u, double v,
                                const math::Vec3 p) const noexcept {
    (void)u;
    (void)v;
    (void)p;
    return {0, 0, 0};
  }
};
} // namespace polaris::scene::texture

#endif
