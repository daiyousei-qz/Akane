#include "akane/mesh.h"

// using namespace nlohmann;
using namespace akane;
using namespace std;

namespace
{
    auto RandCanonical()
    {
        return static_cast<akFloat>(rand()) / RAND_MAX;
    }

    auto RandSpectrum()
    {
        return Vec3f{RandCanonical(), RandCanonical(), RandCanonical()};
    }
} // namespace

SceneDesc::SharedPtr CreateScene0()
{
    auto scene = std::make_shared<SceneDesc>();

    // mesh 0: box
    //
    {
        auto mesh       = std::make_shared<MeshDesc>();
        mesh->vertices  = {{-1, -1, -1}, {-1, -1, +1}, {-1, +1, -1}, {-1, +1, +1},
                          {+1, -1, -1}, {+1, -1, +1}, {+1, +1, -1}, {+1, +1, +1}};
        mesh->triangles = {{0, 1, 2}, {1, 3, 2}, {4, 6, 5}, {5, 6, 7}, {0, 4, 1}, {1, 4, 5},
                           {2, 3, 6}, {3, 7, 6}, {0, 2, 4}, {2, 6, 4}, {1, 5, 3}, {3, 5, 7}};

        // material
        auto texture = vector<Vec3f>{};
        for (const auto& _ : mesh->triangles)
        {
            texture.push_back(RandSpectrum());
        }

        mesh->material         = std::make_shared<MaterialDesc>();
        mesh->material->type   = "lambertian";
        mesh->material->params = {{"albedo", texture}};

        // light
        mesh->area_light = nullptr;

        scene->objects.push_back(mesh);
    }

    // mesh 1: ground
    //
    {
        auto mesh       = std::make_shared<MeshDesc>();
        mesh->vertices  = {{-1e5, -1e5, -2}, {-1e5, 1e5, -2}, {1e5, -1e5, -2}, {1e5, 1e5, -2}};
        mesh->triangles = {{0, 1, 2}, {1, 3, 2}};

        // material
        mesh->material         = std::make_shared<MaterialDesc>();
        mesh->material->type   = "lambertian";
        mesh->material->params = {{"albedo", RandSpectrum()}};

        // light
        mesh->area_light = nullptr;

        scene->objects.push_back(mesh);
    }

    // mesh 2: lightA
    //
    {
        auto mesh       = std::make_shared<MeshDesc>();
        mesh->vertices  = {{-10, -5, 10}, {-10, -3, 10}, {-8, -3, 10}};
        mesh->triangles = {{0, 1, 2}};

        // material
        mesh->material = nullptr;

        // light
        mesh->area_light         = std::make_shared<AreaLightDesc>();
        mesh->area_light->albedo = {1, 1, 1};

        scene->objects.push_back(mesh);
    }

    // global light 0
    //
    {
        auto light    = std::make_shared<GlobalLightDesc>();
        light->type   = "infinite";
        light->params = {{"albedo", Vec3f{0.05, 0.05, 0.2}}};

		scene->global_lights.push_back(light);
    }

    // camera
    //
    {
        auto camera     = std::make_shared<CameraDesc>();
        camera->origin  = {-6, -6, 2};
        camera->forward = {1, 1, 0};
        camera->upward  = {0, 0, 1};
        camera->fov     = {.6f, .6f};

        scene->camera = camera;
    }

	return scene;
}

