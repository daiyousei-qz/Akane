#pragma once
#include "akane/math/math.h"
#include "akane/spectrum.h"
#include <memory>

namespace akane
{
    enum class TextureAddressMode
    {
        Repeat,
        Clamp,
    };

    /**
     * Resolve uv coordinate into range [0, 1]
     */
    template <TextureAddressMode AddressMode = TextureAddressMode::Repeat>
    inline float ResolveUV(float x) noexcept
    {
        if constexpr (AddressMode == TextureAddressMode::Repeat)
        {
            return x - floor(x);
        }
        else if constexpr (AddressMode == TextureAddressMode::Clamp)
        {
            return clamp(x, 0.f, 1.f);
        }
        else
        {
            static_assert(std::false_type::value);
        }
    }

    // TODO: anti-aliasing
    template <typename TVec> class BasicTexture : public Object
    {
    public:
        virtual TVec Eval(float u, float v) const noexcept = 0;
    };

    using Texture2D = BasicTexture<Vec2>;
    using Texture3D = BasicTexture<Vec3>;
} // namespace akane