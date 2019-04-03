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
                                   int width, int height, int sample_per_pixel)
    {
        Canvas canvas{width, height};
        RenderingContext ctx;

        auto integrator = CreatePathTracingIntegrator();
        auto sampler    = CreateRandomSampler();

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                Spectrum acc = kFloatZero;
                for (int i = 0; i < sample_per_pixel; ++i)
                {
                    auto ray = camera.SpawnRay(*sampler, width, height, x, y);
                    auto radiance = integrator->Li(ctx, *sampler, scene, ray);

                    acc += radiance;
                }

                canvas.At(x, y) = acc / static_cast<akFloat>(sample_per_pixel);
            }
        }

        return canvas;
    }

} // namespace akane

