//
// Created by tanhao on 2025/8/13.
//
#include "common.h"


namespace ccn
{
uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice)
{
	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

bool createBuffer( vk::Device& device, vk::PhysicalDevice physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
							   vk::Buffer &buffer, vk::DeviceMemory &memory, const void *data)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size                 = size;
	bufferInfo.usage                = usage;
	bufferInfo.sharingMode          = vk::SharingMode::eExclusive;

	buffer = device.createBuffer(bufferInfo);        // 使用类成员 device

	vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);        // 使用类成员 physicalDevice

	memory = device.allocateMemory(allocInfo);
	device.bindBufferMemory(buffer, memory, 0);

	if (data != nullptr)
	{
		void *mappedData = device.mapMemory(memory, 0, size);
		memcpy(mappedData, data, static_cast<size_t>(size));
		device.unmapMemory(memory);
	}

	return true;

}
 std::vector<char> readFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t            fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}


}