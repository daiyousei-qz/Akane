#pragma once
#include "akane/primitive.h"

namespace akane
{
    // a placeholder primitive for embree scene
    class EmbreeTriangle : public Primitive
    {
    public:
        EmbreeTriangle() = default;

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& info) const override
        {
            AKANE_NO_IMPL();
        }

        float Area() const override
        {
            return area_;
        }

        void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out, float& pdf_out) const override
        {
            float t  = sqrt(u[0]);
            float k1 = 1 - t;
            float k2 = u[1] * t;
            p_out    = v0_ + k1 * e1_ + k2 * e2_;
            n_out    = Cross(e1_, e2_).Normalized(); // TODO: verify this
            pdf_out  = 1.f / area_;
        }

        bool Equals(const Primitive* other) const noexcept override
        {
            auto tri = dynamic_cast<const EmbreeTriangle*>(other);

            return tri && tri->geom_id_ == geom_id_ && tri->prim_id_ == prim_id_;
        }

    private:
        friend class EmbreeScene;

        // triangle tracking id
        unsigned geom_id_;
        unsigned prim_id_;

        // triangle geometric information
        float area_;
        Vec3 v0_, e1_, e2_;
    };
} // namespace akane