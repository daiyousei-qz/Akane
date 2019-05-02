#include "external/imgui/imgui.h"
#include "akane/math/transform.h"
#include "akane/math/coordinate.h"
#include "akane/scene/embree_scene.h"
#include "akane/render.h"
#include "akane/debug.h"

#include "helper.h"

using namespace std;
using namespace akane;

// state that would lead to invalidation of old render canvas
struct GlobalState
{
    int Version = 0; // modify this to force re-render

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

GlobalState CurrentState;
GlobalState PreviousState;

float DisplayScale = 400.f / CurrentState.ResolutionWidth;

constexpr Vec3f kDefaultCameraForward = {1, 0, 0};
constexpr Vec3f kDefaultCameraUpward  = {0, 0, 1};

vector<GenericMaterial*> EditableMaterials;

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
    static Sampler::Ptr sampler = CreateRandomSampler(rand());

    static bool initialized = false;
    static EmbreeScene scene{};
    if (!initialized)
    {
        CreateScene_Default(scene);
        scene.Commit();

        initialized       = true;
        EditableMaterials = scene.GetEditMaterials();
    }

    if (canvas != nullptr && camera != nullptr)
    {
        ExecuteRenderIncremental(*canvas, *sampler, scene, *camera,
                                 {CurrentState.ResolutionWidth, CurrentState.ResolutionHeight}, 1);
    }
}

void UpdateStatusControl()
{
    ImGui::Begin("status");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    ImGui::End();
}
void UpdateImageControl()
{
    ImGui::Begin("image");

    static int resolution[2]{CurrentState.ResolutionWidth, CurrentState.ResolutionHeight};
    ImGui::InputInt2("Resolution", resolution);
    if (ImGui::Button("Change Resolution"))
    {
        if (resolution[0] >= 100 && resolution[1] >= 100)
        {
            // TODO: support resolution change
            CurrentState.ResolutionWidth  = resolution[0];
            CurrentState.ResolutionHeight = resolution[1];

            InitializeDisplayTexture(resolution[0], resolution[1], true);
        }
    }

    ImGui::SliderFloat("Display Scale", &DisplayScale, .5f, 3.f);

    ImGui::End();
}
void UpdateCameraControl()
{
    Vec3f camera_origin   = CurrentCameraOrigin();
    Vec3f camera_forward  = CurrentCameraForward();
    Vec3f camera_upward   = CurrentCameraUpward();
    Vec3f camera_leftward = camera_forward.Cross(camera_upward);

    ImGui::Begin("camera");

    ImGui::Text("origin: (%f, %f, %f)", camera_origin.X(), camera_origin.Y(), camera_origin.Z());
    ImGui::Text("forward: (%f, %f, %f)", camera_forward.X(), camera_forward.Y(),
                camera_forward.Z());
    ImGui::Text("upward: (%f, %f, %f)", camera_upward.X(), camera_upward.Y(), camera_upward.Z());

    ImGui::SliderFloat("Fov X", &CurrentState.CameraFovX, 0.1f, 0.8f);
    if (ImGui::InputFloat("Fov X input", &CurrentState.CameraFovX, 0.1f, 0.8f))
    {
        CurrentState.CameraFovX = clamp(CurrentState.CameraFovX, 0.1f, 0.8f);
    }
    ImGui::SliderFloat("Fov Y", &CurrentState.CameraFovY, 0.1f, 0.8f);
    if (ImGui::InputFloat("Fov Y input", &CurrentState.CameraFovY, 0.1f, 0.8f))
    {
        CurrentState.CameraFovY = clamp(CurrentState.CameraFovY, 0.1f, 0.8f);
    }

    constexpr int kKeyCode_Q = 81;
    constexpr int kKeyCode_W = 87;
    constexpr int kKeyCode_E = 69;
    constexpr int kKeyCode_A = 65;
    constexpr int kKeyCode_S = 83;
    constexpr int kKeyCode_D = 68;

    static float CameraMoveRatePerSec = 1.f;
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

    static float CameraRotateRatePerSec = 1.f;
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

    auto target_origin =
        Vec3f{CurrentState.CameraOriginX, CurrentState.CameraOriginY, CurrentState.CameraOriginZ} +
        camera_forward * camera_move_forward + camera_upward * camera_move_upward +
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
void UpdateMaterialEditor()
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

void UpdateRenderPreview()
{
    static int n = 0;
    static Canvas::SharedPtr canvas =
        make_shared<Canvas>(CurrentState.ResolutionWidth, CurrentState.ResolutionHeight);

    if (DetectGlobalStateChange())
    {
        n = 0;

        if (DetectGlobalResolutionChange())
        {
            canvas =
                make_shared<Canvas>(CurrentState.ResolutionWidth, CurrentState.ResolutionHeight);
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

    UpdateDisplayTexture([&](int x, int y) {
        return SpectrumToRGB(ToneMap_Aces(canvas->GetSpectrum(x, y, 1.f / n)));
    });

    ImGui::Image(RetriveDisplayTexture(), {DisplayScale * CurrentState.ResolutionWidth,
                                           DisplayScale * CurrentState.ResolutionHeight});
}

void ApplicationInit()
{
    InitializeDisplayTexture(CurrentState.ResolutionWidth, CurrentState.ResolutionHeight, false);
    // RenderOnce(nullptr, nullptr);
}

void ApplicationUpdate()
{
    if (false)
    {
        static Canvas::SharedPtr canvas = nullptr;

        if (canvas == nullptr)
        {
            auto file = fopen("d:/test2.raw.txt", "rb");

            int width  = 0;
            int height = 0;
            fscanf(file, "%d %d", &width, &height);

            canvas = std::make_shared<Canvas>(width, height);
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    float r, g, b;
                    fscanf(file, "%f %f %f", &r, &g, &b);
					canvas->Append(r, g, b, x, y);
                }
            }

            InitializeDisplayTexture(width, height, true);
        }

        static float gamma                                          = 1.f;
        static char tone_mapping_name[100]                          = "ACES";
        static std::function<Spectrum(const Spectrum&)> tone_mapper = ToneMap_Aces;
        ImGui::Begin("raw edit");

        if (ImGui::BeginCombo("tone mapper", tone_mapping_name))
        {
            if (ImGui::Selectable("ACES", true))
            {
                strcpy(tone_mapping_name, "ACES");
                tone_mapper = ToneMap_Aces;
            }
            if (ImGui::Selectable("Reinhard"))
            {
                strcpy(tone_mapping_name, "Reinhard");
                tone_mapper = ToneMap_Reinhard;
            }
            if (ImGui::Selectable("None"))
            {
                strcpy(tone_mapping_name, "None");
                tone_mapper = [](const Spectrum& s) { return s; };
            }

            ImGui::EndCombo();
        }

        ImGui::SliderFloat("gamma", &gamma, 0.1f, 4.f);

        ImGui::End();

        UpdateDisplayTexture([&](int x, int y) {
            auto spectrum = canvas->GetSpectrum(x, y, 1.);
            return SpectrumToRGB(GammaCorrect(tone_mapper(spectrum), gamma));
        });

        ImGui::Image(RetriveDisplayTexture(),
                     {1.f * CurrentState.ResolutionWidth, 1.f * CurrentState.ResolutionHeight});
    }
    else
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
}

ImVec4 ApplicationRender()
{
    return {.3, .4, .5, 1};
}