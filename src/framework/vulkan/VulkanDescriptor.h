//
// Created by tanhao on 2025/8/3.
//

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H
#include "VulkanContext.h"

#include <vector>
#include <vulkan/vulkan.hpp>
namespace ccn
{





	vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::Device& device,const std::vector<vk::DescriptorSetLayoutBinding>& bindings);


	vk::DescriptorPool CreateDescriptorPool(const VulkanContext& context,const std::vector<vk::DescriptorPoolSize>&  pool_size);        // 为多少个对象分配 set


	vk::DescriptorSet allocateDescriptorSets(const vk::Device& device,const vk::DescriptorSetLayout& layout,const vk::DescriptorPool& pool);


	void updateDescriptorSets(const vk::Device& device, const std::vector<vk::WriteDescriptorSet>  writeDescriptorSets);







};



#endif        // DESCRIPTOR_H
