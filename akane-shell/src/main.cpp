#include "akane/render.h"
#include "akane/scene/akane_scene.h"
#include "akane/scene/embree_scene.h"
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
	auto texture = scene.CreateTexture_Solid({ RandSpectrum() });
    return scene.CreateMaterial_Lambertian(texture);
}

auto RandCheckboardLambertian(SceneBase& scene)
{
	auto t0 = scene.CreateTexture_Solid({ RandSpectrum() });
	auto t1 = scene.CreateTexture_Solid({ RandSpectrum() });
    auto texture = scene.CreateTexture_Checkerboard(t0, t1);

    return scene.CreateMaterial_Lambertian(texture);
}

auto QuickAddSphere(AkaneScene& scene, Spectrum color, Vec3f center, akFloat radius)
{
	auto texture = scene.CreateTexture_Solid({ color });
    auto material = scene.CreateMaterial_Lambertian(texture);

    return scene.AddSphere(material, center, radius);
}

std::vector<MeshDesc::SharedPtr> CreateScene1();

#pragma optimize("", off)
void foo()
{
    EmbreeScene scene;

	for(const auto& mesh_data : CreateScene1())
	{
		scene.AddMesh(mesh_data);
	}

	scene.CreateGlobalLight_Infinite({ 0.05, 0.05, 0.2 });
	scene.Commit();
	
	auto camera = CreatePinholeCamera({-6, -6, 2}, {1, 1, 0}, {0, 0, 1}, {.6f, .6f});
    
	auto canvas = ExecuteRendering(scene, *camera, {200, 200}, 20);
    canvas.Finalize("d:/test2.png", 2.f);
    return;
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
	srand(10240);
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
