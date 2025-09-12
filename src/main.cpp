#include <image/PPM.hpp>
#include <iostream>
#include <math/ray.hpp>

using namespace polaris;

namespace {
  
bool hit_sphere(const math::Vec3& center, double radius, const math::Ray& r) {
  math::Vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = oc.dot(r.direction());
  auto c = oc.length_squared() - radius * radius;
  auto discriminant = half_b * half_b - a * c;
  return (discriminant > 0);
}

image::PixelF64 ray_color(const math::Ray& r) {
  if (hit_sphere(math::Vec3(0, 0, -1), 0.5, r)) {
    return image::PixelF64(1, 0, 0);
  }

  math::Vec3 unit_direction = r.direction().unit_vector();
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * image::PixelF64(1.0, 1.0, 1.0) +
         a * image::PixelF64(0.5, 0.7, 1.0);
}
}  // namespace

int main(int argc, char** argv) {
  using namespace polaris;

  // Calculate image dimensions, working backwards from width in a 16:9 ratio
  constexpr auto target_aspect_ratio = 16.0 / 9.0;
  constexpr auto image_width = 400;
  int image_height = image_width / target_aspect_ratio;
  image_height = std::max(image_height, 1);  // Minimum height of 1

  const auto aspect_ratio = static_cast<double>(image_width) / image_height;

  // Camera
  auto focal_length = 1.0;
  auto viewport_height = 2.0;
  auto viewport_width = viewport_height * aspect_ratio;

  auto camera_pos = math::Vec3(0, 0, 0);

  // U = top left -> top right   | going right
  // V = top left -> bottom left | going downwards
  auto viewport_u = math::Vec3(viewport_width, 0, 0);
  auto viewport_v = math::Vec3(0, -viewport_height, 0);

  // Per-pixel offsets in U and V directions
  auto pixel_delta_u = viewport_u / image_width;
  auto pixel_delta_v = viewport_v / image_height;

  // Start at the camera position
  // Move back on Z by focal_length (to the center of the viewport plane)
  // Shift left by half of viewport_u (to the left edge)
  // Shift up by half of viewport_v (to the top edge, since viewport_v points
  // down)
  auto viewport_upper_left = camera_pos - math::Vec3(0, 0, focal_length) -
                             viewport_u / 2 - viewport_v / 2;

  auto pixel00_loc =
      viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  image::PPM ppm(image_width, image_height);
  for (int j = 0; j < image_height; j++) {
    for (int i = 0; i < image_width; i++) {
      auto pixel_center =
          pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);

      auto ray_direction = pixel_center - camera_pos;

      math::Ray r(camera_pos, ray_direction);
      ppm.Set(i, j, ray_color(r));
    }
  }

  std::ofstream f("f.ppm");
  if (f.is_open()) {
    ppm.Write(f);
  }

  return 0;
}
