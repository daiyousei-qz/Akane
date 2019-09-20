#include "akane/scene/embree.h"
#include "akane/math/transform.h"
#include "akane/light/diffuse.h"
#include "akane/material/generic.h"
#include "akane/model.h"
#include <string>
#include <unordered_map>

using namespace std;

namespace akane
{
    namespace
    {
        static RTCDevice embree_device = nullptr;

        RTCDevice GetEmbreeDevice()
        {
            if (embree_device == nullptr)
            {
                embree_device = rtcNewDevice(nullptr);
            }

            AKANE_REQUIRE(embree_device != nullptr);

            return embree_device;
        }

        void InitializeEmbree()
        {
            GetEmbreeDevice();
        }

        void ReleaseEmbree()
        {
            if (embree_device != nullptr)
            {
                rtcReleaseDevice(embree_device);
                embree_device = nullptr;
            }
        }

        RTCRayHit CreateEmptyRayHit(const Ray& ak_ray)
        {
            RTCRayHit result;
            auto& ray = result.ray;
            auto& hit = result.hit;

            ray.org_x = ak_ray.o.X();
            ray.org_y = ak_ray.o.Y();
            ray.org_z = ak_ray.o.Z();
            ray.tnear = kTravelDistanceMin;

            ray.dir_x = ak_ray.d.X();
            ray.dir_y = ak_ray.d.Y();
            ray.dir_z = ak_ray.d.Z();
            ray.time  = 0.f;

            ray.tfar  = kTravelDistanceMax;
            ray.mask  = 0u;
            ray.id    = 0u;
            ray.flags = 0u;

            hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
            hit.geomID    = RTC_INVALID_GEOMETRY_ID;
            hit.primID    = RTC_INVALID_GEOMETRY_ID;

            return result;
        }
    } // namespace

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

        Vec3 GetVertex(size_t index) const noexcept
        {
            AKANE_ASSERT(index < vertex_count);

            const float* p = vertex_data.get() + index * kVertexIndexStride;
            return {p[0], p[1], p[2]};
        }

        Vec3 GetNormal(size_t index) const noexcept
        {
            AKANE_ASSERT(index < normal_count);

            const float* p = normal_data.get() + index * kNormalIndexStride;
            return {p[0], p[1], p[2]};
        }

        Vec2 GetUV(size_t index) const noexcept
        {
            AKANE_ASSERT(index < uv_count);

            const float* p = uv_data.get() + index * kUVIndexStride;
            return {p[0], p[1]};
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

        //
        std::vector<const AreaLight*> area_lights;
        const Material* material;

        static constexpr size_t kTriangleIndexStride = 3;
        static constexpr size_t kNormalIndexStride   = 3;
        static constexpr size_t kUVIndexStride       = 3;

        bool ContainAreaLight() const noexcept
        {
            return !area_lights.empty();
        }
        auto GetAreaLight(int prim_id) const noexcept
        {
            return area_lights[prim_id];
        }
        auto GetMaterial() const noexcept
        {
            return material;
        }

        std::tuple<Vec3, Vec3, Vec3> GetTriangle(size_t index) const noexcept
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
        std::tuple<Vec3, Vec3, Vec3> GetVertexNormal(size_t index) const noexcept
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
        std::tuple<Vec2, Vec2, Vec2> GetVertexUV(size_t index) const noexcept
        {
            AKANE_ASSERT(uv_indices != nullptr && index < triangle_count);

            auto p = uv_indices.get() + index * kUVIndexStride;
            return {mesh_buffer->GetUV(p[0]), mesh_buffer->GetUV(p[1]), mesh_buffer->GetUV(p[2])};
        }
    };

    EmbreeScene::EmbreeScene()
    {
        auto device = GetEmbreeDevice();

        scene_ = rtcNewScene(device);
    }

    EmbreeScene::~EmbreeScene()
    {
        rtcReleaseScene(scene_);
    }

    void EmbreeScene::Commit()
    {
        Scene::Commit();
        rtcCommitScene(scene_);
    }

