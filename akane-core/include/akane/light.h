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
        VisibilityTester(const IntersectionInfo& isect, const Point3f& src_point, akFloat src_pdf)
            : source_point_(src_point), source_pdf_(src_pdf)
        {
            object_       = isect.primitive;
			object_point_ = isect.point;
        }

        // TODO: support transparent primitive? medium?
        bool Test(const Scene& scene, Workspace& workspace) const;

        Ray TestRay() const noexcept
        {
            return Ray{source_point_, (object_point_ - source_point_).Normalized()};
        }

        Ray ShadowRay() const noexcept
        {
            return Ray{ object_point_, (source_point_ - object_point_).Normalized()};
        }

        akFloat Pdf() const noexcept
        {
            return source_pdf_;
        }

    private:
        Point3f source_point_;
        akFloat source_pdf_;

        const Primitive* object_;
        Point3f object_point_;
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