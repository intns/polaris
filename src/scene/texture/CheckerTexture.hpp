#ifndef POLARIS_SCENE_CHECKER_TEXTURE_HPP
#define POLARIS_SCENE_CHECKER_TEXTURE_HPP

#include <memory>
#include <scene/texture/Texture.hpp>
#include <utility>

#include "SolidColour.hpp"

namespace polaris::scene::texture {
class CheckerTexture : public Texture {
public:
  CheckerTexture(const double scale, std::shared_ptr<Texture> even,
                 std::shared_ptr<Texture> odd)
                   : inv_scale_(1.0 / scale), even_(std::move(even)), odd_(std::move(odd)) {}

  CheckerTexture(const double scale, const image::PixelF64& c1,
                 const image::PixelF64& c2)
                   : CheckerTexture(scale,
                                    std::make_shared<SolidColour>(c1),
                                    std::make_shared<SolidColour>(c2)) {}

  image::PixelF64 Value(double u, double v,
                        const math::Vec3 p) const noexcept override {
    auto x_integer = static_cast<int>(std::floor(inv_scale_ * p.X()));
    auto y_integer = static_cast<int>(std::floor(inv_scale_ * p.Y()));
    auto z_integer = static_cast<int>(std::floor(inv_scale_ * p.Z()));

    bool is_even = (x_integer + y_integer + z_integer) % 2 == 0;

    return is_even ? even_->Value(u, v, p) : odd_->Value(u, v, p);
  }

private:
  double inv_scale_{};
  std::shared_ptr<Texture> even_;
  std::shared_ptr<Texture> odd_;
};
} // namespace polaris::scene::texture

#endif
