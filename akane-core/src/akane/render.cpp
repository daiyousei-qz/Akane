#include "akane/core.h"
#include "akane/scene.h"
#include "akane/canvas.h"
#include "akane/camera.h"
#include "akane/sampler.h"
#include "akane/integrator.h"
#include "akane/math/geometry.h"

namespace akane
{
    inline Canvas ExecuteRendering(const Scene& scene, const Camera& camera,
                                   int width, int height, int sample_per_pixel)
    {
        Canvas canvas{width, height};
        RenderingContext ctx;

        auto integrator = CreatePathTracingIntegrator();
        auto sampler    = CreateRandomSampler();

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                Spectrum acc = kFloatZero;
                for (int i = 0; i < sample_per_pixel; ++i)
                {
                    auto ray = camera.SpawnRay(*sampler, width, height, x, y);
                    auto radiance = integrator->Li(ctx, *sampler, scene, ray);

                    acc += radiance;
                }

                canvas.At(x, y) = acc / static_cast<akFloat>(sample_per_pixel);
            }
        }

        return canvas;
    }

} // namespace akane

using namespace akane;
using namespace std;

constexpr Vec2f foo(Vec2f x)
{
    Vec2f y = x + x;
    y += 1.f;

    return y - x;
}

int main()
{
    constexpr Vec2f v = Vec2f{1.f, 1.f};
    constexpr Vec2f z = foo(v);

    constexpr auto x0 = z[0];
    constexpr auto x1 = z[1];
    auto sum = z.Sum();
    auto len = z.Length();
    auto z_ = z.Normalized();

    return 0;
}
