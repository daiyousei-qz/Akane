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

    inline MeshDesc::SharedPtr CreateSphereMesh(int horizontal_line, int vertical_line,
                                                MaterialDesc::SharedPtr material,
                                                bool vertex_normal = true, bool vertex_uv = true)
    {
        AKANE_REQUIRE(horizontal_line > 0 && vertical_line > 2);

        std::vector<Point3f> vertices;
        vertices.push_back({0, 0, 1});
        vertices.push_back({0, 0, -1});

        for (int i = 0; i < horizontal_line; ++i)
        {
            auto theta = kPI * (i + 1) / (horizontal_line + 1);
            auto z     = cos(theta);
            auto r     = sqrt(1 - z * z);

            for (int j = 0; j < vertical_line; ++j)
            {
                auto phi = 2 * kPI * j / vertical_line;
                auto x   = r * cos(phi);
                auto y   = r * sin(phi);

                vertices.push_back({x, y, z});
            }
        }

        std::vector<Point3i> triangles;

        int top_ring_offset    = 2;
        int bottom_ring_offset = vertices.size() - vertical_line;
        for (int j = 0; j < vertical_line; ++j)
        {
            // top
            {
                auto v0 = 0;
                auto v1 = top_ring_offset + j;
                auto v2 = top_ring_offset + (j + 1) % vertical_line;

                triangles.push_back({v0, v1, v2});
            }

            // bottom
            {
                auto v0 = bottom_ring_offset + j;
                auto v1 = 1;
                auto v2 = bottom_ring_offset + (j + 1) % vertical_line;

                triangles.push_back({v0, v1, v2});
            }
        }

        for (int i = 0; i < horizontal_line - 1; ++i)
        {
            int ring0_offset = 2 + i * vertical_line;
            int ring1_offset = ring0_offset + vertical_line;

            for (int j = 0; j < vertical_line; ++j)
            {
                // top triangle
                {
                    auto v0 = ring0_offset + j;
                    auto v1 = ring1_offset + (j + 1) % vertical_line;
                    auto v2 = ring0_offset + (j + 1) % vertical_line;

                    triangles.push_back({v0, v1, v2});
                }

                // bottom triangle
                {
                    auto v0 = ring0_offset + j;
                    auto v1 = ring1_offset + j;
                    auto v2 = ring1_offset + (j + 1) % vertical_line;

                    triangles.push_back({v0, v1, v2});
                }
            }
        }

        auto geometry              = std::make_shared<GeometryDesc>();
        geometry->triangle_indices = std::move(triangles);
        geometry->material         = material;

        auto mesh      = std::make_shared<MeshDesc>();
        mesh->vertices = std::move(vertices);
        mesh->geomtries.push_back(geometry);

        if (vertex_normal)
        {
            for (const auto& vertex : mesh->vertices)
            {
                mesh->normals.push_back(vertex);
            }

            geometry->normal_indices = geometry->triangle_indices;
        }

        if (vertex_uv)
        {
            for (const auto& vertex : mesh->vertices)
            {
				using std::min;
				using std::max;

                auto u = 0.5f + atan2(vertex.Y(), vertex.X()) / (2 * kPI);
                auto v = 0.5f - asin(vertex.Z()) / kPI;

				u = max(min(u, 1.f), 0.f);
				v = max(min(v, 1.f), 0.f);

				static auto vert = vertex;
				vert = vertex;
				static auto uu = u, vv = v;;
				uu = u; vv = v;
				mesh->uv.push_back({u, v});
            }

            geometry->uv_indices = geometry->triangle_indices;
        }

        return mesh;
    }
} // namespace akane