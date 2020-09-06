#pragma once
#include "akane/texture.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace akane
{
    struct MaterialDesc
    {
        std::string name;

        Vec3 ka = {}; // ambient
        Vec3 kd = {}; // diffuse
        Vec3 ks = {}; // specular
        Vec3 tr = {}; // transmission

        Vec3 emission = {};

        shared_ptr<Texture3D> ambient_texture  = nullptr;
        shared_ptr<Texture3D> diffuse_texture  = nullptr;
        shared_ptr<Texture3D> specular_texture = nullptr;

        // pbr
        float eta       = 10.f;
        float roughness = 0.f;
    };

    struct GeometryDesc
    {
        std::string name;

        std::vector<Point3i> triangle_indices;
        std::vector<Point3i> normal_indices;
        std::vector<Point3i> uv_indices;

        shared_ptr<MaterialDesc> material;
    };

    struct MeshDesc
    {
        std::string name; // usu. obj file path

        std::vector<Point3f> vertices;
        std::vector<Point3f> normals;
        std::vector<Point2f> uv;

        std::unordered_map<std::string, shared_ptr<Texture3D>> texture_lookup;

        std::vector<shared_ptr<GeometryDesc>> geomtries;
    };

    struct PrimitiveDesc
    {
        shared_ptr<MeshDesc> mesh;

        float scale;
        Vec3 position;
        Vec3 rotation;
    };

    struct CameraDesc
    {
        Vec3 origin;
        Vec3 forward;
        Vec3 upward;

        float fov = 0.5f; // horizontal field of view, should in(0, 1], default is 90 degrees
        float aspect_ratio = 1.f; // vertical over horizontal
    };

    struct SceneDesc
    {
        std::string name;

        CameraDesc camera;
        std::vector<PrimitiveDesc> objects;
    };

    shared_ptr<MeshDesc> LoadMeshDesc(const std::string& filename);
    unique_ptr<SceneDesc> LoadSceneDesc(const std::string& filename);

} // namespace akane