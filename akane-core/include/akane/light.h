#pragma once
#include "akane/common/basic.h"
#include "akane/ray.h"
#include "akane/spectrum.h"
#include "edslib/memory/arena.h"
#include <memory>

namespace akane
{
    class Scene;
    class Primitive;

    // GlobalLight is usually distant light or skybox
    // which would not be explicit sampled during path-tracing
    class GlobalLight : public Object
    {
    public:
        virtual Spectrum Eval(const Ray& ray) const = 0;
    };

    class LightSample
    {
    public:
        LightSample(const Vec3& point, const Vec3& normal, float pdf)
            : point_(point), normal_(normal), pdf_(pdf)
        {
        }

        bool TestVisibility(const Scene& scene, Workspace& workspace, const Vec3& p,
                            const Primitive* obj) const;

        Ray GenerateTestRay(const Vec3& p) const noexcept
        {
            return Ray{point_, (p - point_).Normalized()};
        }

        Ray GenerateShadowRay(const Vec3& p) const noexcept
        {
            return Ray{p, (point_ - p).Normalized()};
        }

        Vec3 Point() const noexcept
        {
            return point_;
        }

        float Pdf() const noexcept
        {
            return pdf_;
        }

    private:
        Vec3 point_;  // point at light source
        Vec3 normal_; // zero if light comes from a delta light source
        float pdf_;
    };

    // Explicit Light Sampling
    // 1. sample a point from light source
    // 2. test visibility from hit point
    // 3. calculate light intensity contributed to the hit point
    class Light : public Object
    {
    public:
        virtual Spectrum Eval(const Ray& ray) const = 0;

        virtual LightSample SampleLi(const Point2f& u) const = 0;

        virtual float Power() const = 0;
    };

    class AreaLight : public Light
    {
    public:
        AreaLight(Primitive* object) : object_(object)
        {
        }

        const Primitive* GetObject() const noexcept
        {
            return object_;
        }

    private:
        const Primitive* object_;
    };
} // namespace akane