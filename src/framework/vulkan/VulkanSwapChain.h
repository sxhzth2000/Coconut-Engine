//
// Created by tanhao on 2025/8/11.
//

#ifndef COCONUT_VULKANSWAPCHAIN_H
#define COCONUT_VULKANSWAPCHAIN_H
#include "VulkanContext.h"
#include "framework/Reference.h"
#include "framework/struct.h"
#include <vulkan/vulkan.hpp>


namespace ccn {


    	/// SwapChain
vk::SwapchainKHR CreateSwapChain(const vk::Device& device,const vk::PhysicalDevice& physical_device,
	vk::SurfaceKHR surface,
	const QueueFamilyIndices& indices,
	GLFWwindow& window);

    	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);



	    vk::SurfaceFormatKHR    chooseSwapSurfaceFormat( std::vector<vk::SurfaceFormatKHR> &formats);
    	vk::Extent2D            chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
    	vk::PresentModeKHR      chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &modes);

std::vector<vk::Image> CreateSwapChainImages(const vk::Device& device, const vk::SwapchainKHR& swapChain);


std::vector<vk::ImageView>  CreateSwapChainImageViews( const vk::Device& device, const std::vector<vk::Image> & images,const vk::Format& format );






} // ccn

#endif //COCONUT_VULKANSWAPCHAIN_H