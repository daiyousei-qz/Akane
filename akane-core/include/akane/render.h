#pragma once
#include "akane/core.h"
#include "akane/canvas.h"
#include "akane/camera.h"
#include "akane/sampler.h"
#include "akane/integrator.h"
#include "akane/scene.h"
#include <vector>
#include <future>

namespace akane
{
    inline thread_local int GlobalThreadId = -1;

    inline void ExecuteRenderIncremental(Canvas& canvas, const Scene& scene, const Camera& camera,
                                         Point2i resolution, int sample_per_pixel)
    {
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
					auto ray = camera.SpawnRay(resolution, { x, y }, sampler->Get2D());
					auto radiance = integrator->Li(ctx, *sampler, scene, ray);

					acc += radiance;
					ctx.workspace.Clear();
				}

				canvas.At(x, y) += acc;
			}
		}
    }

    inline Canvas::SharedPtr ExecuteRenderingSingleThread(const Scene& scene, const Camera& camera,
                                                          Point2i resolution, int sample_per_pixel)
    {
        auto canvas = std::make_shared<Canvas>(resolution.X(), resolution.Y());
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

                canvas->At(x, y) = acc / static_cast<akFloat>(sample_per_pixel);
            }

            if (y % max(1, resolution.Y() / 100) == 0)
            {
                auto progress = static_cast<int>(y * 1.f / resolution.Y() * 100);
                printf("[thd %d] progress: %d%%\n", GlobalThreadId, progress);
            }
        }

        return canvas;
    }

    inline Canvas::SharedPtr ExecuteRenderingMultiThread(const Scene& scene, const Camera& camera,
                                                         Point2i resolution, int sample_per_pixel,
                                                         int thread_count)
    {
        std::atomic<int> next_thd_id = 0;
        auto execute_render          = [&](unsigned seed, bool first) {
            srand(seed);
            GlobalThreadId = next_thd_id++;

            auto ssp =
                sample_per_pixel / thread_count + (first ? sample_per_pixel % thread_count : 0);
            return ExecuteRenderingSingleThread(scene, camera, resolution, ssp);
        };

        std::vector<std::future<Canvas::SharedPtr>> futures;
        for (int i = 0; i < thread_count; ++i)
        {
            futures.push_back(std::async(execute_render, rand(), i == 0));
        }

        auto canvas = std::make_shared<Canvas>(resolution.X(), resolution.Y());
        for (auto& future : futures)
        {
            future.wait();
            auto sub_canvas = future.get();

            for (int y = 0; y < resolution.Y(); ++y)
            {
                for (int x = 0; x < resolution.X(); ++x)
                {
                    canvas->At(x, y) += sub_canvas->At(x, y) / static_cast<akFloat>(thread_count);
                }
            }
        }

        return canvas;
    }

} // namespace akane
