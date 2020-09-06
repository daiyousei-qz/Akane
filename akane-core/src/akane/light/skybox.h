#pragma once
#include "akane/light.h"
#include "akane/math/sampling.h"

namespace akane
{
    // global distant light that casting toward one direction
    class SkyboxLight : public Light
    {
    public:
        SkyboxLight(const Spectrum& albedo, Vec3 world_center, float world_radius)
            : albedo_(albedo), world_center_(world_center), world_radius_(world_radius)
        {
        }

        Spectrum Eval(const Ray& ray) const override
        {
            auto t = .5f * (ray.d.Z() + 1);
            return (1 - t) * Spectrum{1.f} + t * albedo_;
        }

        LightSample SampleLi(const Point2f& u) const override
        {
            auto p   = world_center_ + world_radius_ * SampleUniformSphere(u);
            auto pdf = PdfUniformSphere() / (world_radius_ * world_radius_);

            return LightSample{p, {}, pdf, true};
        }

        float Power() const override
        {
            AKANE_NO_IMPL();
        }

    private:
        Spectrum albedo_;

        Vec3 world_center_;
        float world_radius_;
    };
} // namespace akane
