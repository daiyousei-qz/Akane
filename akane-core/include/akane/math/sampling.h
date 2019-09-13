// facilities to map unit sampling to various dimensions

#pragma once
#include "akane/math/math.h"
#include <cmath>

namespace akane
{
    // samples a point on unit hemisphere
    inline Vec3 SampleUniformHemisphere(Vec2 u) noexcept
    {
        auto z = u[0];
        auto r = sqrt(max(kFloatZero, 1 - z * z)); // avoid float error
        auto phi = 2 * kPI * u[1];

        return Vec3{ r * cos(phi), r * sin(phi), z };
    }

    inline float PdfUniformHemisphere() noexcept
    {
        return 1.f / (kPI * 2.f);
    }

    // samples a point on unit sphere
    inline Vec3 SampleUniformSphere(Vec2 u) noexcept
    {
        auto z = 1 - 2 * u[0];
        auto r = sqrt(max(0.f, 1 - z * z)); // avoid float error
        auto phi = 2 * kPI * u[1];

        return Vec3{ r * cos(phi), r * sin(phi), z };
    }

    inline float PdfUniformSphere() noexcept
    {
        return 1.f / (kPI * 4.f);
    }

    //
    inline Vec3 SampleCosineWeightedHemisphere(Vec2 u) noexcept
    {
        auto theta = u[0] * 2 * kPI;
        auto r = sqrt(u[1]);

        return Vec3(cos(theta) * r, sin(theta) * r, sqrt(1 - u[1]));
    }

    inline float PdfCosineWeightedHemisphere(float z) noexcept
    {
        return z / kPI;
    }

} // namespace akane