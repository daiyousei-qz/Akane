#include "akane/render.h"
#include "akane/scene/akane_scene.h"
#include "akane/scene/embree_scene.h"
#include "akane/math/transform.h"
#include "akane/debug.h"
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
	
	auto camera = CreateScene_LivingRoom(scene);
    scene.Commit();

    auto canvas = ExecuteRenderingMultiThread(scene, *camera, {800, 800}, 50, 4);
    canvas->Finalize("d:/test2.png", 2.f);

    return 0;
}
