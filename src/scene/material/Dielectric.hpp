#ifndef POLARIS_SCENE_MATERIAL_DIELECTRIC_HPP
#define POLARIS_SCENE_MATERIAL_DIELECTRIC_HPP

#include <scene/material/Material.hpp>

#include "scene/Hittable.hpp"

namespace polaris::scene::material {
class Dielectric : public Material {
public:
  Dielectric(double refraction_index) : refraction_index_(refraction_index) {}

  bool Scatter(const math::Ray& in, const scene::HitInfo& info,
                 image::PixelF64& attenuation,
                 math::Ray& scattered) const noexcept override {
    attenuation = image::PixelF64(1.0, 1.0, 1.0);
    double ri = info.front_face_ ? (1.0 / refraction_index_) : refraction_index_;

    math::Vec3 unit_direction = in.direction().Unit();
    double cos_theta = std::fmin((-unit_direction).Dot(info.normal_), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    math::Vec3 direction;

    if (cannot_refract || Reflectance(cos_theta, ri) > math::RandomDouble())
      direction = unit_direction.Reflect(info.normal_);
    else
      direction = unit_direction.Refract(info.normal_, ri);

    scattered = math::Ray(info.point_, direction);
    return true;
  }

private:
  double refraction_index_;

  static double Reflectance(double cosine, double refraction_index) {
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }
};
} // namespace polaris::scene::material

#endif