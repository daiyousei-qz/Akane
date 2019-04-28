#include "akane/integrator.h"

using namespace std;

namespace akane
{
    class DirectIntersectionIntegrator : public Integrator
    {
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                            const Ray& ray) override
        {
            IntersectionInfo info;
            if (scene.Intersect(ray, ctx.workspace, info))
            {
				if (info.area_light)
				{
					return Spectrum(kFloatOne);
				}

                auto nmap = (info.ns.Normalized() + Vec3f{1.f, 1.f, 1.f}) / 2.f;
				return Spectrum{nmap.X(), nmap.Y(), nmap.Z()};
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