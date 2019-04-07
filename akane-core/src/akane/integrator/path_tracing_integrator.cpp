#include "akane/integrator.h"
#include "akane/material.h"

using namespace std;

namespace akane
{
    class PathTracingIntegrator : public Integrator
    {
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler,
                            const Scene& scene, const Ray& ray) override
        {
			Spectrum result = kFloatZero;
			Spectrum contrib = kFloatOne;

			Ray next_ray = ray;
			for (int bounce = 0; bounce < 3; ++bounce)
			{
				IntersectionInfo isect;
				if (!scene.GetWorld().Intersect(next_ray, 0.00001f, 10000.f, isect))
				{
					// skybox
					result += contrib * Spectrum{ .8f, .8f, .8f };
					break;
				}

				Spectrum attenuation;
				Ray scattered;
				if (!isect.material)
				{
					// no avaliable material
					break;
				}

				if (!isect.material->Scatter(next_ray, isect, sampler.Get2D(), attenuation, scattered))
				{
					// scattered light absorbed
					break;
				}

				contrib *= attenuation;
				next_ray = scattered;
			}

			return result;
        }

	private:

    };

    Integrator::Ptr CreatePathTracingIntegrator()
    {
        return make_unique<PathTracingIntegrator>();
    }
} // namespace akane
