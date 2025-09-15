#ifndef POLARIS_SCENE_LAMBERTIAN_HPP
#define POLARIS_SCENE_LAMBERTIAN_HPP

#include <image/Pixel.hpp>
#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Material.hpp>

namespace polaris::scene::material {

class Lambertian : public Material {
 public:
  explicit Lambertian(const image::PixelF64& albedo) : albedo_(albedo) {}
  ~Lambertian() override = default;

  bool Scatter(const math::Ray& in, const scene::HitInfo& info,
               image::PixelF64& attenuation,
               math::Ray& scattered) const noexcept override {
    auto scatter_direction = info.normal_ + math::Vec3::RandomUnitVector();

    if (scatter_direction.NearZero()) {
      scatter_direction = info.normal_;
    }

    scattered = math::Ray(info.point_, scatter_direction, in.Time());
    attenuation = albedo_;
    return true;
  }

 private:
  image::PixelF64 albedo_{};
};

}  // namespace polaris::scene::material

#endif
