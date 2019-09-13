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
        return wo.Z() * wi.Z() > 0;
    }

    inline akFloat CosTheta(const Vec3f& v) noexcept
    {
        return v.Z();
    }
    inline akFloat Cos2Theta(const Vec3f& v) noexcept
    {
        return v.Z() * v.Z();
    }
    inline akFloat AbsCosTheta(const Vec3f& v) noexcept
    {
        return abs(v.Z());
    }
    inline akFloat Sin2Theta(const Vec3f& v) noexcept
    {
        return max(kFloatZero, kFloatOne - Cos2Theta(v));
    }

    inline akFloat SinTheta(const Vec3f& v) noexcept
    {
        return sqrt(Sin2Theta(v));
    }

    inline akFloat TanTheta(const Vec3f& v) noexcept
    {
        return SinTheta(v) / CosTheta(v);
    }

    inline akFloat Tan2Theta(const Vec3f& v) noexcept
    {
        return Sin2Theta(v) / Cos2Theta(v);
    }

    // assume SameHemiSphere(wo, n)
    inline Vec3f ReflectRay(const Vec3f& wo, const Vec3f& n) noexcept
    {
        auto h = wo.Dot(n);
        return -wo + 2 * h * n;
    }

    // assume n = +-kBsdfNormal
    inline Vec3f ReflectRayQuick(const Vec3f& wo) noexcept
    {
        return Vec3f{-wo.X(), -wo.Y(), wo.Z()};
    }

    // assume SameHemiSphere(wo, n)
    // eta = etaI / etaT, i.e. inverse of refractive index
    inline bool RefractRay(const Vec3f& wo, const Vec3f& n, akFloat eta, Vec3f& refracted) noexcept
    {
        akFloat cosThetaI  = n.Dot(wo);
        akFloat sin2ThetaI = max(0.f, 1.f - cosThetaI * cosThetaI);
        akFloat sin2ThetaT = eta * eta * sin2ThetaI;
        if (sin2ThetaT > 1)
        {
            return false;
        }

        akFloat cosThetaT = sqrt(1.f - sin2ThetaT);

        refracted = -eta * wo + (eta * cosThetaI - cosThetaT) * n;
        return true;
    }

    // eta = etaI / etaT
    inline constexpr akFloat Schlick(akFloat cos_theta, akFloat eta) noexcept
    {
        auto r0 = (eta - kFloatOne) / (eta + kFloatOne);
        r0      = r0 * r0;

        auto root  = 1 - cos_theta;
        auto root2 = root * root;
        return r0 + (1 - r0) * root2 * root;
    }

    class BsdfType
    {
    public:
        enum FlagType
        {
            None = 0,

            // major category: BRDF or BTDF
            Reflection   = 1,
            Transmission = 2,

            // sub-category
            Diffuse  = 4,
            Glossy   = 8,
            Specular = 16,

            // quick access
            Any                  = Reflection | Transmission | Diffuse | Glossy | Specular,
            DiffuseRefl          = Reflection | Diffuse,
            GlossyRefl           = Reflection | Glossy,
            SpecularRefl         = Reflection | Specular,
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
            return BsdfType{value_ | type.value_};
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
    // normal vector is assumed at (0, 0, 1)
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
        virtual Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

        // samples a wi and evaluate f_r(wo, wi)
        virtual Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                       akFloat& pdf_out) const noexcept = 0;

        virtual akFloat Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

    protected:
        BsdfType type_ = BsdfType::None;
    };

} // namespace akane