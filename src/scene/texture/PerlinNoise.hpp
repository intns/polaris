#ifndef POLARIS_SCENE_IMAGE_TEXTURE_HPP
#define POLARIS_SCENE_IMAGE_TEXTURE_HPP

#include <algorithm>
#include <random>
#include <scene/texture/Texture.hpp>
#include <math/Interval.hpp>
#include <image/RTWImage.hpp>
#include <image/Pixel.hpp>
#include <math/Common.hpp>
#include <math/Vec.hpp>

namespace polaris::scene {
class Perlin {
public:
    Perlin() {
        for(size_t i{}; i < point_count_; ++i) {
            rand_vec_[i] = math::Vec3::RandomUnitVector();
        }

        PerlinGeneratePerm(perm_x_);
        PerlinGeneratePerm(perm_y_);
        PerlinGeneratePerm(perm_z_);
    }

    double Noise(const math::Vec3& point) const {
        auto u = point.X() - std::floor(point.X());
        auto v = point.Y() - std::floor(point.Y());
        auto w = point.Z() - std::floor(point.Z());

        auto i = static_cast<int>(std::floor(point.X()));
        auto j = static_cast<int>(std::floor(point.Y()));
        auto k = static_cast<int>(std::floor(point.Z()));
        math::Vec3 c[2][2][2];

        for(int di{}; di < 2; ++di) {
            for(int dj{}; dj < 2; ++dj) {
                for(int dk{}; dk < 2; ++dk) {
                    c[di][dj][dk] = rand_vec_[
                        perm_x_[(i + di) & 255] ^
                        perm_y_[(j + dj) & 255] ^
                        perm_z_[(k + dk) & 255] 
                    ];
                }
            }
        } 

        return PerlinInterpolation(c, u, v, w);
    }

    double Turbulence(const math::Vec3& point, int depth) const {
        auto accumulate = 0.0;
        auto temp_p = point;
        auto weight = 1.0;

        for(int i{}; i < depth; ++i) {
            accumulate += weight * Noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return std::fabs(accumulate);
    }

private:
    static void PerlinGeneratePerm(int* p)
    {
        for(int i{}; i < point_count_; ++i) {
            p[i] = i;
        }

        for(int i{}; i < point_count_ - 1; i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(p, p + point_count_, gen);
        } 
    }

    static double PerlinInterpolation(const math::Vec3 c[2][2][2], double u, double v, double w) {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accumulation = 0.0;

        for(int i{}; i < 2; ++i) {
            for(int j{}; j < 2; ++j) {
                for(int k{}; k < 2; ++k) {
                    math::Vec3 weight_v(u - i, v - j, w - k);
                    accumulation += (i * uu + (1 - i) * (1 - uu))
                                 *  (j * vv + (1 - j) * (1 - vv))
                                 *  (k * ww + (1 - k) * (1 - ww))
                                 *  c[i][j][k].Dot(weight_v);
                }
            }
        }

        return accumulation;
    }

    static const int point_count_ = 256;
    math::Vec3 rand_vec_[point_count_];
    int perm_x_[point_count_];
    int perm_y_[point_count_];
    int perm_z_[point_count_];
};
} // namespace polaris::scene

namespace polaris::scene::texture {
class NoiseTexture : public Texture {
public:
    explicit NoiseTexture(double scale) : scale_(scale) {};
    
    image::PixelF64 Value(double, double, const math::Vec3 p) const noexcept override {
        return image::PixelF64(1, 1, 1) * noise_.Turbulence(p, 7);
    }
private:
    Perlin noise_;
    double scale_;
};
} // namespace polaris::scene::texture

#endif