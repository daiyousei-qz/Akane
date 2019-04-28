#pragma once
#include "akane/scene/akane_scene.h"
#include "akane/scene/embree_scene.h"
#include "akane/math/transform.h"

namespace akane
{
    inline MaterialDesc::SharedPtr MakeLambertianMaterial(Vec3f albedo)
    {
        auto result = std::make_shared<MaterialDesc>();
        result->kd  = albedo;

        return result;
    }
    inline MaterialDesc::SharedPtr MakeMirrorMaterial(Vec3f albedo, akFloat roughness = 0.f,
                                                      akFloat eta = 1.f)
    {
        auto result       = std::make_shared<MaterialDesc>();
        result->ks        = albedo;
        result->roughness = roughness;
        result->eta       = eta;

        return result;
    }
    inline MaterialDesc::SharedPtr MakeGlassMaterial(Vec3f albedo)
    {
        auto result = std::make_shared<MaterialDesc>();
        result->tr  = albedo;

        return result;
    }

    inline void AddGem(EmbreeScene& scene, Vec3f center, akFloat r, MaterialDesc::SharedPtr mat)
    {
        scene.AddMesh(CreateSphereMesh(1, 4, mat, false, true),
                      Transform::CreateScale(r).Move(center));
    }
    inline void AddSphere(EmbreeScene& scene, Vec3f center, akFloat r, MaterialDesc::SharedPtr mat)
    {
        scene.AddMesh(CreateSphereMesh(30, 32, mat, true, true),
                      Transform::CreateScale(r).Move(center));
    }

    inline Camera::Ptr CreateScene_Sphere(EmbreeScene& scene)
    {
        auto mat0 = MakeLambertianMaterial({1.f, 1.f, 1.f});
        auto mat1 = MakeMirrorMaterial({1.f, 1.f, 1.f}, 0.f, 10);
        auto mat2 = MakeMirrorMaterial({1.f, 1.f, 1.f}, 0.03f, 10);
        auto mat3 = MakeGlassMaterial({1.f, 1.f, 1.f});
        
		AddSphere(scene, {0, 2, 1}, 1, mat1);
        AddSphere(scene, {0, -2, 1}, 1, mat2);
        AddSphere(scene, {0, 0, 1}, 1, mat0);

        scene.AddGround(0, {.7f, .5f, .4f});
        scene.AddTriangleLight({-1, -1, 6}, {-1, 1, 6}, {1, 1, 6}, {4, 4, 4});
        scene.CreateGlobalLight_Skybox({.5, .7, 1.});

        return CreatePinholeCamera({-5, 0, 1}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
    }

    inline Camera::Ptr CreateScene_Bunny(EmbreeScene& scene)
    {
        auto mesh = LoadMesh("d:/scene/bunny2/bunny2.obj");
		mesh->geomtries.front()->material->roughness = .6f;
		mesh->geomtries.front()->material->eta = 10.f;
		scene.AddMesh(mesh, Transform::CreateScale(0.01).RotateX(-kPI / 2));

        scene.AddGround(-3, {.4, .4, .4});
        scene.AddTriangleLight({-1, -1, 10}, {-1, 1, 10}, {1, 1, 10}, {1, 1, 1});
        scene.CreateGlobalLight_Skybox({.5, .7, 1.});
        return CreatePinholeCamera({-8, 0, 0}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
    }

    inline Camera::Ptr CreateScene_CornellBox(EmbreeScene& scene)
    {
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
                Transform::Identity().RotateX(-kPI / 2).Scale(.002).Move({-0.5, -0.5, -0.5});
            scene.AddMesh(mesh, bunny_transform);
        }

        return CreatePinholeCamera({-2.3, 0, 0}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
    }

    inline Camera::Ptr CreateScene_MC(EmbreeScene& scene)
    {
        auto mesh = LoadMesh("d:/scene/vk/vokselia_spawn.obj");
        scene.AddMesh(mesh, Transform::CreateRotateX(-kPI / 2).RotateZ(kPI));

        scene.AddGround(-3, {.7, .7, .7});
        scene.CreateGlobalLight_Skybox({.5, .7, 1.});

        return CreatePinholeCamera({-.5, 0, .3}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
    }

	inline Camera::Ptr CreateScene_LivingRoom(EmbreeScene& scene)
	{
		auto mesh = LoadMesh("d:/scene/livingroom/living_room.obj");
		scene.AddMesh(mesh, Transform::CreateRotateX(-kPI / 2).RotateZ(kPI));
		
		//return CreatePinholeCamera({ -1.92, 5.43, 1 }, { 0.60, -0.79, 0 }, { 0, 0, 1 }, { .5f, .5f });
		return CreatePinholeCamera({ -2.243, 5.534, 1 }, { 0.633, -0.774, 0 }, { 0, 0, 1 }, { .5f, .5f });
	}
} // namespace akane
