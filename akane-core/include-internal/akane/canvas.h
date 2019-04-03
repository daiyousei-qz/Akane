#pragma once
#include "akane/spectrum.h"
#include <vector>
#include <cassert>

namespace akane
{
    // film where a scene is rendered
    class Canvas
    {
    public:
        Canvas(int width, int height) : width_(width), height_(height)
        {
            assert(width > 0 && height > 0);
            pixels_.resize(width * height);
        }

        Spectrum& At(int x, int y)
        {
            assert(x >= 0 && x < width_);
            assert(y >= 0 && y < height_);

            return pixels_[y * height_ + x];
        }

    private:
        int width_;
        int height_;
        std::vector<Spectrum> pixels_;
    };
} // namespace akane