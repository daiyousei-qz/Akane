#include "akane/light/area.h"
#include "akane/primitive.h"

namespace akane
{
    AreaLight::AreaLight(GeometricPrimitive* primitive, Spectrum albedo)
        : primitive_(primitive), albedo_(albedo)
    {
        AKANE_REQUIRE(primitive != nullptr &&
                      primitive->GetAreaLight() == nullptr);

        primitive->RegisterLightSource(this);
    }

    Spectrum AreaLight::Eval(const Ray& ray) const
    {
        return albedo_;
    }

    void AreaLight::SampleLi(const Point2f& u, const IntersectionInfo& isect,
                             Vec3f& wi_out, akFloat& pdf_out) const
    {
        Point3f point;
        akFloat pdf;
        primitive_->SampleP(u, point, pdf);

        wi_out  = (isect.point - point).Normalized();
        pdf_out = pdf;
    }
    akFloat AreaLight::Power() const
    {
        return albedo_.LengthSq() * primitive_->Area();
    }
} // namespace akane