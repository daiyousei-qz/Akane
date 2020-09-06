#pragma once
#include "akane/scene.h"
#include "akane/primitive.h"
#include "akane/light.h"
#include "akane/material.h"
#include "akane/math/distribution.h"
#include "akane/primitive/integrated/geometric.h"

#include "akane/light/point.h"
#include "akane/light/spot.h"
#include "akane/light/distant.h"
#include "akane/light/skybox.h"

namespace akane
{
    class IntegratedScene : public Scene
    {
    public:
        using Ptr = std::unique_ptr<IntegratedScene>;

        IntegratedScene()
        {
        }

        bool Intersect(const Ray& ray, Workspace& workspace, IntersectionInfo& isect) const override
        {
            return world_->Intersect(ray, kTravelDistanceMin, kTravelDistanceMax, isect);
        }

        // primitive factory
        //

        template <typename ShapeType>
        void AddGeometricPrimitive(ShapeType shape, shared_ptr<Material> mat)
        {
            auto object = arena_.Construct<GeometricPrimitive<ShapeType>>(shape);
            object->BindMaterial(move(mat));

            world_->AddPrimitive(object);
        }

        template <typename ShapeType>
        void AddGeometricLight(ShapeType shape, Vec3 color, float power)
        {
            auto object = arena_.Construct<GeometricPrimitive<ShapeType>>(shape);

            object->BindAreaLight(color, power);
            RegisterLight(object->GetAreaLight());

            world_->AddPrimitive(object);
        }

        void AddPointLight(Vec3 point, Vec3 color, float power)
        {
            RegisterLight(arena_.Construct<PointLight>(point, color, power));
        }
        void AddSpotLight(Vec3 point, Vec3 direction, float theta, Vec3 color, float power)
        {
            RegisterLight(arena_.Construct<SpotLight>(point, direction, theta, color, power));
        }

        void AddDistantLight(const Vec3& direction, const Spectrum& color, Vec3 world_center,
                             float world_radius)
        {
            AKANE_REQUIRE(GetGlobalLight() == nullptr);
            RegisterLight(
                arena_.Construct<DistantLight>(direction, color, world_center, world_radius), true);
        }
        void AddSkybox(const Spectrum& albedo, Vec3 world_center, float world_radius)
        {
            RegisterLight(arena_.Construct<SkyboxLight>(albedo, world_center, world_radius), true);
        }

    private:
        Arena arena_;

        std::unique_ptr<NaiveComposite> world_ = std::make_unique<NaiveComposite>();
    };
} // namespace akane