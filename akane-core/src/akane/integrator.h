#pragma once
#include "akane/spectrum.h"
#include "akane/core.h"
#include "akane/scene.h"
#include "akane/sampler.h"

namespace akane
{
    class Integrator
    {
    public:
        // compute radiance along a camera ray
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler,
                            const Scene& scene, const Ray& ray);
    };

    std::unique_ptr<Integrator> CreatePathTracingIntegrator();

    std::unique_ptr<Integrator> CreateDefaultIntegrator()
    {
        return CreatePathTracingIntegrator();
    }
} // namespace akane