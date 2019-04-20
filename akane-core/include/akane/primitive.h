#pragma once
#include "akane/core.h"

namespace akane
{
	class AreaLight;

	class Primitive : public virtual Object
	{
	public:
		using Ptr = std::unique_ptr<Primitive>;

		// test intersection
		virtual bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
			IntersectionInfo& info) const = 0;

		// compute area of the primitive
		virtual akFloat Area() const = 0;

		// sample a point on the primitive's surface
		virtual void SampleP(const Point2f& u, Point3f& point_out,
			akFloat& pdf_out) const = 0;

		virtual bool Equals(const Primitive* other) const noexcept
		{
			return this == other;
		}
	};

	class Composite : public virtual Object
	{
	public:
		// test intersection
		virtual bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
			IntersectionInfo& info) const = 0;
	};

	inline bool SamePrimitive(const Primitive* lhs, const Primitive* rhs)
	{
		AKANE_ASSERT(lhs != nullptr && rhs != nullptr);
		return lhs == rhs || lhs->Equals(rhs);
	}

} // namespace akane