#include "akane/integrator/path_tracing.h"

namespace akane
{
    Spectrum SampleAllDirectLight(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                                  const IntersectionInfo& isect, const Vec3& wo, const Bsdf& bsdf,
                                  const Transform& world2local)
    {
        Spectrum total_ld = 0.f;
        for (auto light : scene.GetLightVec())
        {
            auto sample = light->SampleLi(sampler.Get2D());

            if (sample.TestVisibility(scene, ctx.workspace, isect.point, isect.object))
            {
                auto shadow_ray = sample.GenerateShadowRay(isect.point);
                auto wi         = world2local.ApplyLinear(shadow_ray.d);

                // direct radiance from light source
                auto f  = bsdf.Eval(wo, wi) * abs(wi.Dot(kBsdfNormal));
                auto ld = light->Eval(shadow_ray) / sample.Pdf();

                total_ld += f * ld;
            }
        }

        return total_ld;
    }

    // TODO: this function is buggy (return nan)
    Spectrum SampleRandomDirectLight(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                                     const IntersectionInfo& isect, const Vec3& wo,
                                     const Bsdf& bsdf, const Transform& world2local)
    {
        float light_choice_pdf;
        auto light = scene.SampleLight(sampler.Get1D(), light_choice_pdf);

        if (light_choice_pdf != 0)
        {
            auto sample = light->SampleLi(sampler.Get2D());

            if (sample.TestVisibility(scene, ctx.workspace, isect.point, isect.object))
            {
                auto shadow_ray = sample.GenerateShadowRay(isect.point);
                auto wi         = world2local.ApplyLinear(shadow_ray.d);

                // direct radiance from light source
                auto f  = bsdf.Eval(wo, wi) * abs(wi.Dot(kBsdfNormal));
                auto ld = light->Eval(shadow_ray) / (sample.Pdf() * light_choice_pdf);

                return f * ld;
            }
        }

        return kBlackSpectrum;
    }

    Spectrum SampleGlobalLight(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                               const IntersectionInfo& isect, const Vec3& wo, const Bsdf& bsdf,
                               const Transform& world2local)
    {
        Light* global_light = scene.GetGlobalLight();

        if (global_light != nullptr)
        {
            auto sample = global_light->SampleLi(sampler.Get2D());

            if (sample.TestVisibility(scene, ctx.workspace, isect.point, isect.object))
            {
                auto shadow_ray = sample.GenerateShadowRay(isect.point);
                auto wi         = world2local.ApplyLinear(shadow_ray.d);

                // direct radiance from light source
                auto f  = bsdf.Eval(wo, wi) * abs(wi.Dot(kBsdfNormal));
                auto ld = global_light->Eval(shadow_ray);

                return f * ld;
            }
        }

        return kBlackSpectrum;
    }

    Spectrum PathTracingIntegrator::Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                                       const Ray& camera_ray) const
    {
        Ray ray          = camera_ray;
        Spectrum result  = 0.f;
        Spectrum contrib = 1.f;

        bool from_camera_or_specular = true;
        for (int bounce = 0; bounce < max_bounce_; ++bounce)
        {
            ctx.workspace.Clear();

            IntersectionInfo isect;
            if (!scene.Intersect(ray, ctx.workspace, isect))
            {
                // blend global lighting
                if (auto global_light = scene.GetGlobalLight(); global_light != nullptr)
                {
                    result += contrib * global_light->Eval(ray);
                }

                break;
            }

            // if the primitive emits light
            // as light source is also explicit sampled, only camera and specular ray needs
            // accumulation
            if (isect.area_light && from_camera_or_specular)
            {
                // TODO: verify this
                result += contrib * isect.area_light->Eval(ray);
                break; // assuming light is dominant by direct illumination
            }

            if (isect.material == nullptr)
            {
                break;
            }

            auto bsdf        = isect.material->ComputeBsdf(ctx.workspace, isect);
            auto world2local = CreateBsdfCoordTransform(isect.ns);
            auto bsdf_wo     = world2local.ApplyLinear(-ray.d);

            // TODO: verify this
            if (bsdf == nullptr)
            {
                break;
            }

            bool is_specular_bsdf   = bsdf->GetType().Contain(BsdfType::Specular);
            from_camera_or_specular = is_specular_bsdf;

            // estimate direct light
            if (!is_specular_bsdf)
            {
                result += contrib * SampleGlobalLight(ctx, sampler, scene, isect, bsdf_wo, *bsdf,
                                                      world2local);

                result += contrib * SampleAllDirectLight(ctx, sampler, scene, isect, bsdf_wo, *bsdf,
                                                         world2local);
            }

            // sample bsdf
            Vec3 bsdf_wi;
            float pdf_wi;
            auto f = bsdf->SampleAndEval(sampler.Get2D(), bsdf_wo, bsdf_wi, pdf_wi);
            if (pdf_wi == 0)
            {
                break;
            }

            contrib *= f * AbsCosTheta(bsdf_wi) / pdf_wi;
            ray = Ray{isect.point, world2local.ApplyLinear(bsdf_wi)};

            // russian roulette
            if (bounce >= min_bounce_)
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
} // namespace akane