#include <math/BVH.hpp>
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

    auto material_ground =
        std::make_shared<Lambertian>(image::PixelF64(0.8, 0.8, 0.0));
    auto material_right =
        std::make_shared<Lambertian>(image::PixelF64(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<Dielectric>(2.50);
    auto material_bubble = std::make_shared<Dielectric>(1.00 / 2.0);
    auto material_center =
        std::make_shared<Metal>(image::PixelF64(0.9, 0.9, 0.9), 0.001);

    world.Add(
        CreateSphere(math::Vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.Add(CreateSphere(math::Vec3(0.0, 0.0, -1.2), 0.5, material_center));
    world.Add(CreateSphere(math::Vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.Add(CreateSphere(math::Vec3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.Add(CreateSphere(math::Vec3(1.0, 0.0, -1.0), 0.5, material_right));
  }
  world = scene::HittableList(std::make_shared<math::BVHNode>(world));

  scene::CameraSettings settings;
  settings.aspect_ratio = 16.0 / 9.0;
  settings.image_width = 1280;
  settings.samples_per_pixel = 100;
  settings.max_depth_ = 10;
  settings.fov = 70.0;

#ifdef _WIN32
  settings.output_format_ = image::FileFormat::BMP;
#else
  settings.output_format_ = image::FileFormat::PPM;
#endif

  scene::Camera cam(settings);
  cam.SetTarget({0, 0, 0}, math::Vec3{0, 0, -1});
  cam.Render(world);
  cam.Write("out");
  return 0;
}
