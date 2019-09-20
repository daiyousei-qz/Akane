#pragma once
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

} // namespace akane