#pragma once
#include "akane/material.h"
#include "akane/io/image.h"
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

    private:
        Spectrum EvalTexture(const Image* img, akFloat u, akFloat v) const noexcept
        {
            while (u < 0) u += 1.f;
            while (u > 1) u -= 1.f;
            while (v < 0) v += 1.f;
            while (v > 1) v -= 1.f;

            if (img != nullptr)
            {
                size_t x = u * (img->Width() - 1);
                size_t y = (1 - v) * (img->Height() - 1);

                return Spectrum{img->At(x, y, 0), img->At(x, y, 1), img->At(x, y, 2)};
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

        Image::SharedPtr texture_diffuse_  = nullptr;
        Image::SharedPtr texture_specular_ = nullptr;
    };
} // namespace akane