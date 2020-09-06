#pragma once
#include "akane/math/math.h"
#include "akane/math/sampling.h"
#include "akane/math/transform.h"
#include "akane/ray.h"

namespace akane::shape
{
    template <typename ShapeType> class TransformedShape
    {
    public:
        constexpr TransformedShape(ShapeType shape, Vec3 rotation = 0.f) : shape_(shape)
        {
            transform_ =
                Transform::CreateRotateX(rotation[0]).RotateY(rotation[1]).RotateZ(rotation[2]);
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, IntersectionInfo& isect) const
            noexcept
        {
            Ray ray2 = Ray{transform_.ApplyLinear(ray.o), transform_.ApplyLinear(ray.d)};

            if (!shape_.Intersect(ray2, t_min, t_max, isect))
            {
                return false;
            }

            isect.point = transform_.InverseLinear(isect.point);
            isect.ng    = transform_.InverseLinear(isect.ng);
            isect.ns    = isect.ng;
            return true;
        }

        float Area() const noexcept
        {
            return shape_.Area();
        }

        void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out, float& pdf_out) const noexcept
        {
            shape_.SamplePoint(u, p_out, n_out, pdf_out);

            p_out = transform_.ApplyLinear(p_out);
            n_out = transform_.ApplyLinear(n_out);
        }

    private:
        ShapeType shape_;
        Transform transform_ = Transform::Identity();
    };
} // namespace akane