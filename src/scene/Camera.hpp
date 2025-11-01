#ifndef POLARIS_CAMERA_CAMERA_HPP
#define POLARIS_CAMERA_CAMERA_HPP

#include <image/FrameBuffer.hpp>
#include <math/Common.hpp>
#include <optional>
#include <scene/Hittable.hpp>

namespace polaris::scene {

struct CameraSettings {
  // Camera
  double aspect_ratio = 16.0 / 9.0;
  int image_width = 400;
  double fov = 90;
  double defocus_angle = 0;
  double focus_dist = 10;

  // Rendering
  std::uint32_t samples_per_pixel =
      10;                         // Random samples per pixel (anti-aliasing)
  std::uint32_t max_depth_ = 10;  // Maximum ray bounces into the scene
  image::FileFormat output_format_ =
      image::FileFormat::BMP;  // Output image format

  // Parallel rendering
  int tile_size = 64;  // Square tile size in pixels
};

class Camera {
 public:
  explicit Camera(const CameraSettings& settings);

  void Render(const Hittable& world);
  void Write(const std::string& filename);

  void SetTarget(const math::Vec3& pos, std::optional<math::Vec3> opt_lookat);

 private:
  math::Ray GetRayFor(double u_norm, double v_norm) const;

  math::Vec3 DefocusDiskSample() const;

  image::PixelF64 RayColour(const math::Ray& r, std::uint32_t depth,
                            const Hittable& world);

  void RenderTile(int x0, int y0, int x1, int y1, const Hittable& world,
                  std::mt19937& rng);

  CameraSettings settings_;

  double pixel_samples_scale_ = 0.0;   // Color scale factor for sampled pixels
  int image_height_ = 0;               // Rendered image height
  math::Vec3 center_;                // Camera center
  math::Vec3 pixel00_loc_;           // Location of pixel 0, 0
  math::Vec3 pixel_delta_u_;         // Offset to pixel to the right
  math::Vec3 pixel_delta_v_;         // Offset to pixel below
  image::FrameBuffer frame_buffer_;  // Destination image

  math::Vec3 position_;
  math::Vec3 lookat_{0, 0, -1};
  const math::Vec3 up_{0, 1, 0};
  math::Vec3 u, v, w;  // Basis vectors

  math::Vec3 defocus_disk_u_;
  math::Vec3 defocus_disk_v_;
};

}  // namespace polaris::scene

#endif
