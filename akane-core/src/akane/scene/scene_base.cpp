#include "akane/scene/scene_base.h"
#include "akane/material/basic.h"
#include "akane/light/distant.h"
#include "akane/light/infinite.h"
#include "akane/light/skybox.h"

namespace akane
{
    Texture* SceneBase::CreateTexture_Solid(const Spectrum& albedo)
    {
        return Construct<SolidTexture>(albedo);
    }
	Texture* SceneBase::CreateTexture_Image(const Spectrum& albedo, Image::SharedPtr img)
	{
		return Construct<ImageTexture>(albedo, std::move(img));
	}
	Texture* SceneBase::CreateTexture_ImageOrSolid(const Spectrum& albedo, Image::SharedPtr img)
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
    Texture* SceneBase::CreateTexture_Checkerboard(const Texture* t0, const Texture* t1)
    {
        return Construct<CheckerboardTexture>(t0, t1);
    }

    Material* SceneBase::CreateMaterial_Lambertian(const Texture* t)
    {
        return Construct<Lambertian>(t);
    }
	Material* SceneBase::CreateMaterial_PerfectMirror(const Texture* t)
	{
		return Construct<PerfectMirror>(t);
	}

	//
	//

	Light* SceneBase::CreateGlobalLight_Infinite(const Spectrum& albedo)
	{
		auto light = Construct<InfiniteLight>(albedo);
		global_lights_.push_back(light);

		return light;
	}
	Light* SceneBase::CreateGlobalLight_Distant(Vec3f direction, const Spectrum& albedo)
	{
		auto light = Construct<DistantLight>(direction, albedo);
		global_lights_.push_back(light);

		return light;
	}
	Light* SceneBase::CreateGlobalLight_Skybox(const Spectrum& albedo)
	{
		auto light = Construct<SkyboxLight>(albedo);
		global_lights_.push_back(light);

		return light;
	}

    AreaLight* SceneBase::CreateLight_Area(Primitive* primitive, const Spectrum& albedo)
    {
		auto light = Construct<AreaLight>(primitive, albedo);
		lights_.push_back(light);

		return light;
    }

	// TODO: refine this
	Material* SceneBase::LoadMaterial(const MaterialDesc& desc)
	{
		if (desc.ks.Max() > 1e-5)
		{
			auto texture =
				CreateTexture_ImageOrSolid(desc.ks, desc.specular_texture);

			return CreateMaterial_PerfectMirror(texture);
		}
		else if (desc.kd.Max() > 1e-5)
		{
			auto texture =
				CreateTexture_ImageOrSolid(desc.kd, desc.diffuse_texture);

			return CreateMaterial_Lambertian(texture);
		}
		else
		{
			return nullptr;
		}
	}

} // namespace akane