//
// Created by tanhao on 2025/8/3.
//

#ifndef BUFFER_H
#define BUFFER_H
#include "common.h"

#include <set>

#include <vulkan/vulkan.hpp>

#include <framework/vulkan/VulkanContext.h>


namespace ccn
{

class Buffer
{
};

class UniformBuffer
{
  public:
	 UniformBuffer();
	~UniformBuffer();

	void setContex(const vk::PhysicalDevice &physical_device,const  vk::Device &device);
	void create(vk::DeviceSize size, const void *initialData);

	vk::DescriptorBufferInfo getDescriptorInfo();

  public:
	void            *initialData;
	vk::Buffer       buffer;
	vk::DeviceMemory bufferMemory;

	vk::DeviceSize            size;
	vk::PhysicalDevice        physicalDevice;
	vk::Device                device;
	std::shared_ptr<VulkanContext> contex;
};





}        // namespace ccn

#endif        // BUFFER_H
