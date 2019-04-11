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
		// distance that ray has travalled
        akFloat t;

		// point where intersection happens
		Point3f point;

		// uv coordianate at point for texture mapping
		Point2f uv;

        Vec3f normal;

		// if ray comes from 
		bool internal_ray;

		// primitive that ray hits
		const Primitive* primitive;

		const Material* material;
	};
} // namespace akane