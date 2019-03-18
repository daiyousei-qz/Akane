#pragma once
#include "akane/typedef.h"
#include "akane/geometry.h"
#include "akane/random.h"
#include "akane/memory_arena.h"

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
        RandomSource random;
	};
}