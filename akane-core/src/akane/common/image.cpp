#include "akane/common/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace akane
{
    std::shared_ptr<uint8_t[]> LoadImage(const char* filename, int& width_out, int& height_out)
    {
        int width, height, channel;
        auto stb_buffer = stbi_load(filename, &width, &height, &channel, 3);
        if (stb_buffer == nullptr || channel != 3)
        {
            return nullptr;
        }

        width_out = width;
        height_out = height;
        return std::shared_ptr<uint8_t[]>(stb_buffer, [](uint8_t* ptr) { stbi_image_free(ptr); });
    }

    void SavePngImage(const char* filename, const uint8_t* data, int width, int height)
    {
        stbi_write_png(filename, width, height, 3, data, 0);
    }
} // namespace akane