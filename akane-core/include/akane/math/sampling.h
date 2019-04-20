// facilities to map unit sampling to various dimensions

#pragma once
#include "akane/math/vector.h"
#include "akane/math/math.h"
#include <cmath>

namespace akane
{
    // samples a point on unit hemisphere
    inline Vec3f SampleUniformHemisphere(Point2f u) noexcept
    {
        auto z   = u[0];
        auto r   = sqrt(max(kFloatZero, 1 - z * z)); // avoid float error
        auto phi = 2 * kPI * u[1];

        return Vec3f{r * cos(phi), r * sin(phi), z};
    }

    inline akFloat PdfUniformHemisphere() noexcept
    {
        return 1.f / (kPI * 2.f);
    }

    // samples a point on unit sphere
    inline Vec3f SampleUniformSphere(Point2f u) noexcept
    {
        auto z   = 1 - 2 * u[0];
        auto r   = sqrt(max(0.f, 1 - z * z)); // avoid float error
        auto phi = 2 * kPI * u[1];

        return Vec3f{r * cos(phi), r * sin(phi), z};
    }

    inline akFloat PdfUniformSphere() noexcept
    {
        return 1.f / (kPI * 4.f);
    }

    //
    inline Vec3f SampleCosineWeightedHemisphere(Point2f u) noexcept
    {
        auto theta = u[0] * 2 * kPI;
        auto r     = sqrt(u[1]);

        return Vec3f(cos(theta) * r, sin(theta) * r, sqrt(1 - u[1]));
    }

    inline akFloat PdfCosineWeightedHemisphere(Vec3f v) noexcept
    {
		return v.Z() / kPI;
    }

} // namespace akane