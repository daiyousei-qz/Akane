#pragma once
#include "akane/bsdf.h"
#include "akane/bsdf/bsdf_geometry.h"
#include "akane/math/sampling.h"

namespace akane
{
    class SpecularReflection : public Bsdf
    {
    public:
        SpecularReflection(Spectrum albedo) : Bsdf(BsdfType::SpecularRefl), albedo_(albedo)
        {
        }

        Spectrum Eval(const Vec3& wo, const Vec3& wi) const noexcept override
        {
            return Spectrum{0.f};
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3& wo, Vec3& wi_out, float& pdf_out) const
            noexcept override
        {
            wi_out  = ReflectRayQuick(wo);
            pdf_out = 1.f;
            return albedo_; // compensate for angle of incidence
        }

        float Pdf(const Vec3& wi, const Vec3& wo) const noexcept override
        {
            return 0;
        }

    private:
        Spectrum albedo_;
    };

    // TODO: refine this
    class SpecularTransmission : public Bsdf
    {
    public:
        SpecularTransmission(Spectrum albedo, float eta_in, float eta_out)
            : Bsdf(BsdfType::SpecularTransmission), albedo_(albedo), eta_in_(eta_in),
              eta_out_(eta_out)
        {
        }

        Spectrum Eval(const Vec3& wo, const Vec3& wi) const noexcept override
        {
            return Spectrum{0.f};
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3& wo, Vec3& wi_out, float& pdf_out) const
            noexcept override
        {
            auto entering = wo.Z() > 0;
            auto eta      = entering ? eta_out_ / eta_in_ : eta_in_ / eta_out_;
            auto n        = entering ? kBsdfNormal : -kBsdfNormal;

            float reflect_ratio;
            Vec3 refracted;
            if (RefractRay(wo, n, eta, refracted))
            {
                reflect_ratio = Schlick(abs(CosTheta(wo)), eta);
            }
            else
            {
                reflect_ratio = 1;
            }

            wi_out  = (u[0] < reflect_ratio) ? ReflectRayQuick(wo) : refracted;
            pdf_out = 1;
            return albedo_ / AbsCosTheta(wi_out); // compensate for angle of incidence
        }

        float Pdf(const Vec3& wi, const Vec3& wo) const noexcept override
        {
            return 0;
        }

    private:
        float eta_in_;
        float eta_out_;
        Spectrum albedo_;
    };
} // namespace akane