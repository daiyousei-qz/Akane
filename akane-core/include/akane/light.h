#pragma once
#include <memory>

namespace akane
{
	class Light
	{
    public:
		using Ptr = std::unique_ptr<Light>;
	};
}