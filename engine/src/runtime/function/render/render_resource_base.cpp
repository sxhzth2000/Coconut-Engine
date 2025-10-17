
#include "render_resource_base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#include "runtime/core/base/macro.h"

namespace Coconut{


    Coconut::RenderResourceBase::~RenderResourceBase() {}

    StaticMeshData RenderResourceBase::loadStaticMesh(std::string filename) {
        StaticMeshData mesh_data;
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config;

    reader_config.vertex_color = false;

    if (!reader.ParseFromFile(filename, reader_config))
    {
        if (!reader.Error().empty())
        {
            LOG_ERROR("loadMesh {} failed, error: {}", filename, reader.Error());
        }
        assert(0);
    }
    if (!reader.Warning().empty())
    {
        LOG_WARN("loadMesh {} warning, warning: {}", filename, reader.Warning());
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();





    std::vector<MeshVertexData> vert;
    std::vector<uint16_t> indices;

    for (size_t s = 0; s < shapes.size(); s++)
    {
        const auto& mesh = shapes[s].mesh;

        for (size_t f = 0; f < mesh.indices.size(); f++) {
            tinyobj::index_t idx = mesh.indices[f];

            MeshVertexData vertex{};

            // position
            vertex.position = {
                attrib.vertices[3 * idx.vertex_index + 0],
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2]
            };

            // normal
            if (idx.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * idx.normal_index + 0],
                    attrib.normals[3 * idx.normal_index + 1],
                    attrib.normals[3 * idx.normal_index + 2]
                };
            }

            // texcoord
            if (idx.texcoord_index >= 0) {
                vertex.uv = {
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    1-attrib.texcoords[2 * idx.texcoord_index + 1]
                };
            }

            // tangent: tinyobj 不提供，需要后续用 MikkTSpace 或手写计算
            vertex.tangent = glm::vec3(0.0f);

            vert.push_back(vertex);
            indices.push_back(static_cast<uint16_t>(vert.size() - 1));
        }
    }

        // 保存到 mesh_data
        mesh_data.m_vertex_buffer_data = std::make_shared<std::vector<MeshVertexData>>(std::move(vert));
        mesh_data.m_index_buffer_data  = std::make_shared<std::vector<uint16_t>>(std::move(indices));



    LOG_INFO("shapes size is: {}",shapes.size());

    return mesh_data;

    }
    std::shared_ptr<TextureData> RenderResourceBase::loadTexture(std::string filename)
    {
        LOG_INFO("");

        std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

        int iw, ih, n;
        texture->m_pixels = stbi_load(filename.c_str(), &iw, &ih, &n, 4);

        if (!texture->m_pixels)
            return nullptr;

        texture->m_width        = iw;
        texture->m_height       = ih;
        texture->m_mip_levels = 1;
        return texture;

    }

}


