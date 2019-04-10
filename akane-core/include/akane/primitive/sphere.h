#pragma once

#include "akane/core.h"
#include "akane/primitive.h"
#include "akane/material.h"

namespace akane
{
    class Sphere : public GeometricPrimitive
    {
    public:
        Sphere(const Material* material, Point3f center, akFloat radius)
            : GeometricPrimitive(material), center_(center), radius_(radius)
        {
        }

        bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
                       IntersectionInfo& isect) const override
        {
            // ray origin
            Vec3f ray_o = ray.o;
            // ray direction
            Vec3f ray_d = ray.d.Normalized();
            // ray origin to sphere center
            Vec3f l = center_ - ray_o;

            // length of l's project onto ray's direction
            // NOTE length of the projection decrease as ray points away from
            // center of the sphere
            akFloat s    = l.Dot(ray_d);
            akFloat l_sq = l.Dot(l);
            akFloat r_sq = radius_ * radius_;
            if (s * s < l_sq - r_sq || l_sq < r_sq)
            {
                return false; // ray outside does not points to sphere
            }

            akFloat m_sq = l_sq - s * s;
            akFloat q    = sqrt(r_sq - m_sq);
            akFloat t    = l_sq > r_sq ? s - q : s + q;

            if (t < t_min || t > t_max)
            {
                return false;
            }

            isect.t            = t;
            isect.point        = ray_o + t * ray_d;
            isect.uv           = {0, 0};
            isect.normal       = (isect.point - center_).Normalized();
            isect.internal_ray = center_.Distance(ray.o) < radius_;
            isect.primitive    = this;
            isect.material     = GetMaterial();
            return true;
        }

        akFloat Area() const override
        {
            return 4.f* kPI* radius_* radius_;
        }

        void SampleP(const Point2f& u, Point3f& point_out,
                     akFloat& pdf_out) const override
        {
            point_out = SampleUniformSphere(u) * radius_ + center_;
            pdf_out   = 1 / Area();
        }

    private:
        Point3f center_;
        akFloat radius_;
    };

    inline GeometricPrimitive::Ptr
    CreateSpherePrimitive(const Material* material, Vec3f center,
                          akFloat radius)
    {
        return std::make_unique<Sphere>(material, center, radius);
    }
} // namespace akane