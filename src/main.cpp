#include <image/PPM.hpp>
#include <iostream>
#include <math/common.hpp>
#include <math/ray.hpp>
#include <scene/camera.hpp>
#include <scene/hittable.hpp>
#include <scene/hittable_list.hpp>
#include <scene/objects/sphere.hpp>

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

    for (int i = 0; i < 100; i++) {
      auto jitterX = math::RandomDouble(-5, 5);
      auto jitterY = math::RandomDouble(-5, 5);
      auto jitterZ = math::RandomDouble(-10, -5);
      world.Add(std::make_shared<Sphere>(math::Vec3(jitterX, jitterY, jitterZ),
                                         math::RandomDouble()));
    }
  }

  scene::CameraSettings settings;
  settings.aspect_ratio_ = 4.0 / 3.0;
  settings.image_width_ = 1000;
  settings.samples_per_pixel = 50;
  settings.max_depth_ = 100;

  scene::Camera cam(settings);
  cam.Render(world);
  cam.Write("out.ppm");
  return 0;
}
