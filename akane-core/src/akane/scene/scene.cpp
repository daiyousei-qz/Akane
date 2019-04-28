#include "akane/scene.h"
#include "akane/material/basic.h"
#include "akane/light/distant.h"
#include "akane/light/infinite.h"
#include "akane/light/skybox.h"
#include <vector>

namespace akane
{
	void Scene::Commit()
	{
		std::vector<akFloat> powers;
		for (auto light : lights_)
		{
			powers.push_back(light->Power());
		}

		auto begin = powers.data();
		auto end = begin + powers.size();

		light_power_ = std::accumulate(begin, end, 0.f);
		light_dist_.Reset(begin, end);
	}

    Texture* Scene::CreateTexture_Solid(const Spectrum& albedo)
    {
        return Construct<SolidTexture>(albedo);
    }
    Texture* Scene::CreateTexture_Image(const Spectrum& albedo, Image::SharedPtr img)
    {
        return Construct<ImageTexture>(albedo, std::move(img));
    }
    Texture* Scene::CreateTexture_ImageOrSolid(const Spectrum& albedo, Image::SharedPtr img)
    {
        if (img != nullptr)
        {
            return Construct<ImageTexture>(albedo, std::move(img));
        }
        else
        {
            return Construct<SolidTexture>(albedo);
        }
    }
    Texture* Scene::CreateTexture_Checkerboard(const Texture* t0, const Texture* t1)
    {
        return Construct<CheckerboardTexture>(t0, t1);
    }

    Material* Scene::CreateMaterial_Lambertian(const Texture* t)
    {
        return Construct<Lambertian>(t);
    }

    //
    //

    Light* Scene::CreateGlobalLight_Infinite(const Spectrum& albedo)
    {
        auto light = Construct<InfiniteLight>(albedo);
        global_lights_.push_back(light);

        return light;
    }
    Light* Scene::CreateGlobalLight_Distant(Vec3f direction, const Spectrum& albedo)
    {
        auto light = Construct<DistantLight>(direction, albedo);
        global_lights_.push_back(light);

        return light;
    }
    Light* Scene::CreateGlobalLight_Skybox(const Spectrum& albedo)
    {
        auto light = Construct<SkyboxLight>(albedo);
        global_lights_.push_back(light);

        return light;
    }

    AreaLight* Scene::CreateLight_Area(Primitive* primitive, const Spectrum& albedo)
    {
        auto light = Construct<AreaLight>(primitive, albedo);
        lights_.push_back(light);

        return light;
    }

    // TODO: refine this
    Material* Scene::LoadMaterial(const MaterialDesc& desc)
    {
		for (auto mat : edit_materials_)
		{
			if (mat->name_ == desc.name)
			{
				return mat;
			}
		}

        auto material = Construct<GenericMaterial>();

        if (!desc.name.empty())
        {
            material->name_ = desc.name;
        }
        else
        {
            material->name_ = "material " + std::to_string(edit_materials_.size());
        }

        material->kd_               = desc.kd;
        material->ks_               = desc.ks;
        material->tr_               = desc.tr;
        material->roughness_        = desc.roughness;
        material->eta_in_           = desc.eta;
        material->eta_out_          = 1.f;
        material->texture_diffuse_  = desc.diffuse_texture;
        material->texture_specular_ = desc.specular_texture;

		edit_materials_.push_back(material);
		return material;
    }

} // namespace akane