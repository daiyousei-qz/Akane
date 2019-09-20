#pragma once
#pragma once
#include "akane/math/math.h"
#include "akane/spectrum.h"
#include <vector>

namespace akane
{
    class BsdfType
    {
    public:
        enum FlagType
        {
            None = 0,

            // major category: BRDF or BTDF
            Reflection = 1,
            Transmission = 2,

            // sub-category
            Diffuse = 4,
            Glossy = 8,
            Specular = 16,

            // quick access
            Any = Reflection | Transmission | Diffuse | Glossy | Specular,
            DiffuseRefl = Reflection | Diffuse,
            GlossyRefl = Reflection | Glossy,
            SpecularRefl = Reflection | Specular,
            SpecularTransmission = Reflection | Transmission | Specular
        };

        constexpr BsdfType(FlagType flag) : value_(flag)
        {
        }
        constexpr BsdfType(int flag) : BsdfType(static_cast<FlagType>(flag))
        {
        }

        constexpr BsdfType Also(BsdfType type) const noexcept
        {
            return BsdfType{ value_ | type.value_ };
        }

        constexpr bool Contain(BsdfType flag) const noexcept
        {
            return (value_ & flag.value_) != 0;
        }

        constexpr bool ContainReflection() const noexcept
        {
            return Contain(BsdfType::Reflection);
        }
        constexpr bool ContainTransmission() const noexcept
        {
            return Contain(BsdfType::Transmission);
        }

    private:
        FlagType value_ = None;
    };

    // implementation interface for generic BSDF
    // normal vector is assumed to be (0, 0, 1)
    class Bsdf
    {
    public:
        Bsdf() = default;
        Bsdf(BsdfType type) : type_(type)
        {
        }

        ~Bsdf() = default;

        BsdfType GetType() const noexcept
        {
            return type_;
        }

        // evaluate f_r(wo, wi)
        virtual Spectrum Eval(const Vec3& wo, const Vec3& wi) const noexcept = 0;

        // samples a wi and evaluate f_r(wo, wi)
        virtual Spectrum SampleAndEval(const Point2f& u, const Vec3& wo, Vec3& wi_out,
            float& pdf_out) const noexcept = 0;

        virtual float Pdf(const Vec3& wo, const Vec3& wi) const noexcept = 0;

    protected:
        BsdfType type_ = BsdfType::None;
    };

} // namespace akane
