#pragma once
#include "akane/common/basic.h"

namespace akane
{
    shared_ptr<uint8_t[]> LoadImage(const char* filename, int& width_out, int& height_out);

    void SavePngImage(const char* filename, const uint8_t* data, int width, int height);
} // namespace akane