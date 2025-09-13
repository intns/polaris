#ifndef POLARIS_CAMERA_CAMERA_HPP
#define POLARIS_CAMERA_CAMERA_HPP

#include <image/PPM.hpp>
#include <math/common.hpp>
#include <scene/hittable.hpp>

namespace polaris::scene {

struct CameraSettings {
  double aspect_ratio_ = 16.0 / 9.0;
  int image_width_ = 400;
  std::uint32_t samples_per_pixel = 10;
};

class Camera {
 public:
  explicit Camera(const CameraSettings& settings) : settings_(settings) {
    Initialise();
  }

  void Render(const Hittable& world);
  void Write(const std::string& filename);

 private:
  void Initialise();

  math::Ray GetRayFor(int x, int y) const;
  image::PixelF64 RayColour(const math::Ray& r, const scene::Hittable& world);

  CameraSettings settings_;

  double pixel_samples_scale_ = 0.0;  // Color scale factor for sampled pixels
  int image_height_ = 0;             // Rendered image height
  math::Vec3 center_;                // Camera center
  math::Vec3 pixel00_loc_;           // Location of pixel 0, 0
  math::Vec3 pixel_delta_u_;         // Offset to pixel to the right
  math::Vec3 pixel_delta_v_;         // Offset to pixel below
  image::PPM ppm_;                   // Destination image
};

}  // namespace polaris::scene

#endif
