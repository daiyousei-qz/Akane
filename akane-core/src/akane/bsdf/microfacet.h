#pragma once
#include "akane/bsdf.h"
#include "akane/bsdf/bsdf_geometry.h"
#include "akane/math/sampling.h"

namespace akane
{
    // Schlick Fresnel Approximation
    struct Fresnel
    {
    public:
        Fresnel(float etaI, float etaT)
        {
            auto r0 = (etaI - etaT) / (etaI + etaT);
            f0_ = Spectrum{ r0 * r0 };
        }
        Fresnel(Spectrum f0) : f0_(f0)
        {
        }

        Spectrum EvalOneChannel(float cos_theta_i) const noexcept
        {
            auto pow5 = [](float x) { return x * x * x * x * x; };
            return f0_[0] + (1 - f0_[0]) * pow5(1 - cos_theta_i);
        }

        Spectrum Eval(float cos_theta_i) const noexcept
        {
            auto pow5 = [](float x) { return x * x * x * x * x; };
            return f0_ + (kWhiteSpectrum - f0_) * pow5(1 - cos_theta_i);
        }

    private:
        Spectrum f0_;
    };

    // GGX Microfacet Distribution
    struct MicrofacetDistribution
    {
    public:
        MicrofacetDistribution(float roughness) : alpha_(roughness* roughness)
        {
        }

        // GGX D term
        float D(const Vec3& wh) const noexcept
        {
            auto cos2Theta = Cos2Theta(wh);

            float root;
            if (cos2Theta == 1.f)
            {
                root = 1.f / alpha_;
            }
            else
            {
                root = alpha_ / (cos2Theta * (alpha_ * alpha_ - 1) + 1);
            }

            return (root * root) / kPi;
        }

        // Smith G term
        float G(const Vec3& wo, const Vec3& wi) const noexcept
        {
            auto SmithG1 = [&](Vec3 v) {
                float tanTheta = TanTheta(v);
                float root = alpha_ * tanTheta;

                return 2.f / (1.f + sqrt(1.f + root * root));
            };

            return SmithG1(wo) * SmithG1(wi);
        }

        Vec3 SampleWh(const Point2f& u) const noexcept
        {
            float alpha2 = alpha_ * alpha_;
            float cos2Theta = (1 - u[0]) / (u[0] * (alpha2 - 1) + 1);
            float r = sqrt(1 - cos2Theta);
            float phi = u[1] * 2 * kPi;

            return Vec3{ r * cos(phi), r * sin(phi), sqrt(cos2Theta) };
        }

        float Pdf(const Vec3& wh) const noexcept
        {
            return D(wh) * AbsCosTheta(wh);
        }

    private:
        float alpha_;
    };

    // Cook Torrance
    class MicrofacetReflection : public Bsdf
    {
    public:
        MicrofacetReflection(Spectrum albedo, Fresnel* fresnel, MicrofacetDistribution* microfacet)
            : Bsdf(BsdfType::GlossyRefl), albedo_(albedo), fresnel_(fresnel),
            microfacet_(microfacet)
        {
        }

        Spectrum Eval(const Vec3& wo, const Vec3& wi) const noexcept override
        {
            if (!SameHemisphere(wo, wi))
            {
                return 0.f;
            }

            auto wh = (wo + wi).Normalized();
            auto D = microfacet_->D(wh);
            auto F = fresnel_->Eval(wh.Dot(wi));
            auto G = microfacet_->G(wo, wi);

            auto f = (D * F * G) / (4 * CosTheta(wo) * CosTheta(wi));
            return albedo_ * f;
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3& wo, Vec3& wi_out,
            float& pdf_out) const noexcept
        {
            // TODO: allow wo from inside of the primitive
            if (wo.Z() <= 0)
            {
                pdf_out = 0.f;
                return 0.f;
            }

            auto wh = microfacet_->SampleWh(u);
            auto wi = ReflectRay(wo, wh);
            if (wi.Z() <= 0)
            {
                pdf_out = 0.f;
                return 0.f;
            }

            auto D = microfacet_->D(wh);
            auto F = fresnel_->Eval(wh.Dot(wi));
            auto G = microfacet_->G(wo, wi);

            auto f = (D * F * G) / (4 * CosTheta(wo) * CosTheta(wi));
            auto pdf = microfacet_->Pdf(wh) / (4 * wh.Dot(wo));

            wi_out = wi;
            pdf_out = pdf;
            return albedo_ * f;
        }

        float Pdf(const Vec3& wo, const Vec3& wi) const noexcept
        {
            if (!SameHemisphere(wo, wi))
            {
                return 0.f;
            }

            auto wh = (wo + wi).Normalized();
            auto pdf = microfacet_->Pdf(wh) / (4 * wh.Dot(wo));

            return pdf;
        }

    private:
        Spectrum albedo_;

        Fresnel* fresnel_;
        MicrofacetDistribution* microfacet_;
    };
} // namespace akane