#include <chrono>
#include <math/Common.hpp>
#include <scene/Camera.hpp>
#include <scene/material/Material.hpp>
#include <stdexcept>

namespace polaris::scene {

void Camera::Initialise() {
  const auto image_width = settings_.image_width;

  image_height_ = static_cast<int>(image_width / settings_.aspect_ratio);
  image_height_ = std::max(image_height_, 1);

  frame_buffer_.Assign(settings_.output_format_, image_width, image_height_);

  pixel_samples_scale_ = 1.0 / settings_.samples_per_pixel;

  center_ = math::Vec3(0, 0, 0);

  // Determine viewport dimensions.
  auto focal_length = 1.0;
  auto viewport_height = 2.0;
  auto viewport_width =
      viewport_height * (static_cast<double>(image_width) / image_height_);

  // U = top left -> top right   | going right
  // V = top left -> bottom left | going downwards
  auto viewport_u = math::Vec3(viewport_width, 0, 0);
  auto viewport_v = math::Vec3(0, -viewport_height, 0);

  // Per-pixel offsets in U and V directions
  pixel_delta_u_ = viewport_u / image_width;
  pixel_delta_v_ = viewport_v / image_height_;

  auto viewport_upper_left = center_ - math::Vec3(0, 0, focal_length) -
                             viewport_u / 2 - viewport_v / 2;

  pixel00_loc_ = viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
}

void Camera::Write(const std::string& filename) {
  std::ofstream f(filename, std::ios::binary);
  if (!f.is_open()) {
    return;
  }

  frame_buffer_.Write(f);
}

void Camera::Render(const Hittable& world) {
  for (int y = 0; y < image_height_; y++) {
    for (int x = 0; x < settings_.image_width; x++) {
      // Iteratively sample the pixel colour
      image::PixelF64 ambient_col(0.0, 0.0, 0.0);
      for (std::uint32_t i = 0; i < settings_.samples_per_pixel; i++) {
        ambient_col += RayColour(GetRayFor(x, y), settings_.max_depth_, world);
      }

      frame_buffer_.Set(
          x, y,
          static_cast<image::PixelU8>(ambient_col * pixel_samples_scale_));
    }
  }
}

math::Ray Camera::GetRayFor(int X, int Y) const {
  const auto offset = math::Vec3(math::RandomDouble(-0.5, 0.5),
                                 math::RandomDouble(-0.5, 0.5), 0.0);

  // Jitter the pixels and scale them
  const auto pixel_offset_u = (X + offset.X()) * pixel_delta_u_;
  const auto pixel_offset_v = (Y + offset.Y()) * pixel_delta_v_;

  const auto pixel_sample = pixel00_loc_ + pixel_offset_u + pixel_offset_v;

  const auto ray_direction = pixel_sample - center_;
  return {center_, ray_direction};
}

image::PixelF64 Camera::RayColour(const math::Ray& r, std::uint32_t depth,
                                  const scene::Hittable& world) {
  if (depth == 0) {
    return {0, 0, 0};
  }

  scene::HitInfo rec;
  if (world.Hit(r, math::Interval(0.001, math::kInfinity), rec)) {
    math::Ray scattered;
    image::PixelF64 attenuation;
    if (rec.material_->Scatter(r, rec, attenuation, scattered)) {
      return attenuation * RayColour(scattered, depth - 1, world);
    }

    return {0, 0, 0};
  }

  math::Vec3 unit_direction = r.direction().Unit();
  auto a = 0.5 * (unit_direction.Y() + 1.0);
  return (1.0 - a) * image::PixelF64(1.0, 1.0, 1.0) +
         a * image::PixelF64(1, 1, 1);
}

}  // namespace polaris::scene
