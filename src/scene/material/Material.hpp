#ifndef POLARIS_SCENE_MATERIAL_HPP
#define POLARIS_SCENE_MATERIAL_HPP

#include <image/Pixel.hpp>

namespace polaris::math {
class Ray;
}

namespace polaris::image {
class PixelF64;
}

namespace polaris::scene {
struct HitInfo;
}

namespace polaris::scene::material {
class Material {
 public:
  virtual ~Material() = default;

  virtual bool Scatter(const math::Ray& in, const scene::HitInfo& hit,
                       image::PixelF64& attenuation,
                       math::Ray& scattered) const noexcept {
    return false;
  }

  virtual image::PixelF64 Emitted(const scene::HitInfo& hit) const noexcept {
    return {0, 0, 0};
  }

 private:
};

}  // namespace polaris::scene::material

#endif
