#ifndef POLARIS_SCENE_HITTABLE_LIST_HPP
#define POLARIS_SCENE_HITTABLE_LIST_HPP

#include <scene/hittable.hpp>

#include <memory>
#include <vector>


namespace polaris::scene {
class HittableList : public Hittable {
public:
  std::vector<std::shared_ptr<Hittable>> objects;

  HittableList() {}
  HittableList(std::shared_ptr<Hittable> object) { Add(object); };

  void Clear() { objects.clear(); }

  void Add(std::shared_ptr<Hittable> object) {
    objects.push_back(object);
  }

  [[nodiscard]] bool Hit(const math::Ray& r, double ray_tmin, double ray_tmax,
                         HitRecord& rec) const override {
    bool hit_anything = false;
    auto closest_so_far = ray_tmax;

    HitRecord temp_rec;
    for (const auto& object : objects) {
      if (object->Hit(r, ray_tmin, closest_so_far, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t_;
        rec = temp_rec;
      }
    }

    return hit_anything;
  }
};
} // namespace polaris::scene

#endif