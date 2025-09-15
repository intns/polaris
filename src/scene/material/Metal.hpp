#ifndef POLARIS_SCENE_METAL_HPP
#define POLARIS_SCENE_METAL_HPP

#include <algorithm>
#include <image/Pixel.hpp>
#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Material.hpp>

namespace polaris::scene::material {

class Metal : public Material {
 public:
  Metal(const image::PixelF64& albedo, double fuzz)
      : albedo_(albedo), fuzz_(std::clamp(fuzz, 0.0, 1.0)) {}
  ~Metal() override = default;

  bool Scatter(const math::Ray& in, const scene::HitInfo& info,
               image::PixelF64& attenuation,
               math::Ray& scattered) const noexcept override {
    auto reflected = in.direction().Unit().Reflect(info.normal_);
    reflected = reflected + (fuzz_ * math::Vec3::RandomUnitVector());
    scattered = math::Ray(info.point_, reflected, in.Time());
    attenuation = albedo_;
    return (scattered.direction().Dot(info.normal_) > 0);
  }

 private:
  image::PixelF64 albedo_{};
  double fuzz_ = 0.0;
};

}  // namespace polaris::scene::material

#endif
