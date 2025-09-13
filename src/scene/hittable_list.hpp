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

  [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval_d& t_interval,
                         HitRecord& rec) const override {
    bool hit_anything = false;
    auto closest_so_far = t_interval.Max();

    HitRecord temp_rec;
    for (const auto& object : objects) {
      if (object->Hit(r, math::Interval_d(t_interval.Min(), closest_so_far),
                      temp_rec)) {
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