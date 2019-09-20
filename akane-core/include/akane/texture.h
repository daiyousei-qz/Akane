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

    template <TextureAddressMode AddressMode = TextureAddressMode::Repeat>
    inline float ResolveUV(float x) noexcept
    {
        if constexpr (AddressMode == TextureAddressMode::Repeat)
        {
            return x - floor(x);
        }
        else if (AddressMode == TextureAddressMode::Clamp)
        {
            return clamp(x, 0.f, 1.f);
        }
        else
        {
            static_assert(std::false_type::value);
        }
    }

    // TODO: anti-aliasing
    template <typename T> class BasicTexture : public Object
    {
    public:
        virtual T Eval(float u, float v) const noexcept = 0;
    };

    using Texture2D = BasicTexture<Vec2>;
    using Texture3D = BasicTexture<Vec3>;

    class CheckerboardTexture : public Texture3D
    {
    public:
        CheckerboardTexture(const Vec3& t0, const Vec3& t1, int n = 1) : t0_(t0), t1_(t1)
        {
            f_ = static_cast<float>(n) * kPi;
        }

        Spectrum Eval(float u, float v) const noexcept override
        {
            return sin(f_ * u) * sin(f_ * v) >= 0 ? t0_ : t1_;
        }

    private:
        float f_;
        Vec3 t0_, t1_;
    };
} // namespace akane