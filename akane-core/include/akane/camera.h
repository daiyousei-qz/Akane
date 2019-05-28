#pragma once
#include "akane/math/vector.h"
#include "akane/core.h"
#include "akane/sampler.h"
#include <memory>

namespace akane
{
    class Camera : public Object
    {
    public:
        using Ptr = std::unique_ptr<Camera>;

        // spawn ray according to resolution and (x, y)
        virtual Ray SpawnRay(Point2i resolution, Point2i pixel, Point2f sample) const noexcept = 0;
    };

    // fov: horizontal field of view
    // aspect_ratio: vertical over horizontal
    Camera::Ptr CreatePinholeCamera(Point3f origin, Vec3f forward, Vec3f upward, float fov = 0.5f,
                                    float aspect_ratio = 1.f);

    Camera::Ptr CreatePinholeCamera(Point3f origin, Vec3f forward, Vec3f upward, Vec2f fov);

} // namespace akane
