#pragma once
#include "akane/bsdf.h"

namespace akane
{
    class HybridBsdf : public Bsdf
    {
    public:
        HybridBsdf() = default;

        void Add(Bsdf* bsdf)
        {
            AKANE_REQUIRE(bsdf != nullptr);
            bsdf_list_[bsdf_n_++] = bsdf;
            type_                 = type_.Also(bsdf->GetType());
        }

        virtual Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept
        {
            auto is_reflection = SameHemisphere(wo, wi);

            int n      = 0;
            Spectrum f = kBlackSpectrum;
            for (int i = 0; i < bsdf_n_; ++i)
            {
                auto bsdf = bsdf_list_[i];
                auto type = bsdf->GetType();

                if ((is_reflection && type.ContainReflection()) ||
                    (!is_reflection && type.ContainTransmission()))
                {
                    f += bsdf_list_[i]->Eval(wo, wi);
                    n += 1;
                }
            }

            if (n == 0)
            {
                return kBlackSpectrum;
            }

            return f / static_cast<akFloat>(n);
        }

        virtual Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                       akFloat& pdf_out) const noexcept
        {
            int index        = floor(u[0] * bsdf_n_);
            auto sample_bsdf = bsdf_list_[index];
            auto u2          = Point2f{u[0] * (index + 1) - index, u[1]};

            Vec3f wi;
            akFloat pdf;
            auto f = sample_bsdf->SampleAndEval(u2, wo, wi, pdf);

            if (pdf == 0)
            {
                pdf_out = 0;
                return kBlackSpectrum;
            }

            auto is_reflection = SameHemisphere(wo, wi);
            int n              = 1;
            for (int i = 0; i < bsdf_n_; ++i)
            {
                auto bsdf = bsdf_list_[i];
                auto type = bsdf->GetType();

                if ((is_reflection && !type.ContainReflection()) ||
                    (!is_reflection && !type.ContainTransmission()) || bsdf == sample_bsdf)
                {
                    continue;
                }

                f += bsdf->Eval(wo, wi);
                pdf += bsdf->Pdf(wo, wi);
                n += 1;
            }

            wi_out  = wi;
            pdf_out = pdf / static_cast<akFloat>(n);
            return f / static_cast<akFloat>(n);
        }

        virtual akFloat Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept
        {
            auto is_reflection = SameHemisphere(wo, wi);

            int n       = 0;
            akFloat pdf = kFloatZero;
            for (int i = 0; i < bsdf_n_; ++i)
            {
                auto bsdf = bsdf_list_[i];
                auto type = bsdf->GetType();

                if ((is_reflection && type.ContainReflection()) ||
                    (!is_reflection && type.ContainTransmission()))
                {
                    pdf += bsdf_list_[i]->Pdf(wo, wi);
                    n += 1;
                }
            }

            if (n == 0)
            {
                return kFloatZero;
            }

            return pdf / static_cast<akFloat>(n);
        }

    private:
        int bsdf_n_         = 0;
        Bsdf* bsdf_list_[8] = {};
    };
} // namespace akane
