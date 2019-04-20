#pragma once
#include "akane/bsdf.h"
#include "akane/math/sampling.h"

namespace akane
{
    class LambertianReflection : public Bsdf
    {
    public:
        LambertianReflection(Spectrum albedo) : Bsdf(BsdfType::DiffuseRefl), albedo_(albedo)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            return SameHemisphere(wo, wi) ? albedo_ / kPI : Spectrum{ kFloatZero };
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                               akFloat& pdf_out) const noexcept
        {
            Vec3f wi = SampleCosineWeightedHemisphere(u);

            wi_out  = wi;
            pdf_out = PdfCosineWeightedHemisphere(wi);
            return Eval(wo, wi);
        }

        akFloat Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept
        {
            return SameHemisphere(wo, wi) ? PdfCosineWeightedHemisphere(wi) : kFloatZero;
        }

    private:
        Spectrum albedo_;
    };
} // namespace akane