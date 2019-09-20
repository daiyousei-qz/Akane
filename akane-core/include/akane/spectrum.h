#pragma once
#include "akane/math/math.h"
#include <algorithm>

namespace akane
{
    // color-related vector type is renamed to Spectrum for distinction
    using Spectrum = Vec3;

    constexpr Spectrum kBlackSpectrum = {0.f, 0.f, 0.f};
    constexpr Spectrum kWhiteSpectrum = {1.f, 1.f, 1.f};

    inline bool InvalidSpectrum(const Spectrum& s)
    {
        auto subzero  = s[0] < 0 || s[1] < 0 || s[2] < 0;
        auto inf_test = isinf(s[0]) || isinf(s[1]) || isinf(s[2]);
        auto nan_test = isnan(s[0]) || isnan(s[1]) || isnan(s[2]);

        return subzero || inf_test || nan_test;
    }

    inline Point3i SpectrumToRGB(const Spectrum& s)
    {
        int r = clamp(static_cast<int>(s[0] * 255.f), 0, 255);
        int g = clamp(static_cast<int>(s[1] * 255.f), 0, 255);
        int b = clamp(static_cast<int>(s[2] * 255.f), 0, 255);

        return {r, g, b};
    }

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
            float v = 1.055f * pow(u, 1.f / 2.4f) - 0.055f;
            return clamp(v, 0.f, 1.f);
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
            float v = pow((u + 0.055f) * (1.f / 1.055f), 2.4f);
            return clamp(v, 0.f, 1.f);
        }
    }

    inline Spectrum GammaCorrect(const Spectrum& s, float gamma)
    {
        return Spectrum{GammaCorrect(s[0], gamma), GammaCorrect(s[1], gamma),
                        GammaCorrect(s[2], gamma)};
    }

    inline Spectrum ToneMap_Reinhard(Spectrum color) noexcept
    {
        return {ToneMap_Reinhard(color[0]), ToneMap_Reinhard(color[1]), ToneMap_Reinhard(color[2])};
    }

    inline Spectrum ToneMap_Aces(Spectrum color) noexcept
    {
        return {ToneMap_Aces(color[0]), ToneMap_Aces(color[1]), ToneMap_Aces(color[2])};
    }

    inline Spectrum Linear2sRGB(Spectrum color) noexcept
    {
        return {Linear2sRGB(color[0]), Linear2sRGB(color[1]), Linear2sRGB(color[2])};
    }
    inline Spectrum sRGB2Linear(Spectrum color) noexcept
    {
        return {sRGB2Linear(color[0]), sRGB2Linear(color[1]), sRGB2Linear(color[2])};
    }

} // namespace akane