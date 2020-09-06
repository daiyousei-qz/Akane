#pragma once
#include "akane/common/basic.h"
#include "akane/math/math.h"

namespace akane
{
    class AreaLight;
    class Material;
    class Primitive;

    struct Ray
    {
        Vec3 o; // origin
        Vec3 d; // direction
    };

    // create a ray from src point to dest point
    inline Ray RayFromTo(Vec3 src, Vec3 dest) noexcept
    {
        return Ray{src, (dest - src).Normalized()};
    }

    struct IntersectionInfo
    {
        // distance that ray travels to make the hit
        float t;

        // point where intersection happens
        Vec3 point;

        // normal vector of geometric surface at the hit point
        Vec3 ng;

        // normal vector of scattering surface at the hit point
        Vec3 ns;

        // uv coordianate at the hit point for texture mapping
        Vec2 uv = {0.f, 0.f};

        // triangle index for embree scene
        unsigned index = 0;

        // object that the ray hits
        const Primitive* object = nullptr;

        // material at the hit surface
        const Material* material = nullptr;

        // area light instance at the hit surface, if any
        const AreaLight* area_light = nullptr;
    };
} // namespace akane