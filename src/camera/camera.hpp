#ifndef POLARIS_CAMERA_CAMERA_HPP
#define POLARIS_CAMERA_CAMERA_HPP

#include <scene/hittable.hpp>
#include <image/PPM.hpp>

namespace polaris::camera {
class Camera {
public:
  double aspect_ratio_ = 16.0 / 9.0; // Ratio of image width over height
  int image_width_ = 400; // Rendered image width in pixel count

  void Render(const scene::Hittable& world) {
    Initialise();

    image::PPM ppm(image_width_, image_height_);

    for (int j = 0; j < image_height_; j++) {
      for (int i = 0; i < image_width_; i++) {
        auto pixel_center =
            pixel00_loc_ + (i * pixel_delta_u_) + (j * pixel_delta_v_);

        auto ray_direction = pixel_center - center_;

        math::Ray r(center_, ray_direction);
        ppm.Set(i, j, RayColour(r, world));
      }
    }

    std::ofstream f("output.ppm");
    if (f.is_open()) {
      ppm.Write(f);
    }
  }

private:
  int image_height_{};   // Rendered image height
  math::Vec3 center_;         // Camera center
  math::Vec3 pixel00_loc_;    // Location of pixel 0, 0
  math::Vec3 pixel_delta_u_;  // Offset to pixel to the right
  math::Vec3 pixel_delta_v_;  // Offset to pixel below

  void Initialise() {
    image_height_ = static_cast<int>(image_width_ / aspect_ratio_);
    image_height_ = std::max(image_height_, 1);

    center_ = math::Vec3(0, 0, 0);

    // Determine viewport dimensions.
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (static_cast<double>(image_width_) / image_height_);

    // U = top left -> top right   | going right
    // V = top left -> bottom left | going downwards
    auto viewport_u = math::Vec3(viewport_width, 0, 0);
    auto viewport_v = math::Vec3(0, -viewport_height, 0);

    // Per-pixel offsets in U and V directions
    pixel_delta_u_ = viewport_u / image_width_;
    pixel_delta_v_ = viewport_v / image_height_;

    // Start at the camera
    // position Move back on Z by focal_length (to the center of the viewport
    // plane) Shift left by half of viewport_u (to the left edge) Shift up by
    // half of viewport_v (to the top edge, since viewport_v points down)
    auto viewport_upper_left = center_ - math::Vec3(0, 0, focal_length) -
                               viewport_u / 2 - viewport_v / 2;
    pixel00_loc_ =
        viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
  }

  // we are using 'Colour' because we are british and proud god dammit
  image::PixelF64 RayColour(const math::Ray& r, const scene::Hittable& world) {
    scene::HitRecord rec;
    if (world.Hit(r, math::Interval_d(0, math::kInfinity), rec)) {
      auto p = image::PixelF64(1.0, 1.0, 1.0);
      return 0.5 * (p + (rec.normal_ * math::kPi).unit_vector());
    }

    math::Vec3 unit_direction = r.direction().unit_vector();
    auto a = 0.5 * (unit_direction.y() + 1.0);
    a = std::tan(a);
    return (1.0 - a) * image::PixelF64(1.0, 1.0, 1.0) +
           a * image::PixelF64(0.5, 0.7, 1.0);
  }};
} // namespace polaris::camera

#endif