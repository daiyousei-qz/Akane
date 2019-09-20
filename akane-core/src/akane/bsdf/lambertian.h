#pragma once
#include "akane/bsdf.h"
#include "akane/bsdf/bsdf_geometry.h"
#include "akane/spectrum.h"
#include "akane/math/sampling.h"

namespace akane
{
    class LambertianReflection : public Bsdf
    {
    public:
        LambertianReflection(Spectrum albedo) : Bsdf(BsdfType::DiffuseRefl), albedo_(albedo)
        {
        }

        Spectrum Eval(const Vec3& wo, const Vec3& wi) const noexcept override
        {
            return albedo_ / kPi;
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3& wo, Vec3& wi_out, float& pdf_out) const
            noexcept
        {
            // cosine-weighted sample from bsdf hemisphere
            Vec3 wi = SampleCosineWeightedHemisphere(u);

            // correct direction if wo comes from inside of primitive
            if (wo.Z() < 0)
            {
                wi.Z() = -wi.Z();
            }

            wi_out  = wi;
            pdf_out = PdfCosineWeightedHemisphere(abs(wi.Z()));
            return albedo_ / kPi;
        }

        float Pdf(const Vec3& wi, const Vec3& wo) const noexcept
        {
            if (!SameHemisphere(wo, wi))
            {
                return 0.f;
            }

            return PdfCosineWeightedHemisphere(abs(wi.Z()));
        }

    private:
        Spectrum albedo_;
    };
} // namespace akane