    bool EmbreeScene::Intersect(const Ray& ray, Workspace& workspace, IntersectionInfo& isect) const
    {
        RTCIntersectContext ctx;
        rtcInitIntersectContext(&ctx);

        RTCRayHit ray_hit = CreateEmptyRayHit(ray);

        rtcIntersect1(scene_, &ctx, &ray_hit);
        auto geom_id = ray_hit.hit.geomID;
        auto prim_id = ray_hit.hit.primID;

        if (geom_id != RTC_INVALID_GEOMETRY_ID && prim_id != RTC_INVALID_GEOMETRY_ID)
        {
            auto geometry = geoms_[geom_id];

            isect.t = ray_hit.ray.tfar;

            isect.point = ray.o + isect.t * ray.d;

            isect.ng = Vec3{ray_hit.hit.Ng_x, ray_hit.hit.Ng_y, ray_hit.hit.Ng_z}.Normalized();

            // override shading normal
            if (geometry->HasVertexNormal())
            {
                auto [n0, n1, n2] = geometry->GetVertexNormal(prim_id);

                auto uu = ray_hit.hit.u;
                auto vv = ray_hit.hit.v;
                auto ww = 1 - uu - vv;

                isect.ns = ww * n0 + uu * n1 + vv * n2;
            }
            else
            {
                isect.ns = isect.ng;
            }

            // override uv
            if (geometry->HasVertexUV())
            {
                auto [uv0, uv1, uv2] = geometry->GetVertexUV(prim_id);

                auto uu = ray_hit.hit.u;
                auto vv = ray_hit.hit.v;
                auto ww = 1 - uu - vv;

                isect.uv = ww * uv0 + uu * uv1 + vv * uv2;
            }
            else
            {
                isect.uv = {ray_hit.hit.u, ray_hit.hit.v};
            }

            isect.index = prim_id;

            isect.object = InstantiateTemporaryPrimitive(workspace, geom_id, prim_id);

            if (geometry->ContainAreaLight())
            {
                isect.area_light = geometry->GetAreaLight(prim_id);
            }

            isect.material = geometry->material;

            return true;
        }
        else
        {
            return false;
        }
    }

    void ParseMeshBuffer(EmbreeMeshBuffer& mesh_buffer, const MeshDesc& mesh_data,
                         const Transform& transform)
    {
        // copy vertex data
        {
            size_t vertex_count      = mesh_data.vertices.size();
            mesh_buffer.vertex_count = vertex_count;
            mesh_buffer.vertex_data  = std::make_unique<float[]>(3 * vertex_count + 1);

            auto p = mesh_buffer.vertex_data.get();
            for (const auto& vertex : mesh_data.vertices)
            {
                auto transformed = transform.Apply(PointToVec(vertex));

                *(p++) = transformed.X();
                *(p++) = transformed.Y();
                *(p++) = transformed.Z();
            }

            *p = 0.f;
        }

        // copy normal data
        {
            size_t normal_count      = mesh_data.normals.size();
            mesh_buffer.normal_count = normal_count;

            if (normal_count > 0)
            {
                mesh_buffer.normal_data = std::make_unique<float[]>(3 * normal_count + 1);

                auto p = mesh_buffer.normal_data.get();
                for (const auto& normal : mesh_data.normals)
                {
                    auto transformed = transform.ApplyLinear(PointToVec(normal));

                    *(p++) = transformed.X();
                    *(p++) = transformed.Y();
                    *(p++) = transformed.Z();
                }

                *p = 0.f;
            }
            else
            {
                mesh_buffer.normal_data = nullptr;
            }
        }

        // copy uv data
        {
            size_t uv_count      = mesh_data.uv.size();
            mesh_buffer.uv_count = uv_count;

            if (uv_count > 0)
            {
                mesh_buffer.uv_data = std::make_unique<float[]>(2 * uv_count + 1);

                auto p = mesh_buffer.uv_data.get();
                for (const auto& uv : mesh_data.uv)
                {
                    *(p++) = uv[0];
                    *(p++) = uv[1];
                }

                *p = 0.f;
            }
            else
            {
                mesh_buffer.uv_data = nullptr;
            }
        }
    }

