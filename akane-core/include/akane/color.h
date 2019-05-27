#pragma once
#include <cmath>
#include <algorithm>
#include "akane/math/vector.h"

namespace akane
{
    inline float GammaCorrect(float u, float gamma) noexcept
    {
        return pow(u, 1.f / gamma);
    }

    inline float ToneMap_Reinhard(float u) noexcept
    {
        constexpr float middle_gray = 1.f;

        float u_ = u * middle_gray;
        return u_ / (1.f + u_);
    }

    inline float ToneMap_Aces(float u) noexcept
    {
        constexpr float a = 2.51f;
        constexpr float b = 0.03f;
        constexpr float c = 2.43f;
        constexpr float d = 0.59f;
        constexpr float e = 0.14f;

        return (u * (a * u + b)) / (u * (c * u + d) + e);
    }

    inline float Linear2sRGB(float u) noexcept
    {
        if (u <= 0.0031308f)
        {
            return 12.92f * u;
        }
        else
        {
            auto v = 1.055f * std::pow(u, 1.f / 2.4f) - 0.055f;
            return std::clamp(v, 0.f, 1.f);
        }
    }

    inline float sRGB2Linear(float u) noexcept
    {
        if (u <= 0.04045f)
        {
            return u * (1.f / 12.92f);
        }
        else
        {
            auto v = std::pow((u + 0.055f) * (1.f / 1.055f), 2.4f);
            return std::clamp(v, 0.f, 1.f);
        }
    }

    inline Vec3f ToneMap_Reinhard(Vec3f color) noexcept
    {
        return {ToneMap_Reinhard(color[0]), ToneMap_Reinhard(color[1]), ToneMap_Reinhard(color[2])};
    }

    inline Vec3f ToneMap_Aces(Vec3f color) noexcept
    {
        return {ToneMap_Aces(color[0]), ToneMap_Aces(color[1]), ToneMap_Aces(color[2])};
    }

    inline Vec3f Linear2sRGB(Vec3f color) noexcept
    {
        return {Linear2sRGB(color[0]), Linear2sRGB(color[1]), Linear2sRGB(color[2])};
    }
    inline Vec3f sRGB2Linear(Vec3f color) noexcept
    {
        return {sRGB2Linear(color[0]), sRGB2Linear(color[1]), sRGB2Linear(color[2])};
    }
} // namespace akane