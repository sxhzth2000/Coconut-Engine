#pragma once

#include <memory>
#include <vector>

#include "render_mesh.h"

namespace Coconut{


    struct MeshVertexData {
        glm::vec3 position;  // 顶点位置
        glm::vec3 normal;    // 法线
        glm::vec3 tangent;   // 切线（可选，tinyobj 本身不带，要自己计算）
        glm::vec2 uv;        // 纹理坐标
    };



    struct StaticMeshData
    {
        std::shared_ptr<std::vector<MeshVertexData>> m_vertex_buffer_data;
        std::shared_ptr<std::vector<uint16_t>> m_index_buffer_data;
    };

    class TextureData
    {
       public:
        uint32_t m_width {0};
        uint32_t m_height {0};
        uint32_t m_depth {0};
        uint32_t m_mip_levels {0};
        uint32_t m_array_layers {0};
        void*    m_pixels {nullptr};


        TextureData() = default;
        ~TextureData()
        {
            if (m_pixels)
            {
                free(m_pixels);
            }
        }


        bool isValid() const { return m_pixels != nullptr; }
    };










}