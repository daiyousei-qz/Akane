#pragma once
#include "akane/core.h"
#include "akane/canvas.h"
#include "akane/camera.h"
#include "akane/sampler.h"
#include "akane/integrator.h"
#include "akane/scene.h"
#include <functional>

namespace akane::gui
{
    template <bool Overwrite>
    inline void RenderFrame(Canvas& canvas, Sampler& sampler, Integrator& integrator,
                            const Scene& scene, const Camera& camera, int spp = 1,
                            std::function<bool()>* activity_query = nullptr)
    {
        RenderingContext ctx;

        Point2i resolution = {canvas.Width(), canvas.Height()};
        for (int y = 0; y < resolution.Y(); ++y)
        {
            for (int x = 0; x < resolution.X(); ++x)
            {
                Spectrum acc = kFloatZero;
                for (int i = 0; i < spp; ++i)
                {
                    auto ray      = camera.SpawnRay(resolution, {x, y}, sampler.Get2D());
                    auto radiance = integrator.Li(ctx, sampler, scene, ray);

                    acc += radiance;
                    ctx.workspace.Clear();
                }

                if constexpr (Overwrite)
                {
                    canvas.Set(acc[0], acc[1], acc[2], x, y);
                }
                else
                {
                    canvas.Append(acc[0], acc[1], acc[2], x, y);
                }
            }

            if (activity_query && !(*activity_query)())
            {
                return;
            }
        }
    }
} // namespace akane::gui