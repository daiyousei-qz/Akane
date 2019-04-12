#pragma once
#include "akane/core.h"
#include "akane/bsdf.h"
#include "akane/spectrum.h"
#include "akane/texture.h"
#include "akane/math/sampling.h"
#include <optional>

namespace akane
{
    class Material : public Object
    {
    public:
        using Ptr = std::unique_ptr<Material>;

        virtual const Bsdf* ComputeBsdf(Workspace& workspace,
                                        const IntersectionInfo& isect) const = 0;

        // compute attenuation, assuming n, wo, wi is normalized
        virtual Spectrum ComputeBSDF(const IntersectionInfo& isect, const Vec3f& wo,
                                     const Vec3f& wi) const noexcept = 0;

        virtual bool Scatter(const Ray& ray, const IntersectionInfo& isect, const Point2f& sample,
                             Vec3f& attenuation, Ray& scatter) const noexcept = 0;
    };

    Material::Ptr CreateLambertian(const Texture* texture);
    Material::Ptr CreateMetal(const Texture* texture, akFloat fuzz);
    Material::Ptr CreateDielectrics(const Texture* texture, akFloat refractive_index);

} // namespace akane