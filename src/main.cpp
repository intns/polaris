#include <scene/Camera.hpp>
#include <scene/Hittable.hpp>
#include <scene/objects/Sphere.hpp>
#include <string>

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  using namespace polaris;
  scene::HittableList world;
  {
    using scene::objects::Sphere;
    // Uncomment for original scene
    // world.Add(std::make_shared<Sphere>(math::Vec3(0, 0, -2), 0.5));
    // world.Add(std::make_shared<Sphere>(math::Vec3(0, -100.5, -1), 100));

    // Custom scene
    for (int i = 0; i < 10; i++) {
      auto jitterX = math::RandomDouble(-5, 5);
      auto jitterY = math::RandomDouble(-5, 5);
      auto jitterZ = math::RandomDouble(-10, -5);
      world.Add(std::make_shared<Sphere>(math::Vec3(jitterX, jitterY, jitterZ),
                                         math::RandomDouble(1.0, 5)));
    }
  }

  scene::CameraSettings settings;
  settings.aspect_ratio = 4.0 / 3.0;
  settings.image_width = 400;
  settings.samples_per_pixel = 10;
  settings.max_depth_ = 50;
  settings.output_format_ = image::FileFormat::BMP;

  scene::Camera cam(settings);
  cam.Render(world);
  cam.Write("out.bmp");
  return 0;
}
