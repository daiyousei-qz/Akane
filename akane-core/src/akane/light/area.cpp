#include "akane/light/area.h"
#include "akane/primitive.h"

namespace akane
{
    AreaLight::AreaLight(Primitive* primitive, Spectrum albedo)
        : primitive_(primitive), albedo_(albedo)
    {
    }

    Spectrum AreaLight::Eval(const Ray& ray) const
    {
        return albedo_;
    }

    VisibilityTester AreaLight::SampleLi(const Point2f& u, const IntersectionInfo& isect) const
    {
        Point3f point;
        akFloat pdf;
        primitive_->SampleP(u, point, pdf);

        return VisibilityTester{isect, point, pdf};
    }
    akFloat AreaLight::Power() const
    {
        return albedo_.Length() * primitive_->Area();
    }
} // namespace akane