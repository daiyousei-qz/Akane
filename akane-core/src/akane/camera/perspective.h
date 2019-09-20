#pragma once
#include "akane/camera.h"
#include "akane/math/transform.h"

namespace akane
{
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(Transform projection) : projection_(projection)
        {
        }

        virtual Ray SpawnRay(Point2f uv) const noexcept
        {
            // original uv coordinate in a screen
            // (0,0)       (1,0)
            //  ------------
            // |            |
            //  ------------
            // (0,1)       (1,1)
            //
            // xy coordinate in projection plane and uv-xy corespondence
            // (0, 0) -> (1, -1)   = (1, 0)
            // (1, 0) -> (1, 1)    = (1, 1)
            // (0, 1) -> (-1, -1)  = (0, 0)
            // (1, 1) -> (-1, 1)   = (0, 1)

            auto xx = 1.f - 2 * uv[1];
            auto yy = 2 * uv[0] - 1.f;
            return Ray{projection_.P(), projection_.InverseLinear({xx, yy, 1.f}).Normalized()};
        }

    private:
        Transform projection_; // world-to-camera projection
    };
} // namespace akane