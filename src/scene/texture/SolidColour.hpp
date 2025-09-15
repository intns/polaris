#ifndef POLARIS_SCENE_SOLID_COLOUR_HPP
#define POLARIS_SCENE_SOLID_COLOUR_HPP

#include <scene/texture/Texture.hpp>

namespace polaris::scene::texture {
class SolidColour : public Texture {
public:
  SolidColour(const image::PixelF64& albedo) : albedo_(albedo) {}

  SolidColour(const double red, const double green, const double blue)
    : SolidColour(image::PixelF64(red, green, blue)) {}

  image::PixelF64 Value(double u, double v,
                        const math::Vec3 p) const noexcept override {
    return albedo_;
  }
private:
  image::PixelF64 albedo_;
};
} // namespace polaris::scene::texture

#endif