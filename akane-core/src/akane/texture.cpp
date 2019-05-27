#include "akane/texture.h"
#include "akane/color.h"

#define STB_IMAGE_IMPLEMENTATION
#include "akane/external/stb/stb_image.h"

namespace akane
{
    ImageTexture::ImageTexture(const std::string& filename)
    {
        int width, height, channel;
        auto stb_buffer = stbi_load(filename.c_str(), &width, &height, &channel, 3);
        AKANE_REQUIRE(stb_buffer != nullptr && channel == 3);

        width_  = width;
        height_ = height;
        data_   = std::make_unique<float[]>(width * height * 3);
        if (channel == 3)
        {
            for (int i = 0; i < width * height * 3; ++i)
            {
                data_[i] = sRGB2Linear(static_cast<float>(stb_buffer[i]) * (1.f / 255.f));
            }
        }
        else if (channel == 4)
        {
            // assuming RGBA layout in stb_buffer
            for (int i = 0; i < width * height; ++i)
            {
                int i_dst = i * 3;
                int i_src = i * 4;
                data_[i_dst] = sRGB2Linear(static_cast<float>(stb_buffer[i_src]) * (1.f / 255.f));
                data_[i_dst+1] = sRGB2Linear(static_cast<float>(stb_buffer[i_src+1]) * (1.f / 255.f));
                data_[i_dst+2] = sRGB2Linear(static_cast<float>(stb_buffer[i_src+2]) * (1.f / 255.f));
            }
        }
        else
        {
            Throw("unsupported image channel {}", channel);
        }

        stbi_image_free(stb_buffer);
    }
} // namespace akane