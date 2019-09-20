#include "akane/texture/image.h"
#include "akane/common/image.h"

namespace akane
{
    ImageTexture::ImageTexture(const std::string& filename)
    {
        int width;
        int height;
        auto image_data = LoadImage(filename.c_str(), width, height);

        data_ = make_unique<float[]>(width * height * 3);
        for (int i = 0; i < width * height * 3; ++i)
        {
            data_[i] = sRGB2Linear(static_cast<float>(image_data[i]) * (1.f / 255.f));
        }

        width_  = width;
        height_ = height;
    }
} // namespace akane