#pragma once
#include "akane/math/math.h"
#include "akane/math/sampling.h"
#include "akane/ray.h"

namespace akane::shape
{
    /**
     * A rectangle that is placed horizontally, paralell to xy plane
     */
    class Rect
    {
    public:
        constexpr Rect(Vec3 center, float len_x, float len_y)
            : center_(center), len_x_(len_x), len_y_(len_y)
        {
            AKANE_REQUIRE(len_x > 0 && len_y > 0);
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, IntersectionInfo& isect) const
            noexcept
        {
            // ray is paralell to the rect
            if (ray.d.Z() == 0)
            {
                return false;
            }

            // compute point P that ray hits at plane rect's plane
            float t = (center_.Z() - ray.o.Z()) / ray.d.Z();
            Vec3 P  = ray.o + t * ray.d;

            if (t < t_min || t > t_max)
            {
                return false;
            }

            // test if hit point is in the rectangle
            Vec3 delta = P - center_;
            if (abs(delta[0]) > len_x_ * .5f || abs(delta[1]) > len_y_ * .5f)
            {
                return false;
            }

            isect.t     = t;
            isect.point = P;
            isect.ng    = {0, 0, -1}; // TODO: not (0, 0, 1)?
            isect.ns    = {0, 0, -1};
            isect.uv    = {0, 0}; // TODO:
            return true;
        }

        float Area() const noexcept
        {
            return len_x_ * len_y_;
        }

        void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out, float& pdf_out) const noexcept
        {
            p_out   = Vec3{(u[0] - .5f) * len_x_, (u[1] - .5f) * len_y_, 0} + center_;
            n_out   = {0, 0, -1};
            pdf_out = 1.f / Area();
        }

    private:
        Vec3 center_;
        float len_x_, len_y_;
    };
} // namespace akane::shape