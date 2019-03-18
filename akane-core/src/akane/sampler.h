#pragma once
#include "akane/typedef.h"
#include "akane/geometry.h"

namespace akane
{
	class Sampler
	{
    public:
        Sampler()          = default;
        virtual ~Sampler() = default;

		virtual akFloat Get1D() = 0;
        virtual Point2f Get2D() = 0;
	};
}