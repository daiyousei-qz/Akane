#include "akane/texture.h"
#include "akane/color.h"

#define STB_IMAGE_IMPLEMENTATION
#include "akane/external/stb/stb_image.h"

namespace akane
{
    ImageTexture::ImageTexture(const std::string& filename)
    {
        int width, height;
        auto stb_buffer = stbi_load(filename.c_str(), &width, &height, nullptr, 3);
        AKANE_REQUIRE(stb_buffer != nullptr);

        width_  = width;
        height_ = height;
        data_   = std::make_unique<float[]>(width * height * 3);
        for (int i = 0; i < width * height * 3; ++i)
        {
            data_[i] = sRGB2Linear(static_cast<float>(stb_buffer[i]) * (1.f / 255.f));
        }

        stbi_image_free(stb_buffer);
    }
} // namespace akane