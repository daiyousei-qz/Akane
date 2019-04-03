#pragma once
#include "akane/core.h"
#include "akane/sampler.h"
#include <memory>

namespace akane
{
    class Camera
    {
    public:
        using Ptr = std::unique_ptr<Camera>;

        Ray SpawnRay(Sampler& sampler, int width, int height, int x,
                     int y) const
        {

            return {};
        }
    };

} // namespace akane
