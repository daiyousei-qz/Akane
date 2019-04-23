#pragma once
#include "akane/material.h"
#include "akane/bsdf/lambertian.h"
#include "akane/bsdf/specular.h"
#include "akane/bsdf/microfacet.h"

namespace akane
{
    // pure diffuse
    class Lambertian : public Material
    {
    public:
        Lambertian(const Texture* texture) : texture_(texture)
        {
            assert(texture != nullptr);
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
            return workspace.Construct<LambertianReflection>(texture_->Value(isect));
        }

    private:
        const Texture* texture_;
    };

    class PerfectMirror : public Material
    {
    public:
        PerfectMirror(const Texture* texture) : texture_(texture)
        {
            assert(texture != nullptr);
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
            return workspace.Construct<SpecularReflection>(texture_->Value(isect));
        }

    private:
        const Texture* texture_;
    };

    class TestMacrofacet : public Material
    {
    public:
        TestMacrofacet(const Texture* texture) : texture_(texture)
        {
            assert(texture != nullptr);
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
            return workspace.Construct<MicrofacetReflection>(texture_->Value(isect));
        }

    private:
        const Texture* texture_;
    };
} // namespace akane