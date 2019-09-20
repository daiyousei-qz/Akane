#pragma once
#include "akane/primitive.h"
#include "akane/math/math.h"
#include "akane/math/sampling.h"
#include "akane/math/transform.h"
#include "akane/light.h"
#include "akane/light/diffuse.h"
#include "akane/ray.h"
#include "akane/material.h"

namespace akane
{
    template <typename GeometryType> class GeometricPrimitive : public Primitive
    {
    public:
        GeometricPrimitive(GeometryType geometry) : geometry_(geometry)
        {
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const override
        {
            bool hit = geometry_.Intersect(ray, t_min, t_max, isect);
            if (hit)
            {
                isect.object     = this;
                isect.area_light = GetAreaLight();
                isect.material   = GetMaterial();
            }

            return hit;
        }

        float Area() const override
        {
            return geometry_.Area();
        }

        void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out, float& pdf_out) const override
        {
            return geometry_.SamplePoint(u, p_out, n_out, pdf_out);
        }

        void BindMaterial(shared_ptr<Material> mat)
        {
            material_ = std::move(mat);
        }
        void BindAreaLight(const Vec3& color, float power)
        {
            area_light_ = make_unique<DiffuseAreaLight>(this, color, power);
        }

        Material* GetMaterial() const noexcept
        {
            return material_.get();
        }
        AreaLight* GetAreaLight() const noexcept
        {
            return area_light_.get();
        }

    private:
        GeometryType geometry_;

        shared_ptr<Material> material_    = nullptr;
        unique_ptr<AreaLight> area_light_ = nullptr;
    };
} // namespace akane