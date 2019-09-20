#pragma once
#include "akane/material.h"
#include "akane/bsdf.h"
#include "akane/bsdf/lambertian.h"

namespace akane
{
    class LambertianMaterial : public Material
    {
    public:
        LambertianMaterial(Vec3 albedo) : albedo_(albedo)
        {
        }

        const Bsdf* ComputeBsdf(Workspace& workspace,
                                const IntersectionInfo& isect) const override
        {
            return workspace.Construct<LambertianReflection>(albedo_);
        }

    private:
        Vec3 albedo_;
    };
} // namespace akane