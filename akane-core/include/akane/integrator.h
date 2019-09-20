#pragma once
#include "akane/common/basic.h"
#include "akane/spectrum.h"
#include "akane/ray.h"
#include "akane/sampler.h"
#include "akane/scene.h"
#include "edslib/memory/arena.h"
#include <memory>

namespace akane
{
    struct RenderingContext
    {
        Workspace workspace;
    };

    class Integrator : public Object
    {
    public:
        // compute radiance along a camera ray
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                            const Ray& camera_ray) const = 0;
    };
} // namespace akane