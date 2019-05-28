#pragma once
#include "akane/primitive.h"
#include "akane/camera.h"
#include "akane/light.h"
#include "akane/material.h"
#include "akane/texture.h"
#include "akane/mesh.h"
#include "akane/common/arena.h"
#include "akane/math/distribution.h"
#include "akane/material/generic.h"
#include "akane/light/area.h"
#include "akane/light/infinite.h"
#include <memory>
#include <vector>

namespace akane
{
    using LightVec     = std::vector<const Light*>;
    using AreaLightVec = std::vector<const AreaLight*>;

    constexpr akFloat kDefaultDistanceMin = 0.001f;
    constexpr akFloat kDefaultDistanceMax = 10000.f;

    class Scene : public virtual Object
    {
    public:
        using Ptr = std::unique_ptr<Scene>;

		virtual void Commit();

        virtual bool Intersect(const Ray& ray, Workspace& workspace,
                               IntersectionInfo& isect) const = 0;

	public:
		Light* CreateGlobalLight_Infinite(const Spectrum& albedo);
		Light* CreateGlobalLight_Distant(Vec3f direction, const Spectrum& albedo);
		Light* CreateGlobalLight_Skybox(const Spectrum& albedo);
		AreaLight* CreateLight_Area(Primitive* primitive, const Spectrum& albedo);

		Material* LoadMaterial(const MaterialDesc& desc);

		const LightVec& GetGlobalLights() const noexcept
		{
			return global_lights_;
		}
		const LightVec& GetLights() const noexcept
		{
			return lights_;
		}

		const auto& GetEditMaterials() const noexcept
		{
			return edit_materials_;
		}

		akFloat PdfLight(const Light* light) const noexcept
		{
			return light->Power() / light_power_;
		}

		const Light* SampleLight(akFloat u, akFloat& pdf_out) const noexcept
		{
			if (lights_.empty())
			{
				pdf_out = 0.f;
				return nullptr;
			}

			auto index = light_dist_.Sample(u, pdf_out);
			return lights_[index];
		}

	protected:
		template <typename T, typename... TArgs> T* Construct(TArgs&& ... args)
		{
			return arena_.Construct<T>(std::forward<TArgs>(args)...);
		}

	private:
		Arena arena_;

		akFloat light_power_;
		DiscrateDistribution light_dist_;
		LightVec global_lights_;
		LightVec lights_;

        int edit_material_counter_ = 0;
        std::unordered_map<std::string, GenericMaterial*> edit_materials_;
    };
} // namespace akane