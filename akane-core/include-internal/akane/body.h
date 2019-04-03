#pragma once
#include "akane/core.h"

namespace akane
{
    class Body
    {
    public:
        virtual bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
                               IntersectionInfo& info) const = 0;
	};
} // namespace akane