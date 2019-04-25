#pragma once
#include "akane/common/language_utils.h"
#include "akane/math/vector.h"

namespace akane
{
    class Transform
    {
    public:
        Transform(Vec3f vx, Vec3f vy, Vec3f vz, Vec3f p = {0, 0, 0})
            : vx_(vx), vy_(vy), vz_(vz), p_(p)
        {
        }

        // only linear part of this transform would be inherited
        Transform Also(const Transform& other) const noexcept
        {
            auto r0 = Vec3f{vx_[0], vy_[0], vz_[0]};
            auto r1 = Vec3f{vx_[1], vy_[1], vz_[1]};
            auto r2 = Vec3f{vx_[2], vy_[2], vz_[2]};
            auto c0 = other.vx_;
            auto c1 = other.vy_;
            auto c2 = other.vz_;

            auto new_vx = Vec3f{r0.Dot(c0), r1.Dot(c0), r2.Dot(c0)};
            auto new_vy = Vec3f{r0.Dot(c1), r1.Dot(c1), r2.Dot(c1)};
            auto new_vz = Vec3f{r0.Dot(c2), r1.Dot(c2), r2.Dot(c2)};

            return Transform(new_vx, new_vy, new_vz, other.p_);
        }

        Transform Scale(akFloat ratio) const noexcept
        {
            return Also(Transform::CreateScale(ratio));
        }

        Transform RotateX(akFloat theta) const noexcept
        {
            return Also(Transform::CreateRotateX(theta));
        }

        Transform RotateY(akFloat theta) const noexcept
        {
            return Also(Transform::CreateRotateY(theta));
        }

        Transform RotateZ(akFloat theta) const noexcept
        {
            return Also(Transform::CreateRotateZ(theta));
        }

        // this method should be called at the last of calling chain
        // as this is non-linear
        Transform Move(const Vec3f& p) const noexcept
        {
            return Transform(vx_, vy_, vz_, p);
        }

        Vec3f Apply(const Vec3f& v) const noexcept
        {
            return Vec3f{v.Dot(vx_), v.Dot(vy_), v.Dot(vz_)} + p_;
        }

        Vec3f ApplyLinear(const Vec3f& v) const noexcept
        {
            return Vec3f{v.Dot(vx_), v.Dot(vy_), v.Dot(vz_)};
        }

        Vec3f Inverse(const Vec3f& v) const noexcept
        {
            auto v_ = v - p_;
            auto xx = v_.X() * vx_.X() + v_.Y() * vy_.X() + v_.Z() * vz_.X();
            auto yy = v_.X() * vx_.Y() + v_.Y() * vy_.Y() + v_.Z() * vz_.Y();
            auto zz = v_.X() * vx_.Z() + v_.Y() * vy_.Z() + v_.Z() * vz_.Z();

            return Vec3f{xx, yy, zz};
        }

        Vec3f InverseLinear(const Vec3f& v) const noexcept
        {
            auto xx = v.X() * vx_.X() + v.Y() * vy_.X() + v.Z() * vz_.X();
            auto yy = v.X() * vx_.Y() + v.Y() * vy_.Y() + v.Z() * vz_.Y();
            auto zz = v.X() * vx_.Z() + v.Y() * vy_.Z() + v.Z() * vz_.Z();

            return Vec3f{xx, yy, zz};
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

        static const Transform CreateScale(akFloat ratio)
        {
            AKANE_REQUIRE(ratio > 0);

            return Transform({ratio, 0, 0}, {0, ratio, 0}, {0, 0, ratio});
        }

        static Transform CreateRotateX(akFloat theta)
        {
            auto cos_theta = cos(theta);
            auto sin_theta = sin(theta);
            return Transform({1, 0, 0}, {0, cos_theta, sin_theta}, {0, -sin_theta, cos_theta});
        }

        static Transform CreateRotateY(akFloat theta)
        {
            auto cos_theta = cos(theta);
            auto sin_theta = sin(theta);
            return Transform({cos_theta, 0, -sin_theta}, {0, 1, 0}, {sin_theta, 0, cos_theta});
        }

        static Transform CreateRotateZ(akFloat theta)
        {
            auto cos_theta = cos(theta);
            auto sin_theta = sin(theta);
            return Transform({cos_theta, sin_theta, 0}, {-sin_theta, cos_theta, 0}, {0, 0, 1});
        }

    private:
        Vec3f vx_, vy_, vz_; // column vectors for linear transform
        Vec3f p_;
    };

} // namespace akane