#include "akane/render.h"

#include "akane/scene/integrated.h"
#include "akane/scene/embree.h"
#include "akane/integrator/normal_mapped.h"
#include "akane/integrator/path_tracing.h"
#include "akane/material/lambertian.h"
#include "akane/material/test.h"
#include "akane/texture.h"

#include "akane/shape/disk.h"
#include "akane/shape/rect.h"
#include "akane/shape/sphere.h"

#include "akane/bsdf/lambertian.h"
#include "akane/bsdf/specular.h"
#include "akane/bsdf/microfacet.h"
#include "akane/bsdf/hybrid.h"

#include "akane/model.h"

#include <vector>

using namespace std;
using namespace akane;

constexpr int kSamplePerPixel = 200;
constexpr Point2i kResolution = {800, 800};

unique_ptr<Camera> LoadEmbreeScene(const string& filename, EmbreeScene& scene)
{
    auto scene_desc = LoadSceneDesc(filename.c_str());
    for (const auto& object : scene_desc->objects)
    {
        auto transform = Transform::CreateScale(object.scale)
                             .RotateX(object.rotation[0])
                             .RotateY(object.rotation[1])
                             .RotateZ(object.rotation[2])
                             .Move(PointToVec(object.position));

        scene.AddMesh(*object.mesh, transform);
    }
    scene.Commit();

    return CreatePinholeCamera(PointToVec(scene_desc->camera.origin),
                               PointToVec(scene_desc->camera.forward),
                               PointToVec(scene_desc->camera.upward), scene_desc->camera.fov,
                               scene_desc->camera.aspect_ratio);
}

int main()
{
    /*
    auto camera = akane::CreatePinholeCamera({ -3.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f
    }); auto scene = std::make_unique<IntegratedScene>();

    auto tex_cb = make_shared<CheckerboardTexture>(Vec3{ 1.f, 1.f, 1.f }, Vec3{ .2f, .2f, .2f },
    10e5);

    auto mat_ground = make_shared<LambertianMaterial>(Vec3{ .7f, .7f, .7f });
    auto mat_ground2 = make_shared<TestMaterial>(tex_cb, [](eds::Workspace& ws, Spectrum albedo) {
        return ws.Construct<LambertianReflection>(albedo);
        });
    auto mat_obj1 = make_shared<LambertianMaterial>(Vec3{ .9f, .9f, .9f });
    scene->AddGeometricPrimitive(Sphere{ {0, 0, 0}, 1 }, mat_obj1);
    scene->AddGeometricPrimitive(Sphere{ {0, 0, -1 - 1e5}, 1e5 }, mat_ground2);
    scene->AddGeometricLight(Rect{ {0, 0, 2}, 1, 1 }, { 1, 1, 1 }, 10);
    scene->AddPointLight({ 0, 0, 5 }, { .7f, .1f, .1f }, 300);
    scene->AddPointLight({ 5, 0, 1 }, { .1f, .7f, .1f }, 300);
    scene->AddPointLight({ 0, -1, 3 }, { .1f, .1f, .7f }, 300);
    scene->Commit();

    //*/
    auto scene  = make_unique<EmbreeScene>();
    auto camera = LoadEmbreeScene("d:/livingroom.json", *scene);

    auto integrator = make_unique<PathTracingIntegrator>(1, 1);

    auto result =
        ExecuteRenderingMultiThread(*integrator, *scene, *camera, kResolution, kSamplePerPixel, 4);

    result.canvas->SaveImage("d:/workspace/test.png", 1.f / result.ssp);
    return 0;
}