#pragma once
#include "akane/common/basic.h"
#include "akane/math/math.h"
#include "akane/math/transform.h"
#include "akane/ray.h"
#include "akane/model.h"
#include "akane/scene.h"
#include "akane/texture.h"
#include "akane/primitive.h"
#include "akane/primitive/embree/triangle.h"

#include <embree3/rtcore.h>
#include <memory>

namespace akane
{
    struct EmbreeMeshBuffer;
    struct EmbreeMeshGeometry;

    class EmbreeScene : public Scene
    {
    public:
        EmbreeScene();
        ~EmbreeScene();

        void Commit() override;

        bool Intersect(const Ray& ray, Workspace& workspace,
                       IntersectionInfo& isect) const override;

        void AddMesh(const MeshDesc& mesh_desc, const Transform& transform = Transform::Identity());

        // for testing
        void AddGround(float z, shared_ptr<Texture3D> tex);
        void AddTriangleLight(const Point3f& v0, const Point3f& v1, const Point3f& v2,
                              const Spectrum& color);

    private:
        unsigned RegisterMeshGeometry(EmbreeMeshGeometry* geometry);

        Primitive* InstantiatePrimitive(unsigned geom_id, unsigned prim_id);
        Primitive* InstantiateTemporaryPrimitive(Workspace& workspace, unsigned geom_id,
                                                 unsigned prim_id) const;

        void CreatePrimitiveAux(EmbreeTriangle& p, unsigned geom_id, unsigned prim_id) const;

        Arena arena_;

        RTCScene scene_;
        std::vector<const EmbreeMeshGeometry*> geoms_;
    };
} // namespace akane