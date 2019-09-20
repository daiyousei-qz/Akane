#pragma once
#include "akane/light.h"

namespace akane
{
    class SpotLight : public Light
    {
    public:
        SpotLight(const Vec3& p, const Vec3& d, float theta, const Vec3& color, float intensity)
            : point_(p), direction_(d.Normalized()), cos_theta_(cos(theta))
        {
            radiance_ = color * intensity;
        }

        Spectrum Eval(const Ray& ray) const override
        {
            if (-Dot(ray.d, direction_) < cos_theta_)
            {
                return 0.f;
            }

            return radiance_ / (point_ - ray.o).LengthSq();
        }

        LightSample SampleLi(const Point2f& u) const override
        {
            return LightSample{point_, 0.f, 1.f};
        }

        float Power() const override
        {
            return radiance_.Length() * AreaUnitCone(cos_theta_);
        }

    private:
        Vec3 point_;
        Vec3 direction_;
        float cos_theta_;

        Spectrum radiance_; // spectrum at unit sphere around the point
    };
} // namespace akane