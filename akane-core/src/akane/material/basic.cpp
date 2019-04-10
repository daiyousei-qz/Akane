#include "akane/material.h"

namespace akane
{
    // assuming v and u is normalized
    static void ComputeReflectedRay(const Vec3f& v, const Vec3f& n, Vec3f& reflected) noexcept
    {
        reflected = v - 2.f * v.Dot(n) * n;
    }

    // assuming v and u is normalized
    static bool ComputedRefractedRay(const Vec3f& v, const Vec3f& n, akFloat n_ratio,
                                     Vec3f& refracted) noexcept
    {
        auto h            = v.Dot(n);
        auto discriminant = 1.f - n_ratio * n_ratio * (1.f - h * h);

        if (discriminant > 0)
        {
            refracted = n_ratio * (v - h * n) - sqrt(discriminant) * n;
            refracted = refracted.Normalized();
            return true;
        }
        else
        {
            return false;
        }
    }

    static akFloat Schlick(akFloat cosine, akFloat refractive_index) noexcept
    {
        auto r0 = (1 - refractive_index) / (1 + refractive_index);
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

    // specular
    class Metal : public Material
    {
    public:
        Metal(const Texture* texture, akFloat fuzz) : texture_(texture), fuzz_(fuzz)
        {
            assert(texture != nullptr);
            assert(fuzz >= 0 && fuzz < 1);
        }

        Spectrum ComputeBSDF(const IntersectionInfo& isect, const Vec3f& wo, const Vec3f& wi) const
            noexcept override
        {
            auto albedo = texture_->Value(isect);

            if (wi.Dot(wo) < 0)
            {
                return albedo * abs(isect.normal.Dot(wi)) * fuzz_;
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

            Vec3f reflected;
            ComputeReflectedRay(ray.d, isect.normal, reflected);

            auto fuzzed = reflected + fuzz_ * SampleUniformSphere(sample);
            if (fuzzed.Dot(isect.normal) > 0)
            {
                attenuation = albedo;
                scatter     = Ray{isect.point, fuzzed};
                return true;
            }
            else
            {
                return false;
            }
        }

    private:
        const Texture* texture_;
        akFloat fuzz_;
    };

    // transmission
    class Dielectrics : public Material
    {
    public:
        Dielectrics(const Texture* texture, akFloat refractive_index)
            : texture_(texture), refractive_index_(refractive_index)
        {
            assert(refractive_index >= 1);
        }

        Spectrum ComputeBSDF(const IntersectionInfo& isect, const Vec3f& wo, const Vec3f& wi) const
            noexcept override
        {
            return Spectrum{kFloatZero};
        }

        bool Scatter(const Ray& ray, const IntersectionInfo& isect, const Point2f& sample,
                     Spectrum& attenuation, Ray& scatter) const noexcept override
        {
            Vec3f reflected;
            ComputeReflectedRay(ray.d, isect.normal, reflected);

            Vec3f transmission_normal;
            akFloat n_ratio, cosine;

            if (ray.d.Dot(isect.normal) < 0)
            {
                // medium to material
                n_ratio             = 1 / refractive_index_;
                transmission_normal = isect.normal;
                cosine              = -ray.d.Dot(isect.normal);
            }
            else
            {
                // material to medium
                n_ratio             = refractive_index_;
                transmission_normal = -isect.normal;
                cosine              = refractive_index_ * ray.d.Dot(isect.normal);
            }

            akFloat reflect_prob = 1.f;
            Vec3f refracted;
            if (ComputedRefractedRay(ray.d, isect.normal, n_ratio, refracted))
            {
                reflect_prob = Schlick(cosine, refractive_index_);
            }

            // TODO: use external sampler
            attenuation = 1.f;
            if (static_cast<akFloat>(rand()) / RAND_MAX < reflect_prob)
            {
                scatter = Ray{isect.point, refracted};
            }
            else
            {
                scatter = Ray{isect.point, reflected};
            }

            return true;
        }

    private:
        const Texture* texture_;
        akFloat refractive_index_;
    };

    Material::Ptr CreateLambertian(const Texture* texture)
    {
        return std::make_unique<Lambertian>(texture);
    }
    Material::Ptr CreateMetal(const Texture* texture, akFloat fuzz)
    {
        return std::make_unique<Metal>(texture, fuzz);
    }
    Material::Ptr CreateDielectrics(const Texture* texture, akFloat refractive_index)
    {
        return std::make_unique<Dielectrics>(texture, refractive_index);
    }
} // namespace akane