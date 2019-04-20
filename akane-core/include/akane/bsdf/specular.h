#pragma once
#include "akane/bsdf.h"
#include "akane/bsdf_util.h"
#include "akane/math/sampling.h"

namespace akane
{
    class SpecularReflection : public Bsdf
    {
    public:
        SpecularReflection(Spectrum albedo) : Bsdf(BsdfType::SpecularRefl), albedo_(albedo)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            return Spectrum{kFloatZero};
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                               akFloat& pdf_out) const noexcept
        {
            Vec3f wi;
            ComputeReflectedRay(wo, kBsdfNormal, wi);

            wi_out  = wi;
            pdf_out = 1.f;
            return albedo_;
        }

        akFloat Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept
        {
            return kFloatZero;
        }

    private:
        Spectrum albedo_;
    };
} // namespace akane