#pragma once
#include "akane/core.h"
#include "akane/spectrum.h"
#include "akane/texture.h"
#include "akane/math/sampling.h"
#include <optional>

namespace akane
{
    class Material
    {
    public:
        using Ptr = std::unique_ptr<Material>;

        virtual bool Scatter(const Ray& ray, const IntersectionInfo& isect,
                             const Point2f& sample, Vec3f& attenuation,
                             Ray& scatter) const noexcept = 0;
    };

    // black body that absorb all incomming lights
    class BlackBody : public Material
    {
    public:
        virtual bool Scatter(const Ray& ray, const IntersectionInfo& isect,
                             const Point2f& sample, Vec3f& attenuation,
                             Ray& scatter) const noexcept override
        {
            return false;
        }
    };

    // pure diffuse
    class Lambertian : public Material
    {
    public:
        Lambertian(const Texture* texture) : texture_(texture)
        {
            assert(texture != nullptr);
        }

        bool Scatter(const Ray& ray, const IntersectionInfo& isect,
                     const Point2f& sample, Spectrum& attenuation,
                     Ray& scatter) const noexcept override
        {
            auto albedo = texture_->Value(isect);

            auto diffuse = SampleUniformSphere(sample);
            if (diffuse.Dot(isect.normal) < 0) { diffuse = -diffuse; }

            attenuation = albedo * diffuse.Dot(isect.normal);
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
        Metal(const Texture* texture, akFloat fuzz)
            : texture_(texture), fuzz_(fuzz)
        {
            assert(texture != nullptr);
            assert(fuzz >= 0 && fuzz < 1);
        }

        bool Scatter(const Ray& ray, const IntersectionInfo& isect,
                     const Point2f& sample, Spectrum& attenuation,
                     Ray& scatter) const noexcept override
        {
            auto albedo = texture_->Value(isect);

            auto reflect = ray.d - 2.f * ray.d.Dot(isect.normal) * isect.normal;
            auto fuzz_vec = fuzz_ * SampleUniformSphere(sample);

            auto d = reflect + fuzz_vec;

            if (d.Dot(isect.normal) > 0)
            {
                attenuation = albedo;
                scatter     = Ray{isect.point, d};
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

} // namespace akane