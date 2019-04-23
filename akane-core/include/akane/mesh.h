#pragma once
#include "akane/core.h"
#include "akane/io/image.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <any>

namespace akane
{
	struct MaterialDesc
	{
		using SharedPtr = std::shared_ptr<MaterialDesc>;

		std::string name;

		Vec3f ka; // ambient
		Vec3f kd; // diffuse
		Vec3f ks; // specular
		Vec3f tr; // transmission

		Vec3f emission;

		Image::SharedPtr ambient_texture;
		Image::SharedPtr diffuse_texture;
		Image::SharedPtr specular_texture;
	};

	struct GeometryDesc
	{
		using SharedPtr = std::shared_ptr<GeometryDesc>;

		std::string name;

		std::vector<Point3i> triangle_indices;
		std::vector<Point3i> normal_indices;
		std::vector<Point3i> uv_indices;

		MaterialDesc::SharedPtr material;
	};

    struct MeshDesc
    {
        using SharedPtr = std::shared_ptr<MeshDesc>;

        std::vector<Point3f> vertices;
		std::vector<Point3f> normals;
		std::vector<Point2f> uv;

		std::unordered_map<std::string, Image::SharedPtr> texture_lookup;

		std::vector<GeometryDesc::SharedPtr> geomtries;
    };

	MeshDesc::SharedPtr LoadMesh(std::string_view filename);
} // namespace akane