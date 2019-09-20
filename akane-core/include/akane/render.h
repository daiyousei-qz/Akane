#pragma once
#include "akane/canvas.h"
#include "akane/camera.h"
#include "akane/scene.h"
#include "akane/integrator.h"
#include <memory>
#include <vector>
#include <future>
#include <functional>

namespace akane
{
    struct RenderResult
    {
        int ssp                  = 0;
        shared_ptr<Canvas> canvas = nullptr;
    };

    RenderResult ExecuteRenderingSingleThread(
        const Integrator& integrator, const Scene& scene, const Camera& camera, Point2i resolution,
        int sample_per_pixel, int thread_id, unsigned seed, std::function<bool()>* activity_query,
        std::function<void(int, const RenderResult&)>* checkpoint_handler);

    RenderResult ExecuteRenderingMultiThread(
        const Integrator& integrator, const Scene& scene, const Camera& camera, Point2i resolution,
        int sample_per_pixel, int thread_count, std::function<bool()>* activity_query = nullptr,
        std::function<void(int, const RenderResult&)>* checkpoint_handler = nullptr);

} // namespace akane
