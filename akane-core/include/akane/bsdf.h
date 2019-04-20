#pragma once
#include "akane/math/float_type.h"
#include "akane/math/math.h"
#include "akane/core.h"
#include "akane/spectrum.h"
#include <vector>
#include <any>

namespace akane
{
    static constexpr Vec3f kBsdfNormal = Vec3f{0.f, 0.f, 1.f};

    inline bool SameHemisphere(const Vec3f& wo, const Vec3f& wi) noexcept
    {
        return wo.Z() * wi.Z() >= 0;
    }

    inline akFloat CosTheta(const Vec3f& v)
    {
        return v.Z();
    }
    inline akFloat Cos2Theta(const Vec3f& v)
    {
        return v.Z()* v.Z();
    }
    inline akFloat AbsCosTheta(const Vec3f& v)
    {
        return abs(v.Z());
    }
    inline akFloat Sin2Theta(const Vec3f& v)
    {
        return max(kFloatZero, kFloatOne - Cos2Theta(v));
    }

    inline akFloat SinTheta(const Vec3f& v)
    {
        return sqrt(Sin2Theta(v));
    }

    inline akFloat TanTheta(const Vec3f& v)
    {
        return SinTheta(v) / CosTheta(v);
    }

    inline akFloat Tan2Theta(const Vec3f& v)
    {
        return Sin2Theta(v) / Cos2Theta(v);
    }

    class BsdfType
    {
    public:
        enum FlagType
        {
            // major category: BRDF or BTDF
            Reflection   = 1,
            Transmission = 2,

            // sub-category
            Diffuse  = 4,
            Glossy   = 8,
            Specular = 16,

            // quick access
            Any          = Reflection | Transmission | Diffuse | Glossy | Specular,
            DiffuseRefl  = Reflection | Diffuse,
            GlossyRefl   = Reflection | Glossy,
            SpecularRefl = Reflection | Specular,
        };

        constexpr BsdfType(FlagType flag) : value_(flag)
        {
        }
        constexpr BsdfType(int flag) : BsdfType(static_cast<FlagType>(flag))
        {
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
        FlagType value_;
    };

    // implementation interface for generic BSDF
    // normal vector is assumed at (0, 0, 1)
    class Bsdf
    {
    public:
        Bsdf(BsdfType type) : type_(type)
        {
        }

        ~Bsdf() = default;

        BsdfType GetType() const noexcept
        {
            return type_;
        }

        // evaluate f_r(wo, wi)
        virtual Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

        // samples a wi and evaluate f_r(wo, wi)
        virtual Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                       akFloat& pdf_out) const noexcept = 0;

        virtual akFloat Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

    private:
        BsdfType type_;
    };

} // namespace akane