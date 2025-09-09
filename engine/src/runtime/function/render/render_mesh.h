//
// Created by tanhao on 2025/9/3.
//

#include <glm/glm.hpp>
#ifndef COCONUT_RENDER_MESH_H
#define COCONUT_RENDER_MESH_H

namespace Coconut
{


        struct Vertex
        {
             glm::vec3 position;
        };

        struct VulkanMeshVertexVaryingEnableBlending
        {
            alignas(16) glm::vec3 normal;
            alignas(16) glm::vec3 tangent;
        };

        struct VulkanMeshVertexVarying
        {
            alignas(8) glm::vec2 texcoord;
        };

        struct VulkanMeshVertexJointBinding
        {
            int indices[4];
            alignas(16) glm::vec4 weights;
        };





} // namespace Piccolo
#endif // COCONUT_RENDER_MESH_H
