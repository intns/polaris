#include <algorithm>
#include <execution>
#include <filesystem>
#include <math/Common.hpp>
#include <scene/Camera.hpp>
#include <scene/material/Material.hpp>
#include <thread>
#include <vector>

namespace polaris::scene {

Camera::Camera(const CameraSettings& settings) : settings_(settings) {
  const auto image_width = settings_.image_width;
  image_height_ = static_cast<int>(image_width / settings_.aspect_ratio);
  image_height_ = std::max(image_height_, 1);
  frame_buffer_.Assign(settings_.output_format_, image_width, image_height_);

  pixel_samples_scale_ = 1.0 / settings_.samples_per_pixel;

  SetTarget({0, 0, 0}, math::Vec3{0, 0, -1});
}

void Camera::SetTarget(const math::Vec3& pos,
                       std::optional<math::Vec3> opt_lookat) {
  lookat_ = opt_lookat.has_value() ? opt_lookat.value() : math::Vec3(0, 0, -1);
  position_ = pos;
  center_ = position_;

  // Determine viewport dimensions.
  auto focal_length = (position_ - lookat_).Length();
  auto theta = math::DegreesToRadians(settings_.fov);
  auto h = std::tan(theta / 2);
  auto viewport_height = 2 * h * focal_length;
  auto viewport_width =
      viewport_height *
      (static_cast<double>(settings_.image_width) / image_height_);

  w = (position_ - lookat_).Unit();
  u = up_.Cross(w).Unit();
  v = w.Cross(u);

  auto viewport_u = viewport_width * u;
  auto viewport_v = viewport_height * -v;

  // Per-pixel offsets in U and V directions
  pixel_delta_u_ = viewport_u / settings_.image_width;
  pixel_delta_v_ = viewport_v / image_height_;

  auto viewport_upper_left =
      center_ - (focal_length * w) - viewport_u / 2 - viewport_v / 2;

  pixel00_loc_ = viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
}

void Camera::Render(const Hittable& world) {
  const int tile = std::max(1, settings_.tile_size);
  const int width = settings_.image_width;
  const int height = image_height_;

  struct Tile {
    int x0, y0, x1, y1;
  };

  std::vector<Tile> tiles;
  for (int y = 0; y < height; y += tile) {
    for (int x = 0; x < width; x += tile) {
      tiles.push_back(
          {x, y, std::min(x + tile, width), std::min(y + tile, height)});
    }
  }

  std::atomic<size_t> next_tile{0};

  std::vector<std::jthread> threads;
  for (size_t t = 0; t < std::jthread::hardware_concurrency(); ++t) {
    threads.emplace_back([&, seed = std::random_device{}() + t] {
      thread_local std::mt19937 rng(seed);

      size_t idx;
      while ((idx = next_tile.fetch_add(1)) < tiles.size()) {
        const auto& [x0, y0, x1, y1] = tiles[idx];
        RenderTile(x0, y0, x1, y1, world, rng);
      }
    });
  }
}

void Camera::RenderTile(int x0, int y0, int x1, int y1, const Hittable& world,
                        std::mt19937& rng) {
  std::uniform_real_distribution<> dist(0.0, 1.0);
  const int sqrt_spp = std::sqrt(settings_.samples_per_pixel);
  const double inv_sqrt_spp = 1.0 / sqrt_spp;
  const double inv_width = 1.0 / (settings_.image_width - 1);
  const double inv_height = 1.0 / (image_height_ - 1);

  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      image::PixelF64 color{};

      // Stratified sampling
      for (int sy = 0; sy < sqrt_spp; ++sy) {
        for (int sx = 0; sx < sqrt_spp; ++sx) {
          auto u = (x + (sx + dist(rng)) * inv_sqrt_spp) * inv_width;
          auto v = (y + (sy + dist(rng)) * inv_sqrt_spp) * inv_height;
          color += RayColour(GetRayFor(u, v), settings_.max_depth_, world);
        }
      }

      frame_buffer_.Set(
          x, y, static_cast<image::PixelU8>(color * pixel_samples_scale_));
    }
  }
}

void Camera::Write(const std::string& filename) {
  std::filesystem::path file_path(filename);
  std::ios_base::openmode file_mode = std::ios::out;

  switch (settings_.output_format_) {
    case image::FileFormat::BMP:
      file_path.replace_extension(".bmp");
      file_mode |= std::ios::binary;
      break;
    case image::FileFormat::PPM:
      file_path.replace_extension(".ppm");
      break;
    default:
      return;  // Unsupported format
  }

  std::ofstream f(file_path, file_mode);
  if (!f.is_open()) {
    return;
  }

  frame_buffer_.Write(f);
}

math::Ray Camera::GetRayFor(double u_norm, double v_norm) const {
  const double px = u_norm * (settings_.image_width - 1);
  const double py = v_norm * (image_height_ - 1);

  const auto pixel_offset_u = px * pixel_delta_u_;
  const auto pixel_offset_v = py * pixel_delta_v_;

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
  // Blue-ish sky gradient from white at the horizon to light blue at the top
  return (1.0 - a) * image::PixelF64(1.0, 1.0, 1.0) +
         a * image::PixelF64(0.5, 0.7, 1.0);
}

}  // namespace polaris::scene
