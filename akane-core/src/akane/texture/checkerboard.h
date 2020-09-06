#pragma once
#include "akane/texture.h"

namespace akane
{
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
