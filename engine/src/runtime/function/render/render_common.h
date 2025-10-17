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

    struct UBO
    {
        alignas(16)     glm::mat4 model;
        alignas(16)     glm::mat4 view_proj;

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



