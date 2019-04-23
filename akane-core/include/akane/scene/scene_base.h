#pragma once
#include "akane/scene.h"
#include "akane/common/arena.h"
#include "akane/material.h"
#include "akane/texture.h"
#include "akane/mesh.h"
#include <vector>

namespace akane
{
    class SceneBase : public Scene
    {
    public:
        Texture* CreateTexture_Solid(const Spectrum& albedo);
        Texture* CreateTexture_Image(const Spectrum& albedo, Image::SharedPtr img);
        Texture* CreateTexture_ImageOrSolid(const Spectrum& albedo, Image::SharedPtr img);
        Texture* CreateTexture_Checkerboard(const Texture* t0, const Texture* t1);

        Material* CreateMaterial_Lambertian(const Texture* t);
		Material* CreateMaterial_PerfectMirror(const Texture* t);

        Light* CreateGlobalLight_Infinite(const Spectrum& albedo);
		Light* CreateGlobalLight_Distant(Vec3f direction, const Spectrum& albedo);
		Light* CreateGlobalLight_Skybox(const Spectrum& albedo);
        AreaLight* CreateLight_Area(Primitive* primitive, const Spectrum& albedo);

		Material* LoadMaterial(const MaterialDesc& desc);

        const LightVec& GetGlobalLights() const noexcept override
        {
            return global_lights_;
        }
        const LightVec& GetLights() const noexcept override
        {
            return lights_;
        }

    protected:
        template <typename T, typename... TArgs> T* Construct(TArgs&&... args)
        {
            return arena_.Construct<T>(std::forward<TArgs>(args)...);
        }

    private:
        Arena arena_;

        LightVec global_lights_;
        LightVec lights_;
    };
} // namespace akane