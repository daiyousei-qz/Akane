#pragma once
#include "quick_imgui.h"
#include "render.h"
#include "akane/math/transform.h"
#include "akane/math/coordinate.h"
#include "akane/scene/embree_scene.h"
#include "akane/debug.h"
#include <mutex>
#include <vector>
#include <unordered_map>

namespace akane::gui
{
    constexpr float kFrameTimeUpdateRate = 0.7f;
    constexpr Point2i kDefaultResolution = {200, 200};

    constexpr Vec3f kDefaultCameraOrigin  = {-5, 0, 1};
    constexpr Vec3f kDefaultCameraForward = {1, 0, 0};
    constexpr Vec3f kDefaultCameraUpward  = {0, 0, 1};
    constexpr float kDefaultCameraFov     = .5f;

    enum class PreviewIntegrator
    {
        DirectIntersection,
        PathTracing,
    };

    // state that would lead to invalidation of old render canvas
    struct RenderingState
    {
        int Version = 0; // modify this to force re-render

        Scene::SharedPtr ThisScene = nullptr;

        PreviewIntegrator IntegrationMode = PreviewIntegrator::DirectIntersection;

        Point2i Resolution  = kDefaultResolution;
        Vec3f CameraOrigin  = kDefaultCameraOrigin;
        Vec3f CameraForward = kDefaultCameraForward;
        Vec3f CameraUpward  = kDefaultCameraUpward;
        float CameraFov     = kDefaultCameraFov;
    };

    inline bool EqualState(const RenderingState& lhs, const RenderingState& rhs)
    {
        return lhs.Version == rhs.Version && lhs.ThisScene == rhs.ThisScene &&
               lhs.IntegrationMode == rhs.IntegrationMode && lhs.Resolution == rhs.Resolution &&
               lhs.CameraOrigin == rhs.CameraOrigin && lhs.CameraForward == rhs.CameraForward &&
               lhs.CameraUpward == rhs.CameraUpward && lhs.CameraFov == rhs.CameraFov;
    }

    class SceneEditWindow
    {
    public:
        SceneEditWindow() = default;
        ~SceneEditWindow()
        {
            IsActive = false;

            if (RenderingThd.joinable())
            {
                RenderingThd.join();
            }
        }

        void Initialize();
        void Update();

    public:
        void UpdateStatusControl();
        void UpdateRenderPreview();

        void UpdateRenderingEditor(RenderingState& state);
        void UpdateCameraEditor(RenderingState& state);
        void UpdateMaterialEditor(RenderingState& state);
        void UpdateLightEditor(RenderingState& state);
        void UpdateCameraAction(RenderingState& state);

        void RenderInBackground();

    private:
        bool IsActive = true;

        //
        //
        RenderingState CurrentState;

        Point2i ResolutionInput = CurrentState.Resolution;
        float DisplayScale      = 400.f / CurrentState.Resolution.X();

        float CameraMoveRatePerSec   = 1.f;
        float CameraRotateRatePerSec = 1.f;

        //
        //
        DynamicTexture::Ptr DisplayTex = nullptr;

        Integrator::Ptr DirectIntersectionIntegrator = CreateDirectIntersectionIntegrator();
        Integrator::Ptr PathTracingIntegrator        = CreatePathTracingIntegrator();

        int CurrentSpp                  = 0;
        float AvgFrameTime              = 1.f; // millisec
        Canvas::SharedPtr DisplayCanvas = nullptr;

        std::mutex SceneUpdatingLock;
        std::mutex DisplayUpdatingLock;

        std::thread RenderingThd;

        //
        //
        Sampler::Ptr sampler;
        // EmbreeScene scene;

        std::string SceneName = "";
        std::shared_ptr<EmbreeScene> scene;
    };
} // namespace akane::gui
