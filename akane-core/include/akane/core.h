#pragma once
#include "akane/common/object.h"
#include "akane/common/language_utils.h"
#include "akane/math/float_type.h"
#include "akane/math/vector.h"
#include "akane/common/workspace.h"

namespace akane
{
	class Primitive;
	class Material;
	class AreaLight;

    struct Ray
    {
        Point3f o;
        Vec3f d;
    };

    struct RenderingContext
    {
         Workspace workspace;
    };

	struct IntersectionInfo
	{
		// distance that ray travels to make the hit
		akFloat t;

		// point where intersection happens
		Point3f point;

		// geometric normal at the hit point
		Vec3f ng;

		// scattering normal at the hit point
        Vec3f ns;

		// uv coordianate at point for texture mapping
		Point2f uv;

		// triangle index for embree scene
		unsigned index;

		// primitive that ray hits
		const Primitive* primitive;

		// area light at the hit point
		const AreaLight* area_light;

		// material at the hit point
		const Material* material;
	};
} // namespace akane