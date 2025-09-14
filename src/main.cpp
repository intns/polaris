#include <math/Vec.hpp>
#include <memory>
#include <scene/Camera.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Dielectric.hpp>
#include <scene/material/Lambertian.hpp>
#include <scene/material/Metal.hpp>
#include <scene/objects/Sphere.hpp>
#include <string>

using namespace polaris;

namespace {
std::shared_ptr<scene::objects::Sphere> CreateSphere(
    const math::Vec3& pos, double radius,
    const std::shared_ptr<scene::material::Material>& mat) {
  return std::make_shared<scene::objects::Sphere>(pos, radius, mat);
}
}  // namespace

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  scene::HittableList world;
  {
    using namespace scene::objects;
    using scene::material::Dielectric;
    using scene::material::Lambertian;
    using scene::material::Metal;

    // auto m1 = std::make_shared<Lambertian>(image::PixelF64(0.1, 0.2, 0.5));
    auto m2 = std::make_shared<Dielectric>(1.50);
    auto m3 = std::make_shared<Metal>(image::PixelF64(0.8, 0.8, 0.8), 0.001);

    world.Add(CreateSphere({0.0, -100.5, -1.0}, 100.0, m3));
    world.Add(CreateSphere({-1.0, 0.0, -10.2}, 2, m2));
    {
      auto pos = math::Vec3{0.0, 0.0, -10};
      auto sz = 10;
      world.Add(CreateSphere(pos, sz, m2));
    }
    world.Add(CreateSphere({1.0, 0.0, -1.0}, 0.5, m3));
  }

  scene::CameraSettings settings;
  settings.aspect_ratio = 16.0 / 9.0;
  settings.image_width = 500;
  settings.samples_per_pixel = 20;
  settings.max_depth_ = 50;
  settings.fov = 90.0;

#ifdef _WIN32
  settings.output_format_ = image::FileFormat::BMP;
#else
  settings.output_format_ = image::FileFormat::PPM;
#endif

  scene::Camera cam(settings);
  cam.Render(world);
  cam.Write("out");
  return 0;
}
