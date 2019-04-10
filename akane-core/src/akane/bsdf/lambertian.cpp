#include "akane/bsdf.h"

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

    private:
        Spectrum albedo_;
    };
} // namespace akane