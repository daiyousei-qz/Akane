#pragma once
#include "akane/light.h"

namespace akane
{
    class InfiniteLight : public Light
    {
    public:
        InfiniteLight(const Vec3f& direction, const Spectrum& albedo)
            : direction_(direction), albedo_(albedo)
        {
        }

        virtual Spectrum Eval(const Ray& ray) const
        {
            auto contrib = -direction_.Dot(ray.d);

            return contrib > 0 ? albedo_ * contrib : Spectrum{kFloatZero};
        }

        virtual void SampleLi(const Point2f& u, const IntersectionInfo& isect,
                              Vec3f& wi_out, akFloat& pdf_out) const
        {
            AKANE_NO_IMPL();
        }

        virtual akFloat Power() const
        {
            AKANE_NO_IMPL();
        }

    private:
        Vec3f direction_;
        Spectrum albedo_;
    };

    inline Light::Ptr CreateInfiniteLight(const Vec3f& direction, const Spectrum& albedo)
    {
		return std::make_unique<InfiniteLight>(direction, albedo);
    }
} // namespace akane