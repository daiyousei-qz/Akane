#include "akane/integrator.h"
#include "akane/material.h"
#include "akane/math/coordinate.h"
#include "fmt/format.h"

using namespace std;

namespace akane
{
    static constexpr int kMinBounce = 3;
    static constexpr int kMaxBounce = 8;

    static akFloat PowerHeuristic(akFloat this_pdf, akFloat other_pdf)
    {
        return this_pdf * this_pdf / (this_pdf * this_pdf + other_pdf * other_pdf);
    }

    class PathTracingIntegrator : public Integrator
    {
        Spectrum SampleAllDirectLight(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                                      const IntersectionInfo& isect, const Vec3f& wo,
                                      const Bsdf& bsdf, const LocalCoordinateTransform& bsdf_coord)
        {
            Spectrum total_ld = kBlackSpectrum;
            for (auto light : scene.GetLights())
            {
                auto vtest = light->SampleLi(sampler.Get2D(), isect);

                if (vtest.Test(scene, ctx.workspace))
                {
                    auto shadow_ray = vtest.ShadowRay();
                    auto wi         = bsdf_coord.WorldToLocal(shadow_ray.d);

                    // direct radiance from light source
                    auto f  = bsdf.Eval(wo, wi) * abs(wi.Dot(kBsdfNormal));
                    auto ld = light->Eval(shadow_ray) / vtest.Pdf();

                    total_ld += f * ld;
                }
            }

            return total_ld;
        }

        Spectrum SampleRandomDirectLight(RenderingContext& ctx, Sampler& sampler,
                                         const Scene& scene, const IntersectionInfo& isect,
                                         const Vec3f& wo, const Bsdf& bsdf,
                                         const LocalCoordinateTransform& bsdf_coord)
        {
            akFloat light_choice_pdf;
            auto light = scene.SampleLight(sampler.Get1D(), light_choice_pdf);

            if (light_choice_pdf != 0)
            {
                auto vtest = light->SampleLi(sampler.Get2D(), isect);

                if (vtest.Test(scene, ctx.workspace))
                {
                    auto shadow_ray = vtest.ShadowRay();
                    auto wi         = bsdf_coord.WorldToLocal(shadow_ray.d);

                    // direct radiance from light source
                    auto f  = bsdf.Eval(wo, wi) * abs(wi.Dot(kBsdfNormal));
                    auto ld = light->Eval(shadow_ray) / (vtest.Pdf() * light_choice_pdf);

                    return f * ld;
                }
            }

            return kBlackSpectrum;
        }

        Spectrum SampleDirectLightMIS(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                                      const IntersectionInfo& isect, const Vec3f& wo,
                                      const Bsdf& bsdf, const LocalCoordinateTransform& bsdf_coord)
        {
            auto total_ld = kBlackSpectrum;

            // TODO: reuse sampled intersection if it does not hit area light

            // sample lights
            {
                akFloat light_choice_pdf;
                auto light = scene.SampleLight(sampler.Get1D(), light_choice_pdf);

                auto vtest = light->SampleLi(sampler.Get2D(), isect);
                if (vtest.Test(scene, ctx.workspace))
                {
                    auto shadow_ray = vtest.ShadowRay();
                    auto wi         = bsdf_coord.WorldToLocal(shadow_ray.d);

                    auto light_pdf = light_choice_pdf * vtest.Pdf();
                    auto bsdf_pdf  = bsdf.Pdf(wo, wi);

                    // direct radiance from light source
                    auto f  = bsdf.Eval(wo, wi);
                    auto ld = light->Eval(shadow_ray) / light_pdf;

                    total_ld += PowerHeuristic(light_pdf, bsdf_pdf) * f * ld * AbsCosTheta(wi);
                }
            }

            // sample bsdf
            {
                Vec3f wi;
                akFloat bsdf_pdf;
                auto f = bsdf.SampleAndEval(sampler.Get2D(), wo, wi, bsdf_pdf);
				
				if (bsdf_pdf != 0)
				{
					auto shadow_ray = Ray{ isect.point, bsdf_coord.LocalToWorld(wi) };

					IntersectionInfo isect2;
					if (scene.Intersect(shadow_ray, ctx.workspace, isect2))
					{
						if (isect2.area_light != nullptr)
						{
							// TODO: here it is assumed that area light is uniformly sampled
							auto light_pdf =
								scene.PdfLight(isect2.area_light) / isect2.primitive->Area();
							auto ld = isect2.area_light->Eval(shadow_ray) / bsdf_pdf;

							total_ld += PowerHeuristic(bsdf_pdf, light_pdf) * f * ld * AbsCosTheta(wi);
						}
					}
				}
            }

            return total_ld;
        }

        virtual Spectrum Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                            const Ray& camera_ray) override
        {
            Spectrum result  = kBlackSpectrum;
            Spectrum contrib = kWhiteSpectrum;

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

				// TODO: should bsdf be nullptr?
				if (bsdf == nullptr)
				{
					break;
				}

                auto is_specular_bsdf = bsdf->GetType().Contain(BsdfType::Specular);
                from_specular         = is_specular_bsdf;

                // estimate direct light if the current bsdf excludes specular
                if (!is_specular_bsdf)
                {
                    result += contrib * SampleDirectLightMIS(ctx, sampler, scene, isect, bsdf_wo,
                                                             *bsdf, bsdf_coord);
                }

                // sample bsdf
                Vec3f wi;
                akFloat pdf;
                auto f = bsdf->SampleAndEval(sampler.Get2D(), bsdf_wo, wi, pdf);
				if(pdf == 0)
				{
					break;
				}

                contrib *= f * AbsCosTheta(wi) / pdf;
                ray = Ray{isect.point, bsdf_coord.LocalToWorld(wi)};

                // russian roulette
                if (bounce >= kMinBounce)
                {
                    auto p = contrib.Max();

					if (p < 1)
                    {
                        if (sampler.Get1D() > p)
                        {
                            break;
                        }

                        contrib /= p;
                    }
                }
            }

			AKANE_CHECK(!InvalidSpectrum(result));
            return result;
        }

    private:
    };

    Integrator::Ptr CreatePathTracingIntegrator()
    {
        return make_unique<PathTracingIntegrator>();
    }
} // namespace akane
