#pragma once
#include "akane/bsdf.h"
#include "akane/bsdf_util.h"
#include "akane/math/sampling.h"

namespace akane
{
    inline akFloat GGX_D(Vec3f wm, akFloat alpha)
    {
        auto tan2Theta = Tan2Theta(wm);
        auto cos2Theta = Cos2Theta(wm);

        auto root = alpha / (cos2Theta * (alpha * alpha + tan2Theta));

        return (root * root) / kPI;
    }

    inline akFloat Smith_G(Vec3f wo, Vec3f wi, Vec3f wm, akFloat alpha)
    {
        auto SmithG1 = [&](Vec3f v) {
            float tanTheta = abs(TanTheta(v));

            if (tanTheta == 0.f)
                return 1.f;

            if (v.Dot(wm) * CosTheta(v) <= 0)
                return 0.f;

            akFloat root = alpha * tanTheta;
            return 2.f / (1.f + sqrt(1.f + root * root));
        };

        return SmithG1(wo) * SmithG1(wi);
    }

    inline akFloat Schlick(akFloat cosine, akFloat eta_from, akFloat eta_to) noexcept
    {
        auto r0 = (eta_from - eta_to) / (eta_from + eta_to);
        r0      = r0 * r0;

        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

    // Cook–Torrance
    class MicrofacetReflection : public Bsdf
    {
    public:
        MicrofacetReflection(Spectrum albedo) : Bsdf(BsdfType::GlossyRefl), albedo_(albedo)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            auto wm = (wo + wi).Normalized();
            auto D  = GGX_D(wm, alpha_);
            auto F  = Schlick(wo.Dot(wm), eta_from_, eta_to_);
            auto G  = Smith_G(wo, wi, wm, alpha_);

            AKANE_REQUIRE(wo.Z() > 0 && wi.Z() > 0 && wm.Sum() != 0);

            return albedo_ * (D * F * G) / (4.f * CosTheta(wo), CosTheta(wi));
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                               akFloat& pdf_out) const noexcept
        {
            if (wo.Z() < 0)
            {
                pdf_out = 0.f;
                return Spectrum{kFloatZero};
            }

            auto theta = atan(alpha_ * sqrt(u[0] / (1 - u[0])));
            auto phi   = 2 * kPI * u[1];

            auto z   = cos(theta);
            auto r   = sqrt(1 - z * z);
            Vec3f wm = {r * cos(phi), r * sin(phi), z};

            auto cosine = wo.Dot(wm);
            if (cosine <= 0)
            {
                pdf_out = 0.f;
                return Spectrum{kFloatZero};
            }

            Vec3f wi;
            ComputeReflectedRay(wo, wm, wi);

            auto D = GGX_D(wm, alpha_);
            auto F = Schlick(cosine, eta_from_, eta_to_);
            auto G = Smith_G(wo, wi, wm, alpha_);

            AKANE_REQUIRE(wo.Z() > 0 && wi.Z() > 0 && wm.Sum() != 0);

            wi_out  = wi;
            pdf_out = D * CosTheta(wm) / (4 * cosine);
            return albedo_ * (D * F * G) / (4.f * CosTheta(wo), CosTheta(wi));
        }

        akFloat Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept
        {
            auto wm = (wo + wi).Normalized();
            return SameHemisphere(wo, wi) ? GGX_D(wm, alpha_) * abs(CosTheta(wm)) : kFloatZero;
        }

    private:
        akFloat alpha_    = 0.3f; // roughness
        akFloat eta_from_ = 1.f;
        akFloat eta_to_   = 3.3f;
        Spectrum albedo_;
    };
} // namespace akane