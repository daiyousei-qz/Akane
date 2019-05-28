#include "akane/camera.h"

namespace akane
{
    // basic pinhole camera
    class PinholeCamera : public Camera
    {
    public:
        PinholeCamera(Point3f origin, Vec3f forward, Vec3f upward, Point2f fov)
        {
            origin_   = origin;
            forward_  = forward.Normalized();
            leftward_ = upward.Cross(forward_).Normalized();
            upward_   = forward_.Cross(leftward_).Normalized();

            leftward_ *= tanf(fov.X() * kPI / 2.f);
            upward_ *= tanf(fov.Y() * kPI / 2.f);
        }

        Ray SpawnRay(Point2i resolution, Point2i pixel, Point2f sample) const noexcept override
        {
            auto u = (pixel.X() + sample.X()) / resolution.X();
            auto v = (pixel.Y() + sample.Y()) / resolution.Y();

            return SpawnRay(u, v);
        }

        // u = horizontal offset
        // v = vertical offset
        // NOTE u, v in [0, 1]
        Ray SpawnRay(akFloat u, akFloat v) const noexcept
        {
            auto direction = forward_ + (.5f - u) * leftward_ + (.5f - v) * upward_;
            return Ray{origin_, direction.Normalized()};
        }

    private:
        Point3f origin_;
        Vec3f forward_;
        Vec3f upward_;
        Vec3f leftward_;
    };

    Camera::Ptr CreatePinholeCamera(Point3f origin, Vec3f forward, Vec3f upward, float fov,
                                    float aspect_ratio)
    {
        return std::make_unique<PinholeCamera>(origin, forward, upward,
                                               Vec2f{fov, fov * aspect_ratio});
    }

    Camera::Ptr CreatePinholeCamera(Point3f origin, Vec3f forward, Vec3f upward, Point2f fov)
    {
        return std::make_unique<PinholeCamera>(origin, forward, upward, fov);
    }

} // namespace akane