// facilities to map unit sampling to various dimensions

#pragma once
#include "akane/math/math.h"
#include <cmath>

namespace akane
{
    // samples a point on unit hemisphere
    inline Vec3 SampleUniformHemisphere(Point2f u) noexcept
    {
        float z   = u[0];
        float r   = sqrt(max(0.f, 1 - z * z)); // avoid float error
        float phi = 2 * kPi * u[1];

        return Vec3{r * cos(phi), r * sin(phi), z};
    }

    inline float PdfUniformHemisphere() noexcept
    {
        return 1.f / (kPi * 2.f);
    }

    // samples a point on unit sphere
    inline Vec3 SampleUniformSphere(Point2f u) noexcept
    {
        float z   = 1 - 2 * u[0];
        float r   = sqrt(max(0.f, 1 - z * z)); // avoid float error
        float phi = 2 * kPi * u[1];

        return Vec3{r * cos(phi), r * sin(phi), z};
    }

    inline float PdfUniformSphere() noexcept
    {
        return 1.f / (kPi * 4.f);
    }

    // samples a point on unit disk on xy plane
    inline Vec3 SampleUniformDisk(Point2f u) noexcept
    {
        float r   = sqrt(u[0]);
        float phi = 2 * kPi * u[1];

        return Vec3{r * cos(phi), r * sin(phi), 0};
    }

    inline float PdfUniformDisk() noexcept
    {
        return 1.f / kPi;
    }

    //
    inline Vec3 SampleCosineWeightedHemisphere(Point2f u) noexcept
    {
        float theta = u[0] * 2 * kPi;
        float r     = sqrt(u[1]);

        return Vec3(cos(theta) * r, sin(theta) * r, sqrt(1 - u[1]));
    }

    inline float PdfCosineWeightedHemisphere(float z) noexcept
    {
        return z / kPi;
    }

} // namespace akane