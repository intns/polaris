#ifndef POLARIS_SCENE_METAL_HPP
#define POLARIS_SCENE_METAL_HPP

#include <image/Pixel.hpp>
#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Material.hpp>

namespace polaris::scene::material {

class Metal : public Material {
 public:
  Metal(const image::PixelF64& albedo) : albedo_(albedo) {}

  bool Scatter(const math::Ray& in, const scene::HitInfo& info,
               image::PixelF64& attenuation,
               math::Ray& scattered) const noexcept override {
    auto reflected = in.direction().Reflect(info.normal_);
    scattered = math::Ray(info.point_, reflected);
    attenuation = albedo_;
    return true;
  }

 private:
  image::PixelF64 albedo_;
};

}  // namespace polaris::scene::material

#endif
