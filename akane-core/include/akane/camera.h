#pragma once
#include "akane/common/basic.h"
#include "akane/math/math.h"
#include "akane/ray.h"
#include <memory>

namespace akane
{
    /**
     * Compute uv coordinate in normalized screen space from the specific point on canvas
     *
     * @param xy Point on canvas
     * @param resolution Resolution of canvas
     * @param u A random sample in unit square for jittering(anti-alias)
     */
    inline Point2f ComputeScreenSpaceUV(Point2i xy, Point2i resolution, Point2f u)
    {
        float uu = (static_cast<float>(xy[0]) + u[0]) / static_cast<float>(resolution[0]);
        float vv = (static_cast<float>(xy[1]) + u[1]) / static_cast<float>(resolution[1]);

        return Point2f{uu, vv};
    }

    /**
     * Interface for camera object
     */
    class Camera : public Object
    {
    public:
        // spawn ray according to uv coordinate in the screen
        // usually, uv is in [-1, 1]
        virtual Ray SpawnRay(Point2f uv) const noexcept = 0;
    };

    // fov: horizontal field of view, should in (0, 1],default is 90 degrees
    // aspect_ratio: vertical over horizontal
    unique_ptr<Camera> CreatePinholeCamera(Vec3 origin, Vec3 forward, Vec3 upward, float fov = 0.5f,
                                           float aspect_ratio = 1.f);

    unique_ptr<Camera> CreatePinholeCamera(Vec3 origin, Vec3 forward, Vec3 upward, Vec2 fov);

} // namespace akane
