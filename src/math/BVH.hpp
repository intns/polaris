#ifndef POLARIS_MATH_BVH_HPP
#define POLARIS_MATH_BVH_HPP

#include <algorithm>
#include <math/AABB.hpp>
#include <math/Common.hpp>
#include <memory>
#include <scene/Hittable.hpp>
#include <vector>

namespace polaris::math {

class BVHNode : public scene::Hittable {
 public:
  // Take vector by rvalue reference to avoid copy
  explicit BVHNode(std::vector<std::shared_ptr<scene::Hittable>>&& objects) {
    Build(objects, 0, objects.size());
  }

  explicit BVHNode(const scene::HittableList& list) {
    auto objects = list.GetObjects();
    Build(objects, 0, objects.size());
  }

  [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval& t_interval,
                         scene::HitInfo& rec) const override {
    if (!box_.Hit(r, t_interval)) return false;

    bool hit_anything = false;
    double closest_so_far = t_interval.Max();

    if (left_) {
      scene::HitInfo left_rec;
      if (left_->Hit(r, math::Interval(t_interval.Min(), closest_so_far),
                     left_rec)) {
        hit_anything = true;
        closest_so_far = left_rec.t_;
        rec = left_rec;
      }
    }

    if (right_) {
      scene::HitInfo right_rec;
      if (right_->Hit(r, math::Interval(t_interval.Min(), closest_so_far),
                      right_rec)) {
        hit_anything = true;
        rec = right_rec;
        closest_so_far = right_rec.t_;
      }
    }

    return hit_anything;
  }

  [[nodiscard]] math::AABB GetBounds() const override { return box_; }

 private:
  void Build(std::vector<std::shared_ptr<scene::Hittable>>& objects,
             size_t start, size_t end) {
    const size_t span = end - start;

    if (span == 1) {
      left_ = right_ = objects[start];
    } else if (span == 2) {
      left_ = objects[start];
      right_ = objects[start + 1];
    } else {
      // Choose axis with largest extent for better splits
      math::AABB bounds;
      for (size_t i = start; i < end; ++i) {
        bounds = math::AABB(bounds, objects[i]->GetBounds());
      }

      int axis = 0;
      auto extent = bounds.Axis(0).Size();
      for (int a = 1; a < 3; ++a) {
        auto e = bounds.Axis(a).Size();
        if (e > extent) {
          extent = e;
          axis = a;
        }
      }

      auto mid = start + span / 2;
      std::nth_element(objects.begin() + start, objects.begin() + mid,
                       objects.begin() + end,
                       [axis](const auto& a, const auto& b) {
                         return a->GetBounds().Axis(axis).Min() <
                                b->GetBounds().Axis(axis).Min();
                       });

      left_ = std::make_shared<BVHNode>(
          std::vector<std::shared_ptr<scene::Hittable>>(objects.begin() + start,
                                                        objects.begin() + mid));
      right_ = std::make_shared<BVHNode>(
          std::vector<std::shared_ptr<scene::Hittable>>(objects.begin() + mid,
                                                        objects.begin() + end));
    }

    box_ = math::AABB(left_->GetBounds(),
                      right_ ? right_->GetBounds() : left_->GetBounds());
  }

  [[nodiscard]] static bool BoxCompare(const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b, int axis_index) {
    auto a_axis_interval = a->GetBounds().Axis(axis_index);
    auto b_axis_interval = b->GetBounds().Axis(axis_index);
    return a_axis_interval.Min() < b_axis_interval.Min();
  }

  [[nodiscard]] static bool BoxXCompare(const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 0);
  }

  [[nodiscard]] static bool BoxYCompare(const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 1);
  }

  [[nodiscard]] static bool BoxZCompare(const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 2);
  }

  std::shared_ptr<scene::Hittable> left_;
  std::shared_ptr<scene::Hittable> right_;
  math::AABB box_;
};

}  // namespace polaris::math

#endif
