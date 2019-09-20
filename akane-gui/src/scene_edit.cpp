#include "scene_edit.h"
#include "akane/camera.h"
#include "akane/model.h"

using namespace std;

namespace akane::gui
{
    SceneEditWindow::~SceneEditWindow()
    {
        IsActive = false;

        if (RenderingThd.joinable())
        {
            RenderingThd.join();
        }
    }

    void SceneEditWindow::Initialize()
    {
        // load scene
        auto scene_desc = LoadSceneDesc("d:/livingroom.json");

        auto scene = make_shared<EmbreeScene>();
        for (const auto& object : scene_desc->objects)
        {
            auto transform = Transform::CreateScale(object.scale)
                                 .RotateX(object.rotation[0])
                                 .RotateY(object.rotation[1])
                                 .RotateZ(object.rotation[2])
                                 .Move(PointToVec(object.position));

            scene->AddMesh(*object.mesh, transform);
        }
        scene->Commit();

        CurrentState.ThisScene     = scene;
        CurrentState.CameraOrigin  = PointToVec(scene_desc->camera.origin);
        CurrentState.CameraForward = PointToVec(scene_desc->camera.forward);
        CurrentState.CameraUpward  = PointToVec(scene_desc->camera.upward);
        CurrentState.CameraFov     = scene_desc->camera.fov;

        // create texture
        DisplayTex = AllocateTexture(CurrentState.Resolution[0], CurrentState.Resolution[1]);

        // start background renderer
        RenderingThd = std::thread([this] { this->RenderInBackground(); });
    }

    void SceneEditWindow::Update()
    {
        // update editors
        RenderingState state = CurrentState;
        UpdateRenderingEditor(state);
        UpdateCameraEditor(state);
        UpdateMaterialEditor(state);
        // UpdateLightEditor(state);
        UpdateCameraAction(state);

        if (state != CurrentState)
        {
            std::unique_lock<std::mutex> lock{SceneUpdatingLock};
            CurrentState = state;
        }

        // update window
        UpdateStatusControl();

        // render preview
        UpdateRenderPreview();
    }

