#include <math/common.hpp>
#include <scene/camera.hpp>

namespace polaris::scene {

void Camera::Initialise() {
  const auto image_width = settings_.image_width_;

  image_height_ = static_cast<int>(image_width / settings_.aspect_ratio_);
  image_height_ = std::max(image_height_, 1);

  ppm_.Resize(image_width, image_height_);

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
  std::ofstream f(filename);
  if (!f.is_open()) {
    return;
  }

  ppm_.Write(f);
}

void Camera::Render(const Hittable& world) {
  for (int y = 0; y < image_height_; y++) {
    for (int x = 0; x < settings_.image_width_; x++) {
      auto pixel_center =
          pixel00_loc_ + (x * pixel_delta_u_) + (y * pixel_delta_v_);

      auto ray_direction = pixel_center - center_;

      // Iteratively sample the pixel colour
      image::PixelF64 pixel(0.0, 0.0, 0.0);
      for (std::uint32_t i = 0; i < settings_.samples_per_pixel; i++) {
        pixel += RayColour(GetRayFor(x, y), world);
      }

      pixel *= pixel_samples_scale_;

      ppm_.Set(x, y, pixel);
    }
  }
}

math::Ray Camera::GetRayFor(int x, int y) const {
  const auto offset = math::Vec3(math::RandomDouble(-0.5, 0.5),
                                 math::RandomDouble(-0.5, 0.5), 0.0);

  // Jitter the pixels and scale them
  const auto pixel_offset_u = (x + offset.x()) * pixel_delta_u_;
  const auto pixel_offset_v = (y + offset.y()) * pixel_delta_v_;

  const auto pixel_sample = pixel00_loc_ + pixel_offset_u + pixel_offset_v;

  const auto ray_direction = pixel_sample - center_;
  return {center_, ray_direction};
}

image::PixelF64 Camera::RayColour(const math::Ray& r,
                                  const scene::Hittable& world) {
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
}

}  // namespace polaris::scene
