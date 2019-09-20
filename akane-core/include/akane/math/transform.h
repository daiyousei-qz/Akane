#pragma once
#include "akane/common/basic.h"
#include "akane/math/math.h"
#include "akane/math/vector.h"

namespace akane
{
    // affine transformation
    class Transform
    {
    public:
        constexpr Transform(Vec3 vx, Vec3 vy, Vec3 vz, Vec3 p = {0, 0, 0})
            : vx_(vx), vy_(vy), vz_(vz), p_(p)
        {
        }

        constexpr const Vec3& X() const noexcept
        {
            return vx_;
        }
        constexpr const Vec3& Y() const noexcept
        {
            return vy_;
        }
        constexpr const Vec3& Z() const noexcept
        {
            return vz_;
        }
        constexpr const Vec3& P() const noexcept
        {
            return p_;
        }

        // only linear part of this transform would be inherited
        constexpr Transform Also(const Transform& other) const noexcept
        {
            auto r0 = Vec3{vx_[0], vy_[0], vz_[0]};
            auto r1 = Vec3{vx_[1], vy_[1], vz_[1]};
            auto r2 = Vec3{vx_[2], vy_[2], vz_[2]};
            auto c0 = other.vx_;
            auto c1 = other.vy_;
            auto c2 = other.vz_;

            auto new_vx = Vec3{Dot(r0, c0), Dot(r1, c0), Dot(r2, c0)};
            auto new_vy = Vec3{Dot(r0, c1), Dot(r1, c1), Dot(r2, c1)};
            auto new_vz = Vec3{Dot(r0, c2), Dot(r1, c2), Dot(r2, c2)};

            return Transform(new_vx, new_vy, new_vz, other.p_);
        }

        constexpr Transform Scale(float ratio) const noexcept
        {
            return Also(Transform::CreateScale(ratio));
        }

        Transform RotateX(float theta) const noexcept
        {
            return Also(Transform::CreateRotateX(theta));
        }

        Transform RotateY(float theta) const noexcept
        {
            return Also(Transform::CreateRotateY(theta));
        }

        Transform RotateZ(float theta) const noexcept
        {
            return Also(Transform::CreateRotateZ(theta));
        }

        // this method should be called at the last of calling chain
        // as this is non-linear
        Transform Move(const Vec3& p) const noexcept
        {
            return Transform(vx_, vy_, vz_, p);
        }

        Vec3 Apply(const Vec3& v) const noexcept
        {
            return ApplyLinear(v) + p_;
        }

        Vec3 ApplyLinear(const Vec3& v) const noexcept
        {
            return Vec3{v.Dot(vx_), v.Dot(vy_), v.Dot(vz_)};
        }

        Vec3 Inverse(const Vec3& v) const noexcept
        {
            return InverseLinear(v - p_);
        }

        Vec3 InverseLinear(const Vec3& v) const noexcept
        {
            auto xx = v.X() * vx_.X() + v.Y() * vy_.X() + v.Z() * vz_.X();
            auto yy = v.X() * vx_.Y() + v.Y() * vy_.Y() + v.Z() * vz_.Y();
            auto zz = v.X() * vx_.Z() + v.Y() * vy_.Z() + v.Z() * vz_.Z();

            return Vec3{xx, yy, zz};
        }

        void Print() const noexcept
        {
            printf("%6f, %6f, %6f\n", vx_[0], vy_[0], vz_[0]);
            printf("%6f, %6f, %6f\n", vx_[1], vy_[1], vz_[1]);
            printf("%6f, %6f, %6f\n", vx_[2], vy_[2], vz_[2]);
            // printf("\n%6f, %6f, %6f\n", p[0], p[1], p[2]);
        }

        // builtin transform factory
        //

        static constexpr Transform Identity() noexcept
        {
            return Transform({1, 0, 0}, {0, 1, 0}, {0, 0, 1});
        }

        static Transform CreateScale(float ratio) noexcept
        {
            AKANE_REQUIRE(ratio > 0);

            return Transform({ratio, 0, 0}, {0, ratio, 0}, {0, 0, ratio});
        }

        static Transform CreateRotateX(float theta) noexcept
        {
            float cos_theta = cos(theta);
            float sin_theta = sin(theta);
            return Transform({1, 0, 0}, {0, cos_theta, sin_theta}, {0, -sin_theta, cos_theta});
        }

        static Transform CreateRotateY(float theta) noexcept
        {
            float cos_theta = cos(theta);
            float sin_theta = sin(theta);
            return Transform({cos_theta, 0, -sin_theta}, {0, 1, 0}, {sin_theta, 0, cos_theta});
        }

        static Transform CreateRotateZ(float theta) noexcept
        {
            float cos_theta = cos(theta);
            float sin_theta = sin(theta);
            return Transform({cos_theta, sin_theta, 0}, {-sin_theta, cos_theta, 0}, {0, 0, 1});
        }

    private:
        Vec3 vx_, vy_, vz_; // column vectors for linear transform
        Vec3 p_;
    };

} // namespace akane