#pragma once
#include "akane/typedef.h"
#include <array>

namespace akane
{
	template <size_t NSample>
	class BasicSpectrum
	{

		std::array<akFloat, NSample> data;
	};

	// RGB
	using Spectrum = BasicSpectrum<3>;
}