    void ParseMeshGeometry(EmbreeMeshGeometry& geometry, const GeometryDesc& geom_desc)
    {
        auto triangle_count = geom_desc.triangle_indices.size();

        // copy triangle indices
        {
            geometry.triangle_count   = triangle_count;
            geometry.triangle_indices = std::make_unique<uint32_t[]>(3 * triangle_count + 1);

            auto p = geometry.triangle_indices.get();
            for (const auto& triangle : geom_desc.triangle_indices)
            {
                *(p++) = triangle[0];
                *(p++) = triangle[1];
                *(p++) = triangle[2];
            }

            *p = 0.f;
        }

        // copy normal indices if presented
        if (!geom_desc.normal_indices.empty())
        {
            // AKANE_REQUIRE(geom_desc.normal_indices.size() == triangle_count);

            geometry.normal_indices = std::make_unique<uint32_t[]>(3 * triangle_count + 1);

            auto p = geometry.normal_indices.get();
            for (const auto& normal : geom_desc.normal_indices)
            {
                *(p++) = normal[0];
                *(p++) = normal[1];
                *(p++) = normal[2];
            }

            *p = 0.f;
        }

        // copy uv indices if presented
        if (!geom_desc.uv_indices.empty())
        {
            // AKANE_REQUIRE(geom_desc.uv_indices.size() == triangle_count);

            geometry.uv_indices = std::make_unique<uint32_t[]>(3 * triangle_count + 1);

            auto p = geometry.uv_indices.get();
            for (const auto& uv : geom_desc.uv_indices)
            {
                *(p++) = uv[0];
                *(p++) = uv[1];
                *(p++) = uv[2];
            }

            *p = 0.f;
        }
    }

    void EmbreeScene::AddMesh(const MeshDesc& mesh_desc, const Transform& transform)
    {
        auto mesh_buffer = arena_.Construct<EmbreeMeshBuffer>();
        ParseMeshBuffer(*mesh_buffer, mesh_desc, transform);

        unordered_map<string, GenericMaterial*> material_cache;
        for (const auto& geom_desc : mesh_desc.geomtries)
        {
            auto geometry = arena_.Construct<EmbreeMeshGeometry>();
            ParseMeshGeometry(*geometry, *geom_desc);

            geometry->mesh_buffer = mesh_buffer;

            // allocate id for the geometry
            auto geom_id      = RegisterMeshGeometry(geometry);
            geometry->geom_id = geom_id;

            // load material and light
            if (geom_desc->material != nullptr)
            {
                const auto& material_desc = *geom_desc->material;
                if (material_desc.emission.Max() > 1e-5)
                {
                    size_t triangle_count = geom_desc->triangle_indices.size();
                    for (size_t i = 0; i < triangle_count; ++i)
                    {
                        auto primitive = InstantiatePrimitive(geom_id, i);
                        auto light     = arena_.Construct<DiffuseAreaLight>(
                            primitive, material_desc.emission.Normalized(),
                            material_desc.emission.Length());

                        RegisterLight(light);
                        geometry->area_lights.push_back(light);
                    }
                }

                auto& cached_material = material_cache[material_desc.name];
                if (cached_material == nullptr)
                {
                    cached_material                    = arena_.Construct<GenericMaterial>();

                    cached_material->name_             = material_desc.name;
                    cached_material->kd_               = material_desc.kd;
                    cached_material->ks_               = material_desc.ks;
                    cached_material->tr_               = material_desc.tr;
                    cached_material->roughness_        = material_desc.roughness;
                    cached_material->eta_in_           = material_desc.eta;
                    cached_material->eta_out_          = 1.f;
                    cached_material->texture_diffuse_  = material_desc.diffuse_texture;
                    cached_material->texture_specular_ = material_desc.specular_texture;
                }

                geometry->material = cached_material;
            }
        }
    }

