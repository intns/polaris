#ifndef POLARIS_SCENE_LAMBERTIAN_HPP
#define POLARIS_SCENE_LAMBERTIAN_HPP

#include <image/Pixel.hpp>
#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Material.hpp>
#include <scene/texture/Texture.hpp>
#include <scene/texture/SolidColour.hpp>

namespace polaris::scene::material {

class Lambertian : public Material {
 public:
  explicit Lambertian(const image::PixelF64& albedo)
    : texture_(std::make_shared<texture::SolidColour>(albedo)) {}
  explicit Lambertian(std::shared_ptr<texture::Texture> texture)
    : texture_(texture) {}
  ~Lambertian() override = default;

  bool Scatter(const math::Ray& in, const scene::HitInfo& info,
               image::PixelF64& attenuation,
               math::Ray& scattered) const noexcept override {
    auto scatter_direction = info.normal_ + math::Vec3::RandomUnitVector();

    if (scatter_direction.NearZero()) {
      scatter_direction = info.normal_;
    }

    scattered = math::Ray(info.point_, scatter_direction, in.Time());
    attenuation = texture_->Value(info.u_, info.v_, info.point_);
    return true;
  }

 private:
  std::shared_ptr<texture::Texture> texture_;
};
}  // namespace polaris::scene::material

#endif
