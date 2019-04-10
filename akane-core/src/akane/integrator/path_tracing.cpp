#include "akane/integrator.h"
#include "akane/material.h"

using namespace std;

namespace akane
{
    class PathTracingIntegrator : public Integrator
    {
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler,
                            const Scene& scene, const Ray& camera_ray) override
        {
            Spectrum result  = kFloatZero;
            Spectrum contrib = kFloatOne;

            Ray ray = camera_ray;
            for (int bounce = 0; bounce < 8; ++bounce)
            {
                IntersectionInfo isect;
                if (!scene.GetWorld().Intersect(ray, 0.0001f, 10000.f, isect))
                {
                    // blend infinite light
                    auto infinite_light = scene.GetInfiniteLight();
                    if (infinite_light)
                    {
                        result += contrib * infinite_light->Eval(ray);
                    }

                    break;
                }

                // if the primitive emits light
                auto area_light = isect.primitive->GetAreaLight();
                if (area_light && bounce == 0)
                {
                    result += contrib * area_light->Eval(ray);
                }

                // if the primitive is totally transparent
                if (!isect.material)
                {
                    break;
                }

                // estimate direct lighting
                for (const auto& light : scene.GetLights())
                {
					//break;
					auto u = sampler.Get2D();
                    Vec3f wi;
                    akFloat pdf;
                    light->SampleLi(u, isect, wi, pdf);

					Point3f p;
					akFloat pdf_;
					reinterpret_cast<const AreaLight*>(light)->GerPrimitive()->SampleP(u, p, pdf_);

					auto shadow_ray = Ray{ isect.point, -wi };
					IntersectionInfo isect2;
					if (!scene.GetWorld().Intersect(shadow_ray, 0.0001f, 10000.f, isect2))
					{
						continue;
					}

					auto dist = (p - isect2.point).Length();
					if (dist < 0.1)
					{
						auto ld = light->Eval(shadow_ray) *
							isect.material->ComputeBSDF(isect, ray.d, wi) / pdf_;

						result += contrib * ld;
					}
					else
					{
						result += contrib * 0.0000000001f;
					}
                }

                // calculate scattered light, i.e. -wi
                Spectrum attenuation;
                Ray scattered;
                if (!isect.material->Scatter(ray, isect, sampler.Get2D(),
                                             attenuation, scattered))
                {
                    // energy absorbed
                    break;
                }

                contrib *= attenuation;
                ray = scattered;
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
