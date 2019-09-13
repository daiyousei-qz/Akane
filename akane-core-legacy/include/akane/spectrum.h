#pragma once
#include "akane/math/float_type.h"
#include "akane/math/vector.h"
#include "akane/math/math.h"
#include "akane/color.h"
#include <array>

namespace akane
{
    // RGB
    using RGBSpectrum = Vec3f;

    using Spectrum = RGBSpectrum;

    constexpr Spectrum kBlackSpectrum = {0, 0, 0};
    constexpr Spectrum kWhiteSpectrum = {1.f, 1.f, 1.f};

	inline bool InvalidSpectrum(const Spectrum& s)
	{
		auto subzero = s[0] < 0 || s[1] < 0 || s[2] < 0;
		auto inf_test = isinf(s[0]) || isinf(s[1]) || isinf(s[2]);
		auto nan_test = isnan(s[0]) || isnan(s[1]) || isnan(s[2]);

		return subzero || inf_test || nan_test;
	}

    inline Point3i SpectrumToRGB(const Spectrum& s)
    {
        auto rr = min(255, static_cast<int>(255.f * s[0]));
        auto gg = min(255, static_cast<int>(255.f * s[1]));
        auto bb = min(255, static_cast<int>(255.f * s[2]));
        return {rr, gg, bb};
    }

    inline Spectrum RGBToSpectrum(uint8_t r, uint8_t g, uint8_t b)
    {
        return Spectrum{static_cast<akFloat>(r) / 255.f, static_cast<akFloat>(g) / 255.f,
                        static_cast<akFloat>(b) / 255.f};
    }

    inline Spectrum GammaCorrect(const Spectrum& s, akFloat gamma)
    {
        auto f = [&](akFloat x) { return pow(x, 1.f / gamma); };
        return Spectrum{f(s[0]), f(s[1]), f(s[2])};
    }
} // namespace akane