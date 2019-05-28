#include "akane/render.h"
#include "akane/scene/akane_scene.h"
#include "akane/scene/embree_scene.h"
#include "akane/math/transform.h"
#include "akane/debug.h"
#include "nlohmann/json.hpp"
#include "fmt/format.h"
#include <random>
#include <vector>
#include <thread>
#include <future>
#include <filesystem>
#include <atomic>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "windows.h"

using namespace akane;
using namespace std;
using namespace std::filesystem;
using namespace nlohmann;

atomic_bool GlobalActiveFlag = true;

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType)
{
    if (dwCtrlType == 0)
    {
        fmt::print("[CTRL-C] force stopping...\n");
        GlobalActiveFlag = false;
    }

    return true;
}

template <typename... TArgs> void Error(const string& format, const TArgs&... args)
{
    fmt::print(format, args...);
    std::exit(0);
}

Vec2f LoadJson_Vec2f(const json& object, const string& key)
{
    const auto& value = object[key];
    if (value == nullptr)
    {
        return Vec2f{0, 0};
    }
    if (value.size() != 2)
    {
        Error("{}({}) is not a valid Vec2f", key, value.dump());
    }

    return Vec2f{value[0].get<float>(), value[1].get<float>()};
}
Vec3f LoadJson_Vec3f(const json& object, const string& key)
{
    const auto& value = object[key];
    if (value == nullptr)
    {
        return Vec3f{0, 0, 0};
    }
    if (value.size() != 3)
    {
        Error("{}({}) is not a valid Vec3f", key, value.dump());
    }

    return Vec3f{value[0].get<float>(), value[1].get<float>(), value[2].get<float>()};
}

Transform LoadJson_Transform(const json& object, const string& key)
{
    const auto& value = object[key];
    if (value == nullptr)
    {
        return Transform::Identity();
    }
    else
    {
        float scale   = value.value("scale", 1.f);
        float rotateX = value.value("rotate_x", 0.f);
        float rotateY = value.value("rotate_y", 0.f);
        float rotateZ = value.value("rotate_z", 0.f);
        Vec3f move    = LoadJson_Vec3f(value, "move");

        return Transform::CreateScale(scale)
            .RotateX(rotateX * kPI / 180.f)
            .RotateY(rotateY * kPI / 180.f)
            .RotateZ(rotateZ * kPI / 180.f)
            .Move(move);
    }
}

void RenderConfig(const json& config)
{
    string name        = config["name"];
    string output_path = config["output_path"];

    Point2i resolution = [&] {
        auto json_res = config["resolution"];
        if (json_res.size() == 1)
        {
            int x = json_res[0];
            return Point2i{x, x};
        }
        else
        {
            int x = json_res[0];
            int y = json_res[1];
            return Point2i{x, y};
        }
    }();

    int ssp = config["ssp"];
    int thd = config["thread"];

    fmt::print("[render session]\n");
    fmt::print("name: {}\n", name);
    fmt::print("output path: {}\n", output_path);
    fmt::print("resolution: {}x{}\n", resolution.X(), resolution.Y());
    fmt::print("sample per pixel: {}\n", ssp);
    fmt::print("thread: {}\n", thd);

    Camera::Ptr camera = [&] {
        const auto& camera_config = config["camera"];

        auto origin  = LoadJson_Vec3f(camera_config, "origin");
        auto forward = LoadJson_Vec3f(camera_config, "forward");
        auto upward  = LoadJson_Vec3f(camera_config, "upward");
        auto fov     = LoadJson_Vec2f(camera_config, "fov");

        fmt::print("camera:\n");
        fmt::print("  origin: ({:.4f}, {:.4f}, {:.4f})\n", origin[0], origin[1], origin[2]);
        fmt::print("  forward: ({:.4f}, {:.4f}, {:.4f})\n", forward[0], forward[1], forward[2]);
        fmt::print("  upward: ({:.4f}, {:.4f}, {:.4f})\n", upward[0], upward[1], upward[2]);
        fmt::print("  field of view: ({:.2f}, {:.2f})\n", fov[0], fov[1]);
        return CreatePinholeCamera(origin, forward, upward, fov);
    }();

    Scene::Ptr scene = [&] {
        auto result = std::make_unique<EmbreeScene>();
        for (const auto item : config["scene"])
        {
            if (item["type"] == "mesh")
            {
                string filename = item["obj_file"];
                auto transform  = LoadJson_Transform(item, "transform");
                auto mesh       = LoadMesh(filename);

                result->AddMesh(mesh, transform);
            }
            else
            {
                Error("unsupported item in scene description");
            }
        }

        result->Commit();
        return result;
    }();

    fmt::print("\n");
    std::function<bool()> active_query = [] { return static_cast<bool>(GlobalActiveFlag); };

    auto result = ExecuteRenderingMultiThread(*scene, *camera, resolution, ssp, thd, &active_query);
    path output_dir = output_path;
    path raw_path   = output_dir / (name + ".raw.bin");
    path image_path = output_dir / (name + ".png");

    fmt::print("FINALIZE {} ssp\n", result.ssp);

    akFloat scalar = 1.f / result.ssp;
    result.canvas->SaveRaw(raw_path.string(), scalar);
    result.canvas->SaveImage(image_path.string(), scalar);
}

void RenderDefault()
{
    /*/
    static char json_buf[10000];
    auto file = fopen("d:/fireplace.json", "rb");
    auto read_sz = fread(json_buf, 1, sizeof json_buf, file);
    auto config = json::parse(json_buf, json_buf + read_sz);

    RenderConfig(config);
    //*/

    //*/
    EmbreeScene scene;

    auto camera_spec = CreateScene_LivingRoom(scene);
    auto camera = CreatePinholeCamera(camera_spec.origin, camera_spec.forward, camera_spec.upward,
                                      camera_spec.fov);
    scene.Commit();

    int size = 400;
    int ssp  = 100;
    int thd  = 4;

    printf("rendering in %d thread, %d ssp, %dx%d\n", thd, ssp, size, size);
    auto result    = ExecuteRenderingMultiThread(scene, *camera, {size, size}, ssp, thd);
    akFloat scalar = 1.f / result.ssp;
    result.canvas->SaveRaw("d:/test2.raw.bin", scalar);
    result.canvas->SaveImage("d:/test2.png", scalar);
    //*/
}

int main(int argc, char** argv)
{
    SetConsoleCtrlHandler(HandlerRoutine, true);
    srand(1040);

    if (argc > 1)
    {
        static char json_buf[10000];
        auto file = fopen(argv[1], "rb");
        if (file == nullptr)
        {
            fmt::print("failed to open file {}\n", argv[1]);
        }

        auto read_sz = fread(json_buf, 1, sizeof json_buf, file);
        auto config  = json::parse(json_buf, json_buf + read_sz);

        RenderConfig(config);
    }
    else
    {
        RenderDefault();
    }
    return 0;
}
