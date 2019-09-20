#pragma once
#include "akane/scene.h"
#include "akane/primitive.h"
#include "akane/light.h"
#include "akane/material.h"
#include "akane/math/distribution.h"
#include "akane/primitive/integrated/geometric.h"

#include "akane/light/point.h"
#include "akane/light/spot.h"

#include "akane/global_light/linear_blend.h"
#include "akane/global_light/distant.h"

namespace akane
{
    class IntegratedScene : public Scene
    {
    public:
        using Ptr = std::unique_ptr<IntegratedScene>;

        IntegratedScene()
        {
        }

        bool Intersect(const Ray& ray, Workspace& workspace,
                       IntersectionInfo& isect) const override
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

        void AddLinearBlendLight(Vec3 color)
        {
            RegisterGlobalLight(arena_.Construct<LinearBlendLight>(color));
        }
        void AddDitantLight(Vec3 color, Vec3 direction)
        {
            RegisterGlobalLight(arena_.Construct<DistantLight>(color, direction));
        }

    private:
        Arena arena_;

        std::unique_ptr<NaiveComposite> world_ = std::make_unique<NaiveComposite>();
    };
} // namespace akane