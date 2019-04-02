#pragma once 
#include "akane/core.h"
#include "akane/sampler.h"

namespace akane
{
    class Camera
    {
        Ray SpawnRay(Sampler& sampler, int width, int height, int x, int y);
    };
}
