#pragma once
#include "akane/bsdf.h"
#include "akane/math/sampling.h"

namespace akane
{
    // Schlick Fresnel Approximation
    struct Fresnel
    {
    public:
        Fresnel(akFloat etaI, akFloat etaT)
        {
            auto r0 = (etaI - etaT) / (etaI + etaT);
            f0_     = Spectrum{r0 * r0};
        }
        Fresnel(Spectrum f0) : f0_(f0)
        {
        }

        Spectrum EvalOneChannel(akFloat cos_theta_i) const noexcept
        {
            auto pow5 = [](akFloat x) { return x * x * x * x * x; };
            return f0_[0] + (kFloatOne - f0_[0]) * pow5(1 - cos_theta_i);
        }

        Spectrum Eval(akFloat cos_theta_i) const noexcept
        {
            auto pow5 = [](akFloat x) { return x * x * x * x * x; };
            return f0_ + (kWhiteSpectrum - f0_) * pow5(1 - cos_theta_i);
        }

    private:
        Spectrum f0_;
    };

    // GGX Microfacet Distribution
    struct MicrofacetDistribution
    {
    public:
        MicrofacetDistribution(akFloat roughness) : alpha_(roughness * roughness)
        {
        }

        // GGX D term
        akFloat D(const Vec3f& wh) const noexcept
        {
            auto cos2Theta = Cos2Theta(wh);

            akFloat root;
            if (cos2Theta == 1.f)
            {
                root = 1.f / alpha_;
            }
            else
            {
                root = alpha_ / (cos2Theta * (alpha_ * alpha_ - 1) + 1);
            }

            return (root * root) / kPI;
        }

        // Smith G term
        akFloat G(const Vec3f& wo, const Vec3f& wi) const noexcept
        {
            auto SmithG1 = [&](Vec3f v) {
                akFloat tanTheta = TanTheta(v);
                akFloat root     = alpha_ * tanTheta;

                return 2.f / (1.f + sqrt(1.f + root * root));
            };

            return SmithG1(wo) * SmithG1(wi);
        }

        Vec3f SampleWh(const Point2f& u) const noexcept
        {
            akFloat alpha2    = alpha_ * alpha_;
            akFloat cos2Theta = (1 - u[0]) / (u[0] * (alpha2 - 1) + 1);
            akFloat r         = sqrt(1 - cos2Theta);
            akFloat phi       = u[1] * 2 * kPI;

            return Vec3f{r * cos(phi), r * sin(phi), sqrt(cos2Theta)};
        }

        akFloat Pdf(const Vec3f& wh) const noexcept
        {
            return D(wh) * AbsCosTheta(wh);
        }

    private:
        akFloat alpha_;
    };

    // Cook–Torrance
    class MicrofacetReflection : public Bsdf
    {
    public:
        MicrofacetReflection(Spectrum albedo, Fresnel* fresnel, MicrofacetDistribution* microfacet)
            : Bsdf(BsdfType::GlossyRefl), albedo_(albedo), fresnel_(fresnel),
              microfacet_(microfacet)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override;

        Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                               akFloat& pdf_out) const noexcept;

        akFloat Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept;

    private:
        Spectrum albedo_;

        Fresnel* fresnel_;
        MicrofacetDistribution* microfacet_;
    };
} // namespace akane