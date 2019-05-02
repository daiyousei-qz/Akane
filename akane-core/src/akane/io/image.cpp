#include "akane/io/image.h"
#include "akane/spectrum.h"

#define STB_IMAGE_IMPLEMENTATION
#include "akane/external/stb/stb_image.h"

namespace akane
{
    Image::SharedPtr LoadImage(const std::string& filename)
    {
        int width, height, channel;
        auto stb_buffer = stbi_load(filename.c_str(), &width, &height, &channel, 3);
        if (stb_buffer == nullptr)
        {
            return nullptr;
        }

        std::vector<akFloat> data;
        data.reserve(width * height * 3);
        for (int i = 0; i < width * height; ++i)
        {
            auto index    = i * 3;
            auto spectrum = GammaCorrect(
                RGBToSpectrum(stb_buffer[index], stb_buffer[index + 1], stb_buffer[index + 2]),
                1.f / 2.4f);

            data.push_back(spectrum[0]);
            data.push_back(spectrum[1]);
            data.push_back(spectrum[2]);
        }

        stbi_image_free(stb_buffer);

        auto result      = std::make_shared<Image>();
        result->width_   = width;
        result->height_  = height;
        result->channel_ = 3;
        result->data_    = std::move(data);

        return result;
    }
} // namespace akane