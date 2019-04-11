#include "akane/bsdf.h"
#include "akane/math/sampling.h"

namespace akane
{
    class LambertianReflection : public Bxdf
    {
    public:
        LambertianReflection(Spectrum albedo)
            : Bxdf(BxdfType{BxdfType::Reflection | BxdfType::Diffuse}), albedo_(albedo)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            return albedo_ / kPI;
        }

		Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
			akFloat& pdf_out) const noexcept
		{
			Vec3f wi = SampleUniformHemisphere(u);

			if (GetType().Contain(BxdfType::Transmission))
			{
				wi = -wi;
			}

			wi_out = wi;
			pdf_out = PdfUniformHemisphere();
			return Eval(wo, wi);
		}

		akFloat Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept
		{
			return PdfUniformHemisphere();
		}

    private:
        Spectrum albedo_;
    };
} // namespace akane