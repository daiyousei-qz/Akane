#include "akane/integrator.h"

using namespace std;

namespace akane
{
    class PathTracingIntegrator : public Integrator
    {
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler,
                            const Scene& scene, const Ray& ray) override
        {
            return Spectrum();
        }
    };

    unique_ptr<Integrator> CreatePathTracingIntegrator()
    {
        return make_unique<PathTracingIntegrator>();
    }
} // namespace akane