#include "akane/material/generic.h"
#include "akane/bsdf/lambertian.h"
#include "akane/bsdf/specular.h"
#include "akane/bsdf/microfacet.h"
#include "akane/bsdf/hybrid.h"

namespace akane
{
    const Bsdf* GenericMaterial::ComputeBsdf(Workspace& workspace,
                                             const IntersectionInfo& isect) const
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
            auto albedo = ks_ * EvalTexture(texture_specular_.get(), u, v);
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
            auto albedo = kd_ * EvalTexture(texture_diffuse_.get(), u, v);
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
} // namespace akane