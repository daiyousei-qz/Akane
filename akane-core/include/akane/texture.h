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

        virtual Spectrum Eval(const IntersectionInfo& isect) const noexcept = 0;
    };

    class SolidTexture : public Texture
    {
    public:
        SolidTexture(const Spectrum& albedo) : albedo_(albedo)
        {
        }

        Spectrum Eval(const IntersectionInfo& isect) const noexcept override
        {
            return albedo_;
        }

    private:
        Spectrum albedo_;
    };

    class ImageTexture : public Texture
    {
    public:
        ImageTexture(const Spectrum& albedo, Image::SharedPtr img) : albedo_(albedo), img_(img)
        {
        }

        Spectrum Eval(const IntersectionInfo& isect) const noexcept override
        {
            size_t x = isect.uv.X() * (img_->Width() - 1);
            size_t y = (1 - isect.uv.Y()) * (img_->Height() - 1);

            return albedo_ * Spectrum{img_->At(x, y, 0), img_->At(x, y, 1), img_->At(x, y, 2)};
        }

    private:
        Spectrum albedo_;
        Image::SharedPtr img_;
    };

    class CheckerboardTexture : public Texture
    {
    public:
        CheckerboardTexture(const Texture* t0, const Texture* t1) : t0_(t0), t1_(t1)
        {
        }

        Spectrum Eval(const IntersectionInfo& isect) const noexcept override
        {
            auto p      = 3.f * isect.point;
            auto choice = sin(p.X()) * sin(p.Y()) * sin(p.Z());

            if (choice > 0)
            {
                return t0_->Eval(isect);
            }
            else
            {
                return t1_->Eval(isect);
            }
        }

    private:
        const Texture* t0_;
        const Texture* t1_;
    };
} // namespace akane