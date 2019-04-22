#include "akane/mesh.h"
#include "tiny_obj_loader.h"
#include <filesystem>

using namespace std;
using namespace std::filesystem;
using namespace akane;

static Vec3f p2v(const akFloat* p)
{
    return Vec3f{p[0], p[1], p[2]};
}

namespace akane
{
	MeshDesc::SharedPtr LoadMesh(string_view filename)
	{
		path file = filename;
		path dir = file.parent_path();

		tinyobj::ObjReader reader{};
		tinyobj::ObjReaderConfig config{};

		config.mtl_search_path = dir.string();
		auto success = reader.ParseFromFile(file.string(), config);

		vector<MaterialDesc::SharedPtr> material_vec;
		for (const auto& material : reader.GetMaterials())
		{
			auto mat = make_shared<MaterialDesc>();
			mat->name = material.name;
			mat->ka = p2v(material.ambient);
			mat->kd = p2v(material.diffuse);
			mat->ks = p2v(material.specular);
			mat->tr = p2v(material.transmittance);
			mat->emission = p2v(material.emission);

			mat->ambient_texture = material.ambient_texname;
			mat->diffuse_texture = material.diffuse_texname;
			mat->specular_texture = material.specular_texname;

			material_vec.push_back(mat);
		}

		auto mesh = make_shared<MeshDesc>();
		for (int i = 0; i < reader.GetAttrib().vertices.size(); i += 3)
		{
			mesh->vertices.push_back({ reader.GetAttrib().vertices[i + 0],
									  reader.GetAttrib().vertices[i + 1],
									  reader.GetAttrib().vertices[i + 2] });
		}
		for (int i = 0; i < reader.GetAttrib().normals.size(); i += 3)
		{
			mesh->normals.push_back({ reader.GetAttrib().normals[i + 0],
									 reader.GetAttrib().normals[i + 1],
									 reader.GetAttrib().normals[i + 2] });
		}
		for (int i = 0; i < reader.GetAttrib().texcoords.size(); i += 3)
		{
			mesh->uv.push_back(
				{ reader.GetAttrib().texcoords[i + 0], reader.GetAttrib().texcoords[i + 1] });
		}

		for (const auto& shape : reader.GetShapes())
		{
			auto geom = make_shared<GeometryDesc>();
			geom->name = shape.name;

			auto vertex_iter = shape.mesh.indices.begin();
			for (int face_vertex_cnt : shape.mesh.num_face_vertices)
			{
				auto v0 = *vertex_iter;
				++vertex_iter;
				--face_vertex_cnt;

				auto v1 = *vertex_iter;
				++vertex_iter;
				--face_vertex_cnt;

				for (int i = 0; i < face_vertex_cnt; ++i)
				{
					auto v2 = *vertex_iter;
					++vertex_iter;

					geom->triangle_indices.push_back(
						{ v0.vertex_index, v1.vertex_index, v2.vertex_index });

					if (v0.normal_index != -1)
					{
						geom->normal_indices.push_back(
							{ v0.normal_index, v1.normal_index, v2.normal_index });
					}
					if (v0.texcoord_index != -1)
					{
						geom->uv_indices.push_back(
							{ v0.texcoord_index, v1.texcoord_index, v2.texcoord_index });
					}
				}
			}

			if (!material_vec.empty())
			{
				geom->material = material_vec[shape.mesh.material_ids.front()];
			}

			mesh->geomtries.push_back(geom);
		}

		return mesh;
	}
}