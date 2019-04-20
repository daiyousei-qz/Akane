#pragma once
#include "akane/scene.h"
#include "akane/mesh.h"
#include "akane/light.h"
#include "akane/common/arena.h"
#include "akane/scene/scene_base.h"
#include "akane/primitive/embree_triangle.h"
#include "rtcore.h"

namespace akane
{
    struct EmbreeMeshGeometry
    {
        using Ptr = std::unique_ptr<EmbreeMeshGeometry>;

        unsigned geom_id;
        MeshDesc::SharedPtr mesh_data;

		AreaLight* area_light;
		Material* material;
    };

    class EmbreeScene : public SceneBase
    {
    public:
        EmbreeScene();
        ~EmbreeScene();

        void Commit();

        bool Intersect(const Ray& ray, Workspace& workspace,
                       IntersectionInfo& isect) const override;

        unsigned AddMesh(MeshDesc::SharedPtr mesh_data);

        Primitive* InstantiatePrimitive(unsigned geom_id, unsigned prim_id)
        {
            auto p = Construct<EmbreeTriangle>();
            CreatePrimitiveAux(*p, geom_id, prim_id);

            return p;
        }

    private:
        Primitive* InstantiateTemporaryPrimitive(Workspace& workspace, unsigned geom_id,
                                                       unsigned prim_id) const
        {
            auto p = workspace.Construct<EmbreeTriangle>();
            CreatePrimitiveAux(*p, geom_id, prim_id);

            return p;
        }

        void CreatePrimitiveAux(EmbreeTriangle& p, unsigned geom_id, unsigned prim_id) const
        {
			p.geom_id_ = geom_id;
			p.prim_id_ = prim_id;

			auto geometry = geoms_.at(geom_id);
			auto tri_indices = geometry->mesh_data->triangles.at(prim_id);

			p.v0_ = geometry->mesh_data->vertices[tri_indices.X()];
			p.v1_ = geometry->mesh_data->vertices[tri_indices.Y()];
			p.v2_ = geometry->mesh_data->vertices[tri_indices.Z()];

			auto e1 = p.v1_ - p.v0_;
			auto e2 = p.v2_ - p.v0_;
			p.area_ = e1.Cross(e2).Length() / 2;
        }

        RTCScene scene_;
        std::vector<EmbreeMeshGeometry*> geoms_;
    };
} // namespace akane