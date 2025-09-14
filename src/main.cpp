#include <memory>
#include <scene/Camera.hpp>
#include <scene/Hittable.hpp>
#include <scene/material/Lambertian.hpp>
#include <scene/material/Metal.hpp>
#include <scene/objects/Sphere.hpp>
#include <string>

using namespace polaris;

namespace {
std::shared_ptr<scene::objects::Sphere> CreateSphere(
    const math::Vec3& pos, double radius,
    std::shared_ptr<scene::material::Material> mat) {
  return std::make_shared<scene::objects::Sphere>(pos, radius, mat);
}
}  // namespace

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  scene::HittableList world;
  {
    using namespace scene::objects;
    using namespace scene::material;

    auto material_ground =
        std::make_shared<Lambertian>(image::PixelF64(0.8, 0.8, 0.0));
    auto material_center =
        std::make_shared<Lambertian>(image::PixelF64(0.1, 0.2, 0.5));
    auto material_left =
        std::make_shared<Metal>(image::PixelF64(0.8, 0.8, 0.8));
    auto material_right =
        std::make_shared<Metal>(image::PixelF64(0.8, 0.6, 0.2));

    world.Add(std::make_shared<Sphere>(math::Vec3(0.0, -100.5, -1.0), 100.0,
                                       material_ground));
    world.Add(std::make_shared<Sphere>(math::Vec3(0.0, 0.0, -1.2), 0.5,
                                       material_center));
    world.Add(std::make_shared<Sphere>(math::Vec3(-1.0, 0.0, -1.0), 0.5,
                                       material_left));
    world.Add(std::make_shared<Sphere>(math::Vec3(1.0, 0.0, -1.0), 0.5,
                                       material_right));
  }

  scene::CameraSettings settings;
  settings.aspect_ratio = 4.0 / 3.0;
  settings.image_width = 500;
  settings.samples_per_pixel = 50;
  settings.max_depth_ = 15;

#ifdef _WIN32
  settings.output_format_ = image::FileFormat::BMP;
#else
  settings.output_format_ = image::FileFormat::PPM;
#endif

  scene::Camera cam(settings);
  cam.Render(world);
  cam.Write("out.bmp");
  return 0;
}
