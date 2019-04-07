#pragma once
#include "akane/math/vector.h"
#include "akane/core.h"
#include "akane/sampler.h"
#include <memory>

namespace akane
{
    class Camera
    {
    public:
        using Ptr = std::unique_ptr<Camera>;

        // spawn ray according to resolution and (x, y)
        virtual Ray SpawnRay(Point2i resolution, Point2i pixel,
                             Point2f sample) const noexcept = 0;
    };

    Camera::Ptr CreatePinholeCamera(Point3f origin, Vec3f forward, Vec3f upward,
                                    Point2f fov);

} // namespace akane
