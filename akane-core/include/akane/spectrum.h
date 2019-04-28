#pragma once
#include "akane/math/float_type.h"
#include "akane/math/vector.h"
#include "akane/math/math.h"
#include <array>

namespace akane
{
    // RGB
    using RGBSpectrum = Vec3f;

    using Spectrum = RGBSpectrum;

    constexpr Spectrum kBlackSpectrum = {0, 0, 0};
    constexpr Spectrum kWhiteSpectrum = {1.f, 1.f, 1.f};

	// TODO: convert to sRGB
	inline Point3i SpectrumToRGB(const Spectrum& s)
	{
		auto rr = min(255, static_cast<int>(256.f * s[0]));
		auto gg = min(255, static_cast<int>(256.f * s[1]));
		auto bb = min(255, static_cast<int>(256.f * s[2]));
		return { rr,gg, bb };
	}

    inline Spectrum GammaCorrect(const Spectrum& s, akFloat gamma)
    {
        auto f = [&](akFloat x) { return pow(x, 1.f / gamma); };
        return Spectrum{f(s[0]), f(s[1]), f(s[2])};
    }

	inline Spectrum ToneMap_Reinhard(const Spectrum& s)
	{
		constexpr akFloat middle_gray = 1.f;
		auto color = s * middle_gray;

		return color / (1.f + color);
	}

	inline Spectrum ToneMap_Aces(const Spectrum& s)
	{
		constexpr float a = 2.51f;
		constexpr float b = 0.03f;
		constexpr float c = 2.43f;
		constexpr float d = 0.59f;
		constexpr float e = 0.14f;

		return (s * (a * s + b)) / (s * (c * s + d) + e);
	}
} // namespace akane