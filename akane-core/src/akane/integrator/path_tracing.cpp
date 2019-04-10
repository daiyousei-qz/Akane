#include "akane/integrator.h"
#include "akane/material.h"

using namespace std;

namespace akane
{
    static constexpr int kMinBounce = 3;
    static constexpr int kMaxBounce = 8;

    class PathTracingIntegrator : public Integrator
    {
        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                            const Ray& camera_ray) override
        {
            Spectrum result  = kFloatZero;
            Spectrum contrib = kFloatOne;

            Ray ray = camera_ray;
            for (int bounce = 0; bounce < kMaxBounce; ++bounce)
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
                // NOTE as light source is also explicit sampled, only camera ray needs accumulation
                // TODO: specular reflection
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
                    auto vtest = light->SampleLi(sampler.Get2D(), isect);

                    if (vtest.Test(scene))
                    {
                        auto shadow_ray = vtest.ShadowRay();

                        auto ld = light->Eval(shadow_ray) *
                                  isect.material->ComputeBSDF(isect, ray.d, shadow_ray.d) /
                                  vtest.Pdf();

                        result += contrib * ld;
                    }
                }

                // calculate scattered light, i.e. -wi
                Spectrum attenuation;
                Ray scattered;
                if (!isect.material->Scatter(ray, isect, sampler.Get2D(), attenuation, scattered))
                {
                    // energy absorbed
                    break;
                }

                contrib *= attenuation;
                ray = scattered;

				// russian roulette
                if (bounce >= kMinBounce)
                {
					auto p = std::max({ contrib.X(), contrib.Y(), contrib.Z() });

					if (p > 1)
					{
						if (sampler.Get1D() > p)
						{
							break;
						}

						contrib /= p;
					}
                }
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
