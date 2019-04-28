#pragma once
#include "akane/scene.h"
#include "akane/mesh.h"
#include "akane/light.h"
#include "akane/common/arena.h"
#include "akane/math/transform.h"
#include "akane/primitive/embree_triangle.h"
#include "rtcore.h"

namespace akane
{
    struct EmbreeMeshBuffer
    {
        size_t vertex_count;
        std::unique_ptr<float[]> vertex_data; // layouts: [x, y, z]...

        size_t normal_count;
        std::unique_ptr<float[]> normal_data; // layouts: [x, y, z]...

        size_t uv_count;
        std::unique_ptr<float[]> uv_data; // layouts: [u, v]...

        static constexpr size_t kVertexIndexStride = 3;
        static constexpr size_t kNormalIndexStride = 3;
        static constexpr size_t kUVIndexStride     = 2;

        Point3f GetVertex(size_t index) const noexcept
        {
            AKANE_ASSERT(index < vertex_count);

            auto p = vertex_data.get() + index * kVertexIndexStride;
            return Point3f{p[0], p[1], p[2]};
        }

        Point3f GetNormal(size_t index) const noexcept
        {
            AKANE_ASSERT(index < normal_count);

            auto p = normal_data.get() + index * kNormalIndexStride;
            return Point3f{p[0], p[1], p[2]};
        }

        Point2f GetUV(size_t index) const noexcept
        {
            AKANE_ASSERT(index < uv_count);

            auto p = uv_data.get() + index * kUVIndexStride;
            return Point2f{p[0], p[1]};
        }
    };

    struct EmbreeMeshGeometry
    {
        unsigned geom_id;
        const EmbreeMeshBuffer* mesh_buffer;

        size_t triangle_count;
        std::unique_ptr<uint32_t[]> triangle_indices; // layouts: [x, y, z]...
        std::unique_ptr<uint32_t[]> normal_indices;   // layouts: [x, y, z]...
        std::unique_ptr<uint32_t[]> uv_indices;       // layouts: [x, y, z]...

        std::vector<const AreaLight*> area_lights;
        const Material* material;

        static constexpr size_t kTriangleIndexStride = 3;
        static constexpr size_t kNormalIndexStride   = 3;
        static constexpr size_t kUVIndexStride       = 3;

        auto GetAreaLight(int prim_id) const noexcept
        {
            return area_lights[prim_id];
        }
        auto GetMaterial() const noexcept
        {
            return material;
        }

        std::tuple<Point3f, Point3f, Point3f> GetTriangle(size_t index) const noexcept
        {
            AKANE_ASSERT(index < triangle_count);

            auto p = triangle_indices.get() + index * kTriangleIndexStride;
            return {mesh_buffer->GetVertex(p[0]), mesh_buffer->GetVertex(p[1]),
                    mesh_buffer->GetVertex(p[2])};
        }

        bool HasVertexNormal() const noexcept
        {
            return normal_indices != nullptr;
        }
        std::tuple<Vec3f, Vec3f, Vec3f> GetVertexNormal(size_t index) const noexcept
        {
            AKANE_ASSERT(normal_indices != nullptr && index < triangle_count);

            auto p = normal_indices.get() + index * kNormalIndexStride;
            return {mesh_buffer->GetNormal(p[0]), mesh_buffer->GetNormal(p[1]),
                    mesh_buffer->GetNormal(p[2])};
        }

        bool HasVertexUV() const noexcept
        {
            return uv_indices != nullptr;
        }
        std::tuple<Point2f, Point2f, Point2f> GetVertexUV(size_t index) const noexcept
        {
            AKANE_ASSERT(uv_indices != nullptr && index < triangle_count);

            auto p = uv_indices.get() + index * kUVIndexStride;
            return {mesh_buffer->GetUV(p[0]), mesh_buffer->GetUV(p[1]), mesh_buffer->GetUV(p[2])};
        }
    };

    class EmbreeScene : public Scene
    {
    public:
        EmbreeScene();
        ~EmbreeScene();

		void Commit() override;

        bool Intersect(const Ray& ray, Workspace& workspace,
                       IntersectionInfo& isect) const override;

        void AddMesh(MeshDesc::SharedPtr mesh_data,
                     const Transform& transform = Transform::Identity());

        // for testing
        void AddGround(akFloat z, const Spectrum& albedo);
        void AddTriangleLight(const Point3f& v0, const Point3f& v1, const Point3f& v2,
                              const Spectrum& albedo);

    private:
        unsigned RegisterMeshGeometry(EmbreeMeshGeometry* geometry);

        Primitive* InstantiatePrimitive(unsigned geom_id, unsigned prim_id)
        {
            auto p = Construct<EmbreeTriangle>();
            CreatePrimitiveAux(*p, geom_id, prim_id);

            return p;
        }

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

            auto [v0, v1, v2] = geoms_.at(geom_id)->GetTriangle(prim_id);
            p.v0_             = v0;
            p.v1_             = v1;
            p.v2_             = v2;

            auto e1 = p.v1_ - p.v0_;
            auto e2 = p.v2_ - p.v0_;
            p.area_ = e1.Cross(e2).Length() / 2;
        }

        RTCScene scene_;
        std::vector<const EmbreeMeshGeometry*> geoms_;
    };
} // namespace akane