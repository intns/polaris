#ifndef POLARIS_SCENE_HITTABLE_HPP
#define POLARIS_SCENE_HITTABLE_HPP

#include <math/AABB.hpp>
#include <math/Interval.hpp>
#include <math/Ray.hpp>
#include <math/Vec.hpp>
#include <memory>
#include <scene/material/Material.hpp>

namespace polaris::scene {

struct HitInfo {
  math::Vec3 point_;
  math::Vec3 normal_;
  double t_ = 0.0;
  double u_ = 0.0;
  double v_ = 0.0;
  bool front_face_ = false;
  std::shared_ptr<material::Material> material_;

  void SetNormal(const math::Ray& r, const math::Vec3& outward_normal) {
    if (r.Direction().Dot(outward_normal) < 0) {
      front_face_ = true;
      normal_ = outward_normal;
    } else {
      front_face_ = false;
      normal_ = -outward_normal;
    }
  }
};

class Hittable {
 public:
  virtual ~Hittable() = default;

  [[nodiscard]] virtual bool Hit(const math::Ray& r,
                                 const math::Interval& t_interval,
                                 HitInfo& rec) const = 0;

  [[nodiscard]] virtual math::AABB GetBounds() const = 0;
};

class HittableList : public Hittable {
 public:
  HittableList() = default;
  explicit HittableList(const std::shared_ptr<Hittable>& object) { Add(object); };

  void Clear() { objects.clear(); }

  void Add(const std::shared_ptr<Hittable>& object) {
    objects.push_back(object);
    bb_ = math::AABB(bb_, object->GetBounds());
  }

  [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval& t_interval,
                         HitInfo& rec) const override {
    bool hit_anything = false;
    auto closest_so_far = t_interval.Max();

    HitInfo temp_rec;
    for (const auto& object : objects) {
      if (object->Hit(r, math::Interval(t_interval.Min(), closest_so_far),
                      temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t_;
        rec = temp_rec;
      }
    }

    return hit_anything;
  }

  [[nodiscard]] math::AABB GetBounds() const override { return bb_; }

  [[nodiscard]] const std::vector<std::shared_ptr<Hittable>>& GetObjects()
      const {
    return objects;
  }

 private:
  std::vector<std::shared_ptr<Hittable>> objects;
  math::AABB bb_;
};

}  // namespace polaris::scene

#endif
