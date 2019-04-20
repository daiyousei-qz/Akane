#pragma once
#include "akane/scene/scene_base.h"

namespace akane
{
    class AkaneScene : public SceneBase
    {
    public:
		using Ptr = std::unique_ptr<AkaneScene>;

		AkaneScene()
        {
            world_ = std::make_unique<NaiveComposite>();
        }

        // primitive factory
        //

        Primitive* AddSphere(const Material* material, Vec3f center, akFloat radius)
        {
            return AddGeometricPrimitiveAux(CreateSpherePrimitive(material, center, radius));
        }

        bool Intersect(const Ray& ray, Workspace& workspace, IntersectionInfo& isect) const override
        {
            return world_->Intersect(ray, kDefaultDistanceMin, kDefaultDistanceMax, isect);
        }

    private:
        Primitive* AddGeometricPrimitiveAux(Primitive::Ptr primitive)
        {
            auto ptr = primitive.get();
            world_->AddPrimitive(ptr);
            bodies_.push_back(std::move(primitive));

            return ptr;
        }

        std::vector<Primitive::Ptr> bodies_;

        std::unique_ptr<NaiveComposite> world_;
    };
} // namespace akane