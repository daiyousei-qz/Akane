#pragma once
#include "akane/math/float_type.h"
#include <array>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <cassert>

namespace akane
{
    template <typename T, size_t N> struct Vec
    {
        static_assert(std::is_floating_point_v<T>);

        std::array<T, N> data;

        constexpr Vec() noexcept = default;
        constexpr Vec(T value) noexcept { data.fill(value); }
        constexpr Vec(std::initializer_list<T> init) noexcept
        {
            assert(init.size() <= N);

            auto iter_init = init.begin();
            for (auto& item : data)
            {
                if (iter_init != init.end())
                {
                    item = *iter_init;
                    ++iter_init;
                }
            }
        }

        constexpr T& operator[](size_t index) noexcept { return data[index]; }
        constexpr const T& operator[](size_t index) const noexcept
        {
            return data[index];
        }

        constexpr Vec& operator=(Vec other) noexcept
        {
            data = other.data;
            return *this;
        }

        constexpr auto begin() noexcept { return data.begin(); }
        constexpr auto end() noexcept { return data.end(); }

        constexpr auto begin() const noexcept { return data.begin(); }
        constexpr auto end() const noexcept { return data.end(); }

        T Sum() const noexcept
        {
            T result{};
            for (const auto& x : data) { result += x; }

            return result;
        }

        T Length() const noexcept
        {
            auto self = *this;
            return static_cast<T>(sqrt((self * self).Sum()));
        }

        Vec Normalized() const noexcept { return *this / Length(); }

        T Dot(Vec other) const noexcept
        {
            auto self = *this;
            return (self * other).Sum();
        }
    }; // namespace akane

    using Vec2f   = Vec<akFloat, 2>;
    using Vec3f   = Vec<akFloat, 3>;
    using Point2f = Vec<akFloat, 2>;
    using Point3f = Vec<akFloat, 3>;

    template <typename T, size_t N>
    inline constexpr Vec<T, N> operator+(Vec<T, N> v) noexcept
    {
        return x;
    }
    template <typename T, size_t N>
    inline constexpr Vec<T, N> operator-(Vec<T, N> v) noexcept
    {
        for (auto& x : v) { x = -x; }
        return v;
    }

// comparison op
#define DEF_COMPARISON_OP(OP)                                                  \
    template <typename T, size_t N>                                            \
    inline constexpr bool operator OP(Vec<T, N> lhs, Vec<T, N> rhs) noexcept   \
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

#define APPLY_BINARY_OPSET(F) F(+) F(-) F(*) F(/)
#define MAKE_ASSIGN_OP(OP) OP## =

// elemwise assign op
#define DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST(OP)                                \
    template <typename T, size_t N>                                            \
    inline constexpr Vec<T, N>& operator MAKE_ASSIGN_OP(OP)(                   \
        Vec<T, N>& lhs, Vec<T, N> rhs) noexcept                                \
    {                                                                          \
        for (size_t i = 0; i < N; ++i) { lhs[i] MAKE_ASSIGN_OP(OP) rhs[i]; }   \
        return lhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST)

#undef DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST

// elemwise assign op with broadcast
#define DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST(OP)                              \
    template <typename T, size_t N>                                            \
    inline constexpr Vec<T, N>& operator MAKE_ASSIGN_OP(OP)(Vec<T, N>& lhs,    \
                                                            T rhs) noexcept    \
    {                                                                          \
        for (size_t i = 0; i < N; ++i) { lhs[i] MAKE_ASSIGN_OP(OP) rhs; }      \
        return lhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST)
#undef DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST

// elemwise op
#define DEF_ELEMWISE_OP_NO_BROADCAST(OP)                                       \
    template <typename T, size_t N>                                            \
    inline constexpr Vec<T, N> operator OP(Vec<T, N> lhs,                      \
                                           Vec<T, N> rhs) noexcept             \
    {                                                                          \
        lhs MAKE_ASSIGN_OP(OP) rhs;                                            \
        return lhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_OP_NO_BROADCAST)
#undef DEF_ELEMWISE_OP_NO_BROADCAST

// elemwise op with broadcast
#define DEF_ELEMWISE_OP_WITH_BROADCAST(OP)                                     \
    template <typename T, size_t N>                                            \
    inline constexpr Vec<T, N> operator OP(Vec<T, N> lhs, T rhs) noexcept      \
    {                                                                          \
        lhs MAKE_ASSIGN_OP(OP) rhs;                                            \
        return lhs;                                                            \
    }                                                                          \
    template <typename T, size_t N>                                            \
    inline constexpr Vec<T, N> operator OP(T lhs, Vec<T, N> rhs) noexcept      \
    {                                                                          \
        rhs MAKE_ASSIGN_OP(OP) lhs;                                            \
        return rhs;                                                            \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_OP_WITH_BROADCAST)
#undef DEF_ELEMWISE_OP_NO_BROADCAST

#undef APPLY_BINARY_OPSET
#undef MAKE_ASSIGN_OP

} // namespace akane