    void SceneEditWindow::UpdateRenderingEditor(RenderingState& state)
    {
        auto window = ImScoped::Window{"Renderer"};

        ImGui::InputInt2("Resolution", &ResolutionInput[0], &ResolutionInput[1]);
        if (ImGui::Button("Change Resolution"))
        {
            if (ResolutionInput[0] >= 100 && ResolutionInput[1] >= 100)
            {
                state.Resolution = ResolutionInput;
            }
        }

        ImGui::SliderFloat("Display Scale", &DisplayScale, .5f, 3.f);

        auto combo_preview = [](IntegrationMode mode) {
            switch (mode)
            {
            case IntegrationMode::NormalMapped:
                return "Normal Mapped";
            case IntegrationMode::PreviewPathTracing:
                return "Preview Path Tracing";
            case IntegrationMode::PathTracing:
                return "Path Tracing";
            default:
                return "";
            }
        }(state.Mode);

        if (auto combo = ImScoped::Combo{"Integrator", combo_preview})
        {
            if (ImGui::Selectable("Normal Mapped"))
            {
                state.Mode = IntegrationMode::NormalMapped;
            }
            if (ImGui::Selectable("Preview Path Tracing"))
            {
                state.Mode = IntegrationMode::PreviewPathTracing;
            }
            if (ImGui::Selectable("Path Tracing"))
            {
                state.Mode = IntegrationMode::PathTracing;
            }
        }
    }
    void SceneEditWindow::UpdateCameraEditor(RenderingState& state)
    {
        auto window = ImScoped::Window{"Camera"};

        Vec3 camera_origin  = state.CameraOrigin;
        Vec3 camera_forward = state.CameraForward;
        Vec3 camera_upward  = state.CameraUpward;

        ImGui::Text("Origin: (%f, %f, %f)", camera_origin.X(), camera_origin.Y(),
                    camera_origin.Z());
        ImGui::Text("Forward: (%f, %f, %f)", camera_forward.X(), camera_forward.Y(),
                    camera_forward.Z());
        ImGui::Text("Upward: (%f, %f, %f)", camera_upward.X(), camera_upward.Y(),
                    camera_upward.Z());
        ImGui::Separator();

        ImGui::SliderFloat("Fov", &state.CameraFov, 0.1f, 0.8f);
        if (ImGui::InputFloat("Fov Input", &state.CameraFov))
        {
            state.CameraFov = clamp(state.CameraFov, 0.1f, 0.8f);
        }

        ImGui::SliderFloat("Move Rate", &CameraMoveRatePerSec, 0.f, 2.f);
        ImGui::SliderFloat("Rotate Rate", &CameraRotateRatePerSec, 0.f, 2.f);
    }
    void SceneEditWindow::UpdateMaterialEditor(RenderingState& state)
    {
        /*
        ImScoped::Window window{"Material"};

        bool scene_changed = false;
        for (const auto& [name, material] : state.ThisScene->GetEditMaterials())
        {
            if (ImGui::TreeNode(name.c_str()))
            {
                if (ImGui::ColorEdit3("Kd", material->kd_.data.data()))
                {
                    scene_changed = true;
                }
                if (ImGui::ColorEdit3("Ks", material->ks_.data.data()))
                {
                    scene_changed = true;
                }
                if (ImGui::ColorEdit3("Tr", material->tr_.data.data()))
                {
                    scene_changed = true;
                }
                if (ImGui::SliderFloat("roughness", &material->roughness_, 0.f, 1.f))
                {
                    scene_changed = true;
                }
                if (ImGui::SliderFloat("eta", &material->eta_in_, 0.5f, 10.f))
                {
                    scene_changed = true;
                }

                ImGui::TreePop();
                ImGui::Separator();
            }
        }

        if (scene_changed)
        {
            state.Version += 1;
        }
        */
    }
    void SceneEditWindow::UpdateLightEditor(RenderingState& state)
    {
        /*
        const auto& light_vec = state.ThisScene->GetLights();
        if (light_vec.empty())
        {
            return;
        }

        auto area_light0      = dynamic_cast<const AreaLight*>(const_cast<Light*>(light_vec[0]));
        static float strength = 1.f;
        float multiplier      = area_light0->albedo_.Max();
        Vec3 color           = area_light0->albedo_ / multiplier;

        auto window = ImScoped::Window{"Light"};

        bool light_changed = false;
        if (ImGui::ColorPicker3("Color", color.data.data()))
        {
            light_changed = true;
        }
        if (ImGui::SliderFloat("Strength", &strength, 0.1f, 5.f))
        {
            light_changed = true;
        }

        if (light_changed)
        {
            state.Version += 1;

            for (auto light : light_vec)
            {
                auto area_light     = dynamic_cast<AreaLight*>(const_cast<Light*>(light));
                area_light->albedo_ = color * multiplier * strength;
            }
        }
        */
    }
    void SceneEditWindow::UpdateCameraAction(RenderingState& state)
    {
        Vec3 camera_origin   = state.CameraOrigin;
        Vec3 camera_forward  = state.CameraForward;
        Vec3 camera_upward   = state.CameraUpward;
        Vec3 camera_leftward = Cross(camera_forward, camera_upward);

        // handle camera movement
        constexpr int kKeyCode_Q = 81;
        constexpr int kKeyCode_W = 87;
        constexpr int kKeyCode_E = 69;
        constexpr int kKeyCode_A = 65;
        constexpr int kKeyCode_S = 83;
        constexpr int kKeyCode_D = 68;

        float camera_move_rate     = CameraMoveRatePerSec * 2.f / ImGui::GetIO().Framerate;
        float camera_move_forward  = 0.f;
        float camera_move_upward   = 0.f;
        float camera_move_leftward = 0.f;

        if (ImGui::IsKeyDown(kKeyCode_Q))
        {
            camera_move_upward += camera_move_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_W))
        {
            camera_move_forward += camera_move_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_E))
        {
            camera_move_upward -= camera_move_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_A))
        {
            camera_move_leftward -= camera_move_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_S))
        {
            camera_move_forward -= camera_move_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_D))
        {
            camera_move_leftward += camera_move_rate;
        }

        state.CameraOrigin = camera_origin + camera_forward * camera_move_forward +
                             camera_upward * camera_move_upward +
                             camera_leftward * camera_move_leftward;

        // handle camera rotation
        constexpr int kKeyCode_ArrowUp    = 38;
        constexpr int kKeyCode_ArrowDown  = 40;
        constexpr int kKeyCode_ArrowLeft  = 37;
        constexpr int kKeyCode_ArrowRight = 39;
        constexpr int kKeyCode_Comma      = 188;
        constexpr int kKeyCode_Period     = 190;

        float camera_rotate_rate       = CameraRotateRatePerSec / ImGui::GetIO().Framerate;
        float camera_rotate_horizontal = 0.f;
        float camera_rotate_vertical   = 0.f;
        float camera_rotate_view       = 0.f;
        if (ImGui::IsKeyDown(kKeyCode_ArrowUp))
        {
            camera_rotate_vertical += camera_rotate_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_ArrowDown))
        {
            camera_rotate_vertical -= camera_rotate_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_ArrowLeft))
        {
            camera_rotate_horizontal += camera_rotate_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_ArrowRight))
        {
            camera_rotate_horizontal -= camera_rotate_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_Comma))
        {
            camera_rotate_view += camera_rotate_rate;
        }
        if (ImGui::IsKeyDown(kKeyCode_Period))
        {
            camera_rotate_view -= camera_rotate_rate;
        }

        // LocalCoordinateTransform camera_coord(camera_forward, camera_leftward, camera_upward);
        auto camera_coord            = Transform(camera_forward, camera_leftward, camera_upward);
        auto camera_rotate_transform = Transform::Identity()
                                           .RotateZ(camera_rotate_horizontal)
                                           .RotateY(camera_rotate_vertical)
                                           .RotateX(camera_rotate_view);

        state.CameraForward =
            camera_coord.InverseLinear(camera_rotate_transform.ApplyLinear(kDefaultCameraForward));
        state.CameraUpward =
            camera_coord.InverseLinear(camera_rotate_transform.ApplyLinear(kDefaultCameraUpward));
    }

    void SceneEditWindow::UpdateStatusControl()
    {
        ImGui::Begin("Status");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Renderer average %.3f ms/frame (%.1f FPS)", AvgFrameTime,
                    1000.f / AvgFrameTime);

        ImGui::Text("Culmulative Sample: %d", CurrentSpp);

        ImGui::End();
    }
    void SceneEditWindow::UpdateRenderPreview()
    {
        if (DisplayCanvas != nullptr)
        {
            std::unique_lock<std::mutex> lock{DisplayUpdatingLock};

            if (DisplayTex->Width() != DisplayCanvas->Width() ||
                DisplayTex->Height() != DisplayCanvas->Height())
            {
                DisplayTex = AllocateTexture(DisplayCanvas->Width(), DisplayCanvas->Height());
            }

            DisplayTex->Update([&](void* p, int row_pitch) {
                auto buf = reinterpret_cast<uint32_t*>(p);

                for (int y = 0; y < DisplayTex->Height(); ++y)
                {
                    for (int x = 0; x < DisplayTex->Width(); ++x)
                    {
                        auto spectrum = DisplayCanvas->GetPixel(x, y) * (1.f / CurrentSpp);
                        auto rgb      = SpectrumToRGB(Linear2sRGB(ToneMap_Aces(spectrum)));

                        uint32_t rr    = rgb[0];
                        uint32_t gg    = rgb[1];
                        uint32_t bb    = rgb[2];
                        uint32_t pixel = (0xffu << 24) | (bb << 16) | (gg << 8) | rr;

                        buf[y * row_pitch / 4 + x] = pixel;
                    }
                }

                static int cnt = 0;
                if (CurrentState.Mode == IntegrationMode::PreviewPathTracing)
                {
                    cnt += 1;
                    if (cnt == 100) {
                        DisplayCanvas->SaveImage("d:/workspace/gui_dump.png", 1.f / CurrentSpp);
                    }
                }
            });
        }

        ImScoped::Window window{"Preview"};
        ImGui::Image(DisplayTex->Id(), {DisplayScale * CurrentState.Resolution[0],
                                        DisplayScale * CurrentState.Resolution[1]});
    }

    template <bool Overwrite>
    inline void RenderFrame(Canvas& canvas, Sampler& sampler, Integrator& integrator,
                            const Scene& scene, const Camera& camera, int spp = 1,
                            std::function<bool()>* activity_query = nullptr)
    {
        RenderingContext ctx;

        for (int y = 0; y < canvas.Height(); ++y)
        {
            for (int x = 0; x < canvas.Width(); ++x)
            {
                Spectrum acc = 0.f;
                for (int i = 0; i < spp; ++i)
                {
                    auto resolution = Point2i{canvas.Width(), canvas.Height()};
                    auto uv         = ComputeScreenSpaceUV({x, y}, resolution, sampler.Get2D());
                    auto ray        = camera.SpawnRay(uv);
                    auto radiance   = integrator.Li(ctx, sampler, scene, ray);

                    acc += radiance;
                }

                if constexpr (Overwrite)
                {
                    canvas.SetPixel(x, y, acc);
                }
                else
                {
                    canvas.IncrementPixel(x, y, acc);
                }
            }

            if (activity_query && !(*activity_query)())
            {
                return;
            }
        }
    }

    void SceneEditWindow::RenderInBackground()
    {
        int spp = 0;
        RenderingState last_state{};
        shared_ptr<Canvas> canvas =
            make_shared<Canvas>(last_state.Resolution[0], last_state.Resolution[1]);

        std::function<bool()> activity_query = [&] { return IsActive; };

        while (IsActive)
        {
            // fetch latest rendering parameters
            RenderingState state;
            {
                std::unique_lock<std::mutex> lock{SceneUpdatingLock};
                state = CurrentState;
            }

            // detect if canvas has to be altered
            bool canvas_replaced = false;
            if (last_state != state)
            {
                spp = 0;

                if (last_state.Resolution != state.Resolution)
                {
                    canvas = std::make_shared<Canvas>(state.Resolution[0], state.Resolution[1]);
                    canvas_replaced = true;
                }
            }

            // render a frame
            auto aspect_ratio =
                static_cast<float>(state.Resolution[1]) / static_cast<float>(state.Resolution[0]);
            auto camera      = CreatePinholeCamera(state.CameraOrigin, state.CameraForward,
                                              state.CameraUpward, state.CameraFov, aspect_ratio);
            auto& integrator = state.Mode == IntegrationMode::NormalMapped
                                   ? *NormalMappedIntegrator
                                   : state.Mode == IntegrationMode::PreviewPathTracing
                                         ? *PreviewPathTracingIntegrator
                                         : *PathTracingIntegrator;

            auto t0 = std::chrono::high_resolution_clock::now();
            if (spp == 0)
            {
                RenderFrame<true>(*canvas, *sampler, integrator, *state.ThisScene, *camera, 1,
                                  &activity_query);
            }
            else
            {
                RenderFrame<false>(*canvas, *sampler, integrator, *state.ThisScene, *camera, 1,
                                   &activity_query);
            }
            if (!IsActive)
            {
                return;
            }
            auto t1 = std::chrono::high_resolution_clock::now();

            float frame_time =
                std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
            frame_time /= 1000.f;

            spp += 1;

            // update display buffer
            {
                std::unique_lock<std::mutex> lock{DisplayUpdatingLock};

                if (canvas_replaced || DisplayCanvas == nullptr)
                {
                    DisplayCanvas =
                        std::make_shared<Canvas>(state.Resolution[0], state.Resolution[1]);
                }

                CurrentSpp = spp;
                AvgFrameTime =
                    (1.f - kFrameTimeUpdateRate) * AvgFrameTime + kFrameTimeUpdateRate * frame_time;

                DisplayCanvas->Set(*canvas);
            }

            // store state
            last_state = state;
        }
    }
} // namespace akane::gui