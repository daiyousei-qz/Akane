#pragma once
#include "akane/common/memory_arena.h"
#include "akane/body.h"
#include <memory>
#include <vector>

namespace akane
{
    class Scene final
    {
    public:
        using Ptr = std::unique_ptr<Scene>;

		void AddSphere();

    private:
        MemoryArena arena_;
		std::vector<Body*> bodies_;
    };
} // namespace akane