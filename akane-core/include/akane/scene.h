#pragma once
#include "akane/primitive.h"
#include "akane/camera.h"
#include "akane/light.h"
#include "akane/material.h"
#include "akane/primitive/composite.h"
#include "akane/primitive/sphere.h"
#include "akane/light/area.h"
#include "akane/light/infinite.h"
#include <memory>
#include <vector>

namespace akane
{
    using LightVec = std::vector<const Light*>;

    constexpr akFloat kDefaultDistanceMin = 0.001f;
    constexpr akFloat kDefaultDistanceMax  = 10000.f;

    class Scene : public virtual Object
    {
    public:
        using Ptr = std::unique_ptr<Scene>;

        virtual const const LightVec& GetGlobalLights() const noexcept = 0;
        virtual const LightVec& GetLights() const noexcept   = 0;

        virtual bool Intersect(const Ray& ray, Workspace& workspace,
                               IntersectionInfo& isect) const = 0;
    };
} // namespace akane