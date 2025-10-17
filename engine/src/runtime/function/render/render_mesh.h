//
// Created by tanhao on 2025/9/3.
//

#include <glm/glm.hpp>
#ifndef COCONUT_RENDER_MESH_H
#define COCONUT_RENDER_MESH_H

namespace Coconut
{

    struct MeshVertex
    {
        struct Vertex
        {
              glm::vec3 position;
        };

        struct VulkanMeshVertexVaryingEnableBlending
        {
             glm::vec3 normal;
             glm::vec3 tangent;
        };

        struct VulkanMeshVertexVarying
        {
             glm::vec2 texcoord;
        };
    };

    struct VulkanMeshVertexJointBinding
    {
        int indices[4];
         glm::vec4 weights;
    };

} // namespace Coconut
#endif // COCONUT_RENDER_MESH_H
