#include "scene_edit.h"

using namespace std;

namespace akane::gui
{
    void SceneEditWindow::Initialize()
    {
        // load fonts
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("d:/fonts/simhei.ttf", 16.0f, nullptr,
                                     io.Fonts->GetGlyphRangesDefault());

        // load scene
        sampler = CreateRandomSampler(rand());

        CreateScene_Default(scene);
        scene.Commit();

        EditableMaterials = scene.GetEditMaterials();

        // create
        auto resolution = CurrentState.Resolution;
        DisplayTex      = AllocateTexture(resolution.X(), resolution.Y());

        CurrentSpp    = 0;
        DisplayCanvas = std::make_shared<Canvas>(resolution.X(), resolution.Y());

        // start background renderer
        RenderingThd = std::thread([this] { this->RenderInBackground(); });
    }

    void SceneEditWindow::Update()
    {
        // backup state
        MutatingState = CurrentState;

        // update window
        UpdateStatusControl();
        UpdateImageControl();
        UpdateCameraControl();
        UpdateMaterialEditor();

        if (!EqualState(MutatingState, CurrentState))
        {
            std::unique_lock<std::mutex> lock{SceneUpdatingLock};
            CurrentState = MutatingState;
        }

        // render preview
        UpdateRenderPreview();
    }

    void SceneEditWindow::UpdateStatusControl()
    {
        ImGui::Begin("Status");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Renderer average %.3f ms/frame (%.1f FPS)",
            AvgFrameTime, 1000.f / AvgFrameTime);

