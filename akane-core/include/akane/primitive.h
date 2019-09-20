#pragma once
#include "akane/common/basic.h"
#include "akane/ray.h"
#include <memory>
#include <vector>

namespace akane
{
    class Primitive : public virtual Object
    {
    public:
        using Ptr = std::unique_ptr<Primitive>;

        // test intersection
        virtual bool Intersect(const Ray& ray, float t_min, float t_max,
                               IntersectionInfo& isect) const = 0;

        // compute surface area of the primitive
        virtual float Area() const = 0;

        // sample a point on the primitive's surface
        virtual void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out,
                                 float& pdf_out) const = 0;

        virtual bool Equals(const Primitive* other) const
        {
            return this == other;
        }
    };

    inline bool SamePrimitive(const Primitive* lhs, const Primitive* rhs)
    {
        AKANE_ASSERT(lhs != nullptr);
        return lhs == rhs || lhs->Equals(rhs);
    }

    class Composite : public virtual Object
    {
    public:
        // test intersection
        virtual bool Intersect(const Ray& ray, float t_min, float t_max,
                               IntersectionInfo& isect) const = 0;
    };

    class NaiveComposite : public Composite
    {
    public:
        void AddPrimitive(Primitive* body)
        {
            objects_.push_back(body);
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const override
        {
            bool any_hit = false;
            float t      = t_max;

            IntersectionInfo isect_buf;
            for (auto child : objects_)
            {
                if (child->Intersect(ray, t_min, t, isect_buf))
                {
                    any_hit = true;
                    t       = isect_buf.t;
                }
            }

            if (any_hit)
            {
                isect = isect_buf;
            }
            return any_hit;
        }

    private:
        std::vector<Primitive*> objects_;
    };
} // namespace akane