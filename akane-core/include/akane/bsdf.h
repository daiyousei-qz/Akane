#pragma once
#include "akane/math/float_type.h"
#include "akane/core.h"
#include "akane/spectrum.h"

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

        BxdfType(FlagType flag) : value_(flag)
        {
        }

        bool Contain(BxdfType flag)
        {
            return (value & flag.value_) != 0;
        }

    private:
        FlagType value_;
    };

    // implementation interface for generic BSDF
    class Bxdf
    {
		akFloat Pdf() const noexcept;
        Spectrum Sample(const Ray& wo, Ray& wi, akFloat& pdf, Point2f sample,
                        BxdfType type = BxdfType::Any) const noexcept;

    private:
        BxdfType type_;
    };

    // interface for bidirectional scatter distribution function
    class Bsdf
    {
		Spectrum Sample(const Ray& wo, Ray& wi, akFloat& pdf, Point2f sample,
			BxdfType type = BxdfType::Any) const noexcept;

        Spectrum Sample(const Ray& wo, Ray& wi, BxdfType type) const noexcept;
    };
} // namespace akane