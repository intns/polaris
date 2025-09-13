#include <image/PPM.hpp>
#include <iostream>
#include <math/common.hpp>
#include <math/ray.hpp>
#include <scene/camera.hpp>
#include <scene/hittable.hpp>
#include <scene/hittable_list.hpp>
#include <scene/objects/sphere.hpp>

int main(int argc, char** argv) {
  using namespace polaris;
  (void)argc;
  (void)argv;

  scene::HittableList world;
  {
    using scene::objects::Sphere;
    world.Add(std::make_shared<Sphere>(math::Vec3(0, 0, -1), 0.5));
    world.Add(std::make_shared<Sphere>(math::Vec3(0, -100.5, -1), 100));
  }

  scene::CameraSettings settings;
  settings.aspect_ratio_ = 4.0 / 3.0;
  settings.image_width_ = 500;
  settings.samples_per_pixel = 10;

  scene::Camera cam(settings);
  cam.Render(world);
  cam.Write("out.ppm");
  return 0;
}
