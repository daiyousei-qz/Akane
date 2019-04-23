#include "akane/render.h"
#include "akane/scene/akane_scene.h"
#include "akane/scene/embree_scene.h"
#include "akane/math/transform.h"
#include <random>
#include <vector>
#include <thread>
#include <future>

using namespace akane;
using namespace std;

int main()
{
    srand(1040);
	EmbreeScene scene;

	/*
	// cornell box
	{
		auto mesh = LoadMesh("d:/scene/cb/CornellBox-Original.obj");
		auto room_transform =
			Transform::Identity().RotateY(kPI / 2).RotateX(-kPI / 2).Move({0, 0, -1});
		scene.AddMesh(mesh, room_transform);
	}

	// stanford bunny
	{
		auto mesh = LoadMesh("d:/scene/bunny2/bunny2.obj");
		auto bunny_transform =
			Transform::Identity().RotateX(-kPI / 2).Scale(.002).Move({0, -0.5, -0.5});
		scene.AddMesh(mesh, bunny_transform);
	}
	auto camera = CreatePinholeCamera({-2.3, 0, 0}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
	//*/

	//*/
	auto mesh = LoadMesh("d:/scene/vk/vokselia_spawn.obj");
	scene.AddMesh(mesh, Transform::CreateRotateX(-kPI / 2).RotateZ(kPI));

	scene.AddGround(-3, { .7, .7, .7 });
	scene.CreateGlobalLight_Skybox({ .5, .7, 1. });
	auto camera = CreatePinholeCamera({ -.5, 0, .3 }, { 1, 0, 0 }, { 0, 0, 1 }, { .6f, .6f });
	// auto camera = CreatePinholeCamera({-3, 0, .3}, {1, 0, 0}, {0, 0, 1}, {.2f, .2f});
	//*/

	/*/
	auto mesh = LoadMesh("d:/scene/bunny2/bunny2.obj");
	scene.AddMesh(mesh, Transform::CreateScale(0.01).RotateX(-kPI/2));

	// scene.AddGround(-3, { .4, .4, .4 });
	// scene.AddTriangleLight({ -1,-1,10 }, { -1,1,10 }, { 1,1,10 }, { 1,1,1 });
	scene.CreateGlobalLight_Skybox({.5, .7, 1.});
	auto camera = CreatePinholeCamera({-8, 0, 0}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
	//*/

	scene.Commit();

	auto canvas = ExecuteRenderingMultiThread(scene, *camera, { 1200, 1200 }, 400, 4);
	canvas->Finalize("d:/test2.png", 2.f);

    return 0;
}
