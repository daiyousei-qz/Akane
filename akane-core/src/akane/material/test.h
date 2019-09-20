#pragma once
#include "akane/material.h"
#include "akane/texture.h"
#include "edslib/memory/arena.h"
#include <string>
#include <functional>

namespace akane
{
    class TestMaterial : public Material
    {
    public:
        TestMaterial(shared_ptr<Texture3D> texture,
                     std::function<Bsdf*(Workspace&, Spectrum)> factory)
            : texture_(std::move(texture)), factory_(factory)
        {
            AKANE_ASSERT(texture_ != nullptr);
        }

        const Bsdf* ComputeBsdf(Workspace& workspace,
                                const IntersectionInfo& isect) const override
        {
            return factory_(workspace, texture_->Eval(isect.uv[0], isect.uv[1]));
        }

    private:
        std::function<Bsdf*(Workspace&, Spectrum)> factory_;
        shared_ptr<Texture3D> texture_;
    };
} // namespace akane