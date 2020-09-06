#include "akane/model.h"
#include "akane/texture.h"
#include "akane/texture/image.h"
#include <tiny_obj_loader.h>
#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>
#include <unordered_map>

using namespace std;
using namespace std::filesystem;
using namespace nlohmann;

namespace akane
{
    static Vec3 p2v(const float* p)
    {
        return Vec3{p[0], p[1], p[2]};
    }

    static shared_ptr<MaterialDesc>
    ParseMaterial(const tinyobj::material_t& mat, const path& dir,
                  unordered_map<string, shared_ptr<Texture3D>>& texture_cache)
    {
        auto try_load_texture = [&](const string& name) -> shared_ptr<Texture3D> {
            if (name.empty())
            {
                return nullptr;
            }

            auto iter = texture_cache.find(name);
            if (iter != texture_cache.end())
            {
                return iter->second;
            }
            else
            {
                auto texture_path   = dir / name;
                texture_cache[name] = make_shared<ImageTexture>(texture_path.string());

                return texture_cache[name];
            }
        };

        auto result = make_shared<MaterialDesc>();

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

    shared_ptr<MeshDesc> LoadMeshDesc(const string& filename)
    {
        path file = filename;
        path dir  = file.parent_path();

        tinyobj::ObjReader reader{};
        tinyobj::ObjReaderConfig config{};

        // config.mtl_search_path = dir.string();
        bool success = reader.ParseFromFile(file.string(), config);
        AKANE_REQUIRE(success);

        vector<shared_ptr<MaterialDesc>> material_vec;
        unordered_map<string, shared_ptr<Texture3D>> texture_cache;
        for (const auto& material : reader.GetMaterials())
        {
            material_vec.push_back(ParseMaterial(material, dir, texture_cache));
        }

        auto result  = make_shared<MeshDesc>();
        result->name = filename;

        for (size_t i = 0; i < reader.GetAttrib().vertices.size(); i += 3)
        {
            result->vertices.push_back({reader.GetAttrib().vertices[i],
                                        reader.GetAttrib().vertices[i + 1u],
                                        reader.GetAttrib().vertices[i + 2u]});
        }
        for (size_t i = 0; i < reader.GetAttrib().normals.size(); i += 3)
        {
            result->normals.push_back({reader.GetAttrib().normals[i],
                                       reader.GetAttrib().normals[i + 1u],
                                       reader.GetAttrib().normals[i + 2u]});
        }
        for (size_t i = 0; i < reader.GetAttrib().texcoords.size(); i += 2)
        {
            result->uv.push_back(
                {reader.GetAttrib().texcoords[i], reader.GetAttrib().texcoords[i + 1u]});
        }

        for (const auto& shape : reader.GetShapes())
        {
            std::unordered_map<int, shared_ptr<GeometryDesc>> geom_map;

            auto vertex_iter = shape.mesh.indices.begin();
            for (int face_index = 0; face_index < shape.mesh.num_face_vertices.size(); ++face_index)
            {
                auto face_vertex_count = shape.mesh.num_face_vertices[face_index];
                auto face_material     = shape.mesh.material_ids[face_index];

                auto& geom = geom_map[face_material];
                if (geom == nullptr)
                {
                    geom       = make_shared<GeometryDesc>();
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
                        {v0.vertex_index, v1.vertex_index, v2.vertex_index});

                    if (!result->normals.empty() && v0.normal_index != -1)
                    {
                        geom->normal_indices.push_back(
                            {v0.normal_index, v1.normal_index, v2.normal_index});
                    }
                    if (!result->uv.empty() && v0.texcoord_index != -1)
                    {
                        geom->uv_indices.push_back(
                            {v0.texcoord_index, v1.texcoord_index, v2.texcoord_index});
                    }
                }
            }

            for (const auto& [mat_id, geom] : geom_map)
            {
                result->geomtries.push_back(geom);
            }
        }

        result->texture_lookup = move(texture_cache);
        return result;
    }

    static json LoadJsonFile(const string& filename)
    {
        char json_buf[10000];
        size_t file_size = 0;

        // read scene desc file
        {
            auto file = fopen(filename.c_str(), "rb");
            AKANE_REQUIRE(file != nullptr);

            // TODO: check file is exhausted
            file_size = fread(json_buf, 1, sizeof json_buf, file);
            fclose(file);
        }

        return json::parse(json_buf, json_buf + file_size);
    }

    static CameraDesc ParseJson_CameraDesc(const json& value)
    {
        AKANE_REQUIRE(value.is_object());

        CameraDesc result{};
        result.origin  = value.value<Vec3>("origin", {});
        result.forward = value.value<Vec3>("forward", {});
        result.upward  = value.value<Vec3>("upward", {});
        result.fov     = value.value<float>("fov", 0.5f);

        return result;
    }

    static PrimitiveDesc
    ParseJson_PrimitiveDesc(const json& value,
                            unordered_map<string, shared_ptr<MeshDesc>>& mesh_cache)
    {
        AKANE_REQUIRE(value.is_object());
        AKANE_REQUIRE(value["type"] == "mesh");

        PrimitiveDesc result{};

        // parse mesh
        string obj_filename = value["obj_file"];
        auto& cached_item   = mesh_cache[obj_filename];
        if (cached_item == nullptr)
        {
            cached_item = LoadMeshDesc(obj_filename);
        }

        result.mesh = cached_item;

        // parse transform
        auto transform_config = value["transform"];
        AKANE_REQUIRE(transform_config.is_object());

        result.rotation = transform_config.value<Vec3>("rotation", {}) / 180.f * kPi;
        result.position = transform_config.value<Vec3>("position", {});
        result.scale    = transform_config.value<float>("scale", 1.f);

        // finalize
        return result;
    }

    unique_ptr<SceneDesc> LoadSceneDesc(const string& filename)
    {
        auto config = LoadJsonFile(filename);

        unordered_map<string, shared_ptr<MeshDesc>> mesh_cache;

        auto result    = make_unique<SceneDesc>();
        result->name   = config["name"];
        result->camera = ParseJson_CameraDesc(config["camera"]);

        auto scene_config = config["scene"];
        auto primitives   = scene_config["primitives"];
        AKANE_REQUIRE(primitives.is_array());
        for (const auto& item : primitives)
        {
            result->objects.push_back(ParseJson_PrimitiveDesc(item, mesh_cache));
        }

        return result;
    }
} // namespace akane