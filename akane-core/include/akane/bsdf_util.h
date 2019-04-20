#pragma once
#include "akane/math/vector.h"

namespace akane
{
    // v: direction of incoming ray
    // n: surface normal
    // assuming v and u is normalized
    inline void ComputeReflectedRay(const Vec3f& v, const Vec3f& n, Vec3f& reflected) noexcept
    {
        reflected = v - 2.f * v.Dot(n) * n;
    }

    // v: direction of incoming ray
    // n: surface normal
    // assuming v and u is normalized
    inline bool ComputedRefractedRay(const Vec3f& v, const Vec3f& n, akFloat eta,
                                     Vec3f& refracted) noexcept
    {
        auto h            = v.Dot(n);
        auto discriminant = 1.f - eta * eta * (1.f - h * h);

        if (discriminant > 0)
        {
            refracted = eta * (v - h * n) - sqrt(discriminant) * n;
            refracted = refracted.Normalized();
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace akane