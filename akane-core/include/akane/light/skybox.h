#pragma once
#include "akane/light.h"

namespace akane
{
    class SkyboxLight : public Light
    {
    public:
        SkyboxLight(const Spectrum& albedo) : albedo_(albedo)
        {
        }

        Spectrum Eval(const Ray& ray) const override
        {
            auto t = .5f * (ray.d.Z() + 1);

            return (1 - t) * Spectrum{kFloatOne} + t * albedo_;
        }

        VisibilityTester SampleLi(const Point2f& u, const IntersectionInfo& isect) const override
        {
            AKANE_NO_IMPL();
        }

        akFloat Power() const override
        {
            AKANE_NO_IMPL();
        }

    private:
        Spectrum albedo_;
    };
} // namespace akane
