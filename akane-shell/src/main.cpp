#include "akane/render.h"
#include "akane/scene/akane_scene.h"
#include "akane/scene/embree_scene.h"
#include "akane/math/transform.h"
#include <random>
#include <vector>

using namespace akane;
using namespace std;

auto RandCanonical()
{
    return static_cast<akFloat>(rand()) / RAND_MAX;
}

auto RandSpectrum()
{
    return Spectrum{RandCanonical(), RandCanonical(), RandCanonical()};
}

auto RandSolidLambertian(SceneBase& scene)
{
    auto texture = scene.CreateTexture_Solid({RandSpectrum()});
    return scene.CreateMaterial_Lambertian(texture);
}

auto RandCheckboardLambertian(SceneBase& scene)
{
    auto t0      = scene.CreateTexture_Solid({RandSpectrum()});
    auto t1      = scene.CreateTexture_Solid({RandSpectrum()});
    auto texture = scene.CreateTexture_Checkerboard(t0, t1);

    return scene.CreateMaterial_Lambertian(texture);
}

auto QuickAddSphere(AkaneScene& scene, Spectrum color, Vec3f center, akFloat radius)
{
    auto texture  = scene.CreateTexture_Solid({color});
    auto material = scene.CreateMaterial_Lambertian(texture);

    return scene.AddSphere(material, center, radius);
}

#pragma optimize("", off)
void foo()
{

    EmbreeScene scene;

    // cornell box
    {
        auto mesh = LoadMesh("d:/scene/cb/CornellBox-Original.obj");
        auto room_transform =
            Transform::RotateY(kPI / 2).Also(Transform::RotateX(kPI / 2 * 3)).MoveTo({0, 0, -1});
        scene.AddMesh(mesh, room_transform);
    }

    // stanford bunny
    {
        auto mesh            = LoadMesh("d:/scene/bunny/bunny.obj");
        auto bunny_transform = Transform::RotateX(kPI / 2 * 3).Also(Transform::Scale(.5)).MoveTo({0, -.5, -1});
        scene.AddMesh(mesh, bunny_transform);
    }
    // scene.AddGround(-1, {.7, .7, .7});
    // scene.AddTriangleLight({1, 1, 10}, {-1, 1, 10}, {1, -1, 10}, {1, 1, 1});
    // scene.CreateGlobalLight_Distant({ 0, 0, -1 }, { .2, .2, .6 });
    // scene.CreateGlobalLight_Infinite({.6, .6, .6});
    // auto camera = CreatePinholeCamera({0, 0, 5}, {0, 0, -1}, {1, 0, 0}, {.6f, .6f});
    auto camera = CreatePinholeCamera({-2.3, 0, 0}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});

    scene.Commit();
    auto canvas = ExecuteRendering(scene, *camera, {400, 400}, 100);
    canvas.Finalize("d:/test2.png", 2.f);
    return;

    /*

    auto scene_desc = CreateScene0();

    for (const auto& mesh_data : scene_desc->objects)
    {
        scene.AddMesh(mesh_data);
    }

    for (const auto& global_light : scene_desc->global_lights)
    {
        if (global_light->type == "infinite")
        {
            const auto& albedo = global_light->params.at("albedo");

            scene.CreateGlobalLight_Infinite(any_cast<Vec3f>(albedo));
        }
        else if (global_light->type == "distant")
        {
            const auto& direction = global_light->params.at("direction");
            const auto& albedo    = global_light->params.at("albedo");

            scene.CreateGlobalLight_Distant(any_cast<Vec3f>(direction), any_cast<Vec3f>(albedo));
        }
    }

    auto camera = CreatePinholeCamera(scene_desc->camera->origin, scene_desc->camera->forward,
                                      scene_desc->camera->upward, scene_desc->camera->fov);




    scene.Commit();
    auto canvas = ExecuteRendering(scene, *camera, {200, 200}, 20);
    canvas.Finalize("d:/test2.png", 2.f);
    return;
    //*/
}

#pragma optimize("", on)

//*
AkaneScene::Ptr CreateCornellBox()
{
    auto scene = std::make_unique<AkaneScene>();

    // box
    QuickAddSphere(*scene, {.9, .9, .9}, {0, 0, -1e5}, 1e5);    // bottom
    QuickAddSphere(*scene, {.9, .9, .9}, {0, 0, 8 + 1e5}, 1e5); // top
    QuickAddSphere(*scene, {.9, 0, 0}, {0, 5 + 1e5, 4}, 1e5);   // left
    QuickAddSphere(*scene, {0, .9, 0}, {0, -5 - 1e5, 4}, 1e5);  // right
    QuickAddSphere(*scene, {.9, .9, .9}, {5 + 1e5, 0, 4}, 1e5); // front

    // spheres
    QuickAddSphere(*scene, RandSpectrum(), {-2, -2, 1}, 1);
    QuickAddSphere(*scene, RandSpectrum(), {2, 1, 2.5}, 2.5);

    // light
    auto bulb = QuickAddSphere(*scene, {1, 1, 1}, {0, 0, 6}, 0.3f);
    scene->CreateLight_Area(bulb, {1, 1, 1});

    return scene;
}

Camera::Ptr CreateCornellBoxCamera()
{
    return CreatePinholeCamera({-8, 0, 2}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
}
//*/

int main()
{
    srand(1040);
    foo();
    return 0;

    auto scene  = CreateCornellBox();
    auto camera = CreateCornellBoxCamera();

    /*
    BasicScene scene;
    auto txt0 = scene.AddSolidTexture(RandSpectrum());
    auto txt1 = scene.AddSolidTexture(RandSpectrum());
    auto txt2 = scene.AddSolidTexture(RandSpectrum());

    auto txt_check = scene.AddCheckerboardTexture(scene.AddSolidTexture(RandSpectrum()),
                                                  scene.AddSolidTexture(RandSpectrum()));

    auto lambertian0 = scene.AddLambertian(txt0);
    auto lambertian1 = scene.AddLambertian(txt_check);
    auto metal0      = scene.AddMetal(txt2, 0.7f);
    auto metal1      = scene.AddMetal(txt_check, 0.3f);

    auto s0 = scene.AddSphere(lambertian0, {0, 0, 1}, 1);
    auto s1 = scene.AddSphere(lambertian1, {0, -2, 0.7f}, 0.7f);
    auto s2 = scene.AddSphere(lambertian0, {0, 0, 7.f}, 0.3f);

    // ground
    scene.AddSphere(lambertian1, {0, 0, -1e5}, 1e5);

    scene.AddAreaLight(s2, {1, 1, 1});
    // scene.ConfigureInfiniteLight({0, 0, -1}, {1, 1, 1});

    auto camera = CreatePinholeCamera({-5, 0, 1}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
    //*/

    auto canvas = ExecuteRendering(*scene, *camera, {200, 200}, 3);
    canvas.Finalize("d:/test.png", 2.f);

    return 0;
}
