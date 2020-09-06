#pragma once
#include "akane/math/math.h"
#include "akane/math/transform.h"

namespace akane
{
    // calculation of Bsdf instance would be done in local coordinate
    // where scattering normal vector is mapped to kBsdfNormal
    constexpr Vec3 kBsdfNormal = Vec3{0.f, 0.f, 1.f};

    // create transform from **world coordinate** to **bsdf local coordinate**
    // TODO: this transform only support non-polarized bsdf
    inline Transform CreateBsdfCoordTransform(Vec3 n)
    {
        if (n.X() != 0 || n.Y() != 0)
        {
            Vec3 nz = n.Normalized();
            Vec3 nx = Vec3{n.Y(), -n.X(), 0.f}.Normalized();
            Vec3 ny = Cross(n, nx);

            return Transform(nx, ny, nz);
        }
        else
        {
            Vec3 nz = n.Normalized();
            Vec3 nx = Vec3{n.Z() > 0 ? 1.f : -1.f, 0.f, 0.f};
            Vec3 ny = Vec3{0.f, 1.f, 0.f};

            return Transform(nx, ny, nz);
        }
    }

    inline bool SameHemisphere(const Vec3& wo, const Vec3& wi) noexcept
    {
        return wo.Z() * wi.Z() > 0;
    }

    inline float CosTheta(const Vec3& v) noexcept
    {
        return v.Z();
    }
    inline float Cos2Theta(const Vec3& v) noexcept
    {
        return v.Z() * v.Z();
    }
    inline float AbsCosTheta(const Vec3& v) noexcept
    {
        return abs(v.Z());
    }
    inline float Sin2Theta(const Vec3& v) noexcept
    {
        return max(0.f, 1.f - Cos2Theta(v));
    }

    inline float SinTheta(const Vec3& v) noexcept
    {
        return sqrt(Sin2Theta(v));
    }

    inline float TanTheta(const Vec3& v) noexcept
    {
        return SinTheta(v) / CosTheta(v);
    }

    inline float Tan2Theta(const Vec3& v) noexcept
    {
        return Sin2Theta(v) / Cos2Theta(v);
    }

    // assume SameHemiSphere(wo, n)
    inline Vec3 ReflectRay(const Vec3& wo, const Vec3& n) noexcept
    {
        auto h = wo.Dot(n);
        return -wo + 2 * h * n;
    }

    // assume n = +-kBsdfNormal
    inline Vec3 ReflectRayQuick(const Vec3& wo) noexcept
    {
        return Vec3{-wo.X(), -wo.Y(), wo.Z()};
    }

    // assume SameHemiSphere(wo, n)
    // eta = etaI / etaT, i.e. inverse of refractive index
    inline bool RefractRay(const Vec3& wo, const Vec3& n, float eta, Vec3& refracted) noexcept
    {
        float cosThetaI  = n.Dot(wo);
        float sin2ThetaI = max(0.f, 1.f - cosThetaI * cosThetaI);
        float sin2ThetaT = eta * eta * sin2ThetaI;
        if (sin2ThetaT > 1)
        {
            return false;
        }

        float cosThetaT = sqrt(1.f - sin2ThetaT);

        refracted = -eta * wo + (eta * cosThetaI - cosThetaT) * n;
        return true;
    }

    // eta = etaI / etaT
    inline constexpr float Schlick(float cos_theta, float eta) noexcept
    {
        auto r0 = (eta - 1) / (eta + 1);
        r0      = r0 * r0;

        auto root  = 1 - cos_theta;
        auto root2 = root * root;
        return r0 + (1 - r0) * root2 * root;
    }
} // namespace akane