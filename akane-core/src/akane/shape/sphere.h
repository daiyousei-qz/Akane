#pragma once
#include "akane/math/math.h"
#include "akane/math/sampling.h"
#include "akane/ray.h"

namespace akane::shape
{
    class Sphere
    {
    public:
        constexpr Sphere(Vec3 center, float radius) : center_(center), radius_(radius)
        {
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, IntersectionInfo& isect) const
            noexcept
        {
            // ray P(t) = A + t*B
            // sphere | P - C |^2 = r^2
            auto A = ray.o;
            auto B = ray.d;
            auto C = center_;
            auto D = A - C;
            auto r = radius_;

            // equation
            // |P(t) - C|^2 = |A + t*B - C|^2 = r^2, D = A - C
            // ...
            // a*t^2 + b*t + c = 0
            // a = dot(B, B)
            // b = 2*dot(B, D)
            // c = dot(D, D) - r^2
            auto a = Dot(B, B);
            auto b = 2 * Dot(B, D);
            auto c = Dot(D, D) - r * r;

            // no solution
            auto delta_sq = b * b - 4 * a * c;
            if (delta_sq < 0)
            {
                return false;
            }

            // has solution
            auto delta = sqrt(delta_sq);
            auto t0    = (-b - delta) / (2 * a);
            auto t1    = (-b + delta) / (2 * a);

            auto t = t0 >= 0 ? t0 : t1;
            if (t < 0 || t < t_min || t > t_max)
            {
                return false;
            }

            auto P      = A + t * B;
            auto normal = (P - C).Normalized();
            auto u      = 0.5f + atan2(-normal.Z(), -normal.X()) * kInvTwoPi;
            auto v      = 0.5f - asin(-normal.Y()) * kInvPi;

            isect.t     = t;
            isect.point = P;
            isect.ng    = normal;
            isect.ns    = normal;
            isect.uv    = {u, v};
            return true;
        }

        float Area() const noexcept
        {
            return 4.f * kPi * radius_;
        }

        void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out, float& pdf_out) const noexcept
        {
            n_out   = SampleUniformSphere(u);
            p_out   = n_out * radius_ + center_;
            pdf_out = 1.f / Area();
        }

    private:
        Vec3 center_;
        float radius_;
    };

} // namespace akane