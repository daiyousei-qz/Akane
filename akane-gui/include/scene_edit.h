#pragma once
#include "quick_imgui.h"
#include "akane/math/transform.h"
#include "akane/math/coordinate.h"
#include "akane/scene/embree_scene.h"
#include "akane/render.h"
#include "akane/debug.h"
#include <mutex>
#include <vector>
#include <chrono>

namespace akane::gui
{
    constexpr float kFrameTimeUpdateRate  = 0.7f;
    constexpr Vec3f kDefaultCameraForward = {1, 0, 0};
    constexpr Vec3f kDefaultCameraUpward  = {0, 0, 1};

    enum class PreviewIntegrator
    {
        DirectIntersection,
        PathTracing,
    };

    // state that would lead to invalidation of old render canvas
    struct RenderingState
    {
        int Version                = 0; // modify this to force re-render
        PreviewIntegrator Strategy = PreviewIntegrator::PathTracing;

        Point2i Resolution    = {200, 200};
        Point3f CameraOrigin  = {-5, 0, 1};
        Point3f CameraForward = {1, 0, 0};
        Point3f CameraUpward  = {0, 0, 1};
        Point2f CameraFov     = {.6f, .6f};
    };

    inline bool EqualState(const RenderingState& lhs, const RenderingState& rhs)
    {
        return lhs.Version == rhs.Version && lhs.Strategy == rhs.Strategy &&
               lhs.Resolution == rhs.Resolution && lhs.CameraOrigin == rhs.CameraOrigin &&
               lhs.CameraForward == rhs.CameraForward && lhs.CameraUpward == rhs.CameraUpward &&
               lhs.CameraFov == rhs.CameraFov;
    }

    class SceneEditWindow
    {
    public:
        void Initialize();
        void Update();

    public:
        void RenderOnce(Canvas* canvas, const Camera* camera, PreviewIntegrator strategy)
        {
            if (canvas != nullptr && camera != nullptr)
            {
                ExecuteRenderIncremental(*canvas, *sampler, scene, *camera,
                                         {canvas->Width(), canvas->Height()}, 1,
                                         strategy == PreviewIntegrator::DirectIntersection);
            }
        }

        void UpdateStatusControl();
        void UpdateImageControl();
        void UpdateCameraControl();
        void UpdateMaterialEditor();
        void UpdateRenderPreview();

        void RenderInBackground()
        {
            int spp = 0;
            RenderingState last_state{};
            Canvas::SharedPtr canvas =
                std::make_shared<Canvas>(last_state.Resolution.X(), last_state.Resolution.Y());

            while (true)
            {
                bool canvas_replaced = false;
                RenderingState state;
                {
                    std::unique_lock<std::mutex> lock{SceneUpdatingLock};
                    state = CurrentState;
                }

                if (!EqualState(last_state, state))
                {
                    spp = 0;

                    if (last_state.Resolution == state.Resolution)
                    {
                        canvas->Clear();
                    }
                    else
                    {
                        canvas =
                            std::make_shared<Canvas>(state.Resolution.X(), state.Resolution.Y());
                        canvas_replaced = true;
                    }
                }

                auto camera = CreatePinholeCamera(state.CameraOrigin, state.CameraForward,
                                                  state.CameraUpward, state.CameraFov);

                auto t0 = std::chrono::high_resolution_clock::now();
                RenderOnce(canvas.get(), camera.get(), state.Strategy);
                auto t1 = std::chrono::high_resolution_clock::now();

                float frame_time =
                    std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
                frame_time /= 1000.f;

                spp += 1;

                {
                    std::unique_lock<std::mutex> lock{DisplayUpdatingLock};

                    if (canvas_replaced)
                    {
                        DisplayCanvas =
                            std::make_shared<Canvas>(state.Resolution.X(), state.Resolution.Y());
                    }

                    CurrentSpp   = spp;
                    AvgFrameTime = (1.f - kFrameTimeUpdateRate) * AvgFrameTime +
                                   kFrameTimeUpdateRate * frame_time;

                    DisplayCanvas->Set(*canvas);
                }

                last_state = state;
            }
        }

    private:
        //
        //
        RenderingState MutatingState;
        RenderingState CurrentState;

        float DisplayScale = 400.f / CurrentState.Resolution.X();

        float CameraMoveRatePerSec   = 1.f;
        float CameraRotateRatePerSec = 1.f;

        //
        //
        DynamicTexture::Ptr DisplayTex;

        int n;
        Canvas::SharedPtr canvas;

        PreviewIntegrator RenderingStatus;

        int CurrentSpp;
        float AvgFrameTime = 1.f; // millisec
        Canvas::SharedPtr DisplayCanvas;

        std::mutex SceneUpdatingLock;
        std::mutex DisplayUpdatingLock;

        std::thread RenderingThd;

        //
        //
        Sampler::Ptr sampler;
        EmbreeScene scene;
        std::vector<GenericMaterial*> EditableMaterials;
    };
} // namespace akane::gui
