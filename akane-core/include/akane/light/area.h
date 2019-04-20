#pragma once
#include "akane/light.h"

namespace akane
{
    class Primitive;

    // diffuse area light source
    class AreaLight : public Light
    {
    public:
        AreaLight(Primitive* primitive, Spectrum albedo);

        Spectrum Eval(const Ray& ray) const override;

        VisibilityTester SampleLi(const Point2f& u, const IntersectionInfo& isect) const override;

        akFloat Power() const override;

        const Primitive* GerPrimitive() const noexcept
        {
            return primitive_;
        }

    private:
        Spectrum albedo_;
        const Primitive* primitive_;
    };

    inline Light::Ptr CreateAreaLight(Primitive* primitive, Spectrum albedo)
    {
        return std::make_unique<AreaLight>(primitive, albedo);
    }
} // namespace akane