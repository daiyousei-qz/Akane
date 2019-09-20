#pragma once
#include "akane/texture.h"

namespace akane
{
    class SolidTexture : public Texture3D
    {
    public:
        SolidTexture(const Vec3& albedo) : albedo_(albedo)
        {
        }

        Vec3 Eval(float u, float v) const noexcept override
        {
            return albedo_;
        }

    private:
        Vec3 albedo_;
    };
}