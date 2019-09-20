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

    struct IntersectionInfo
    {
        // distance that ray travels to make the hit
        float t;

        // point where intersection happens
        Vec3 point;

        // geometric normal of hit object at the hit point
        Vec3 ng;

        // scattering normal of hit object at the hit point
        Vec3 ns;

        // uv coordianate at the hit point for texture mapping
        Vec2 uv = {0.f, 0.f};

        // triangle index for embree scene
        unsigned index = 0;

        // object that ray hits
        const Primitive* object = nullptr;

        // material at the hit point
        const Material* material = nullptr;

        // area light at the hit point
        const AreaLight* area_light = nullptr;
    };
} // namespace akane