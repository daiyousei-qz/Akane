#pragma once
#include "quick_imgui.h"
#include "akane/math/transform.h"
#include "akane/math/coordinate.h"
#include "akane/scene/embree_scene.h"
#include "akane/render.h"
#include "akane/debug.h"
#include <mutex>
#include <vector>

namespace akane::gui
{
    constexpr Vec3f kDefaultCameraForward = {1, 0, 0};
    constexpr Vec3f kDefaultCameraUpward  = {0, 0, 1};

    enum class RenderingStrategy
    {
        DirectIntersection,
        PathTracing,
    };

    // state that would lead to invalidation of old render canvas
    struct RenderingState
    {
        int Version                = 0; // modify this to force re-render
        RenderingStrategy Strategy = RenderingStrategy::DirectIntersection;

        Point2i Resolution    = {200, 200};
        Point3f CameraOrigin  = {-5, 0, 1};
        Point3f CameraForward = {1, 0, 0};
        Point3f CameraUpward  = {0, 0, 1};
        Point2f CameraFov     = {.6f, .6f};

        int ResolutionWidth  = 200;
        int ResolutionHeight = 200;

        float CameraOriginX = -5;
        float CameraOriginY = 0;
        float CameraOriginZ = 1;

        float CameraForwardX = 1;
        float CameraForwardY = 0;
        float CameraForwardZ = 0;

        float CameraUpwardX = 0;
        float CameraUpwardY = 0;
        float CameraUpwardZ = 1;

        float CameraFovX = .6f;
        float CameraFovY = .6f;
    };

    bool EqualState(const RenderingState& lhs, const RenderingState& rhs)
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
        Vec3f CurrentCameraOrigin()
        {
            return Vec3f{CurrentState.CameraOriginX, CurrentState.CameraOriginY,
                         CurrentState.CameraOriginZ};
        }
        Vec3f CurrentCameraForward()
        {
            return Vec3f{CurrentState.CameraForwardX, CurrentState.CameraForwardY,
                         CurrentState.CameraForwardZ};
        }
        Vec3f CurrentCameraUpward()
        {
            return Vec3f{CurrentState.CameraUpwardX, CurrentState.CameraUpwardY,
                         CurrentState.CameraUpwardZ};
        }
        Vec2f CurrentCameraFov()
        {
            return Vec2f{CurrentState.CameraFovX, CurrentState.CameraFovY};
        }

        bool DetectGlobalResolutionChange()
        {
            return CurrentState.ResolutionWidth != PreviousState.ResolutionWidth ||
                   CurrentState.ResolutionHeight != PreviousState.ResolutionHeight;
        }
        bool DetectGlobalStateChange()
        {
            return CurrentState.Version != PreviousState.Version ||
                   CurrentState.ResolutionWidth != PreviousState.ResolutionWidth ||
                   CurrentState.ResolutionHeight != PreviousState.ResolutionHeight ||
                   CurrentState.CameraOriginX != PreviousState.CameraOriginX ||
                   CurrentState.CameraOriginY != PreviousState.CameraOriginY ||
                   CurrentState.CameraOriginZ != PreviousState.CameraOriginZ ||
                   CurrentState.CameraForwardX != PreviousState.CameraForwardX ||
                   CurrentState.CameraForwardY != PreviousState.CameraForwardY ||
                   CurrentState.CameraForwardZ != PreviousState.CameraForwardZ ||
                   CurrentState.CameraUpwardX != PreviousState.CameraUpwardX ||
                   CurrentState.CameraUpwardY != PreviousState.CameraUpwardY ||
                   CurrentState.CameraUpwardZ != PreviousState.CameraUpwardZ ||
                   CurrentState.CameraFovX != PreviousState.CameraFovX ||
                   CurrentState.CameraFovY != PreviousState.CameraFovY;
        }

        void ForceRestartRender()
        {
            CurrentState.Version += 1;
        }

        void RenderOnce(Canvas* canvas, const Camera* camera)
        {
            if (canvas != nullptr && camera != nullptr)
            {
                ExecuteRenderIncremental(
                    *canvas, *sampler, scene, *camera,
                    {CurrentState.ResolutionWidth, CurrentState.ResolutionHeight}, 1, false);
            }
        }

        void UpdateStatusControl();
        void UpdateImageControl();
        void UpdateCameraControl();
        void UpdateMaterialEditor();
        void UpdateRenderPreview();

        void RenderInBackground()
        {
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
                RenderOnce(canvas.get(), camera.get());

                {
                    std::unique_lock<std::mutex> lock{ DisplayUpdatingLock };

                    if (canvas_replaced)
                    {
                        DisplayCanvas =
                            std::make_shared<Canvas>(state.Resolution.X(), state.Resolution.Y());
                    }

                    CurrentSpp += 1;
                    DisplayCanvas->Set(*canvas);
                }
            }
        }

    private:
        //
        //
        RenderingState CurrentState;
        RenderingState PreviousState;

        float DisplayScale = 400.f / CurrentState.ResolutionWidth;

        float CameraMoveRatePerSec   = 1.f;
        float CameraRotateRatePerSec = 1.f;

        //
        //
        DynamicTexture::Ptr DisplayTex;

        int n;
        Canvas::SharedPtr canvas;

        RenderingStrategy RenderingStatus;

        int CurrentSpp;
        Canvas::SharedPtr DisplayCanvas;
        Canvas::SharedPtr RenderingCanvas;

        std::mutex SceneUpdatingLock;
        std::mutex DisplayUpdatingLock;

        //
        //
        Sampler::Ptr sampler;
        EmbreeScene scene;
        std::vector<GenericMaterial*> EditableMaterials;
    };
} // namespace akane::gui
