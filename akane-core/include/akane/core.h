#pragma once
#include "akane/math/float_type.h"
#include "akane/math/geometry.h"
#include "akane/common/memory_arena.h"

namespace akane
{
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
        akFloat t;

		Point3f point;
        Vec3f normal;
	};
} // namespace akane