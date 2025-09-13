#include <image/PPM.hpp>
#include <iostream>
#include <math/common.hpp>
#include <math/ray.hpp>
#include <scene/hittable.hpp>
#include <scene/hittable_list.hpp>
#include <scene/objects/sphere.hpp>
#include <camera/camera.hpp>

using namespace polaris;

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

  camera::Camera cam;

  cam.Render(world);

  return 0;
}
