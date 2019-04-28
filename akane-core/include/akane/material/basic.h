#pragma once
#include "akane/material.h"
#include "akane/bsdf/lambertian.h"
#include "akane/bsdf/specular.h"
#include "akane/bsdf/microfacet.h"
#include <string>
#include <functional>

namespace akane
{
    class TestMaterial : public Material
    {
    public:
        TestMaterial(const Texture* texture, std::function<Bsdf*(Workspace&, Spectrum)> factory)
            : texture_(texture), factory_(factory)
        {
            assert(texture != nullptr);
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
			return factory_(workspace, texture_->Eval(isect));
        }

    private:
		std::function<Bsdf* (Workspace&, Spectrum)> factory_;
        const Texture* texture_;
    };

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
            return workspace.Construct<LambertianReflection>(texture_->Eval(isect));
        }

    private:
        const Texture* texture_;
    };
} // namespace akane