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

    // TODO: allow visibility test for global light
    class LightSample
    {
    public:
        LightSample(const Vec3& point, const Vec3& normal, float pdf, bool global = false)
            : point_(point), normal_(normal), pdf_(pdf), global_(global)
        {
        }

        bool TestVisibility(const Scene& scene, Workspace& workspace, const Vec3& p,
                            const Primitive* obj) const;

        Ray GenerateTestRay(const Vec3& p) const noexcept
        {
            return RayFromTo(point_, p);
        }

        Ray GenerateShadowRay(const Vec3& p) const noexcept
        {
            return RayFromTo(p, point_);
        }

        // point of light source
        Vec3 Point() const noexcept
        {
            return point_;
        }

        // probability distribution of the particular point
        float Pdf() const noexcept
        {
            return pdf_;
        }

    private:
        Vec3 point_;  // point at light source
        Vec3 normal_; // zero if light comes from a delta light source
        float pdf_;
        bool global_; // if this is a sample from global light
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