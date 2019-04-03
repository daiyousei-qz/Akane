// facilities to map unit sampling to various dimensions

#pragma once
#include "akane/math/geometry.h"
#include "akane/math/math.h"
#include <cmath>

namespace akane
{
    // samples a point on unit sphere
    inline Vec3f SampleUniformSphere(Point2f unit) noexcept
    {
        auto z   = 1 - 2 * unit[0];
        auto r   = sqrt(max(0.f, 1 - z * z)); // avoid float error
        auto phi = 2 * kPI * unit[1];

        return Vec3f{r * cos(phi), r * sin(phi), z};
    }

    inline akFloat PdfUniformSphere() noexcept { return 1.f / (kPI * 4.f); }

} // namespace akane