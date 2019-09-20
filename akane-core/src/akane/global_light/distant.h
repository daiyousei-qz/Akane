#pragma once
#include "akane/light.h"

namespace akane
{
    // global distant light that casting toward one direction
    class DistantLight : public GlobalLight
    {
    public:
        DistantLight(const Vec3& direction, const Spectrum& color)
            : direction_(direction), color_(color)
        {
        }

        Spectrum Eval(const Ray& ray) const override
        {
            float contrib = -Dot(direction_, ray.d);

            return contrib > 0 ? color_ * contrib : Spectrum{ 0.f };
        }

    private:
        Vec3 direction_;
        Spectrum color_;
    };
}