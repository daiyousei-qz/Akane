#pragma once
#include "akane/common/basic.h"
#include <type_traits>
#include <array>

namespace akane
{
    template <typename T, size_t N> struct Point
    {
        static_assert(std::is_arithmetic_v<T>);

        std::array<T, N> data;

        constexpr Point() noexcept
        {
            data.fill({});
        }
        constexpr Point(T value) noexcept
        {
            data.fill(value);
        }
        constexpr Point(std::array<T, N> xs) noexcept
        {
            data = xs;
        }

        constexpr Point(T x, T y) : data{x, y}
        {
            static_assert(N == 2);
        }
        constexpr Point(T x, T y, T z) : data{x, y, z}
        {
            static_assert(N == 3);
        }
        constexpr Point(T x, T y, T z, T w) : data{x, y, z, w}
        {
            static_assert(N == 4);
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

        constexpr T& operator[](size_t index) noexcept
        {
            return data[index];
        }
        constexpr const T& operator[](size_t index) const noexcept
        {
            return data[index];
        }

        constexpr Point& operator=(Point other) noexcept
        {
            data = other.data;
            return *this;
        }
    };

    template <typename T, size_t N>
    inline constexpr bool operator==(Point<T, N> lhs, Point<T, N> rhs) noexcept
    {
        return lhs.data == rhs.data;
    }
    template <typename T, size_t N>
    inline constexpr bool operator!=(Point<T, N> lhs, Point<T, N> rhs) noexcept
    {
        return lhs.data != rhs.data;
    }

    template <typename T, size_t N> void to_json(nlohmann::json& j, const Point<T, N>& p)
    {
        j = nlohmann::json(p.begin(), p.end());
    }
    template <typename T, size_t N> void from_json(const nlohmann::json& j, Point<T, N>& p)
    {
        // TODO: distinguish int&float?
        AKANE_REQUIRE(j.is_array() && j.size() == N);
        for (int i = 0; i < N; ++i)
        {
            j.at(i).get_to(p[i]);
        }
    }

} // namespace akane

template <typename T, size_t N> struct fmt::formatter<akane::Point<T, N>>
{
    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        // TODO
        return ctx.end();
    }

    template <typename FormatContext> auto format(const akane::Point<T, N>& v, FormatContext& ctx)
    {
        fmt::format_to(ctx.out(), "Point({}", v[0]);
        for (int i = 1; i < N; ++i)
        {
            fmt::format_to(ctx.out(), ", {}", v[i]);
        }
        fmt::format_to(ctx.out(), ")");
    }
};