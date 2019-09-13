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
        auto end   = begin + powers.size();

        light_power_ = std::accumulate(begin, end, 0.f);
        light_dist_.Reset(begin, end);
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
        std::string name;
        if (desc.name.empty())
        {
            name = "material " + std::to_string(edit_material_counter_);
            edit_material_counter_ += 1;
        }
        else
        {
            name = desc.name;
        }

        auto& material = edit_materials_[name];
        if (material == nullptr)
        {
            material = Construct<GenericMaterial>();

            material->kd_ = desc.kd;
            material->ks_ = desc.ks;
            material->tr_ = desc.tr;
            material->roughness_ = desc.roughness;
            material->eta_in_ = desc.eta;
            material->eta_out_ = 1.f;
            material->tex_diffuse_ = desc.diffuse_texture;
            material->tex_specular_ = desc.specular_texture;
        }

        return material;
    }

} // namespace akane