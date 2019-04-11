#pragma once
#include "akane/math/float_type.h"
#include "akane/core.h"
#include "akane/spectrum.h"
#include <vector>
#include <any>

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

        constexpr bool ContainReflection() const noexcept
        {
            return Contain(BxdfType::Reflection);
        }
        constexpr bool ContainTransmission() const noexcept
        {
            return Contain(BxdfType::Transmission);
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

        ~Bxdf() = default;

        BxdfType GetType() const noexcept
        {
            return type_;
        }

        // evaluate f_r(wo, wi)
        virtual Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

        virtual Spectrum SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                       akFloat& pdf_out) const noexcept = 0;

        virtual akFloat Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

    private:
        BxdfType type_;
    };

    // interface for bidirectional scatter distribution function
    class Bsdf
    {
    public:
        static constexpr Vec3f kBsdfNormal = Vec3f{0.f, 0.f, 1.f};

        void Add(Bxdf* bxdf)
        {
        }

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi, BxdfType type = BxdfType::Any) const
            noexcept
        {
            auto reflect = kBsdfNormal.Dot(wi) > 0;

            Spectrum f{kFloatZero};
            for (int i = 0; i < bxdf_cnt_; ++i)
            {
                auto bxdf      = bxdf_list_[i];
                auto item_type = bxdf->GetType();

                if (item_type.Contain(type))
                {
                    if ((reflect && item_type.ContainReflection()) ||
                        (!reflect && item_type.ContainTransmission()))
                    {
                        f += bxdf->Eval(wo, wi);
                    }
                }
            }

            return f;
        }

        Spectrum SampleAndEval(akFloat u_bxdf, const Point2f& u_wi, const Vec3f& wo, Vec3f& wi_out,
                               akFloat& pdf_out, BxdfType type = BxdfType::Any) const noexcept
        {
            // AKANE_ASSERT(type == BxdfType::Any);
        }

        akFloat Pdf(const Vec3f& wo, const Vec3f& wi, BxdfType type = BxdfType::Any) const noexcept
        {
            akFloat pdf = kFloatZero;
			int counter = 0;
			for (int i = 0; i < bxdf_cnt_; ++i)
			{
				auto bxdf = bxdf_list_[i];
				auto item_type = bxdf->GetType();

				if (item_type.Contain(type))
				{
					pdf += bxdf->Pdf(wo, wi);
					counter += 1;
				}
			}

            return pdf / static_cast<akFloat>(counter);
        }

    private:
        static constexpr int kMaxBxdfCount = 4;

        int bxdf_cnt_ = 0;
        const Bxdf* bxdf_list_[kMaxBxdfCount];
    };

    class BsdfTransform
    {
    public:
        BsdfTransform(const Vec3f& n)
        {
            Vec3f hh = (n.X() == 0.f) ? Vec3f{1.f, 0, 0} : Vec3f{0, 1.f, 0};
            Vec3f yy = n.Cross(hh);
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