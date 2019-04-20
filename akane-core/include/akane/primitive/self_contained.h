#pragma once
#include "akane/primitive.h"

namespace akane
{
    class SelfContainedPrimitive : public Primitive
    {
    public:
        SelfContainedPrimitive(const Material* material) : material_(material)
        {
        }

        void BindAreaLight(AreaLight* light) noexcept
        {
            AKANE_ASSERT(light != nullptr);

            light_ = light;
        }

        const Material* GetMaterial() const noexcept
        {
            return material_;
        }

        const AreaLight* GetAreaLight() const noexcept
        {
            return light_;
        }

    private:
        const Material* material_;
        const AreaLight* light_;
    };
} // namespace akane