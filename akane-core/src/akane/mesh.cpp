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
    static MaterialDesc::SharedPtr
    ParseMaterial(unordered_map<string, Image::SharedPtr>& texture_lookup,
                  const tinyobj::material_t& mat, const path& dir)
    {
        auto try_load_texture = [&](const string& name) -> Image::SharedPtr {
            if (name.empty())
            {
                return nullptr;
            }

            auto iter = texture_lookup.find(name);
            if (iter != texture_lookup.end())
            {
                return iter->second;
            }
            else
            {
                auto texture_path    = dir / name;
                texture_lookup[name] = LoadImage(texture_path.string());

                return texture_lookup[name];
            }
        };

        auto result = std::make_shared<MaterialDesc>();

        result->name     = mat.name;
        result->ka       = p2v(mat.ambient);
        result->kd       = p2v(mat.diffuse);
        result->ks       = p2v(mat.specular);
        result->tr       = p2v(mat.transmittance);
        result->emission = p2v(mat.emission);

        result->ambient_texture  = try_load_texture(mat.ambient_texname);
        result->diffuse_texture  = try_load_texture(mat.diffuse_texname);
        result->specular_texture = try_load_texture(mat.specular_texname);

        if (mat.roughness != 0)
        {
            result->roughness = mat.roughness;
        }
        else if (mat.shininess != 0)
        {
			result->roughness = pow(2 / (2 + mat.shininess), 1.f / 4.f);
        }
		else
		{
			result->roughness = 0;
		}

        return result;
    }

    MeshDesc::SharedPtr LoadMesh(string_view filename)
    {
        path file = filename;
        path dir  = file.parent_path();

        tinyobj::ObjReader reader{};
        tinyobj::ObjReaderConfig config{};

        config.mtl_search_path = dir.string();
        auto success           = reader.ParseFromFile(file.string(), config);

        vector<MaterialDesc::SharedPtr> material_vec;
        unordered_map<string, Image::SharedPtr> texture_lookup;
        for (const auto& material : reader.GetMaterials())
        {
            material_vec.push_back(ParseMaterial(texture_lookup, material, dir));
        }

        auto mesh = make_shared<MeshDesc>();
        for (int i = 0; i < reader.GetAttrib().vertices.size(); i += 3)
        {
            mesh->vertices.push_back({reader.GetAttrib().vertices[i + 0],
                                      reader.GetAttrib().vertices[i + 1],
                                      reader.GetAttrib().vertices[i + 2]});
        }
        for (int i = 0; i < reader.GetAttrib().normals.size(); i += 3)
        {
            mesh->normals.push_back({reader.GetAttrib().normals[i + 0],
                                     reader.GetAttrib().normals[i + 1],
                                     reader.GetAttrib().normals[i + 2]});
        }
        for (int i = 0; i < reader.GetAttrib().texcoords.size(); i += 2)
        {
            mesh->uv.push_back(
                {reader.GetAttrib().texcoords[i + 0], reader.GetAttrib().texcoords[i + 1]});
        }

        for (const auto& shape : reader.GetShapes())
        {
			std::unordered_map<int, GeometryDesc::SharedPtr> geom_map;

			auto vertex_iter = shape.mesh.indices.begin();
			for (int face_index = 0; face_index < shape.mesh.num_face_vertices.size(); ++face_index)
			{
				auto face_vertex_count = shape.mesh.num_face_vertices[face_index];
				auto face_material = shape.mesh.material_ids[face_index];

				auto& geom = geom_map[face_material];
				if (geom == nullptr)
				{
					geom = make_shared<GeometryDesc>();
					geom->name = shape.name;

					geom->material = material_vec[face_material];
				}

				auto v0 = *vertex_iter;
				++vertex_iter;
				--face_vertex_count;

				auto v1 = *vertex_iter;
				++vertex_iter;
				--face_vertex_count;

				for (int i = 0; i < face_vertex_count; ++i)
				{
					auto v2 = *vertex_iter;
					++vertex_iter;

					geom->triangle_indices.push_back(
						{ v0.vertex_index, v1.vertex_index, v2.vertex_index });

					if (!mesh->normals.empty() && v0.normal_index != -1)
					{
						geom->normal_indices.push_back(
							{ v0.normal_index, v1.normal_index, v2.normal_index });
					}
					if (!mesh->uv.empty() && v0.texcoord_index != -1)
					{
						geom->uv_indices.push_back(
							{ v0.texcoord_index, v1.texcoord_index, v2.texcoord_index });
					}
				}
			}

			for (const auto& [mat_id, geom] : geom_map)
			{
				mesh->geomtries.push_back(geom);
			}
        }

        mesh->texture_lookup = std::move(texture_lookup);
        return mesh;
    }
} // namespace akane