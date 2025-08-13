#pragma once
#include "Application.h"
#include <string>
#include <vulkan/vulkan.hpp>
namespace ccn
{
class Application;
}

class Texture
{
  public:
	Texture(vk::Device         device,
	        vk::PhysicalDevice physicalDevice,
	        vk::CommandPool    commandPool,
	        vk::Queue          graphicsQueue);

	~Texture();

	void loadFromFile(const std::string &filename);

	vk::ImageView getImageView() const
	{
		return imageView;
	}
	vk::Sampler getSampler() const
	{
		return sampler;
	}

  public:
	vk::DescriptorImageInfo getDescriptorInfo();

  private:
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::Format format,
	                 vk::ImageTiling tiling, vk::ImageUsageFlags usage,
	                 vk::MemoryPropertyFlags properties,
	                 vk::Image &image, vk::DeviceMemory &imageMemory);

	void createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);
	void createSampler();

	void transitionImageLayout(vk::Image image, vk::Format format,
	                           vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
	                           uint32_t mipLevels);
	bool hasStencilComponent(vk::Format format);
	void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

	void generateMipmaps(vk::Image image, vk::Format imageFormat,
	                     int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	vk::CommandBuffer beginSingleTimeCommands();
	void              endSingleTimeCommands(vk::CommandBuffer commandBuffer);

  private:
	vk::Device         device;
	vk::PhysicalDevice physicalDevice;
	vk::CommandPool    commandPool;
	vk::Queue          graphicsQueue;

	vk::Image        image;
	vk::DeviceMemory imageMemory;
	vk::ImageView    imageView;
	vk::Sampler      sampler;

	uint32_t mipLevels;
};
