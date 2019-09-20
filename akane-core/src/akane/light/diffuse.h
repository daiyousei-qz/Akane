#pragma once
#include "akane/light.h"

namespace akane
{
    class DiffuseAreaLight : public AreaLight
    {
    public:
        DiffuseAreaLight(Primitive* object, Vec3 color, float intensity) : AreaLight(object)
        {
            radiance_ = color * intensity / kPi;
        }

        Spectrum Eval(const Ray& ray) const override
        {
            return radiance_;
        }

        LightSample SampleLi(const Point2f& u) const override
        {
            Vec3 point;
            Vec3 normal;
            float pdf;
            GetObject()->SamplePoint(u, point, normal, pdf);

            return LightSample{point, normal, pdf};
        }

        float Power() const override
        {
            return radiance_.Length() * kPi * GetObject()->Area();
        }

    private:
        Vec3 radiance_;
    };
} // namespace akane