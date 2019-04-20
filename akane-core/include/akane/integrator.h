#pragma once
#include "akane/common/object.h"
#include "akane/spectrum.h"
#include "akane/core.h"
#include "akane/scene.h"
#include "akane/sampler.h"

namespace akane
{
    class Integrator : public Object
    {
    public:
        using Ptr = std::unique_ptr<Integrator>;

        // compute radiance along a camera ray
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler,
                            const Scene& scene, const Ray& ray) = 0;
    };

	Integrator::Ptr CreateDirectIntersectionIntegrator();
	Integrator::Ptr CreatePathTracingIntegrator();
} // namespace akane