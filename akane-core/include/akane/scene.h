#pragma once
#include "akane/common/memory_arena.h"
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
    class Scene final
    {
    public:
        using Ptr = std::unique_ptr<Scene>;

        Scene()
        {
            world_ = std::make_unique<NaiveComposite>();
        }

        // texture factory
        //

        Texture* AddSolidTexture(Spectrum albedo)
        {
            return AddTextureAux(std::make_unique<SolidTexture>(albedo));
        }
        Texture* AddCheckerboardTexture(const Texture* t0, const Texture* t1)
        {
            return AddTextureAux(std::make_unique<CheckerboardTexture>(t0, t1));
        }

        // material factory
        //

        Material* AddLambertian(const Texture* texture)
        {
            return AddMaterialAux(CreateLambertian(texture));
        }
        Material* AddMetal(const Texture* texture, akFloat fuzz)
        {
            return AddMaterialAux(CreateMetal(texture, fuzz));
        }
        Material* AddDielectrics(const Texture* texture,
                                 akFloat refractive_index)
        {
            return AddMaterialAux(CreateDielectrics(texture, refractive_index));
        }

        // primitive factory
        //

        GeometricPrimitive* AddSphere(const Material* material, Vec3f center,
                                      akFloat radius)
        {
            return AddGeometricPrimitiveAux(
                CreateSpherePrimitive(material, center, radius));
        }

        // light factory
        //

        Light* AddAreaLight(GeometricPrimitive* primitive,
                            const Spectrum& albedo)
        {
            return AddLightAux(CreateAreaLight(primitive, albedo));
        }

        void ConfigureInfiniteLight(const Vec3f direction,
                                    const Spectrum& albedo)
        {
            infinite_light_ =
                std::make_unique<InfiniteLight>(direction, albedo);
        }

        // getters
        //

        const Light* GetInfiniteLight() const noexcept
        {
            return infinite_light_.get();
        }

		const auto& GetLights() const noexcept
		{
			return lights_inteface_;
		}

        const Primitive& GetWorld() const noexcept
        {
            return *world_;
        }

    private:
        Texture* AddTextureAux(Texture::Ptr texture)
        {
            auto ptr = texture.get();
            textures_.push_back(std::move(texture));

            return ptr;
        }

        Material* AddMaterialAux(Material::Ptr material)
        {
            auto ptr = material.get();
            materials_.push_back(std::move(material));

            return ptr;
        }

        GeometricPrimitive*
        AddGeometricPrimitiveAux(GeometricPrimitive::Ptr primitive)
        {
            auto ptr = primitive.get();
            world_->AddPrimitive(ptr);
            bodies_.push_back(std::move(primitive));

            return ptr;
        }

        Light* AddLightAux(Light::Ptr light)
        {
            auto ptr = light.get();
            lights_.push_back(std::move(light));
			lights_inteface_.push_back(ptr);

            return ptr;
        }

        std::vector<Texture::Ptr> textures_;
        std::vector<Material::Ptr> materials_;
        std::vector<Primitive::Ptr> bodies_;
        std::vector<Light::Ptr> lights_;

        std::unique_ptr<InfiniteLight> infinite_light_;
        std::unique_ptr<NaiveComposite> world_;

		std::vector<const Light*> lights_inteface_;
    };
} // namespace akane