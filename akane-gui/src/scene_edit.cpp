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

        //
        DisplayTex = AllocateTexture(CurrentState.ResolutionWidth, CurrentState.ResolutionHeight);

        n = 0;
        canvas =
            std::make_shared<Canvas>(CurrentState.ResolutionWidth, CurrentState.ResolutionHeight);
    }

    void SceneEditWindow::Update()
    {
        // backup state
        PreviousState = CurrentState;

        // update window
        UpdateStatusControl();
        UpdateImageControl();
        UpdateCameraControl();
        UpdateMaterialEditor();

        // render preview
        UpdateRenderPreview();
    }

    void SceneEditWindow::UpdateStatusControl()
    {
        ImGui::Begin("status");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }
    void SceneEditWindow::UpdateImageControl()
    {
        ImGui::Begin("image");

        int resolution[2]{CurrentState.ResolutionWidth, CurrentState.ResolutionHeight};
        ImGui::InputInt2("Resolution", resolution);
        if (ImGui::Button("Change Resolution"))
        {
            if (resolution[0] >= 100 && resolution[1] >= 100)
            {
                // TODO: support resolution change
                CurrentState.ResolutionWidth  = resolution[0];
                CurrentState.ResolutionHeight = resolution[1];

                DisplayTex = AllocateTexture(CurrentState.ResolutionWidth, CurrentState.ResolutionHeight);
            }
        }

        ImGui::SliderFloat("Display Scale", &DisplayScale, .5f, 3.f);

        ImGui::End();
    }
    void SceneEditWindow::UpdateCameraControl()
    {
        Vec3f camera_origin   = CurrentCameraOrigin();
        Vec3f camera_forward  = CurrentCameraForward();
        Vec3f camera_upward   = CurrentCameraUpward();
        Vec3f camera_leftward = camera_forward.Cross(camera_upward);

        ImGui::Begin("camera");

        ImGui::Text("origin: (%f, %f, %f)", camera_origin.X(), camera_origin.Y(),
                    camera_origin.Z());
        ImGui::Text("forward: (%f, %f, %f)", camera_forward.X(), camera_forward.Y(),
                    camera_forward.Z());
        ImGui::Text("upward: (%f, %f, %f)", camera_upward.X(), camera_upward.Y(),
                    camera_upward.Z());

        ImGui::SliderFloat("Fov X", &CurrentState.CameraFovX, 0.1f, 0.8f);
        if (ImGui::InputFloat("Fov X input", &CurrentState.CameraFovX, 0.1f, 0.8f))
        {
            CurrentState.CameraFovX = std::clamp(CurrentState.CameraFovX, 0.1f, 0.8f);
        }
        ImGui::SliderFloat("Fov Y", &CurrentState.CameraFovY, 0.1f, 0.8f);
        if (ImGui::InputFloat("Fov Y input", &CurrentState.CameraFovY, 0.1f, 0.8f))
        {
            CurrentState.CameraFovY = std::clamp(CurrentState.CameraFovY, 0.1f, 0.8f);
        }

        constexpr int kKeyCode_Q = 81;
        constexpr int kKeyCode_W = 87;
        constexpr int kKeyCode_E = 69;
        constexpr int kKeyCode_A = 65;
        constexpr int kKeyCode_S = 83;
        constexpr int kKeyCode_D = 68;

        ImGui::SliderFloat("move rate", &CameraMoveRatePerSec, 0.f, 2.f);

        float camera_move_rate     = CameraMoveRatePerSec / ImGui::GetIO().Framerate;
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

        auto target_origin = Vec3f{CurrentState.CameraOriginX, CurrentState.CameraOriginY,
                                   CurrentState.CameraOriginZ} +
                             camera_forward * camera_move_forward +
                             camera_upward * camera_move_upward +
                             camera_leftward * camera_move_leftward;
        CurrentState.CameraOriginX = target_origin.X();
        CurrentState.CameraOriginY = target_origin.Y();
        CurrentState.CameraOriginZ = target_origin.Z();

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
        CurrentState.CameraForwardX = target_forward.X();
        CurrentState.CameraForwardY = target_forward.Y();
        CurrentState.CameraForwardZ = target_forward.Z();
        CurrentState.CameraUpwardX  = target_upward.X();
        CurrentState.CameraUpwardY  = target_upward.Y();
        CurrentState.CameraUpwardZ  = target_upward.Z();

        ImGui::End();
    }
    void SceneEditWindow::UpdateMaterialEditor()
    {
        ImGui::Begin("material edit");

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
            ForceRestartRender();
        }

        ImGui::End();
    }
    void SceneEditWindow::UpdateRenderPreview()
    {
        if (DetectGlobalStateChange())
        {
            n = 0;

            if (DetectGlobalResolutionChange())
            {
                canvas = make_shared<Canvas>(CurrentState.ResolutionWidth,
                                             CurrentState.ResolutionHeight);
            }
            else
            {
                canvas->Clear();
            }
        }

        auto camera = CreatePinholeCamera(CurrentCameraOrigin(), CurrentCameraForward(),
                                          CurrentCameraUpward(), CurrentCameraFov());

        RenderOnce(canvas.get(), camera.get());
        n += 1;

        DisplayTex->Update([&](void* p, int row_pitch) {
            auto buf = reinterpret_cast<uint32_t*>(p);

            for (int y = 0; y < DisplayTex->Height(); ++y)
            {
                for (int x = 0; x < DisplayTex->Width(); ++x)
                {
                    auto rgb = SpectrumToRGB(
                        GammaCorrect(ToneMap_Aces(canvas->GetSpectrum(x, y, 1.f / n)), 2.4f));
                    uint32_t rr    = rgb[0];
                    uint32_t gg    = rgb[1];
                    uint32_t bb    = rgb[2];
                    uint32_t pixel = (0xffu << 24) | (bb << 16) | (gg << 8) | rr;

                    buf[y * row_pitch / 4 + x] = pixel;
                }
            }
        });

        ImGui::Begin("Preview");
        ImGui::Image(DisplayTex->Id(), {DisplayScale * CurrentState.ResolutionWidth,
                                        DisplayScale * CurrentState.ResolutionHeight});
        ImGui::End();
    }
} // namespace akane::gui
