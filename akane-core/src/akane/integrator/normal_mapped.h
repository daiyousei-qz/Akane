#pragma once
#include "akane/integrator.h"

namespace akane
{
    class NormalMappedIntegrator : public Integrator
    {
    public:
        NormalMappedIntegrator() = default;

        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                            const Ray& camera_ray) const override
        {
            ctx.workspace.Clear();

            IntersectionInfo isect;
            if (scene.Intersect(camera_ray, ctx.workspace, isect))
            {
                return (isect.ns.Normalized() + Vec3{1.f, 1.f, 1.f}) / 2.f;
            }
            else
            {
                return kBlackSpectrum;
            }
        }
    };
} // namespace akane