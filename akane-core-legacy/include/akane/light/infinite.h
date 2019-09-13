#pragma once
#include "akane/light.h"

namespace akane
{
    class InfiniteLight : public Light
    {
    public:
        InfiniteLight(const Spectrum& albedo)
            : albedo_(albedo)
        {
        }

        Spectrum Eval(const Ray& ray) const override
        {
            return albedo_;
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