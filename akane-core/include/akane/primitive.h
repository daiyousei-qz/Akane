#pragma once
#include "akane/core.h"

namespace akane
{
	class AreaLight;

    class Primitive : public Object
    {
    public:
        using Ptr = std::unique_ptr<Primitive>;

        virtual ~Primitive() = default;

        // virtual AABB GetBoundBox() const = 0;

		virtual const AreaLight* GetAreaLight() const noexcept
		{
			return nullptr;
		}

        // test intersection
        virtual bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
                               IntersectionInfo& info) const = 0;
    };

    class GeometricPrimitive : public Primitive
    {
    public:
		using Ptr = std::unique_ptr<GeometricPrimitive>;

        GeometricPrimitive(const Material* material)
            : material_(material), light_(nullptr)
        {
        }

        const Material* GetMaterial() const noexcept
        {
            return material_;
        }

		const AreaLight* GetAreaLight() const noexcept override
        {
            return light_;
        }

        // compute area of the primitive
        virtual akFloat Area() const = 0;

        // sample a point on the primitive's surface
        virtual void SampleP(const Point2f& u, Point3f& point_out,
                             akFloat& pdf_out) const = 0;

		void RegisterLightSource(AreaLight* light) noexcept
		{
			AKANE_ASSERT(light != nullptr);

			light_ = light;
		}

    private:
        const Material* material_;
        const AreaLight* light_;
    };
} // namespace akane