#pragma once
#include "akane/core.h"
#include "akane/spectrum.h"
#include <memory>

namespace akane
{
    class Scene;

    class VisibilityTester
    {
    public:
        VisibilityTester()
        {
        }
        VisibilityTester(const IntersectionInfo& isect, const Point3f& pt_light, akFloat pdf_light)
            : point_light_(pt_light), pdf_light_(pdf_light)
        {
            object_       = isect.primitive;
            point_object_ = isect.point;
        }

        // TODO: support transparent primitive? medium?
        bool Test(const Scene& scene) const;

        Ray TestRay() const noexcept
        {
            return Ray{point_light_, (point_object_ - point_light_).Normalized()};
        }

        Ray ShadowRay() const noexcept
        {
            return Ray{point_object_, (point_light_ - point_object_).Normalized()};
        }

        akFloat Pdf() const noexcept
        {
            return pdf_light_;
        }

    private:
        Point3f point_light_;
        akFloat pdf_light_;

        const Primitive* object_;
        Point3f point_object_;
    };

    class Light : public Object
    {
    public:
        using Ptr = std::unique_ptr<Light>;

        virtual Spectrum Eval(const Ray& ray) const = 0;

        virtual VisibilityTester SampleLi(const Point2f& u,
                                          const IntersectionInfo& isect) const = 0;

        virtual akFloat Power() const = 0;
    };
} // namespace akane