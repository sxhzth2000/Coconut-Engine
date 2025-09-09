#pragma once

#include <glm/glm.hpp>

namespace{







    struct MeshPerframeStorageBufferObject
    {
        alignas(16)     glm::vec4 view_proj_matrix;


    };

    struct VulkanMeshInstance
    {
        alignas(16)     glm::vec4 model_matrix;

    };

    struct Color
    {
        alignas(16)     glm::vec3 color;
    };

    // mesh
    struct VulkanMesh
    {
        //uniform buffer
        vk::Buffer      uniformBuffer;
        // vertex buffer
        vk::Buffer       vertexBuffer;
        vk::DeviceMemory vertexMemory;
        // index buffer
        vk::Buffer       indexBuffer;
        vk::DeviceMemory indexMemory;


    };

}



