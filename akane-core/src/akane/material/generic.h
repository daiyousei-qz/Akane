#pragma once
#include "akane/material.h"
#include "akane/texture.h"
#include <string>
#include <functional>

namespace akane
{
    class GenericMaterial : public Material
    {
    public:
        GenericMaterial()
        {
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override;

    public:
        Spectrum EvalTexture(const Texture3D* tex, float u, float v) const
        {
            if (tex != nullptr)
            {
                return tex->Eval(u, v);
            }
            else
            {
                return Spectrum{ 1.f };
            }
        }

        std::string name_;

        Spectrum kd_;
        Spectrum ks_;
        Spectrum tr_;

        float roughness_ = .3f;
        float eta_in_    = 1.f;
        float eta_out_   = 1.f;

        shared_ptr<Texture3D> texture_diffuse_  = nullptr;
        shared_ptr<Texture3D> texture_specular_ = nullptr;
    };
} // namespace akane