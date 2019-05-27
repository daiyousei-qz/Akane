#pragma once
#include "akane/material.h"
#include "akane/texture.h"
#include "akane/bsdf/lambertian.h"
#include "akane/bsdf/specular.h"
#include "akane/bsdf/microfacet.h"
#include "akane/bsdf/hybrid.h"
#include <string>
#include <functional>

namespace akane
{
    class GenericMaterial : public Material
    {
    public:
        using SharedPtr = std::shared_ptr<GenericMaterial>;

        GenericMaterial()
        {
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
            auto u = isect.uv.X();
            auto v = isect.uv.Y();

            Bsdf* bsdf         = nullptr;
            HybridBsdf* hybrid = nullptr;

            auto register_bsdf = [&](Bsdf* ff) {
                if (hybrid != nullptr)
                {
                    hybrid->Add(ff);
                }
                else if (bsdf != nullptr)
                {
                    hybrid = workspace.Construct<HybridBsdf>();
                    hybrid->Add(bsdf);
                    hybrid->Add(ff);
                }
                else
                {
                    bsdf = ff;
                }
            };

            if (tr_.Min() > 1e-5)
            {
                register_bsdf(workspace.Construct<SpecularTransmission>(tr_, eta_in_, eta_out_));
            }

            if (ks_.Max() > 1e-5)
            {
                auto albedo = ks_ * EvalTexture(tex_specular_.get(), u, v);
                if (roughness_ < 0.01f)
                {
                    register_bsdf(workspace.Construct<SpecularReflection>(albedo));
                }
                else
                {
                    auto fresnel    = workspace.Construct<Fresnel>(eta_out_, eta_in_);
                    auto microfacet = workspace.Construct<MicrofacetDistribution>(roughness_);

                    register_bsdf(
                        workspace.Construct<MicrofacetReflection>(albedo, fresnel, microfacet));
                }
            }

            if (kd_.Max() > 1e-5)
            {
                auto albedo = kd_ * EvalTexture(tex_diffuse_.get(), u, v);
                register_bsdf(workspace.Construct<LambertianReflection>(albedo));
            }

            if (hybrid != nullptr)
            {
                return hybrid;
            }
            else if (bsdf != nullptr)
            {
                return bsdf;
            }
            else
            {
                return nullptr;
            }
        }

    private:
        Spectrum EvalTexture(const Texture3D* tex, float u, float v) const noexcept
        {
            if (tex != nullptr)
            {
                return tex->Eval(u, v);
            }
            else
            {
                return {1, 1, 1};
            }
        }

    public:
        std::string name_;

        Spectrum kd_;
        Spectrum ks_;
        Spectrum tr_;

        akFloat roughness_ = .3f;
        akFloat eta_in_    = 1.f;
        akFloat eta_out_   = 1.f;

        Texture3D::SharedPtr tex_diffuse_ = nullptr;
        Texture3D::SharedPtr tex_specular_ = nullptr;
    };
} // namespace akane