#include "akane/camera/perspective.h"

namespace akane
{
    unique_ptr<Camera> CreatePinholeCamera(Vec3 origin, Vec3 forward, Vec3 upward, float fov,
                                           float aspect_ratio)
    {
        return CreatePinholeCamera(origin, forward, upward, {fov, fov * aspect_ratio});
    }

    unique_ptr<Camera> CreatePinholeCamera(Vec3 origin, Vec3 forward, Vec3 upward, Vec2 fov)
    {
        auto ku = tan(fov.X() * kPi / 2.f);
        auto kv = tan(fov.Y() * kPi / 2.f);

        auto zz = forward.Normalized();           // forward
        auto yy = Cross(zz, upward).Normalized(); // rightward
        auto xx = Cross(yy, zz).Normalized();     // upward

        return make_unique<PerspectiveCamera>(Transform{xx * kv, yy * ku, zz, origin});
    }
} // namespace akane