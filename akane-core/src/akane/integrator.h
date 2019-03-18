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
        virtual Spectrum Li(RenderingContext& ctx, const Ray& ray, const Scene& scene, const Sampler& sampler);
	};
}