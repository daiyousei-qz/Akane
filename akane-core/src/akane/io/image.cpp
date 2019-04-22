#include "akane/io/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "akane/external/stb/stb_image.h"

namespace akane
{
    Image::SharedPtr Image::LoadPng(const std::string& filename)
    {
        int width, height, channel;
        auto stb_buffer = stbi_load(filename.c_str(), &width, &height, &channel, 3);
        std::vector<uint8_t> data(stb_buffer, stb_buffer + width * height * channel);
        stbi_image_free(stb_buffer);

        auto result     = std::make_shared<Image>();
        result->width_  = width;
        result->height_ = height;
        result->stride_ = channel;
        result->data_   = std::move(data);

        return result;
    }
} // namespace akane