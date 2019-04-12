#include "akane/material.h"
#include "akane/bsdf/lambertian.h"

namespace akane
{
    // v: direction of incoming ray
    // n: surface normal
    // assuming v and u is normalized
    static void ComputeReflectedRay(const Vec3f& v, const Vec3f& n, Vec3f& reflected) noexcept
    {
        reflected = v - 2.f * v.Dot(n) * n;
    }

    // v: direction of incoming ray
    // n: surface normal
    // assuming v and u is normalized
    static bool ComputedRefractedRay(const Vec3f& v, const Vec3f& n, akFloat eta,
                                     Vec3f& refracted) noexcept
    {
        auto h            = v.Dot(n);
        auto discriminant = 1.f - eta * eta * (1.f - h * h);

        if (discriminant > 0)
        {
            refracted = eta * (v - h * n) - sqrt(discriminant) * n;
            refracted = refracted.Normalized();
            return true;
        }
        else
        {
            return false;
        }
    }

    static akFloat Schlick(akFloat cosine, akFloat eta) noexcept
    {
        auto r0 = (1 - eta) / (1 + eta);
        r0      = r0 * r0;
		
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

    // pure diffuse
    class Lambertian : public Material
    {
    public:
        Lambertian(const Texture* texture) : texture_(texture)
        {
            assert(texture != nullptr);
        }

		const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
		{
			return workspace.New<LambertianReflection>(texture_->Value(isect));
		}

        Spectrum ComputeBSDF(const IntersectionInfo& isect, const Vec3f& wo, const Vec3f& wi) const
            noexcept override
        {
            auto albedo = texture_->Value(isect);

            return albedo / kPI * abs(isect.normal.Dot(wi));
            if (wi.Dot(wo) < 0)
            {
                return albedo / kPI * abs(isect.normal.Dot(wi));
            }
            else
            {
                // lambertain does not transmit any light
                return Spectrum{kFloatZero};
            }
        }

        bool Scatter(const Ray& ray, const IntersectionInfo& isect, const Point2f& sample,
                     Spectrum& attenuation, Ray& scatter) const noexcept override
        {
            auto albedo = texture_->Value(isect);

            auto diffuse = SampleUniformSphere(sample);
            if (diffuse.Dot(isect.normal) < 0)
            {
                diffuse = -diffuse;
            }

            attenuation = albedo / kPI * diffuse.Dot(isect.normal) / PdfUniformHemisphere();
            scatter     = Ray{isect.point, diffuse};
            return true;
        }

    private:
        const Texture* texture_;
    };

    Material::Ptr CreateLambertian(const Texture* texture)
    {
        return std::make_unique<Lambertian>(texture);
    }
} // namespace akane