#ifndef POLARIS_SCENE_OBJECTS_QUAD_HPP
#define POLARIS_SCENE_OBJECTS_QUAD_HPP

#include <scene/Hittable.hpp>
#include <math/AABB.hpp>
#include <scene/material/Material.hpp>
#include <utility>
#include <math/Vec.hpp>

namespace polaris::scene::objects {
class Quad : public Hittable {
public:
    Quad(const math::Vec3& q, const math::Vec3& u, const math::Vec3 v, std::shared_ptr<material::Material> mat)
    : Q_(q), u_(u), v_(v), mat_(std::move(mat))
    {
        auto n = u.Cross(v);
        normal_ = n.Normalized();
        D_ = normal_.Dot(Q_);
        w_ = n / n.Dot(n);
        SetBoundingBox();
    }

    virtual void SetBoundingBox() {
       auto bbox_diagonal_one = math::AABB(Q_, Q_ + u_ + v_); 
       auto bbox_diagonal_two = math::AABB(Q_ + u_, Q_ + v_);
       bb_ = math::AABB(bbox_diagonal_one, bbox_diagonal_two); 
    }

    [[nodiscard]] bool Hit(const math::Ray& r, const math::Interval& t_interval,
                         HitInfo& rec) const override;

    [[nodiscard]] virtual bool IsInterior(double a, double b, HitInfo& rec) const;
    
    [[nodiscard]] math::AABB GetBounds() const override { return bb_; }

private:
    math::Vec3 Q_;
    math::Vec3 u_;
    math::Vec3 v_;
    math::Vec3 w_;
    std::shared_ptr<material::Material> mat_;
    math::AABB bb_;
    math::Vec3 normal_;
    double D_;
};
} // namespace polaris::scene::objects

#endif