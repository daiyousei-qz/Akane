#pragma once
#include "akane/math/math.h"
#include "akane/math/sampling.h"
#include "akane/ray.h"

namespace akane::shape
{
    /**
     * An empty shape that doesn't exist in the space
     */
    class Empty
    {
    public:
        constexpr Empty() noexcept
        {
        }

        /**
         * Test if a ray intersect with the geometric object
         */
        bool Intersect(const Ray& ray, float t_min, float t_max, IntersectionInfo& isect) const
            noexcept
        {
            return false;
        }

        /**
         * Surface area for the geometric object
         */
        float Area() const noexcept
        {
            return 0.f;
        }

        /**
         * Samples a point on the surface area
         */
        void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out, float& pdf_out) const noexcept
        {
            p_out   = Vec3{};
            n_out   = {0, 0, 1};
            pdf_out = 1.f;
        }
    };
} // namespace akane::shape
