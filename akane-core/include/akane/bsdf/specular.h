#pragma once
#include "akane/bsdf.h"
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
                               akFloat& pdf_out) const noexcept override
        {
            wi_out  = ReflectRayQuick(wo);
            pdf_out = 1.f;
            return albedo_ / AbsCosTheta(wi_out); // compensate for angle of incidence
        }

        akFloat Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept override
        {
            return kFloatZero;
        }

    private:
        Spectrum albedo_;
    };

    class SpecularTransmission : public Bsdf
    {
    public:
        SpecularTransmission(Spectrum albedo, akFloat eta_in, akFloat eta_out)
            : Bsdf(BsdfType::SpecularTransmission), albedo_(albedo), eta_in_(eta_in),
              eta_out_(eta_out)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            return Spectrum{kFloatZero};
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                               akFloat& pdf_out) const noexcept override
        {
            auto entering = wo.Z() > 0;
            auto eta      = entering ? eta_out_ / eta_in_ : eta_in_ / eta_out_;
            auto n        = entering ? kBsdfNormal : -kBsdfNormal;

            akFloat reflect_ratio;
            Vec3f refracted;
            if (RefractRay(wo, n, eta, refracted))
            {
                reflect_ratio = Schlick(abs(CosTheta(wo)), eta);
            }
            else
            {
                reflect_ratio = kFloatOne;
            }

            wi_out  = (u[0] < reflect_ratio) ? ReflectRayQuick(wo) : refracted;
            pdf_out = kFloatOne;
            return albedo_ / AbsCosTheta(wi_out); // compensate for angle of incidence
        }

        akFloat Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept override
        {
            return kFloatZero;
        }

    private:
        akFloat eta_in_;
        akFloat eta_out_;
        Spectrum albedo_;
    };
} // namespace akane