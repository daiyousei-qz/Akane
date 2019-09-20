#pragma once
#include "akane/light.h"

namespace akane
{
    class LinearBlendLight : public GlobalLight
    {
    public:
        LinearBlendLight(const Spectrum& albedo) : albedo_(albedo)
        {
        }

        Spectrum Eval(const Ray& ray) const override
        {
            auto t = .5f * (ray.d.Z() + 1);

            return (1 - t) * Spectrum{1.f} + t * albedo_;
        }

    private:
        Spectrum albedo_;
    };
} // namespace akane
