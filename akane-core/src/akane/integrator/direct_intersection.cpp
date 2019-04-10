#include "akane/integrator.h"

using namespace std;

namespace akane
{
    class DirectIntersectionIntegrator : public Integrator
    {
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler,
                            const Scene& scene, const Ray& ray) override
        {
            IntersectionInfo info;
            if (scene.GetWorld().Intersect(ray, 0.00001f, 10000.f, info))
            {
                auto a          = std::acos(ray.d.Dot(info.normal));
                auto gray_scale = (a / kPI - 0.5f) * 2.f;
                return Spectrum(gray_scale);
            }
            else
            {
                return Spectrum(kFloatZero);
            }
        }
    };

    Integrator::Ptr CreateDirectIntersectionIntegrator()
    {
        return make_unique<DirectIntersectionIntegrator>();
    }
} // namespace akane