    /*
    void EmbreeScene::AddGround(akFloat z, Texture3D::SharedPtr tex)
    {
        auto mesh = std::make_shared<MeshDesc>();
        mesh->vertices = { {-1e5f, -1e5f, z}, {-1e5f, 1e5f, z}, {1e5f, 1e5f, z}, {1e5f, -1e5f, z} };
        mesh->uv = { {-1e4f, -1e4f}, {-1e4f, 1e4f}, {1e4f, 1e4f}, {1e4f, -1e4f} };

        auto material = std::make_shared<MaterialDesc>();
        material->name = "_ak_ground";
        material->kd = { 1.f, 1.f, 1.f };
        material->diffuse_texture = tex;

        auto geometry = std::make_shared<GeometryDesc>();
        geometry->name = "_ak_ground";
        geometry->triangle_indices = { {0, 2, 1}, {0, 3, 2} };
        geometry->uv_indices = { {0, 2, 1}, {0, 3, 2} };
        geometry->material = material;

        mesh->geomtries.push_back(geometry);
        AddMesh(mesh);
    }
    void EmbreeScene::AddTriangleLight(const Point3f& v0, const Point3f& v1, const Point3f& v2,
        const Spectrum& albedo)
    {
        auto mesh_buffer = Construct<EmbreeMeshBuffer>();

        {
            mesh_buffer->vertex_count = 3;
            mesh_buffer->vertex_data = std::make_unique<float[]>(10);

            auto p = mesh_buffer->vertex_data.get();
            p[0] = v0[0];
            p[1] = v0[1];
            p[2] = v0[2];
            p[3] = v1[0];
            p[4] = v1[1];
            p[5] = v1[2];
            p[6] = v2[0];
            p[7] = v2[1];
            p[8] = v2[2];
            p[9] = 0;

            mesh_buffer->normal_count = 0;
            mesh_buffer->normal_data = nullptr;

            mesh_buffer->uv_count = 0;
            mesh_buffer->uv_data = nullptr;
        }

        auto geometry = Construct<EmbreeMeshGeometry>();
        geometry->mesh_buffer = mesh_buffer;
        {
            geometry->triangle_count = 1;
            geometry->triangle_indices = std::make_unique<uint32_t[]>(7);

            auto p = geometry->triangle_indices.get();
            p[0] = 0;
            p[1] = 1;
            p[2] = 2;
            p[6] = 0;

            geometry->normal_indices = nullptr;
            geometry->uv_indices = nullptr;
        }

        RegisterMeshGeometry(geometry);

        auto primitive = InstantiatePrimitive(geometry->geom_id, 0);

        geometry->material = nullptr;
        geometry->area_lights = { CreateLight_Area(primitive, albedo) };
    }
    */

    unsigned EmbreeScene::RegisterMeshGeometry(EmbreeMeshGeometry* geometry)
    {
        auto id          = this->geoms_.size();
        auto mesh_buffer = geometry->mesh_buffer;

        // crate embree geometry instance
        auto rtc_geom =
            rtcNewGeometry(GetEmbreeDevice(), RTCGeometryType::RTC_GEOMETRY_TYPE_TRIANGLE);

        // register vertex buffer
        rtcSetSharedGeometryBuffer(rtc_geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
                                   mesh_buffer->vertex_data.get(), 0, 3 * 4,
                                   mesh_buffer->vertex_count);

        // register triangle buffer
        rtcSetSharedGeometryBuffer(rtc_geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
                                   geometry->triangle_indices.get(), 0, 3 * 4,
                                   geometry->triangle_count);

        // finalize
        rtcCommitGeometry(rtc_geom);
        rtcAttachGeometryByID(this->scene_, rtc_geom, id);

        // rtcReleaseGeometry(rtc_geom);

        // register id
        geometry->geom_id = id;
        this->geoms_.push_back(geometry);

        return id;
    }

    Primitive* EmbreeScene::InstantiatePrimitive(unsigned geom_id, unsigned prim_id)
    {
        auto p = arena_.Construct<EmbreeTriangle>();
        CreatePrimitiveAux(*p, geom_id, prim_id);

        return p;
    }

    Primitive* EmbreeScene::InstantiateTemporaryPrimitive(Workspace& workspace, unsigned geom_id,
                                                          unsigned prim_id) const
    {
        auto p = workspace.Construct<EmbreeTriangle>();
        CreatePrimitiveAux(*p, geom_id, prim_id);

        return p;
    }

    void EmbreeScene::CreatePrimitiveAux(EmbreeTriangle& p, unsigned geom_id,
                                         unsigned prim_id) const
    {
        p.geom_id_ = geom_id;
        p.prim_id_ = prim_id;

        auto [v0, v1, v2] = geoms_.at(geom_id)->GetTriangle(prim_id);
        p.v0_             = v0;
        p.e1_             = v1 - v0;
        p.e2_             = v2 - v0;

        p.area_ = Cross(p.e1_, p.e2_).Length() * .5f;
    }

} // namespace akane