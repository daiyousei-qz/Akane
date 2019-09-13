#pragma once
#include "akane/math/vector.h"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace akane
{
    using Vec2 = Vec<float, 2>;
    using Vec3 = Vec<float, 3>;

    constexpr float kPi = 3.1415925f;

    using std::abs;
    using std::cos;
    using std::max;
    using std::min;
    using std::pow;
    using std::sin;
    using std::sqrt;

    template <typename Float, size_t N>
    inline constexpr Float Normalize(Vec<Float, N> a) noexcept
    {
        return a * (static_cast<Float>(1.f) / a.Length());
    }

    template <typename Float, size_t N>
    inline constexpr Float Distance(Vec<Float, N> a, Vec<Float, N> b) noexcept
    {
        return (a - b).Length();
    }

    template <typename Float, size_t N>
    inline constexpr Float Dot(Vec<Float, N> a, Vec<Float, N> b) noexcept
    {
        return (a * b).Sum();
    }

    template <typename Float>
    inline constexpr Vec<Float, 3> Cross(Vec<Float, 3> a, Vec<Float, 3> b) noexcept
    {
        return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
    }
} // namespace akane