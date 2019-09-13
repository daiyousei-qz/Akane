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

            auto new_vx = Vec3{r0.Dot(c0), r1.Dot(c0), r2.Dot(c0)};
            auto new_vy = Vec3{r0.Dot(c1), r1.Dot(c1), r2.Dot(c1)};
            auto new_vz = Vec3{r0.Dot(c2), r1.Dot(c2), r2.Dot(c2)};

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
            return Vec3{v.Dot(vx_), v.Dot(vy_), v.Dot(vz_)} + p_;
        }

        Vec3 ApplyLinear(const Vec3& v) const noexcept
        {
            return Vec3{v.Dot(vx_), v.Dot(vy_), v.Dot(vz_)};
        }

        Vec3 Inverse(const Vec3& v) const noexcept
        {
            auto v_ = v - p_;
            auto xx = v_.X() * vx_.X() + v_.Y() * vy_.X() + v_.Z() * vz_.X();
            auto yy = v_.X() * vx_.Y() + v_.Y() * vy_.Y() + v_.Z() * vz_.Y();
            auto zz = v_.X() * vx_.Z() + v_.Y() * vy_.Z() + v_.Z() * vz_.Z();

            return Vec3{xx, yy, zz};
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

        static const Transform& Identity()
        {
            static auto id = Transform({1, 0, 0}, {0, 1, 0}, {0, 0, 1});

            return id;
        }

        static const Transform CreateScale(float ratio)
        {
            AKANE_REQUIRE(ratio > 0);

            return Transform({ratio, 0, 0}, {0, ratio, 0}, {0, 0, ratio});
        }

        static Transform CreateRotateX(float theta)
        {
            auto cos_theta = Cos(theta);
            auto sin_theta = Sin(theta);
            return Transform({1, 0, 0}, {0, cos_theta, sin_theta}, {0, -sin_theta, cos_theta});
        }

        static Transform CreateRotateY(float theta)
        {
            auto cos_theta = Cos(theta);
            auto sin_theta = Sin(theta);
            return Transform({cos_theta, 0, -sin_theta}, {0, 1, 0}, {sin_theta, 0, cos_theta});
        }

        static Transform CreateRotateZ(float theta)
        {
            auto cos_theta = Cos(theta);
            auto sin_theta = Sin(theta);
            return Transform({cos_theta, sin_theta, 0}, {-sin_theta, cos_theta, 0}, {0, 0, 1});
        }

    private:
        Vec3 vx_, vy_, vz_; // column vectors for linear transform
        Vec3 p_;
    };

    Transform CreateWorldCoordTransform(Vec3 local_x, Vec3 local_y, Vec3 local_z)
    {
        return Transform{local_x.Normalized(), local_y.Normalized(), local_z.Normalized()};
    }

} // namespace akane