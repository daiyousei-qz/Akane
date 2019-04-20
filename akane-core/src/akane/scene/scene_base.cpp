#include "akane/scene/scene_base.h"
#include "akane/material/basic.h"
#include "akane/light/distant.h"
#include "akane/light/infinite.h"

namespace akane
{
    Texture* SceneBase::CreateTexture_Solid(std::vector<Spectrum> albedo)
    {
        return Construct<SolidTexture>(std::move(albedo));
    }
    Texture* SceneBase::CreateTexture_Checkerboard(const Texture* t0, const Texture* t1)
    {
        return Construct<CheckerboardTexture>(t0, t1);
    }

    Material* SceneBase::CreateMaterial_Lambertian(const Texture* t)
    {
        return Construct<Lambertian>(t);
    }

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

    AreaLight* SceneBase::CreateLight_Area(Primitive* primitive, const Spectrum& albedo)
    {
		auto light = Construct<AreaLight>(primitive, albedo);
		lights_.push_back(light);

		return light;
    }
} // namespace akane