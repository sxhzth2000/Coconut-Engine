//
// Created by tanhao on 2025/8/3.
//

#include "Buffer.h"

#include "Application.h"

namespace ccn
{

UniformBuffer::UniformBuffer()
{
}
UniformBuffer::~UniformBuffer()
{
}
void UniformBuffer::setContex(const vk::PhysicalDevice &inphysical_device,const vk::Device &indevice)
{
	physicalDevice = inphysical_device;
	device         = indevice;
}
void UniformBuffer::create(vk::DeviceSize size, const void *initialData)
{
	this->size = size;
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.size        = size;
	bufferInfo.usage       = vk::BufferUsageFlagBits::eUniformBuffer;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	buffer = device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements =
	    device.getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(
	    memRequirements.memoryTypeBits,
	    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, physicalDevice);

	bufferMemory = device.allocateMemory(allocInfo);
	device.bindBufferMemory(buffer, bufferMemory, 0);

	// 可选：写入初始数据
	if (initialData != nullptr)
	{
		void *mapped = device.mapMemory(bufferMemory, 0, size);
		memcpy(mapped, initialData, static_cast<size_t>(size));
		device.unmapMemory(bufferMemory);
	}
}
vk::DescriptorBufferInfo UniformBuffer::getDescriptorInfo()
{
	vk::DescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = this->buffer;
	bufferInfo.offset = 0;
	bufferInfo.range  = this->size;
	return bufferInfo;
}

} // ccn