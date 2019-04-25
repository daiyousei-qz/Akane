#include "external/imgui/imgui.h"
#include "akane/math/transform.h"
#include "akane/math/coordinate.h"
#include "akane/render.h"
#include "akane/scene/embree_scene.h"
#include <functional>

#include <d3d11.h>

using namespace std;
using namespace akane;

// state that would lead to invalidation of old render canvas
struct GlobalState
{
    int ResolutionWidth  = 200;
    int ResolutionHeight = 200;

    float CameraOriginX = -3;
    float CameraOriginY = 0;
    float CameraOriginZ = 0;

    float CameraForwardX = 1;
    float CameraForwardY = 0;
    float CameraForwardZ = 0;

    float CameraUpwardX = 0;
    float CameraUpwardY = 0;
    float CameraUpwardZ = 1;

    float CameraFovX = .5f;
    float CameraFovY = .5f;
};

GlobalState CurrentState;
GlobalState PreviousState;

float DisplayScale     = 1.f;

constexpr Vec3f kDefaultCameraForward = {1, 0, 0};
constexpr Vec3f kDefaultCameraUpward  = {0, 0, 1};

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

bool DetectGlobalStateChange()
{
    return CurrentState.ResolutionWidth != PreviousState.ResolutionWidth ||
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

float CorrectInputRange(float input, float min, float max)
{
    if (input < min)
        return min;
    if (input > max)
        return max;

    return input;
}

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;

static ID3D11ShaderResourceView* texSRV = nullptr;
static ID3D11Texture2D* tex             = nullptr;

void InitializeDisplayTexture(bool override = false)
{
    if (!override && tex != nullptr)
    {
        return;
    }
    if (texSRV != nullptr)
    {
        texSRV->Release();
        texSRV = nullptr;
    }
    if (tex != nullptr)
    {
        tex->Release();
        tex = nullptr;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width                = CurrentState.ResolutionWidth;
    desc.Height               = CurrentState.ResolutionHeight;
    desc.MipLevels            = 1;
    desc.ArraySize            = 1;
    desc.Format               = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count     = 1;
    desc.Usage                = D3D11_USAGE_DYNAMIC;
    desc.BindFlags            = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags       = D3D10_CPU_ACCESS_WRITE;

    HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, nullptr, &tex);

    if (SUCCEEDED(hr))
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.Format                          = DXGI_FORMAT_R8G8B8A8_UNORM;
        SRVDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels             = 1;

        hr = g_pd3dDevice->CreateShaderResourceView(tex, &SRVDesc, &texSRV);
    }
}

void UpdateDisplayTexture(const std::function<Spectrum(int x, int y)>& pixel_source,
                          float gamma = 2.f)
{
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    ZeroMemory(&mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    HRESULT hr = g_pd3dDeviceContext->Map(tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

    if (SUCCEEDED(hr))
    {
        auto data = reinterpret_cast<uint32_t*>(mapped_resource.pData);

        for (int y = 0; y < CurrentState.ResolutionHeight; ++y)
        {
            for (int x = 0; x < CurrentState.ResolutionWidth; ++x)
            {
                auto spectrum  = pixel_source(x, y);
                uint32_t rr    = min(255, static_cast<int>(255.f * powf(spectrum[0], 1 / gamma)));
                uint32_t gg    = min(255, static_cast<int>(255.f * powf(spectrum[1], 1 / gamma)));
                uint32_t bb    = min(255, static_cast<int>(255.f * powf(spectrum[2], 1 / gamma)));
                uint32_t pixel = (0xffu << 24) | (bb << 16) | (gg << 8) | rr;

                data[y * mapped_resource.RowPitch / 4 + x] = pixel;
            }
        }
        g_pd3dDeviceContext->Unmap(tex, 0);
    }
}

void RenderOnce(Canvas* canvas, const Camera* camera)
{
    static bool initialized = false;
    static EmbreeScene scene{};
    if (!initialized)
    {
        auto mesh = LoadMesh("d:/scene/bunny2/bunny2.obj");
        scene.AddMesh(mesh, Transform::CreateScale(0.005));
        // auto mesh = LoadMesh("d:/scene/vk/vokselia_spawn.obj");
        // scene.AddMesh(mesh);
        scene.CreateGlobalLight_Skybox({.5, .7, 1.});
        // scene.CreateGlobalLight_Infinite({1, 1, 1});

        scene.Commit();

        initialized = true;
    }

    if (canvas != nullptr && camera != nullptr)
    {
        ExecuteRenderIncremental(*canvas, scene, *camera,
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

    static int resolution[2]{};
    ImGui::InputInt2("Resolution", resolution);
    if (ImGui::Button("Change Resolution"))
    {
        if (resolution[0] >= 100 && resolution[1] >= 100)
        {
            // TODO: support resolution change
            // CurrentState.ResolutionWidth  = resolution[0];
            // CurrentState.ResolutionHeight = resolution[1];
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
        CurrentState.CameraFovX = CorrectInputRange(CurrentState.CameraFovX, 0.1f, 0.8f);
    }
    ImGui::SliderFloat("Fov Y", &CurrentState.CameraFovY, 0.1f, 0.8f);
    if (ImGui::InputFloat("Fov Y input", &CurrentState.CameraFovY, 0.1f, 0.8f))
    {
        CurrentState.CameraFovY = CorrectInputRange(CurrentState.CameraFovY, 0.1f, 0.8f);
    }

    constexpr int kKeyCode_Q = 81;
    constexpr int kKeyCode_W = 87;
    constexpr int kKeyCode_E = 69;
    constexpr int kKeyCode_A = 65;
    constexpr int kKeyCode_S = 83;
    constexpr int kKeyCode_D = 68;

    static float CameraMoveRatePerSec = 0.5f;
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

    static float CameraRotateRatePerSec = 0.5f;
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

void UpdateRenderPreview()
{
    static int n = 0;
    static Canvas::SharedPtr canvas =
        make_shared<Canvas>(CurrentState.ResolutionWidth, CurrentState.ResolutionHeight);

    if (DetectGlobalStateChange())
    {
        canvas->Clear();
        n = 0;
    }

    auto camera = CreatePinholeCamera(CurrentCameraOrigin(), CurrentCameraForward(),
                                      CurrentCameraUpward(), CurrentCameraFov());

    RenderOnce(canvas.get(), camera.get());
    n += 1;

    UpdateDisplayTexture([&](int x, int y) { return canvas->At(x, y) / static_cast<akFloat>(n); });

    auto list = ImGui::GetBackgroundDrawList();
    list->AddImage(texSRV, {0, 0},
                   {DisplayScale * CurrentState.ResolutionWidth,
                    DisplayScale * CurrentState.ResolutionHeight});
}

void ApplicationInit()
{
    InitializeDisplayTexture(false);
    // RenderOnce(nullptr, nullptr);
}

void ApplicationUpdate()
{
    // backup state
    PreviousState = CurrentState;

    // update window
    UpdateStatusControl();
    UpdateImageControl();
    UpdateCameraControl();

    // render preview
    UpdateRenderPreview();
}

ImVec4 ApplicationRender()
{
    return {.3, .4, .5, 1};
}