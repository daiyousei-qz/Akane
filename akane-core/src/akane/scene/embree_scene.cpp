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

        RTCRayHit InitRayHit(const Ray& ak_ray)
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
        rtcCommitScene(scene_);
    }

    bool EmbreeScene::Intersect(const Ray& ray, Workspace& workspace, IntersectionInfo& isect) const
    {
        RTCIntersectContext ctx;
        rtcInitIntersectContext(&ctx);

        RTCRayHit ray_hit = InitRayHit(ray);

        rtcIntersect1(scene_, &ctx, &ray_hit);
        auto geom_id = ray_hit.hit.geomID;
        auto prim_id = ray_hit.hit.primID;

        if (geom_id != RTC_INVALID_GEOMETRY_ID && prim_id != RTC_INVALID_GEOMETRY_ID)
        {
            isect.t = ray_hit.ray.tfar;

            isect.point = ray.o + isect.t * ray.d;

            isect.ng = Vec3f{ray_hit.hit.Ng_x, ray_hit.hit.Ng_y, ray_hit.hit.Ng_z}.Normalized();

            isect.ns = isect.ng;
            if (isect.ns.Dot(ray.d) > 0)
            {
                isect.ns = -isect.ns;
            }

            isect.uv = {ray_hit.hit.u, ray_hit.hit.v};

            isect.index = prim_id;

            isect.primitive = InstantiateTemporaryPrimitive(workspace, geom_id, prim_id);

            isect.area_light = geoms_[geom_id]->area_light;

            isect.material = geoms_[geom_id]->material;

            return true;
        }
        else
        {
            return false;
        }
    }

    unsigned EmbreeScene::AddMesh(MeshDesc::SharedPtr mesh_data)
    {
        auto device   = GetEmbreeDevice();
        auto rtc_mesh = rtcNewGeometry(device, RTCGeometryType::RTC_GEOMETRY_TYPE_TRIANGLE);

        // register vertices
        auto rtc_vertices = rtcSetNewGeometryBuffer(rtc_mesh, RTCBufferType::RTC_BUFFER_TYPE_VERTEX,
                                                    0, RTC_FORMAT_FLOAT3, 3 * sizeof(float),
                                                    mesh_data->vertices.size());
        {
            auto p = reinterpret_cast<float*>(rtc_vertices);
            for (const auto& vertex : mesh_data->vertices)
            {
                *p = vertex.X();
                ++p;

                *p = vertex.Y();
                ++p;

                *p = vertex.Z();
                ++p;
            }
        }

        // register triangles
        auto rtc_triangles = rtcSetNewGeometryBuffer(
            rtc_mesh, RTCBufferType::RTC_BUFFER_TYPE_INDEX, 0, RTCFormat::RTC_FORMAT_UINT3,
            3 * sizeof(uint32_t), mesh_data->triangles.size());
        {
            auto p = reinterpret_cast<uint32_t*>(rtc_triangles);
            for (const auto& triangle : mesh_data->triangles)
            {
                *p = triangle.X();
                ++p;

                *p = triangle.Y();
                ++p;

                *p = triangle.Z();
                ++p;
            }
        }

        // finalize
        rtcCommitGeometry(rtc_mesh);

        auto id = this->geoms_.size();
        rtcAttachGeometryByID(scene_, rtc_mesh, id);

        rtcReleaseGeometry(rtc_mesh);

        // create primitive item
        auto geometry       = Construct<EmbreeMeshGeometry>();
		geometry->geom_id   = id;
        geometry->mesh_data = mesh_data;

		this->geoms_.push_back(geometry);

		// load light
		if (mesh_data->area_light != nullptr)
		{
			if (mesh_data->triangles.size() != 1)
			{
				throw 0;
			}

			auto primitive = InstantiatePrimitive(id, 0);
			auto light = CreateLight_Area(primitive, mesh_data->area_light->albedo);

			geometry->area_light = reinterpret_cast<AreaLight*>(light);
		}
		else
		{
			geometry->area_light = nullptr;
		}

		// load material
        if (mesh_data->material != nullptr && mesh_data->material->type == "lambertian")
        {
			const auto& albedo = mesh_data->material->params.at("albedo");

			if (albedo.type() == typeid(Vec3f))
			{
				std::vector<Spectrum> v;
				v.resize(mesh_data->triangles.size(), std::any_cast<Vec3f>(albedo));

				auto texture = CreateTexture_Solid(v);
				auto material = CreateMaterial_Lambertian(texture);

				geometry->material = material;
			}
			else if (albedo.type() == typeid(std::vector<Vec3f>))
			{
				auto texture = CreateTexture_Solid(std::any_cast<std::vector<Vec3f>>(albedo));
				auto material = CreateMaterial_Lambertian(texture);

				geometry->material = material;
			}
			else
			{
				throw 0;
			}
        }
		else
		{
			geometry->material = nullptr;
		}

        return id;
    }

} // namespace akane