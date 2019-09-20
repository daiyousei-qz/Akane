#pragma once
#include "akane/render.h"
#include "akane/integrator/normal_mapped.h"
#include "akane/integrator/path_tracing.h"
#include "akane/scene/embree.h"
#include "quick_imgui.h"

namespace akane::gui
{
    constexpr float kFrameTimeUpdateRate = 0.7f;
    constexpr Point2i kDefaultResolution = {200, 200};

    constexpr Vec3 kDefaultCameraOrigin  = {-5, 0, 1};
    constexpr Vec3 kDefaultCameraForward = {1, 0, 0};
    constexpr Vec3 kDefaultCameraUpward  = {0, 0, 1};
    constexpr float kDefaultCameraFov    = .5f;

    enum class IntegrationMode
    {
        NormalMapped,
        PreviewPathTracing,
        PathTracing,
    };

    // state that would lead to invalidation of old render canvas
    struct RenderingState
    {
        int Version = 0; // modify this to force re-render

        shared_ptr<Scene> ThisScene = nullptr;

        IntegrationMode Mode = IntegrationMode::NormalMapped;

        Point2i Resolution = kDefaultResolution;
        Vec3 CameraOrigin  = kDefaultCameraOrigin;
        Vec3 CameraForward = kDefaultCameraForward;
        Vec3 CameraUpward  = kDefaultCameraUpward;
        float CameraFov    = kDefaultCameraFov;
    };

    inline bool operator==(const RenderingState& lhs, const RenderingState& rhs)
    {
        return lhs.Version == rhs.Version && lhs.ThisScene == rhs.ThisScene &&
               lhs.Mode == rhs.Mode && lhs.Resolution == rhs.Resolution &&
               lhs.CameraOrigin == rhs.CameraOrigin && lhs.CameraForward == rhs.CameraForward &&
               lhs.CameraUpward == rhs.CameraUpward && lhs.CameraFov == rhs.CameraFov;
    }
    inline bool operator!=(const RenderingState& lhs, const RenderingState& rhs)
    {
        return !(lhs == rhs);
    }

    class SceneEditWindow
    {
    public:
        SceneEditWindow() = default;
        ~SceneEditWindow();

        void Initialize();
        void Update();

    private:
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
        float DisplayScale      = 400.f / CurrentState.Resolution[0];

        float CameraMoveRatePerSec   = 1.f;
        float CameraRotateRatePerSec = 1.f;

        //
        //
        DynamicTexture::Ptr DisplayTex = nullptr;

        unique_ptr<Integrator> NormalMappedIntegrator =
            make_unique<akane::NormalMappedIntegrator>();
        unique_ptr<Integrator> PreviewPathTracingIntegrator =
            make_unique<akane::PathTracingIntegrator>(1, 1);
        unique_ptr<Integrator> PathTracingIntegrator =
            make_unique<akane::PathTracingIntegrator>(2, 6);

        int CurrentSpp                   = 0;
        float AvgFrameTime               = 1.f; // millisec
        shared_ptr<Canvas> DisplayCanvas = nullptr;

        std::mutex SceneUpdatingLock{};
        std::mutex DisplayUpdatingLock{};

        std::thread RenderingThd;

        //
        //
        unique_ptr<Sampler> sampler = CreateRandomSampler(rand());
    };

} // namespace akane::gui