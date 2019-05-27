#pragma once
#include "akane/material.h"
#include <string>
#include <functional>

namespace akane
{
    class TestMaterial : public Material
    {
    public:
        TestMaterial(Texture3D::SharedPtr texture, std::function<Bsdf*(Workspace&, Spectrum)> factory)
            : texture_(texture), factory_(factory)
        {
            assert(texture != nullptr);
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
			return factory_(workspace, texture_->Eval(isect.uv[0], isect.uv[1]));
        }

    private:
		std::function<Bsdf* (Workspace&, Spectrum)> factory_;
        Texture3D::SharedPtr texture_;
    };
} // namespace akane