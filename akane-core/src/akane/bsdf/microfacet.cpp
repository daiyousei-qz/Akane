#include "akane/bsdf/microfacet.h"

namespace akane
{
    static akFloat GGX_D(const Vec3f& wm, akFloat alpha)
    {
        auto cos2Theta = Cos2Theta(wm);

        akFloat root;
        if (cos2Theta == 1.f)
        {
            root = 1.f / alpha;
        }
        else
        {
            root = alpha / (cos2Theta * (alpha * alpha - 1) + 1);
        }

        return (root * root) / kPI;
    }

    static akFloat Smith_G(const Vec3f& wo, const Vec3f& wi, akFloat alpha)
    {
        auto SmithG1 = [&](Vec3f v) {
            akFloat tanTheta = TanTheta(v);
            akFloat root     = alpha * tanTheta;

            return 2.f / (1.f + sqrt(1.f + root * root));
        };

        return SmithG1(wo) * SmithG1(wi);
    }

    Spectrum MicrofacetReflection::Eval(const Vec3f& wo, const Vec3f& wi) const noexcept
    {
        if (!SameHemisphere(wo, wi))
        {
            return kBlackSpectrum;
        }

		auto wh = (wo + wi).Normalized();
		auto D = microfacet_->D(wh);
		auto F = fresnel_->Eval(wh.Dot(wi));
		auto G = microfacet_->G(wo, wi);

		auto f = (D * F * G) / (4 * CosTheta(wo) * CosTheta(wi));

        return albedo_ * f;
    }

    Spectrum MicrofacetReflection::SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                                 akFloat& pdf_out) const noexcept
    {
        if (wo.Z() < 0)
        {
            pdf_out = 0.f;
            return Spectrum{kFloatZero};
        }

        auto wh = microfacet_->SampleWh(u);
        auto wi = ReflectRay(wo, wh);
        if (wi.Z() < 0)
        {
            pdf_out = 0.f;
            return kBlackSpectrum;
        }

        auto D = microfacet_->D(wh);
        auto F = fresnel_->Eval(wh.Dot(wi));
        auto G = microfacet_->G(wo, wi);

        auto f   = (D * F * G) / (4 * CosTheta(wo) * CosTheta(wi));
        auto pdf = microfacet_->Pdf(wh) / (4 * wh.Dot(wi));

        wi_out  = wi;
        pdf_out = pdf;
        return albedo_ * f;
    }

    akFloat MicrofacetReflection::Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept
    {
        if (!SameHemisphere(wo, wi))
        {
            return kFloatZero;
        }

        auto wh = (wo + wi).Normalized();
        return microfacet_->Pdf(wh) / (4 * wh.Dot(wi));
    }
} // namespace akane