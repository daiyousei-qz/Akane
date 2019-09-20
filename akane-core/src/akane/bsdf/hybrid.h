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
            type_ = type_.Also(bsdf->GetType());
        }

        virtual Spectrum Eval(const Vec3& wo, const Vec3& wi) const noexcept
        {
            auto is_reflection = SameHemisphere(wo, wi);

            int n = 0;
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

            return f / static_cast<float>(n);
        }

        virtual Spectrum SampleAndEval(const Point2f& u, const Vec3& wo, Vec3& wi_out,
            float& pdf_out) const noexcept
        {
            static auto su = u;
            su = u;
            int index = floor(u[0] * bsdf_n_);
            auto sample_bsdf = bsdf_list_[index];
            auto u2 = Point2f{ u[0] * bsdf_n_ - index, u[1] };

            Vec3 wi;
            float pdf;
            auto f = sample_bsdf->SampleAndEval(u2, wo, wi, pdf);

            if (pdf == 0)
            {
                pdf_out = 0;
                return kBlackSpectrum;
            }

            auto is_reflection = SameHemisphere(wo, wi);
            int n = 1;
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

            wi_out = wi;
            pdf_out = pdf / static_cast<float>(n);
            return f / static_cast<float>(n);
        }

        virtual float Pdf(const Vec3& wo, const Vec3& wi) const noexcept
        {
            auto is_reflection = SameHemisphere(wo, wi);

            int n = 0;
            float pdf = 0;
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
                return 0;
            }

            return pdf / static_cast<float>(n);
        }

    private:
        int bsdf_n_ = 0;
        Bsdf* bsdf_list_[8] = {};
    };
} // namespace akane
