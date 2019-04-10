#include "akane/render.h"
#include <random>
#include <vector>

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

std::default_random_engine rng{3125};

struct SphereRecord
{
    Vec3f center;
    akFloat radius;
};

void AddRandomSphere(Scene& scene, vector<SphereRecord>& vec,
                     const Material* mat, akFloat radius)
{
    std::uniform_real_distribution<float> dist{-10.f, 10.f};

    while (true)
    {
        auto x = dist(rng);
        auto y = dist(rng);
        auto z = radius;

        for (const auto& rec : vec)
        {
            auto distance = rec.center.Distance(Vec3f{x, y, z});
            if (distance < rec.radius + radius)
            {
                continue;
            }
        }

        scene.AddSphere(mat, Vec3f{x, y, z}, radius);
        vec.push_back(SphereRecord{Vec3f{x, y, z}, radius});
        break;
    }
}

void ConstructScene(Scene& scene)
{
    std::discrete_distribution<> dist_mat{.2f, .2f, .6f};
    std::uniform_real_distribution<float> dist_color{0.f, 1.f};
    std::uniform_real_distribution<float> dist_radius{0.8f, 1.4f};
    std::uniform_real_distribution<float> dist_fuzz{0.4f, 0.7f};
    std::uniform_real_distribution<float> dist_refindex{1.3f, 1.8f};

    vector<SphereRecord> vec;

    // large balls
    {
        auto txt = scene.AddSolidTexture(
            Spectrum{dist_color(rng), dist_color(rng), dist_color(rng)});
        auto mat = scene.AddMetal(txt, dist_fuzz(rng));

        AddRandomSphere(scene, vec, mat, 3.f);
    }
    {
        auto txt = scene.AddSolidTexture(
            Spectrum{dist_color(rng), dist_color(rng), dist_color(rng)});
        auto mat = scene.AddDielectrics(txt, dist_refindex(rng));

        // AddRandomSphere(scene, vec, mat, 3.f);
    }
    {
        auto txt = scene.AddSolidTexture(
            Spectrum{dist_color(rng), dist_color(rng), dist_color(rng)});
        auto mat = scene.AddLambertian(txt);

        AddRandomSphere(scene, vec, mat, 3.f);
    }

    // small balls
    for (int i = 0; i < 30; ++i)
    {
        auto mat_choice = dist_mat(rng);
        auto txt        = scene.AddSolidTexture(
            Spectrum{dist_color(rng), dist_color(rng), dist_color(rng)});

        const Material* mat;
        if (mat_choice == 0)
        {
            mat = scene.AddMetal(txt, dist_fuzz(rng));
        }
        else if (mat_choice == 1)
        {
            mat = scene.AddDielectrics(txt, dist_refindex(rng));
        }
        else
        {
            mat = scene.AddLambertian(txt);
        }

        auto radius = dist_radius(rng);
        AddRandomSphere(scene, vec, mat, radius);
    }

    // ground
    auto txt_check = scene.AddCheckerboardTexture(
        scene.AddSolidTexture(Spectrum{.4, .4, .4}),
        scene.AddSolidTexture(Spectrum{.9, .9, .9}));
    auto lambertian = scene.AddLambertian(txt_check);
    scene.AddSphere(lambertian, {0, 0, -1e5}, 1e5);
}

int main()
{
    // srand(1024);
    Scene scene;
    /*
    ConstructScene(scene);
    auto camera =
        CreatePinholeCamera({ -20, 0, 2 }, { 3, 0, 0.f }, { 0, 0, 1 }, { .75f,
    .7f });
        //*/
    //*
    auto txt0 = scene.AddSolidTexture(RandSpectrum());
    auto txt1 = scene.AddSolidTexture(RandSpectrum());
    auto txt2 = scene.AddSolidTexture(RandSpectrum());

    auto txt_check =
        scene.AddCheckerboardTexture(scene.AddSolidTexture(RandSpectrum()),
                                     scene.AddSolidTexture(RandSpectrum()));

    auto lambertian0 = scene.AddLambertian(txt0);
    auto lambertian1 = scene.AddLambertian(txt_check);
    auto metal0      = scene.AddMetal(txt2, 0.7f);
    auto metal1      = scene.AddMetal(txt_check, 0.3f);

    auto s0 = scene.AddSphere(lambertian0, {0, 0, 1}, 1);
	auto s1 = scene.AddSphere(lambertian1, { 0, -2, 0.7f }, 0.7f);
	auto s2 = scene.AddSphere(lambertian0, {0, 0, 7.f}, 3.f);

    // ground
    scene.AddSphere(lambertian1, {0, 0, -1e5}, 1e5);

    scene.AddAreaLight(s2, {1, 1, 1});
    // scene.ConfigureInfiniteLight({0, 0, -1}, {1, 1, 1});

    auto camera =
        CreatePinholeCamera({-5, 0, 1}, {1, 0, 0}, {0, 0, 1}, {.6f, .6f});
    //*/

    auto canvas = ExecuteRendering(scene, *camera, {400, 400}, 100);
    canvas.Finalize("d:/test.png", 2.f);

    return 0;
}
