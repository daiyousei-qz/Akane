#pragma once
#include "akane/math/float_type.h"
#include <array>

namespace akane
{
    template <typename T, size_t N> struct Vec
    {
        std::array<T, N> data;
    };

    using Vec2f   = Vec<akFloat, 2>;
    using Vec3f   = Vec<akFloat, 3>;
    using Point2f = Vec<akFloat, 2>;
    using Point3f = Vec<akFloat, 3>;

    template <typename T, size_t N>
    Vec<T, N>& operator+=(Vec<T, N>& lhs, Vec<T, N> rhs)
    {
        for (int i = 0; i < N; ++i) { lhs.data[i] += rhs.data[i]; }
        return lhs;
    }
    template <typename T, size_t N>
    Vec<T, N>& operator-=(Vec<T, N>& lhs, Vec<T, N> rhs)
    {
        for (int i = 0; i < N; ++i) { lhs.data[i] -= rhs.data[i]; }
        return lhs;
    }
    template <typename T, size_t N>
    Vec<T, N>& operator*=(Vec<T, N>& lhs, Vec<T, N> rhs)
    {
        for (int i = 0; i < N; ++i) { lhs.data[i] *= rhs.data[i]; }
        return lhs;
    }
    template <typename T, size_t N>
    Vec<T, N>& operator+=(Vec<T, N>& lhs, Vec<T, N> rhs)
    {
        for (int i = 0; i < N; ++i) { lhs.data[i] /= rhs.data[i]; }
        return lhs;
    }


} // namespace akane
