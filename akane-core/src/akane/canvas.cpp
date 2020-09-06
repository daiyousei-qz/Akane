#include "akane/canvas.h"
#include "akane/common/image.h"

namespace akane
{
    void Canvas::SaveRaw(const std::string& filename, float scalar)
    {
        constexpr uint32_t magic = 0x41414141;

        uint32_t width  = static_cast<uint32_t>(width_);
        uint32_t height = static_cast<uint32_t>(height_);

        FILE* file = fopen(filename.c_str(), "wb");
        AKANE_REQUIRE(file != nullptr);

        // write header
        fwrite(&magic, 1, 4, file);
        fwrite(&width, 1, 4, file);
        fwrite(&height, 1, 4, file);

        // write body
        for (auto value : buffer_)
        {
            float x = static_cast<float>(value * scalar);
            fwrite(&x, 1, 4, file);
        }

        fclose(file);
    }

    void Canvas::SaveImage(const std::string& filename, float scalar)
    {
        std::vector<uint8_t> image_data;
        image_data.reserve(3 * width_ * height_);

        for (int y = 0; y < height_; ++y)
        {
            for (int x = 0; x < width_; ++x)
            {
                auto spectrum = GetPixel(x, y) * scalar;
                auto color    = Linear2sRGB(ToneMap_Aces(spectrum)) * 255.f;

                image_data.push_back(static_cast<uint8_t>(color[0]));
                image_data.push_back(static_cast<uint8_t>(color[1]));
                image_data.push_back(static_cast<uint8_t>(color[2]));
            }
        }

        SavePngImage(filename.c_str(), image_data.data(), width_, height_);
    }
} // namespace akane