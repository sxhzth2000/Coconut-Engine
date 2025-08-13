//
// Created by tanhao on 2025/8/11.
//

#include "VulkanSwapChain.h"
#include <GLFW/glfw3.h>
namespace ccn {


vk::SwapchainKHR CreateSwapChain(const vk::Device& device,const vk::PhysicalDevice& physical_device,
	vk::SurfaceKHR surface,
	const QueueFamilyIndices& indices,
	GLFWwindow& window)
{
	auto support       = querySwapChainSupport(physical_device, surface);
	auto surfaceFormat = chooseSwapSurfaceFormat(support.formats);
	auto presentMode   = chooseSwapPresentMode(support.presentModes);
	auto extent        = chooseSwapExtent(support.capabilities, &window);

	uint32_t imageCount = support.capabilities.minImageCount;

	if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount)
	{
		imageCount = support.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo{};
	createInfo.surface          = surface;
	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent      = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage       = vk::ImageUsageFlagBits::eColorAttachment;

	std::cout<<"SwapChainImageCount: "<<imageCount<<std::endl;

	uint32_t queueFamilyIndices[] = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	createInfo.preTransform   = support.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode    = presentMode;
	createInfo.clipped        = true;
	createInfo.oldSwapchain   = nullptr;

	return 	 device.createSwapchainKHR(createInfo);

}




SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
{
	SwapChainSupportDetails details;
	details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	details.formats      = device.getSurfaceFormatsKHR(surface);
	details.presentModes = device.getSurfacePresentModesKHR(surface);
	return details;
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat( std::vector<vk::SurfaceFormatKHR> &formats)
{

	for (const auto &format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Srgb &&
			format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}
	return formats[0];
}

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

		actualExtent.width  = std::clamp(actualExtent.width,
										 capabilities.minImageExtent.width,
										 capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height,
										 capabilities.minImageExtent.height,
										 capabilities.maxImageExtent.height);
		return actualExtent;
	}
}

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &modes)
{
	for (const auto &mode : modes)
	{
		if (mode == vk::PresentModeKHR::eMailbox)
		{
			return mode;
		}
	}
	return vk::PresentModeKHR::eFifo;
}

std::vector<vk::Image> CreateSwapChainImages(const vk::Device& device, const vk::SwapchainKHR& swapChain) {
	// 先查询交换链里有多少图像
	uint32_t imageCount = 0;
	  device.getSwapchainImagesKHR(swapChain, &imageCount, nullptr);

	// 分配存储容器
	std::vector<vk::Image> swapChainImages(imageCount);

	// 获取图像句柄
	device.getSwapchainImagesKHR(swapChain, &imageCount, swapChainImages.data());

	return swapChainImages;
}



std::vector<vk::ImageView  >    CreateSwapChainImageViews( const vk::Device& device, const std::vector<vk::Image> & images,const vk::Format& format )
{

	std::vector<vk::ImageView>  swapChainImageViews(images.size());


	for (size_t i = 0; i < images.size(); i++) {
		vk::ImageViewCreateInfo createInfo{};
		createInfo.image = images[i];
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.format = format;  // 和 swapchain 格式一致
		createInfo.components = {
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity
		};
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		swapChainImageViews[i] = device.createImageView(createInfo);
	}

	return swapChainImageViews;

}


} // ccn