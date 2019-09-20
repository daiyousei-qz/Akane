#pragma once
#include "akane/light.h"

namespace akane
{
    class PointLight : public Light
    {
    public:
        PointLight(const Vec3& p, const Vec3& color, float intensity)
            : point_(p)
        {
            radiance_ = color * intensity;
        }

        Spectrum Eval(const Ray& ray) const override
        {
            return radiance_ / (point_ - ray.o).LengthSq();
        }

        LightSample SampleLi(const Point2f& u) const override
        {
            return LightSample{point_, 0.f, 1.f};
        }

        float Power() const override
        {
            return radiance_.Length() * kAreaUnitSphere;
        }

    private:
        Vec3 point_;
        Spectrum radiance_; // spectrum at unit sphere around the point
    };
} // namespace akane