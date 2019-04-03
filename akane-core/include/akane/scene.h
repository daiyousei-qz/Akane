#pragma once
#include "akane/common/memory_arena.h"
#include "akane/body.h"
#include "akane/camera.h"
#include "akane/light.h"
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
        Camera::Ptr camera_;
        std::vector<Body::Ptr> bodies_;
        std::vector<Light::Ptr> lights_;
    };
} // namespace akane