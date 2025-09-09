#include "vulkan_util.h"

namespace vkutil
{

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    vk::ShaderModule createShaderModule(vk::Device device, const std::vector<unsigned char> &shader_code)
    {
        if (shader_code.size() % sizeof(uint32_t) != 0)
        {
            throw std::runtime_error("Shader code size must be a multiple of 4.");
        }

        vk::ShaderModuleCreateInfo shader_module_create_info{};
        shader_module_create_info.codeSize = shader_code.size();
        shader_module_create_info.pCode    = reinterpret_cast<const uint32_t *>(shader_code.data());

        vk::ShaderModule shader_module;
        try
        {
            device.createShaderModule(&shader_module_create_info, nullptr, &shader_module);
            return shader_module;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to create shader module: " << e.what() << "\nCode size: " << shader_code.size()
                      << " bytes\n";

            throw;
        }
    }

    std::vector<unsigned char> readSPVFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t                     fileSize = (size_t)file.tellg();
        std::vector<unsigned char> buffer(fileSize);

        file.seekg(0);
        file.read(reinterpret_cast<char *>(buffer.data()), fileSize);
        file.close();

        return buffer;
    }

} // namespace vkutil