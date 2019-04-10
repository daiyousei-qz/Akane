#pragma once
#include "akane/core.h"
#include "akane/spectrum.h"
#include <memory>

namespace akane
{
    class Light : public Object
    {
    public:
        using Ptr = std::unique_ptr<Light>;

        virtual Spectrum Eval(const Ray& ray) const = 0;

        virtual void SampleLi(const Point2f& u, const IntersectionInfo& isect,
                              Vec3f& wi_out, akFloat& pdf_out) const = 0;

        virtual akFloat Power() const = 0;
    };

	class VisibilityTester
	{
		
	};
} // namespace akane