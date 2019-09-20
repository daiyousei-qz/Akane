#pragma once
#include "akane/integrator.h"
#include "akane/light.h"
#include "akane/material.h"
#include "akane/math/transform.h"
#include "akane/bsdf/bsdf_geometry.h"

namespace akane
{
    class PathTracingIntegrator : public Integrator
    {
    public:
        PathTracingIntegrator(int min_bounce = 2, int max_bounce = 6)
            : min_bounce_(min_bounce), max_bounce_(max_bounce)
        {
            AKANE_REQUIRE(min_bounce > 0 && max_bounce >= min_bounce);
        }

        Spectrum SampleAllDirectLight(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                                      const IntersectionInfo& isect, const Vec3& wo,
                                      const Bsdf& bsdf, const Transform& world2local) const;

        Spectrum SampleRandomDirectLight(RenderingContext& ctx, Sampler& sampler,
                                         const Scene& scene, const IntersectionInfo& isect,
                                         const Vec3& wo, const Bsdf& bsdf,
                                         const Transform& world2local) const;

        Spectrum Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                    const Ray& camera_ray) const override;

    private:
        int min_bounce_ = 1;
        int max_bounce_ = 1;
    };
} // namespace akane