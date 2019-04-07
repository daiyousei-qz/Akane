#pragma once
#include "akane/math/float_type.h"
#include "akane/math/vector.h"
#include "akane/common/memory_arena.h"

namespace akane
{
	class Material;

    struct Ray
    {
        Point3f o;
        Vec3f d;
    };

    struct RenderingContext
    {
        MemoryArena arena;
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

		const Material* material;
	};
} // namespace akane