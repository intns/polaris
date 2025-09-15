#ifndef POLARIS_SCENE_MATERIAL_HPP
#define POLARIS_SCENE_MATERIAL_HPP

#include <image/Pixel.hpp>

namespace polaris::math {
class Ray;
} // namespace polaris::math

namespace polaris::image {
class PixelF64;
} // namespace polaris::image

namespace polaris::scene {
struct HitInfo;
} // namespace polaris::scene

namespace polaris::scene::material {
class Material {
 public:
  Material() = default;
  Material(const Material&) = default;
  Material& operator=(const Material&) = default;
  Material(Material&&) = default;
  Material& operator=(Material&&) = default;
  virtual ~Material() = default;

  virtual bool Scatter(const math::Ray& in, const scene::HitInfo& hit,
                       image::PixelF64& attenuation,
                       math::Ray& scattered) const noexcept {
    (void)in;
    (void)hit;
    (void)attenuation;
    (void)scattered;
    return false;
  }

  virtual image::PixelF64 Emitted(const scene::HitInfo& hit) const noexcept {
    (void)hit;
    return {0, 0, 0};
  }

 private:
};

}  // namespace polaris::scene::material

#endif
