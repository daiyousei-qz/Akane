#pragma once
#include "akane/core.h"
#include "akane/canvas.h"
#include "akane/camera.h"
#include "akane/sampler.h"
#include "akane/integrator.h"
#include "akane/scene.h"

namespace akane
{
    inline Canvas ExecuteRendering(const Scene& scene, const Camera& camera, Point2i resolution,
                                   int sample_per_pixel)
    {
        Canvas canvas{resolution.X(), resolution.Y()};
        RenderingContext ctx;

        //auto integrator = CreateDirectIntersectionIntegrator();
        auto integrator = CreatePathTracingIntegrator();
        auto sampler = CreateRandomSampler();

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
                    ctx.workspace.Clear();
                }

                canvas.At(x, y) = acc / static_cast<akFloat>(sample_per_pixel);
            }

			if (y % max(1, resolution.Y() / 100) == 0)
			{
				auto progress = static_cast<int>(y * 1.f / resolution.Y() * 100);
				printf("progress: %d%\n", progress);
			}
        }

        return canvas;
    }

} // namespace akane
