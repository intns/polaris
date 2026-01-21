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

#include "scene/texture/CheckerTexture.hpp"

using namespace polaris;

// unused function
// namespace {
// std::shared_ptr<scene::objects::Sphere> CreateSphere(
//     const math::Vec3& pos, double radius,
//     const std::shared_ptr<scene::material::Material>& mat) {
//   return std::make_shared<scene::objects::Sphere>(pos, radius, mat);
// }
// }  // namespace

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  scene::HittableList world;
  {
    using namespace scene::objects;
    using scene::material::Dielectric;
    using scene::material::Lambertian;
    using scene::material::Material;
    using scene::material::Metal;

    // Book cover image test

    auto checker = std::make_shared<scene::texture::CheckerTexture>(0.32, image::PixelF64(0.2, 0.3, 0.1), image::PixelF64(0.9, 0.9, 0.9));
    world.Add(std::make_shared<Sphere>(math::Vec3(0,-1000,0), 1000, std::make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
        auto choose_mat = math::RandomDouble();
        const math::Vec3 center(a + (0.9 * math::RandomDouble()), 0.2, b + (0.9 * math::RandomDouble()));

        if ((center - math::Vec3(4, 0.2, 0)).Length() > 0.9) {
          std::shared_ptr<Material> sphere_material;

          if (choose_mat < 0.8) {
            // diffuse
            auto albedo = image::PixelF64(math::RandomDouble(), math::RandomDouble(), math::RandomDouble()) * image::PixelF64(math::RandomDouble(), math::RandomDouble(), math::RandomDouble());
            sphere_material = std::make_shared<Lambertian>(albedo);
            auto center2 = center + math::Vec3(0, math::RandomDouble(0, 0.5), 0);
            world.Add(std::make_shared<Sphere>(center, center2, 0.2, sphere_material));
          } else if (choose_mat < 0.95) {
            // metal
            auto albedo = image::PixelF64(math::RandomDouble(), math::RandomDouble(), math::RandomDouble());
            auto fuzz = math::RandomDouble(0, 0.5);
            sphere_material = std::make_shared<Metal>(albedo, fuzz);
            world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
          } else {
            // glass
            sphere_material = std::make_shared<Dielectric>(1.5);
            world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
          }
        }
      }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.Add(std::make_shared<Sphere>(math::Vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(image::PixelF64(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(math::Vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(image::PixelF64(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Sphere>(math::Vec3(4, 1, 0), 1.0, material3));
  }
  world = scene::HittableList(std::make_shared<math::BVHNode>(world));

  scene::CameraSettings settings;
  settings.aspect_ratio = 16.0 / 9.0;
  settings.image_width = 1280;
  settings.samples_per_pixel = 10;
  settings.max_depth_ = 10;
  settings.fov = 20.0;
  settings.defocus_angle = 0.6;
  settings.focus_dist = 1.0;

  settings.output_format_ = image::FileFormat::PNG;

  scene::Camera cam(settings);
  cam.SetTarget(math::Vec3(13, 2, 3), math::Vec3{0, 0, 0});
  cam.Render(world);
  cam.Write("out");
  return 0;
}
