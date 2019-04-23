#pragma once
#include <vector>
#include <string>
#include <memory>

namespace akane
{
    struct RGBPixel
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    class Image
    {
    public:
        using SharedPtr = std::shared_ptr<Image>;

		auto Width() const noexcept { return width_; }
		auto Height() const noexcept { return height_; }
		auto Channel() const noexcept { return 3; }

        RGBPixel At(size_t x, size_t y) const noexcept
        {
            auto index = y * height_ * stride_ + x * stride_;
            return RGBPixel{data_[index + 0], data_[index + 1], data_[index + 2]};
        }

		static Image::SharedPtr LoadImage(const std::string& filename);

    private:
        size_t stride_; // pixel stride

        size_t width_;
        size_t height_;
        std::vector<uint8_t> data_;
    };
} // namespace akane