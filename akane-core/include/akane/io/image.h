#pragma once
#include <vector>
#include <string>
#include <memory>

namespace akane
{
    template <typename T> class BasicImage
    {
    public:
        using SharedPtr = std::shared_ptr<BasicImage>;

        BasicImage() : BasicImage(0, 0, 3)
        {
        }
        BasicImage(int width, int height, int channel = 3)
            : channel_(channel), width_(width), height_(height)
        {
            data_.resize(width * height * channel, 0);
        }

        int Width() const noexcept
        {
            return width_;
        }
        int Height() const noexcept
        {
            return height_;
        }
        int Channel() const noexcept
        {
            return channel_;
        }

        T At(int x, int y, int channel) const noexcept
        {
            return data_[y * width_ * channel_ + x * channel_ + channel];
        }
        T& MutableAt(int x, int y, int channel) noexcept
        {
            return data_[y * width_ * channel_ + x * channel_ + channel];
        }
        void Clear(T x = {})
        {
            std::fill(std::begin(data_), std::end(data_), x);
        }

    private:
        friend BasicImage::SharedPtr LoadImage(const std::string& filename);

        int width_;
        int height_;
        int channel_;

        std::vector<T> data_;
    };

    using Image  = BasicImage<float>;
    using ImageD = BasicImage<double>;

    Image::SharedPtr LoadImage(const std::string& filename);

} // namespace akane
