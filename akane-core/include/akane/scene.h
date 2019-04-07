#pragma once
#include "akane/common/memory_arena.h"
#include "akane/body.h"
#include "akane/camera.h"
#include "akane/light.h"
#include "akane/material.h"
#include "akane/body/composite.h"
#include "akane/body/sphere.h"
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

		const Texture* RegisterSolidTexture(Spectrum albedo)
		{
			textures_.push_back(std::make_unique<SolidTexture>(albedo));

			return textures_.back().get();
		}
		const Texture* RegisterCheckerboardTexture(const Texture* t0, const Texture* t1)
		{
			textures_.push_back(std::make_unique<CheckerboardTexture>(t0, t1));

			return textures_.back().get();
		}

		const Material* RegisterLambertian(const Texture* texture)
		{
			materials_.push_back(std::make_unique<Lambertian>(texture));

			return materials_.back().get();
		}
		const Material* RegisterMetal(const Texture* texture, akFloat fuzz)
		{
			materials_.push_back(std::make_unique<Metal>(texture, fuzz));

			return materials_.back().get();
		}

        void AddSphere(const Material* material, Vec3f center, akFloat radius)
        {
            bodies_.push_back(CreateSphereBody(material, center, radius));

            world_->AddBody(bodies_.back().get());
        }

        const Body& GetWorld() const
        {
            return *world_;
        }

    private:
		std::vector<Texture::Ptr> textures_;
		std::vector<Material::Ptr> materials_;
        std::vector<Body::Ptr> bodies_;
        std::vector<Light::Ptr> lights_;

        std::unique_ptr<NaiveComposite> world_;
    };
} // namespace akane