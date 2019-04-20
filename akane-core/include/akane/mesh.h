#pragma once
#include "akane/core.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <any>

namespace akane
{

	// "albedo": Vec3 | std::vector<Vec3>
	struct MaterialDesc
	{
		using SharedPtr = std::shared_ptr<MaterialDesc>;

		std::string name;
		std::string type;
		std::unordered_map<std::string, std::any> params;
	};

	struct AreaLightDesc
	{
		using SharedPtr = std::shared_ptr<AreaLightDesc>;

		std::string name;
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

	inline MeshDesc::SharedPtr LoadMesh(const void* data, size_t length)
	{

	}
}