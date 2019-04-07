#pragma once
#include "akane/core.h"
#include "akane/scene.h"
#include "akane/canvas.h"
#include "akane/camera.h"
#include "akane/sampler.h"
#include "akane/integrator.h"

namespace akane
{
    inline Canvas ExecuteRendering(const Scene& scene, const Camera& camera,
                                   Point2i resolution, int sample_per_pixel)
    {
        Canvas canvas{resolution.X(), resolution.Y()};
        RenderingContext ctx;

        // auto integrator = CreateDirectIntersectionIntegrator();
        auto integrator = CreatePathTracingIntegrator();
        auto sampler    = CreateRandomSampler();

        for (int y = 0; y < resolution.Y(); ++y)
        {
            for (int x = 0; x < resolution.X(); ++x)
            {
                Spectrum acc = kFloatZero;
                for (int i = 0; i < sample_per_pixel; ++i)
                {
                    auto ray      = camera.SpawnRay(resolution, {x, y}, sampler->Get2D());
                    auto radiance = integrator->Li(ctx, *sampler, scene, ray);

                    acc += radiance;
                }

                canvas.At(x, y) = acc / static_cast<akFloat>(sample_per_pixel);
            }
        }

        return canvas;
    }

} // namespace akane
