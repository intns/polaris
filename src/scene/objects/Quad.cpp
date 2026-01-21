#include <scene/objects/Quad.hpp>

namespace polaris::scene::objects {
bool Quad::Hit(const math::Ray& r, const math::Interval& t_interval,
                HitInfo& rec) const {
    auto demon = normal_.Dot(r.Direction());

    if(std::fabs(demon) < 1e-8) {
        return false;
    }

    auto t = (D_ - normal_.Dot(r.Origin())) / demon;
    if(!t_interval.Contains(t)) {
        return false;
    }

    auto intersection = r.at(t);
    math::Vec3 planar_hitpt_vector = intersection - Q_;
    auto alpha = w_.Dot(planar_hitpt_vector.Cross(v_));
    auto beta = w_.Dot(u_.Cross(planar_hitpt_vector));

    if(!IsInterior(alpha, beta, rec)) {
        return false;
    }

    rec.t_ = t;
    rec.point_ = intersection;
    rec.material_ = mat_;
    rec.SetNormal(r, normal_);

    return true;
}

bool Quad::IsInterior(double a, double b, HitInfo& rec) const {
    math::Interval unit_interval = math::Interval(0, 1);

    if(!unit_interval.Contains(a) || !unit_interval.Contains(b)) {
        return false;
    }

    rec.u_ = a;
    rec.v_ = b;
    return true;
}
} // namespace polaris::scene::objects