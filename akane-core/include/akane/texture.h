#pragma once
#include "akane/core.h"
#include "akane/spectrum.h"
#include "akane/io/image.h"
#include <memory>

namespace akane
{
    class Texture : public Object
    {
    public:
        using Ptr = std::unique_ptr<Texture>;

        virtual Spectrum Value(const IntersectionInfo& isect) const noexcept = 0;
    };

    class SolidTexture : public Texture
    {
    public:
        SolidTexture(std::vector<Spectrum> albedo) : albedo_(std::move(albedo))
        {
        }

        Spectrum Value(const IntersectionInfo& isect) const noexcept override
        {
            return albedo_[0];
        }

    private:
        std::vector<Spectrum> albedo_;
    };

    class CheckerboardTexture : public Texture
    {
    public:
        CheckerboardTexture(const Texture* t0, const Texture* t1) : t0_(t0), t1_(t1)
        {
        }

        Spectrum Value(const IntersectionInfo& isect) const noexcept override
        {
            auto p      = 3.f * isect.point;
            auto choice = sin(p.X()) * sin(p.Y()) * sin(p.Z());

            if (choice > 0)
            {
                return t0_->Value(isect);
            }
            else
            {
                return t1_->Value(isect);
            }
        }

    private:
        const Texture* t0_;
        const Texture* t1_;
    };

	class ImageTexture : public Texture
	{
	public:
		ImageTexture(Image::SharedPtr img) : img_(img)
		{
		}

		Spectrum Value(const IntersectionInfo& isect) const noexcept override
		{
			auto x = isect.uv.X() * (img_->Width() - 1);
			auto y = isect.uv.Y() * (img_->Height() - 1);
			auto pixel = img_->At(x, y);

			auto f = 1.f / 255.f;
			return Spectrum{ pixel.r * f, pixel.g * f ,pixel.b * f };
		}
	private:
		Image::SharedPtr img_;
	};
} // namespace akane