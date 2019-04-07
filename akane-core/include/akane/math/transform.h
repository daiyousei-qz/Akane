#pragma once
#include "akane/math/vector.h"

namespace akane
{
    struct Mat4x4
    {
        Vec4f m[4];

        Mat4x4(Vec4f v0, Vec4f v1, Vec4f v2, Vec4f v3) : m{v0, v1, v2, v3}
        {
        }

        Mat4x4 Transpose() const noexcept
        {
        }

        Mat4x4 Inverse() const noexcept
        {
        }
    };

    // affine transform over 3d space
    class Transform
    {
    public:
        Transform(Vec4f v0, Vec4f v1, Vec4f v2, Vec4f v3) : m_(v0, v1, v2, v3)
        {
        }
        Transform(Mat4x4 m) : m_(m)
        {
        }

        // v{x,y,z} * M_3x3
        Vec3f ApplyLinear(Vec3f v) const noexcept
        {
            const auto& m = m_.m;
            auto x        = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2];
            auto y        = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2];
            auto z        = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2];

            return Vec3f{x, y, z};
        }

        // v{x,y,z,1} * M_4x4
        Vec3f Apply(Vec3f v) const noexcept
        {
            const auto& m = m_.m;
            auto x = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + m[0][3];
            auto y = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + m[1][3];
            auto z = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + m[2][3];

            return Vec3f{x, y, z};
        }

        static Transform Identity() noexcept
        {
            return Transform{
                {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
        }

    private:
        Mat4x4 m_;
    };
} // namespace akane