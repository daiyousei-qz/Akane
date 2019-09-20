#pragma once
#include "akane/texture.h"

namespace akane
{
    class ImageTexture : public Texture3D
    {
    public:
        ImageTexture(const std::string& filename);

        Vec3 Eval(float u, float v) const noexcept override
        {
            return SampleNearest(u, v);
        }

    private:
        Vec3 GetPixel(size_t x, size_t y) const noexcept
        {
            auto index = y * width_ * 3 + x * 3;

            return Vec3{ data_[index], data_[index + 1], data_[index + 2] };
        }

        Vec3 SampleNearest(float u, float v) const noexcept
        {
            size_t x = ResolveUV<>(u) * (width_ - 1);
            size_t y = (1.f - ResolveUV<>(v)) * (height_ - 1);

            return GetPixel(x, y);
        }

        // TODO: store redundent texture to boost bilinear performance
        Vec3 SampleBilinear(float u, float v) const noexcept
        {
            auto u_ = (u - floor(u)) * width_;
            auto v_ = (ceil(v) - v) * height_;

            auto x0 = static_cast<size_t>(u_);
            auto y0 = static_cast<size_t>(v_);
            auto x1 = min(x0 + 1, width_ - 1);
            auto y1 = min(y0 + 1, height_ - 1);

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
        size_t width_ = 0;
        size_t height_ = 0;

        std::unique_ptr<float[]> data_ = nullptr;
    };
}