//
// Created by tanhao on 2025/7/12.
//

#ifndef STRUCH_H
#define STRUCH_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR        capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR>   presentModes;
};

static vk::Bool32 VKAPI_CALL
    debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT             messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void                                         *pUserData)
{
	if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
	{
		std::cerr << "[VULKAN ERROR][" << pCallbackData->pMessage << std::endl;
	}
	else if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	{
		std::cerr << "[VULKAN WARNING][" << pCallbackData->pMessage << std::endl;
	}
	else if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
	{
		std::cerr << "[VULKAN INFO][" << pCallbackData->pMessage << std::endl;
	}
	else
	{
		std::cerr << "[VULKAN VERBOSE]:" << pCallbackData->pMessage << std::endl;
	}

	return VK_FALSE;
}
#endif        // STRUCH_H
