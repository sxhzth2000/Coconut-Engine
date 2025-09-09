
#pragma once

#include <runtime/core/base/macro.h>

#include <fstream>
#include <iostream>
#include <vulkan/vulkan.hpp>
namespace vkutil
{

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice);

    vk::ShaderModule createShaderModule(vk::Device device, const std::vector<unsigned char>& shader_code);

    std::vector<unsigned char> readSPVFile(const std::string& filename);

} // namespace vkutil