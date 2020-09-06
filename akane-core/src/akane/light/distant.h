#pragma once
#include "akane/light.h"
#include "akane/math/sampling.h"

namespace akane
{
    // global distant light that casting toward one direction
    class DistantLight : public Light
    {
    public:
        DistantLight(const Vec3& direction, const Spectrum& radiance, Vec3 world_center,
                     float world_radius)
            : direction_(direction), radiance_(radiance), world_center_(world_center),
              world_radius_(world_radius)
        {
        }

        Spectrum Eval(const Ray& ray) const override
        {
            float contrib = -Dot(direction_, ray.d);

            return contrib > 0 ? radiance_ * contrib : Spectrum{0.f};
        }

        LightSample SampleLi(const Point2f& u) const override
        {
            Vec3 p_border     = world_center_ + world_radius_ * SampleUniformSphere(u);
            Vec3 delta_center = world_center_ - p_border;
            if (Dot(direction_, delta_center) < 0)
            {
                p_border += 2.f * delta_center;
                delta_center = -delta_center;
            }

            auto pdf = PdfUniformHemisphere() / (world_radius_ * world_radius_);

            return LightSample{p_border, delta_center.Normalized(), pdf, true};
        }

        float Power() const override
        {
            return radiance_.Length() * kPi * world_radius_ * world_radius_;
        }

    private:
        Vec3 direction_;
        Spectrum radiance_;

        Vec3 world_center_;
        float world_radius_;
    };
} // namespace akane
