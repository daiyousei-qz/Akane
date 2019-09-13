#pragma once
#include "akane/common/basic.h"
#include <array>
#include <type_traits>

namespace akane
{
    template <typename Float, size_t N> struct Vec
    {
        static_assert(std::is_floating_point_v<T>);

        std::array<T, N> data;

        constexpr Vec() noexcept
        {
            data.fill({});
        }
        constexpr Vec(Float value) noexcept
        {
            data.fill(value);
        }
        constexpr Vec(std::array<Float, N> xs) noexcept
        {
            data = xs;
        }

        constexpr Vec(Float x, Float y) : data{x, y}
        {
            static_assert(N == 2);
        }
        constexpr Vec(Float x, Float y, Float z) : data{x, y, z}
        {
            static_assert(N == 3);
        }
        constexpr Vec(Float x, Float y, Float z, Float w) : data{x, y, z, w}
        {
            static_assert(N == 4);
        }

        constexpr Float& operator[](size_t index) noexcept
        {
            return data[index];
        }
        constexpr const Float& operator[](size_t index) const noexcept
        {
            return data[index];
        }

        constexpr Vec& operator=(Vec other) noexcept
        {
            data = other.data;
            return *this;
        }

        constexpr auto begin() noexcept
        {
            return data.begin();
        }
        constexpr auto end() noexcept
        {
            return data.end();
        }

        constexpr auto begin() const noexcept
        {
            return data.begin();
        }
        constexpr auto end() const noexcept
        {
            return data.end();
        }

        constexpr Float& X() noexcept
        {
            static_assert(N >= 1);
            return data[0];
        }
        constexpr Float& Y() noexcept
        {
            static_assert(N >= 2);
            return data[1];
        }
        constexpr Float& Z() noexcept
        {
            static_assert(N >= 3);
            return data[2];
        }
        constexpr Float X() const noexcept
        {
            static_assert(N >= 1);
            return data[0];
        }
        constexpr Float Y() const noexcept
        {
            static_assert(N >= 2);
            return data[1];
        }
        constexpr Float Z() const noexcept
        {
            static_assert(N >= 3);
            return data[2];
        }

        constexpr Float Max() const noexcept
        {
            Float result = data[0];
            for (const auto& x : data)
            {
                result = std::max(result, x);
            }

            return result;
        }

        constexpr Float Min() const noexcept
        {
            Float result = data[0];
            for (const auto& x : data)
            {
                result = std::min(result, x);
            }

            return result;
        }

        constexpr Float Sum() const noexcept
        {
            Float result{};
            for (const auto& x : data)
            {
                result += x;
            }

            return result;
        }

        constexpr Float LengthSq() const noexcept
        {
            auto self = *this;
            return (self * self).Sum();
        }

        Float Length() const noexcept
        {
            return static_cast<Float>(std::sqrt(LengthSq()));
        }
    };

    template <typename Float, size_t N>
    inline constexpr Vec<Float, N> operator+(Vec<Float, N> v) noexcept
    {
        return v;
    }
    template <typename Float, size_t N>
    inline constexpr Vec<Float, N> operator-(Vec<Float, N> v) noexcept
    {
        for (auto& x : v)
        {
            x = -x;
        }
        return v;
    }

    // comparison op
#define DEF_COMPARISON_OP(OP)                                                                      \
    template <typename Float, size_t N>                                                            \
    inline constexpr bool operator OP(Vec<Float, N> lhs, Vec<Float, N> rhs) noexcept               \
    {                                                                                              \
        return lhs.data OP rhs.data;                                                               \
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
#define DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST(OP)                                                    \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N>& operator MAKE_ASSIGN_OP(OP)(Vec<Float, N>& lhs,                \
                                                                Vec<Float, N> rhs) noexcept        \
    {                                                                                              \
        for (size_t i = 0; i < N; ++i)                                                             \
        {                                                                                          \
            lhs[i] MAKE_ASSIGN_OP(OP) rhs[i];                                                      \
        }                                                                                          \
        return lhs;                                                                                \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST)

#undef DEF_ELEMWISE_ASSIGN_OP_NO_BROADCAST

    // elemwise assign op with broadcast
#define DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST(OP)                                                  \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N>& operator MAKE_ASSIGN_OP(OP)(Vec<Float, N>& lhs,                \
                                                                Float rhs) noexcept                \
    {                                                                                              \
        for (size_t i = 0; i < N; ++i)                                                             \
        {                                                                                          \
            lhs[i] MAKE_ASSIGN_OP(OP) rhs;                                                         \
        }                                                                                          \
        return lhs;                                                                                \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST)
#undef DEF_ELEMWISE_ASSIGN_OP_WITH_BROADCAST

    // elemwise op
#define DEF_ELEMWISE_OP_NO_BROADCAST(OP)                                                           \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N> operator OP(Vec<Float, N> lhs, Vec<Float, N> rhs) noexcept      \
    {                                                                                              \
        lhs MAKE_ASSIGN_OP(OP) rhs;                                                                \
        return lhs;                                                                                \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_OP_NO_BROADCAST)
#undef DEF_ELEMWISE_OP_NO_BROADCAST

    // elemwise op with broadcast
#define DEF_ELEMWISE_OP_WITH_BROADCAST(OP)                                                         \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N> operator OP(Vec<Float, N> lhs, Float rhs) noexcept              \
    {                                                                                              \
        lhs MAKE_ASSIGN_OP(OP) rhs;                                                                \
        return lhs;                                                                                \
    }                                                                                              \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N> operator OP(Float lhs, Vec<Float, N> rhs) noexcept              \
    {                                                                                              \
        rhs MAKE_ASSIGN_OP(OP) lhs;                                                                \
        return rhs;                                                                                \
    }

    APPLY_BINARY_OPSET(DEF_ELEMWISE_OP_WITH_BROADCAST)
#undef DEF_ELEMWISE_OP_NO_BROADCAST

#undef APPLY_BINARY_OPSET
#undef MAKE_ASSIGN_OP

} // namespace akane