        ImGui::End();
    }
    void SceneEditWindow::UpdateImageControl()
    {
        ImScoped::Window window{ "Renderer" };

        int resolution[2]{MutatingState.Resolution.X(), MutatingState.Resolution.Y()};
        ImGui::InputInt2("Resolution", resolution);
        if (ImGui::Button("Change Resolution"))
        {
            if (resolution[0] >= 100 && resolution[1] >= 100)
            {
                // TODO: support resolution change
                MutatingState.Resolution = {resolution[0], resolution[1]};

                DisplayTex =
                    AllocateTexture(MutatingState.Resolution.X(), MutatingState.Resolution.Y());
            }
        }

        ImGui::SliderFloat("Display Scale", &DisplayScale, .5f, 3.f);

        bool c0_selected = MutatingState.Strategy == PreviewIntegrator::DirectIntersection;
        bool c1_selected = MutatingState.Strategy == PreviewIntegrator::PathTracing;
        if (ImGui::BeginCombo("Integrator", "NormalMap"))
        {
            if (ImGui::Selectable("NormalMap", &c0_selected))
            {
                MutatingState.Strategy = PreviewIntegrator::DirectIntersection;
            }
            if (ImGui::Selectable("PathTracing", &c1_selected))
            {
                MutatingState.Strategy = PreviewIntegrator::PathTracing;
            }
            ImGui::EndCombo();
        }

        ImGui::End();
    }
    void SceneEditWindow::UpdateCameraControl()
    {
        ImScoped::Window window{ "Camera" };

        Vec3f camera_origin   = MutatingState.CameraOrigin;
        Vec3f camera_forward  = MutatingState.CameraForward;
        Vec3f camera_upward   = MutatingState.CameraUpward;
        Vec3f camera_leftward = camera_forward.Cross(camera_upward);

        ImGui::Text("Origin: (%f, %f, %f)", camera_origin.X(), camera_origin.Y(),
                    camera_origin.Z());
        ImGui::Text("Forward: (%f, %f, %f)", camera_forward.X(), camera_forward.Y(),
                    camera_forward.Z());
        ImGui::Text("Upward: (%f, %f, %f)", camera_upward.X(), camera_upward.Y(),
                    camera_upward.Z());

        auto camera_fov = MutatingState.CameraFov;
        ImGui::SliderFloat("Fov X", &camera_fov.X(), 0.1f, 0.8f);
        if (ImGui::InputFloat("Fov X input", &camera_fov.X(), 0.1f, 0.8f))
        {
            camera_fov.X() = clamp(camera_fov.X(), 0.1f, 0.8f);
        }
        ImGui::SliderFloat("Fov Y", &camera_fov.Y(), 0.1f, 0.8f);
        if (ImGui::InputFloat("Fov Y input", &camera_fov.Y(), 0.1f, 0.8f))
        {
            camera_fov.Y() = clamp(camera_fov.Y(), 0.1f, 0.8f);
        }

        constexpr int kKeyCode_Q = 81;
        constexpr int kKeyCode_W = 87;
        constexpr int kKeyCode_E = 69;
        constexpr int kKeyCode_A = 65;
        constexpr int kKeyCode_S = 83;
        constexpr int kKeyCode_D = 68;

        ImGui::SliderFloat("move rate", &CameraMoveRatePerSec, 0.f, 2.f);

        float camera_move_rate     = CameraMoveRatePerSec * (0.05f * AvgFrameTime / ImGui::GetIO().Framerate);
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

        constexpr int kKeyCode_ArrowUp    = 38;
        constexpr int kKeyCode_ArrowDown  = 40;
        constexpr int kKeyCode_ArrowLeft  = 37;
        constexpr int kKeyCode_ArrowRight = 39;
        constexpr int kKeyCode_Comma      = 188;
        constexpr int kKeyCode_Period     = 190;

        ImGui::SliderFloat("rotate rate", &CameraRotateRatePerSec, 0.f, 2.f);

        float camera_rotate_rate       = CameraRotateRatePerSec * (0.02f * AvgFrameTime / ImGui::GetIO().Framerate);
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

        MutatingState.CameraOrigin = camera_origin + camera_forward * camera_move_forward +
                                     camera_upward * camera_move_upward +
                                     camera_leftward * camera_move_leftward;

        // LocalCoordinateTransform camera_coord(camera_forward, camera_leftward, camera_upward);
        auto camera_coord            = Transform(camera_forward, camera_leftward, camera_upward);
        auto camera_rotate_transform = Transform::Identity()
                                           .RotateZ(camera_rotate_horizontal)
                                           .RotateY(camera_rotate_vertical)
                                           .RotateX(camera_rotate_view);
        auto target_forward =
            camera_coord.InverseLinear(camera_rotate_transform.ApplyLinear(kDefaultCameraForward));
        auto target_upward =
            camera_coord.InverseLinear(camera_rotate_transform.ApplyLinear(kDefaultCameraUpward));
        MutatingState.CameraForward = target_forward;
        MutatingState.CameraUpward  = target_upward;
    }
    void SceneEditWindow::UpdateMaterialEditor()
    {
        ImScoped::Window window{ "Material Edit" };

        bool scene_changed = false;
        for (auto material : EditableMaterials)
        {
            if (ImGui::TreeNode(material->name_.c_str()))
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
            MutatingState.Version += 1;
        }
    }
    void SceneEditWindow::UpdateRenderPreview()
    {
        {
            std::unique_lock<std::mutex> lock{DisplayUpdatingLock};

            DisplayTex->Update([&](void* p, int row_pitch) {
                auto buf = reinterpret_cast<uint32_t*>(p);

                for (int y = 0; y < DisplayTex->Height(); ++y)
                {
                    for (int x = 0; x < DisplayTex->Width(); ++x)
                    {
                        auto rgb       = SpectrumToRGB(GammaCorrect(
                            ToneMap_Aces(DisplayCanvas->GetSpectrum(x, y, 1.f / CurrentSpp)),
                            2.4f));
                        uint32_t rr    = rgb[0];
                        uint32_t gg    = rgb[1];
                        uint32_t bb    = rgb[2];
                        uint32_t pixel = (0xffu << 24) | (bb << 16) | (gg << 8) | rr;

                        buf[y * row_pitch / 4 + x] = pixel;
                    }
                }
            });
        }

        ImScoped::Window window{ "Preview" };
        ImGui::Image(DisplayTex->Id(), {DisplayScale * CurrentState.Resolution.X(),
                                        DisplayScale * CurrentState.Resolution.Y()});
    }
} // namespace akane::gui
