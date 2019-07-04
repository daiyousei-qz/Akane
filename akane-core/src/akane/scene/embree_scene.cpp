#include "akane/scene/embree_scene.h"
#include "akane/math/vector.h"
#include "akane/primitive/embree_triangle.h"
#include "akane/core.h"
#include "akane/scene.h"
#include "akane/render.h"
#include "akane/mesh.h"
#include <vector>

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

            if (embree_device == nullptr)
            {
                throw "";
            }

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
            ray.tnear = kDefaultDistanceMin;

            ray.dir_x = ak_ray.d.X();
            ray.dir_y = ak_ray.d.Y();
            ray.dir_z = ak_ray.d.Z();
            ray.time  = 0.f;

            ray.tfar  = kDefaultDistanceMax;
            ray.mask  = 0u;
            ray.id    = 0u;
            ray.flags = 0u;

            hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
            hit.geomID    = RTC_INVALID_GEOMETRY_ID;
            hit.primID    = RTC_INVALID_GEOMETRY_ID;

            return result;
        }
    } // namespace

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

            isect.ng = Vec3f{ray_hit.hit.Ng_x, ray_hit.hit.Ng_y, ray_hit.hit.Ng_z}.Normalized();

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

            isect.primitive = InstantiateTemporaryPrimitive(workspace, geom_id, prim_id);

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
            auto vertex_count        = mesh_data.vertices.size();
            mesh_buffer.vertex_count = vertex_count;
            mesh_buffer.vertex_data  = std::make_unique<float[]>(3 * vertex_count + 1);

            auto p = mesh_buffer.vertex_data.get();
            for (const auto& vertex : mesh_data.vertices)
            {
                auto transformed = transform.Apply(vertex);

                *(p++) = transformed.X();
                *(p++) = transformed.Y();
                *(p++) = transformed.Z();
            }

            *p = 0.f;
        }

        // copy normal data
        {
            auto normal_count        = mesh_data.normals.size();
            mesh_buffer.normal_count = normal_count;

            if (normal_count > 0)
            {
                mesh_buffer.normal_data = std::make_unique<float[]>(3 * normal_count + 1);

                auto p = mesh_buffer.normal_data.get();
                for (const auto& normal : mesh_data.normals)
                {
                    auto transformed = transform.ApplyLinear(normal);

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
            auto uv_count        = mesh_data.uv.size();
            mesh_buffer.uv_count = uv_count;

            if (uv_count > 0)
            {
                mesh_buffer.uv_data = std::make_unique<float[]>(2 * uv_count + 1);

                auto p = mesh_buffer.uv_data.get();
                for (const auto& uv : mesh_data.uv)
                {
                    *(p++) = uv.X();
                    *(p++) = uv.Y();
                }

                *p = 0.f;
            }
            else
            {
                mesh_buffer.uv_data = nullptr;
            }
        }
    }

    void ParseMeshGeometry(EmbreeMeshGeometry& geometry, const GeometryDesc& geom_desc,
                           const Transform& transform)
    {
        auto triangle_count = geom_desc.triangle_indices.size();

        // copy triangle indices
        {
            geometry.triangle_count   = triangle_count;
            geometry.triangle_indices = std::make_unique<uint32_t[]>(3 * triangle_count + 1);

            auto p = geometry.triangle_indices.get();
            for (const auto& triangle : geom_desc.triangle_indices)
            {
                *(p++) = triangle.X();
                *(p++) = triangle.Y();
                *(p++) = triangle.Z();
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
                *(p++) = normal.X();
                *(p++) = normal.Y();
                *(p++) = normal.Z();
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
                *(p++) = uv.X();
                *(p++) = uv.Y();
                *(p++) = uv.Z();
            }

            *p = 0.f;
        }
    }

    void EmbreeScene::AddMesh(MeshDesc::SharedPtr mesh_desc, const Transform& transform)
    {
        auto mesh_buffer = Construct<EmbreeMeshBuffer>();
        ParseMeshBuffer(*mesh_buffer, *mesh_desc, transform);

        for (const auto& geom_desc : mesh_desc->geomtries)
        {
            auto geometry = Construct<EmbreeMeshGeometry>();
            ParseMeshGeometry(*geometry, *geom_desc, transform);

            geometry->mesh_buffer = mesh_buffer;

            // allocate id for the geometry
            auto geom_id      = RegisterMeshGeometry(geometry);
            geometry->geom_id = geom_id;

            // load material and light
            if (geom_desc->material != nullptr)
            {
                const auto& material = *geom_desc->material;
                if (material.emission.Max() > 1e-5)
                {
                    auto triangle_count = geom_desc->triangle_indices.size();
                    for (int i = 0; i < triangle_count; ++i)
                    {
                        auto primitive = InstantiatePrimitive(geom_id, i);
                        auto light     = CreateLight_Area(primitive, material.emission);

                        geometry->area_lights.push_back(light);
                    }
                }

                geometry->material = LoadMaterial(material);
            }
        }
    }

    void EmbreeScene::AddGround(akFloat z, Texture3D::SharedPtr tex)
    {
        auto mesh      = std::make_shared<MeshDesc>();
        mesh->vertices = {{-1e5f, -1e5f, z}, {-1e5f, 1e5f, z}, {1e5f, 1e5f, z}, {1e5f, -1e5f, z}};
        mesh->uv       = {{-1e4f, -1e4f}, {-1e4f, 1e4f}, {1e4f, 1e4f}, {1e4f, -1e4f}};

        auto material             = std::make_shared<MaterialDesc>();
        material->name            = "_ak_ground";
        material->kd              = {1.f, 1.f, 1.f};
        material->diffuse_texture = tex;

        auto geometry              = std::make_shared<GeometryDesc>();
        geometry->name             = "_ak_ground";
        geometry->triangle_indices = {{0, 2, 1}, {0, 3, 2}};
        geometry->uv_indices       = {{0, 2, 1}, {0, 3, 2}};
        geometry->material         = material;

        mesh->geomtries.push_back(geometry);
        AddMesh(mesh);
    }
    void EmbreeScene::AddTriangleLight(const Point3f& v0, const Point3f& v1, const Point3f& v2,
                                       const Spectrum& albedo)
    {
        auto mesh_buffer = Construct<EmbreeMeshBuffer>();

        {
            mesh_buffer->vertex_count = 3;
            mesh_buffer->vertex_data  = std::make_unique<float[]>(10);

            auto p = mesh_buffer->vertex_data.get();
            p[0]   = v0[0];
            p[1]   = v0[1];
            p[2]   = v0[2];
            p[3]   = v1[0];
            p[4]   = v1[1];
            p[5]   = v1[2];
            p[6]   = v2[0];
            p[7]   = v2[1];
            p[8]   = v2[2];
            p[9]   = 0;

            mesh_buffer->normal_count = 0;
            mesh_buffer->normal_data  = nullptr;

            mesh_buffer->uv_count = 0;
            mesh_buffer->uv_data  = nullptr;
        }

        auto geometry         = Construct<EmbreeMeshGeometry>();
        geometry->mesh_buffer = mesh_buffer;
        {
            geometry->triangle_count   = 1;
            geometry->triangle_indices = std::make_unique<uint32_t[]>(7);

            auto p = geometry->triangle_indices.get();
            p[0]   = 0;
            p[1]   = 1;
            p[2]   = 2;
            p[6]   = 0;

            geometry->normal_indices = nullptr;
            geometry->uv_indices     = nullptr;
        }

        RegisterMeshGeometry(geometry);

        auto primitive = InstantiatePrimitive(geometry->geom_id, 0);

        geometry->material    = nullptr;
        geometry->area_lights = {CreateLight_Area(primitive, albedo)};
    }

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

} // namespace akane