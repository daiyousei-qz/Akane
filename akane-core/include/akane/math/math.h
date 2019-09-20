#pragma once
#include "akane/math/point.h"
#include "akane/math/vector.h"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace akane
{
    using std::abs;
    using std::ceil;
    using std::clamp;
    using std::cos;
    using std::floor;
    using std::max;
    using std::min;
    using std::pow;
    using std::sin;
    using std::sqrt;
    using std::tan;

    using Point2i = Point<int, 2>;
    using Point3i = Point<int, 3>;
    using Point2f = Point<float, 2>;
    using Point3f = Point<float, 3>;

    using Vec2 = Vec<float, 2>;
    using Vec3 = Vec<float, 3>;

    inline constexpr Vec2 PointToVec(Point2i p) noexcept
    {
        return Vec2{static_cast<float>(p[0]), static_cast<float>(p[1])};
    }
    inline constexpr Vec3 PointToVec(Point3i p) noexcept
    {
        return Vec3{static_cast<float>(p[0]), static_cast<float>(p[1]), static_cast<float>(p[2])};
    }
    inline constexpr Vec2 PointToVec(Point2f p) noexcept
    {
        return Vec2{p[0], p[1]};
    }
    inline constexpr Vec3 PointToVec(Point3f p) noexcept
    {
        return Vec3{p[0], p[1], p[2]};
    }

    constexpr float kPi       = 3.1415925f;
    constexpr float kInvPi    = 1.f / kPi;
    constexpr float kTwoPi    = 2.f * kPi;
    constexpr float kInvTwoPi = 1.f / kTwoPi;

    constexpr float kAreaUnitSphere     = 4.f * kPi;
    constexpr float kAreaUnitHemisphere = 2.f * kPi;

    inline constexpr float AreaUnitCone(float cos_theta) noexcept
    {
        return 2.f * kPi * (1 - cos_theta);
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