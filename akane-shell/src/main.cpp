#include "akane/render.h"

using namespace akane;
using namespace std;

/*
Scene CreateCornellBox()
{
    Scene scene;

    // box
    scene.AddSphere({1e5 + 1, 40.8, 81.6}, 1e5);
    scene.AddSphere({1e5 + 99, 40.8, 81.6}, 1e5);
    scene.AddSphere({50, 40.8, 1e5}, 1e5);
    scene.AddSphere({50, 40.8, -1e5 + 170}, 1e5);
    scene.AddSphere({50, 1e5, 81.6}, 1e5);
    scene.AddSphere({50, -1e5 + 81.6, 81.6}, 1e5);

    // spheres
    scene.AddSphere({27, 16.5, 47}, 16.5);
    scene.AddSphere({73, 16.5, 78}, 16.5);

    // light
    // scene.AddSphere({ 73, 16.5, 78 }, 16.5);

    return scene;
}

Camera::Ptr CreateCornellBoxCamera()
{
    return CreatePinholeCamera({50, 52, 295.6}, {0, 0, -1}, {-1, 0, 0},
                               {.5f, .5f});
}
//*/
auto RandCanonical()
{
    return static_cast<akFloat>(rand()) / RAND_MAX;
}

auto RandSpectrum()
{
    return Spectrum{RandCanonical(), RandCanonical(), RandCanonical()};
}

int main()
{
    // srand(1024);
    Scene scene;
    auto txt0 = scene.RegisterSolidTexture(RandSpectrum());
    auto txt1 = scene.RegisterSolidTexture(RandSpectrum());
    auto txt2 = scene.RegisterSolidTexture(RandSpectrum());

    auto txt_check = scene.RegisterCheckerboardTexture(
        scene.RegisterSolidTexture(RandSpectrum()),
        scene.RegisterSolidTexture(RandSpectrum()));

    auto lambertian0 = scene.RegisterLambertian(txt0);
    auto lambertian1 = scene.RegisterLambertian(txt_check);
    auto metal0      = scene.RegisterMetal(txt2, 0.7f);
    auto metal1      = scene.RegisterMetal(txt_check, 0.3f);

    scene.AddSphere(lambertian0, {0, 0, 1}, 1);
    scene.AddSphere(metal0, {0, -2, 0.7f}, 0.7f);
    scene.AddSphere(metal1, {0, 0, -1e5}, 1e5);

    auto camera =
        CreatePinholeCamera({-5, 0, 1}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});

    auto canvas = ExecuteRendering(scene, *camera, {400, 400}, 100);
    canvas.Finalize("d:/test.png", 2.f);

    return 0;
}
