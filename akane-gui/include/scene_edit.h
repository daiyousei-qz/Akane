#pragma once
#include "quick_imgui.h"
#include "render.h"
#include "akane/math/transform.h"
#include "akane/math/coordinate.h"
#include "akane/scene/embree_scene.h"
#include "akane/debug.h"
#include <mutex>
#include <vector>

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
        int Version = 0; // modify this to force re-render

        PreviewIntegrator IntegrationMode = PreviewIntegrator::PathTracing;

        Point2i Resolution    = {200, 200};
        Point3f CameraOrigin  = {-5, 0, 1};
        Point3f CameraForward = {1, 0, 0};
        Point3f CameraUpward  = {0, 0, 1};
        Point2f CameraFov     = {.6f, .6f};
    };

    inline bool EqualState(const RenderingState& lhs, const RenderingState& rhs)
    {
        return lhs.Version == rhs.Version && lhs.IntegrationMode == rhs.IntegrationMode &&
               lhs.Resolution == rhs.Resolution && lhs.CameraOrigin == rhs.CameraOrigin &&
               lhs.CameraForward == rhs.CameraForward && lhs.CameraUpward == rhs.CameraUpward &&
               lhs.CameraFov == rhs.CameraFov;
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
        void UpdateRenderingEditor();
        void UpdateCameraEditor();
        void UpdateMaterialEditor();
        void UpdateRenderPreview();

        void RenderInBackground();

    private:
        bool IsActive = true;

        //
        //
        RenderingState MutatingState;
        RenderingState CurrentState;

        Point2i ResolutionInput = CurrentState.Resolution;
        float DisplayScale = 400.f / CurrentState.Resolution.X();

        float CameraMoveRatePerSec   = 1.f;
        float CameraRotateRatePerSec = 1.f;

        //
        //
        DynamicTexture::Ptr DisplayTex;

        Integrator::Ptr DirectIntersectionIntegrator = CreateDirectIntersectionIntegrator();
        Integrator::Ptr PathTracingIntegrator        = CreatePathTracingIntegrator();

        int CurrentSpp = 0;
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
