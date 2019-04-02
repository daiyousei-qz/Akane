#pragma once
#include "akane/integrator.h"
#include "akane/film.h"
#include "akane/camera.h"

namespace akane
{
    inline Film ExecuteRendering(const Scene& scene, const Camera& camera, int width,
                          int height, int sample_per_pixel)
    {
        Film film{width, height};
        RenderingContext ctx;

        auto integrator = CreateDefaultIntegrator();
        auto sampler    = CreateDefaultSampler();

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                Spectrum acc = kFloatZero;
                for (int i = 0; i < sample_per_pixel; ++i)
                {
                    auto ray = camera.SpawnRay(sampler, width, height, x, y);
                    auto radiance = integrator->Li(ctx, sampler, scene, ray);

					acc += radiance;
                }

				film.Set(acc / static_cast<akFloat>(sample_per_pixel));
            }
        }

		return film;
    }
} // namespace akane