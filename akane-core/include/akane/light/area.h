#pragma once
#include "akane/light.h"

namespace akane
{
    class GeometricPrimitive;

    // diffuse area light source
    class AreaLight : public Light
    {
    public:
        AreaLight(GeometricPrimitive* primitive, Spectrum albedo);

        Spectrum Eval(const Ray& ray) const override;

        void SampleLi(const Point2f& u, const IntersectionInfo& isect,
                      Vec3f& wi, akFloat& pdf) const override;

        akFloat Power() const override;

		const GeometricPrimitive* GerPrimitive() const noexcept
		{
			return primitive_;
		}

    private:
        Spectrum albedo_;
        const GeometricPrimitive* primitive_;
    };

    inline Light::Ptr CreateAreaLight(GeometricPrimitive* primitive, Spectrum albedo)
    {
		return std::make_unique<AreaLight>(primitive, albedo);
    }
} // namespace akane