#pragma once
#include "akane/core.h"
#include "akane/spectrum.h"
#include "akane/math/math.h"
#include <memory>

namespace akane
{
    enum class TextureAddressMode
    {
        Repeat,
        Clamp,
    };

    template <TextureAddressMode AddressMode = TextureAddressMode::Repeat>
    inline float ResolveUV(float x) noexcept
    {
        if constexpr (AddressMode == TextureAddressMode::Repeat)
        {
            return x - std::floor(x);
        }
        else if (AddressMode == TextureAddressMode::Clamp)
        {
            return std::clamp(x, 0.f, 1.f);
        }
        else
        {
            static_assert(std::false_type::value);
        }
    }

    template <typename T> class BasicTexture : public Object
    {
    public:
        using Ptr       = std::unique_ptr<BasicTexture>;
        using SharedPtr = std::shared_ptr<BasicTexture>;

        virtual T Eval(float u, float v) const noexcept = 0;
    };

    using Texture2D = BasicTexture<Vec2f>;
    using Texture3D = BasicTexture<Vec3f>;

    class SolidTexture : public Texture3D
    {
    public:
        SolidTexture(const Vec3f& albedo) : albedo_(albedo)
        {
        }

        Vec3f Eval(float u, float v) const noexcept override
        {
            return albedo_;
        }

    private:
        Vec3f albedo_;
    };

    class CheckerboardTexture : public Texture3D
    {
    public:
        CheckerboardTexture(const Vec3f& t0, const Vec3f& t1, int n = 1) : t0_(t0), t1_(t1)
        {
            f_ = static_cast<float>(n) * kPI;
        }

        Spectrum Eval(float u, float v) const noexcept override
        {
            return sin(f_ * u) * sin(f_ * v) >= 0 ? t0_ : t1_;
        }

    private:
        float f_;
        Vec3f t0_, t1_;
    };

    class ImageTexture : public Texture3D
    {
    public:
        ImageTexture(const std::string& filename);

        // SampleNearest
        Vec3f Eval(float u, float v) const noexcept override
        {
            size_t x = ResolveUV<>(u) * (width_ - 1);
            size_t y = (1.f - ResolveUV<>(v)) * (height_ - 1);

            return GetPixel(x, y);
        }

    private:
        Vec3f GetPixel(size_t x, size_t y) const noexcept
        {
            auto index = y * width_ * 3 + x * 3;

            return Vec3f{data_[index], data_[index + 1], data_[index + 2]};
        }

        // TODO: store redundent texture to boost bilinear
        Vec3f SampleBilinear(float u, float v) const noexcept
        {
            auto u_ = (u - floor(u)) * width_;
            auto v_ = (ceil(v) - v) * height_;

            auto x0 = static_cast<size_t>(u_);
            auto y0 = static_cast<size_t>(v_);
            auto x1 = std::min(x0 + 1, width_ - 1);
            auto y1 = std::min(y0 + 1, height_ - 1);

            auto kx = x0 == x1 ? 1.f : u_ - x0;
            auto ky = y0 == y1 ? 1.f : v_ - y0;

            auto p00 = GetPixel(x0, y0);
            auto p01 = GetPixel(x0, y1);
            auto p10 = GetPixel(x1, y0);
            auto p11 = GetPixel(x1, y1);

            auto py0 = kx * p00 + (1.f - kx) * p01;
            auto py1 = kx * p10 + (1.f - kx) * p11;
            return ky * py0 + (1.f - ky) * py1;
        }

    private:
        size_t width_  = 0;
        size_t height_ = 0;

        std::unique_ptr<float[]> data_ = nullptr;
    };
} // namespace akane