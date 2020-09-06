#pragma once
#include "akane/common/basic.h"
#include "akane/spectrum.h"
#include <vector>
#include <memory>

namespace akane
{
    /**
     * Film buffer where a rendered scene is written
     */
    class Canvas
    {
    public:
        Canvas(int width, int height) : width_(width), height_(height)
        {
            AKANE_ASSERT(width > 0 && height > 0);
            buffer_.resize(width * height * 3, 0.);
        }

        int Width() const noexcept
        {
            return width_;
        }
        int Height() const noexcept
        {
            return height_;
        }

        void Set(const Canvas& other)
        {
            AKANE_REQUIRE(width_ == other.width_ && height_ == other.height_);
            std::copy(other.buffer_.begin(), other.buffer_.end(), buffer_.begin());
        }
        void Increment(const Canvas& other)
        {
            AKANE_REQUIRE(width_ == other.width_ && height_ == other.height_);
            for (size_t i = 0; i < buffer_.size(); ++i)
            {
                buffer_[i] += other.buffer_[i];
            }
        }

        void SetPixel(int x, int y, Spectrum color)
        {
            AKANE_ASSERT(x >= 0 && x < width_);
            AKANE_ASSERT(y >= 0 && y < height_);
            size_t offset = 3 * (y * width_ + x);

            buffer_[offset]      = color[0];
            buffer_[offset + 1u] = color[1];
            buffer_[offset + 2u] = color[1];
        }
        void IncrementPixel(int x, int y, Spectrum delta)
        {
            AKANE_ASSERT(x >= 0 && x < width_);
            AKANE_ASSERT(y >= 0 && y < height_);
            size_t offset = 3 * (y * width_ + x);

            buffer_[offset] += delta[0];
            buffer_[offset + 1u] += delta[1];
            buffer_[offset + 2u] += delta[2];
        }
        Spectrum GetPixel(int x, int y)
        {
            AKANE_ASSERT(x >= 0 && x < width_);
            AKANE_ASSERT(y >= 0 && y < height_);
            size_t offset = 3 * (y * width_ + x);

            return Spectrum{buffer_[offset], buffer_[offset + 1u], buffer_[offset + 2u]};
        }

        void Clear()
        {
            std::fill(buffer_.begin(), buffer_.end(), 0.);
        }

        void SaveRaw(const std::string& filename, float scalar = 1.f);
        void SaveImage(const std::string& filename, float scalar = 1.f);

    private:
        int width_;
        int height_;
        std::vector<float> buffer_;
    };
} // namespace akane