#pragma once
#include "akane/math/float_type.h"
#include "akane/core.h"
#include "akane/spectrum.h"
#include <vector>

namespace akane
{
    class BxdfType
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

            //
            Any = Reflection | Transmission | Diffuse | Glossy | Specular,
        };

        constexpr BxdfType(FlagType flag) : value_(flag)
        {
            AKANE_ASSERT((value_ & 0b11 != 0) && (value_ & ~0b11 != 0));
        }
        constexpr BxdfType(int flag) : BxdfType(static_cast<FlagType>(flag))
        {
        }

        constexpr bool Contain(BxdfType flag) const noexcept
        {
            return (value_ & flag.value_) != 0;
        }

    private:
        FlagType value_;
    };

    // implementation interface for generic BSDF
    // normal vector is assumed at (0, 0, 1)
    class Bxdf
    {
    public:
        Bxdf(BxdfType type) : type_(type)
        {
        }

        BxdfType GetType() const noexcept
        {
            return type_;
        }

        // evaluate f_r(wo, wi)
        virtual Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

        virtual Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                       akFloat& pdf_out) const noexcept;

        virtual akFloat Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept;

    private:
        BxdfType type_;
    };

    // interface for bidirectional scatter distribution function
    class Bsdf
    {
    public:
        static constexpr Vec3f kBsdfNormal = Vec3f{0.f, 0.f, 1.f};

        Bsdf(std::unique_ptr<Bxdf> bxdf)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi, BxdfType type = BxdfType::Any) const
            noexcept
        {
            //AKANE_ASSERT(type == BxdfType::Any);
            return bxdf_->Eval(wo, wi);

            /*
            auto reflect = kBsdfNormal.Dot(wi) > 0;

            Spectrum f{kFloatZero};
            for (const auto& bxdf : bxdfs_)
            {
                auto item_type = bxdf->GetType();

                if (item_type.Contain(type) && item_type.Contain(BxdfType::Reflection) == reflect)
                {
                    f += bxdf->Eval(wo, wi);
                }
            }

            return f;
            */
        }

        Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out, akFloat& pdf_out,
                               BxdfType type = BxdfType::Any) const noexcept
        {
            //AKANE_ASSERT(type == BxdfType::Any);
            return bxdf_->SampleAndEval(u, wo, wi_out, pdf_out);
        }

        akFloat Pdf(const Vec3f& wo, const Vec3f& wi, BxdfType type = BxdfType::Any) const noexcept
        {
            //AKANE_ASSERT(type == BxdfType::Any);
            return bxdf_->Pdf(wo, wi);

            /*
            akFloat pdf = kFloatZero;
            for (const auto& bxdf : bxdfs_)
            {
                if (bxdf->GetType().Contain(type))
                {
                    pdf += bxdf->Pdf(wo, wi);
                }
            }

            return pdf / static_cast<akFloat>(bxdfs_.size());
            */
        }

    private:
        std::unique_ptr<Bxdf> bxdf_;
        // std::vector<std::unique_ptr<Bxdf>> bxdfs_;
    };

    class BsdfTransform
    {
    public:
        BsdfTransform(const Vec3f& n)
        {
            Vec3f yy = n.Cross(n.X() < n.Y() ? Vec3f{1.f, 0, 0} : Vec3f{0, 1.f, 0});
            Vec3f xx = yy.Cross(n);

            ss_ = xx.Normalized();
            ts_ = yy.Normalized();
            ns_ = n.Normalized();

            AKANE_ASSERT((ss_.Cross(ts_) - ns_).LengthSq() < 0.01f);
        }

        Vec3f WorldToLocal(const Vec3f& v) const noexcept
        {
            return Vec3f{v.Dot(ts_), v.Dot(ss_), v.Dot(ns_)};
        }

        Vec3f LocalToWorld(const Vec3f& v) const noexcept
        {
            auto xx = v.X() * ss_.X() + v.Y() * ts_.X() + v.Z() * ns_.X();
            auto yy = v.X() * ss_.Y() + v.Y() * ts_.Y() + v.Z() * ns_.Y();
            auto zz = v.X() * ss_.Z() + v.Y() * ts_.Z() + v.Z() * ns_.Z();

            return Vec3f{xx, yy, zz};
        }

    private:
        // BSDF local base vector
        // x, y, z, corespondingly
        // NOTE ns_ is also normal vector
        Vec3f ss_, ts_, ns_;
    };
} // namespace akane