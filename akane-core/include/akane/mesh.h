#pragma once
#include "akane/core.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <any>

namespace akane
{
    // lambertian
    // "albedo": Vec3 | std::vector<Vec3>
    struct MaterialDesc
    {
        using SharedPtr = std::shared_ptr<MaterialDesc>;

        std::string type;
        std::unordered_map<std::string, std::any> params;
    };

	struct GlobalLightDesc
	{
		using SharedPtr = std::shared_ptr<GlobalLightDesc>;

		std::string type;
		std::unordered_map<std::string, std::any> params;
	};

    struct AreaLightDesc
    {
        using SharedPtr = std::shared_ptr<AreaLightDesc>;

		Vec3f albedo;
    };

    struct MeshDesc
    {
        using SharedPtr = std::shared_ptr<MeshDesc>;

        std::vector<Point3f> vertices;
        std::vector<Point3i> triangles;

        MaterialDesc::SharedPtr material;
        AreaLightDesc::SharedPtr area_light;
    };

    struct CameraDesc
    {
        using SharedPtr = std::shared_ptr<CameraDesc>;

        Point3f origin;
        Vec3f forward;
        Vec3f upward;
        Point2f fov;
    };

    struct SceneDesc
    {
        using SharedPtr = std::shared_ptr<SceneDesc>;

        CameraDesc::SharedPtr camera;
		std::vector<GlobalLightDesc::SharedPtr> global_lights;
		std::vector<MeshDesc::SharedPtr> objects;
    };
} // namespace akane