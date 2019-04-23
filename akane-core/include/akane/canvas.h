#pragma once
#include "akane/spectrum.h"
#include "akane/external/svpng.inc"
#include <vector>
#include <string>
#include <cassert>

namespace akane
{
    // film where a scene is rendered
    class Canvas
    {
    public:
		using SharedPtr = std::shared_ptr<Canvas>;

        Canvas(int width, int height) : width_(width), height_(height)
        {
            assert(width > 0 && height > 0);
            pixels_.resize(width * height);
        }

        Spectrum& At(int x, int y)
        {
            assert(x >= 0 && x < width_);
            assert(y >= 0 && y < height_);

            return pixels_[y * width_ + x];
        }

        void Finalize(const std::string& filename, akFloat gamma)
        {
            std::vector<uint8_t> graph;
            for (const auto& pixel : pixels_)
            {
                for (int i = 0; i < 3; ++i)
                {
                    auto x = std::min(
                        255,
                        static_cast<int>(255.f * powf(pixel[i], 1 / gamma)));
                    graph.push_back(x);
                }
            }

            auto file = fopen(filename.c_str(), "wb");
            svpng(file, width_, height_, graph.data(), 0);
            fclose(file);
        }

    private:
        int width_;
        int height_;
        std::vector<Spectrum> pixels_;
    };
} // namespace akane