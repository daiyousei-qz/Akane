#pragma once
#include "akane/primitive.h"

namespace akane
{
    class EmbreeTriangle : public Primitive
    {
    public:
		EmbreeTriangle() = default;

        bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
                       IntersectionInfo& info) const override
        {
            AKANE_NO_IMPL();
        }

        akFloat Area() const override
        {
            return area_;
        }

        void SampleP(const Point2f& u, Point3f& point_out, akFloat& pdf_out) const override
        {
            auto e1 = v1_ - v0_;
            auto e2 = v2_ - v0_;

			auto t = sqrt(u[0]);
			auto k1 = 1 - t;
			auto k2 = u[1] * t;
            point_out = v0_ + k1 * e1 + k2 * e2;
            pdf_out   = 1. / area_;
        }

        bool Equals(const Primitive* other) const noexcept override
        {
            auto tri = dynamic_cast<const EmbreeTriangle*>(other);

            return tri && tri->geom_id_ == geom_id_ && tri->prim_id_ == prim_id_;
        }

    private:
        friend class EmbreeScene;

        unsigned geom_id_;
        unsigned prim_id_;

        akFloat area_;
        Vec3f v0_, v1_, v2_;
    };
} // namespace akane