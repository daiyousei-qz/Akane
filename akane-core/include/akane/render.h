#pragma once
#include "akane/core.h"
#include "akane/canvas.h"
#include "akane/camera.h"
#include "akane/sampler.h"
#include "akane/integrator.h"
#include "akane/scene.h"
#include "fmt/format.h"
#include <vector>
#include <functional>
#include <future>

namespace akane
{
    struct RenderResult
    {
        int ssp                  = 0;
        Canvas::SharedPtr canvas = nullptr;
    };

    inline RenderResult
    ExecuteRenderingSingleThread(const Scene& scene, const Camera& camera, Point2i resolution,
                                 int sample_per_pixel, int thread_id, unsigned seed,
                                 std::function<bool()>* activity_query,
                                 std::function<void(int, const RenderResult&)>* checkpoint_handler)
    {
        RenderResult result{};
        result.canvas = std::make_shared<Canvas>(resolution.X(), resolution.Y());

        RenderingContext ctx;
        auto working_canvas = std::make_shared<Canvas>(resolution.X(), resolution.Y());

        // auto integrator = CreateDirectIntersectionIntegrator();
        auto integrator = CreatePathTracingIntegrator();
        auto sampler    = CreateRandomSampler(seed);

        int ssp_per_batch =
            static_cast<int>(ceil(sample_per_pixel / min(sample_per_pixel * 1.f, 100.f)));

        int progress = 0;
        for (int batch = 0; batch < 100; ++batch)
        {
            auto ssp_this_batch = min(ssp_per_batch, sample_per_pixel - result.ssp);
            if (ssp_this_batch <= 0)
            {
                break;
            }

            bool active = true;
            for (int y = 0; active && y < resolution.Y(); ++y)
            {
                for (int x = 0; active && x < resolution.X(); ++x)
                {
                    double rr = 0;
                    double gg = 0;
                    double bb = 0;
                    for (int i = 0; i < ssp_this_batch; ++i)
                    {
                        auto ray      = camera.SpawnRay(resolution, {x, y}, sampler->Get2D());
                        auto radiance = integrator->Li(ctx, *sampler, scene, ray);

                        rr += static_cast<double>(radiance[0]);
                        gg += static_cast<double>(radiance[1]);
                        bb += static_cast<double>(radiance[2]);
                        ctx.workspace.Clear();
                    }

                    working_canvas->Append(rr, gg, bb, x, y);
                }

                if (activity_query != nullptr && !(*activity_query)())
                {
                    active = false;
                }
            }

            if (active)
            {
                result.ssp += ssp_this_batch;
                result.canvas->Set(*working_canvas);

                progress = static_cast<int>(result.ssp * 100.f / sample_per_pixel);
            }
            else
            {
                break;
            }

            if (checkpoint_handler != nullptr)
            {
                (*checkpoint_handler)(progress, result);
            }

            fmt::print("[thd {}] {} ssp finished({}%)\n", thread_id, result.ssp, progress);
        }

        fmt::print("[thd {}] exit, {} ssp finished({}%)\n", thread_id, result.ssp, progress);
        return result;
    }

    inline RenderResult ExecuteRenderingMultiThread(
        const Scene& scene, const Camera& camera, Point2i resolution, int sample_per_pixel,
        int thread_count, std::function<bool()>* activity_query = nullptr,
        std::function<void(int, const RenderResult&)>* checkpoint_handler = nullptr)
    {
        auto execute_render = [&](unsigned seed, int ssp, int id) {
            fmt::print("[thd {}] assigned with {} ssp, start working...\n", id, ssp);

            return ExecuteRenderingSingleThread(scene, camera, resolution, ssp, id, seed,
                                                activity_query, checkpoint_handler);
        };

        std::random_device rnd{};
        std::vector<std::future<RenderResult>> futures;
        for (int i = 0; i < thread_count; ++i)
        {
            auto ssp =
                sample_per_pixel / thread_count + (i == 0 ? sample_per_pixel % thread_count : 0);
            futures.push_back(std::async(execute_render, rnd(), ssp, i));
        }

        auto ssp    = 0;
        auto canvas = std::make_shared<Canvas>(resolution.X(), resolution.Y());
        for (auto& future : futures)
        {
            future.wait();
            auto result = future.get();

            ssp += result.ssp;
            canvas->Append(*result.canvas);
        }

        return RenderResult{ssp, canvas};
    }

} // namespace akane
