#include "akane/integrator.h"
#include "akane/material.h"
#include "akane/math/coordinate.h"

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
            Spectrum contrib = {1, 1, 1};

            bool from_specular = true;
            Ray ray            = camera_ray;
            for (int bounce = 0; bounce < kMaxBounce; ++bounce)
            {
                ctx.workspace.Clear();

                IntersectionInfo isect;
                if (!scene.Intersect(ray, ctx.workspace, isect))
                {
                    // blend global light
                    for (auto global_light : scene.GetGlobalLights())
                    {
                        result += contrib * global_light->Eval(ray);
                    }

                    break;
                }

                // if the primitive emits light
                // NOTE as light source is also explicit sampled, only camera ray needs accumulation
                // TODO: specular reflection
                if (isect.area_light && from_specular)
                {
                    result += contrib * isect.area_light->Eval(ray);
                }

                if (isect.material == nullptr)
                {
                    break;
                }

                auto bsdf       = isect.material->ComputeBsdf(ctx.workspace, isect);
                auto bsdf_coord = LocalCoordinateTransform(isect.ns);
                auto bsdf_wo    = bsdf_coord.WorldToLocal(-ray.d);

				auto is_specular_bsdf = bsdf->GetType().Contain(BsdfType::Specular);
				from_specular = is_specular_bsdf;

                // estimate direct light if the current bsdf excludes specular
				if (!is_specular_bsdf)
				{
					for (const auto& light : scene.GetLights())
					{
						if (light == isect.area_light)
						{
							continue;
						}

						auto vtest = light->SampleLi(sampler.Get2D(), isect);

						if (vtest.Test(scene, ctx.workspace))
						{
							auto shadow_ray = vtest.ShadowRay();
							auto wi = bsdf_coord.WorldToLocal(shadow_ray.d);

							// direct radiance from light source
							auto f = bsdf->Eval(bsdf_wo, wi) * wi.Dot(kBsdfNormal);
							auto ld = light->Eval(shadow_ray);

							result += contrib * ld * f / vtest.Pdf();
						}
					}
				}

                // sample bsdf
                Vec3f wi;
                akFloat pdf;
                auto f = bsdf->SampleAndEval(sampler.Get2D(), bsdf_wo, wi, pdf);

                if (pdf == kFloatZero)
                {
                    break;
                }

                contrib *= f * wi.Dot(kBsdfNormal) / pdf;
                ray = Ray{isect.point, bsdf_coord.LocalToWorld(wi)};

                // russian roulette
                if (bounce >= kMinBounce)
                {
                    auto p = std::max({contrib.X(), contrib.Y(), contrib.Z()});

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
