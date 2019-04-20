#pragma once
#include "akane/light.h"

namespace akane
{
	class DistantLight : public Light
	{
	public:
		DistantLight(const Vec3f& direction, const Spectrum& albedo)
			: direction_(direction), albedo_(albedo)
		{
		}

		Spectrum Eval(const Ray& ray) const override
		{
			auto contrib = -direction_.Dot(ray.d);

			return contrib > 0 ? albedo_ * contrib : Spectrum{ kFloatZero };
		}

		VisibilityTester SampleLi(const Point2f& u, const IntersectionInfo& isect) const override
		{
			AKANE_NO_IMPL();
		}

		akFloat Power() const override
		{
			AKANE_NO_IMPL();
		}

	private:
		Vec3f direction_;
		Spectrum albedo_;
	};
}