#pragma once
#include "akane/math/float_type.h"
#include <array>
#include <algorithm>
#include <type_traits>

namespace akane
{
    template <typename T, size_t N> struct Vec
    {
        static_assert(std::is_arithmetic_v<T>);

        std::array<T, N> data;

        Vec() noexcept = default;
        Vec(T value) noexcept { std::fill_n(&data[0], N, value); }
        Vec(std::initializer_list<T> init) noexcept : data(init) {}

        T& operator[](size_t index) noexcept { return data[index]; }
        const T& operator[](size_t index) const noexcept { return data[index]; }

        Vec& operator=(Vec other) noexcept
        {
            data = other.data;
            return *this;
        }
    };

    using Vec2f   = Vec<akFloat, 2>;
    using Vec3f   = Vec<akFloat, 3>;
    using Point2f = Vec<akFloat, 2>;
    using Point3f = Vec<akFloat, 3>;

    template <typename T, size_t N>
    inline Vec<T, N> operator+(Vec<T, N> v) noexcept
    {
        return x;
    }
    template <typename T, size_t N>
    inline Vec<T, N> operator-(Vec<T, N> v) noexcept
    {
        for (auto& x : v) { x = -x; }
        return v;
    }

    // comparison op
#define DEF_COMPARISON_OP(OP)                                                  \
    template <typename T, size_t N>                                            \
    inline bool operator OP(Vec<T, N> lhs, Vec<T, N> rhs) noexcept             \
    {                                                                          \
        return lhs.data OP rhs.data;                                           \
    }

    DEF_COMPARISON_OP(==)
    DEF_COMPARISON_OP(!=)
    DEF_COMPARISON_OP(>)
    DEF_COMPARISON_OP(>=)
    DEF_COMPARISON_OP(<)
    DEF_COMPARISON_OP(<=)
#undef DEF_COMPARISON_OP

#define APPLY_BINARY_OPSET(F) F(+) F(-) F(*) F(/) F(%) F(&) F(|) F (^)

    // elemwise assign op
#define DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST(OP)                                \
    template <typename T, size_t N>                                            \
    inline Vec<T, N>& operator OP##=(Vec<T, N>& lhs, Vec<T, N> rhs) noexcept   \
    {                                                                          \
        for (size_t i = 0; i < N; ++i) { lhs[i] += rhs[i]; }                   \
        return lhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST)

#undef DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST

    // elemwise assign op with broadcast
#define DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST(OP)                              \
    template <typename T, size_t N>                                            \
    inline Vec<T, N>& operator OP##=(Vec<T, N>& lhs, T rhs) noexcept           \
    {                                                                          \
        for (size_t i = 0; i < N; ++i) { lhs[i] += rhs; }                      \
        return lhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST)
#undef DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST

    // elemwise op
#define DEF_ELEMWISE_OP_NO_BROADCAST(OP)                                       \
    template <typename T, size_t N>                                            \
    inline Vec<T, N> operator OP(Vec<T, N> lhs, Vec<T, N> rhs) noexcept        \
    {                                                                          \
        lhs += rhs;                                                            \
        return lhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_OP_NO_BROADCAST)
#undef DEF_ELEMWISE_OP_NO_BROADCAST

    // elemwise op with broadcast
#define DEF_ELEMWISE_OP_WITH_BROADCAST(OP)                                     \
    template <typename T, size_t N>                                            \
    inline Vec<T, N> operator OP(Vec<T, N> lhs, T rhs) noexcept                \
    {                                                                          \
        lhs += rhs;                                                            \
        return lhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_OP_WITH_BROADCAST)
#undef DEF_ELEMWISE_OP_NO_BROADCAST

#undef APPLY_BINARY_OPSET

} // namespace akane
