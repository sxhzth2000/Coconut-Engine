//
// Created by tanhao on 2025/8/3.
//

#include "VulkanDescriptor.h"

#include "../common.h"

namespace ccn
{






 vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::Device& device,const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
{


	vk::DescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings    = bindings.data();

	return   device.createDescriptorSetLayout(layoutInfo);
}

vk::DescriptorPool CreateDescriptorPool(const VulkanContext& context,const std::vector<vk::DescriptorPoolSize>&  pool_size)
{

	vk::DescriptorPoolCreateInfo        poolInfo{};
	poolInfo.poolSizeCount = static_cast<uint32_t>(pool_size.size());
	poolInfo.pPoolSizes    = pool_size.data();
	poolInfo.maxSets       = 1;
	return           context.device.createDescriptorPool(poolInfo);

}

vk::DescriptorSet allocateDescriptorSets(const vk::Device& device,const vk::DescriptorSetLayout& layout,const vk::DescriptorPool& pool)
{
	vk::DescriptorSetAllocateInfo allocInfo{};
	allocInfo.descriptorPool     = pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts        = &layout;

	return device.allocateDescriptorSets(allocInfo)[0];
}
void updateDescriptorSets(const vk::Device& device, const std::vector<vk::WriteDescriptorSet>  writeDescriptorSets)
{
	device.updateDescriptorSets(static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
}

}        // namespace ccn