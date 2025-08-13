//
// Created by tanhao on 2025/8/11.
//

#include "VulkanContext.h"

#include <set>

namespace ccn
{
VulkanContext::VulkanContext()
{}

VulkanContext::~VulkanContext()
{}

void VulkanContext::Init()
{
	// setup
	createInstance();

	// 初始化动态加载器，注意要放在 createInstance 后
	dldi = vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
	setupDebugMessenger();        // 初始化调试回调

	pickPhysicalDevice();

	isDeviceSuitable(physicalDevice);
	checkQueueFamilies();

	surface = createSurface();

	indices = findQueueFamilies(physicalDevice, surface);

	createLogicalDevice();

}

void VulkanContext::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	// app info
	vk::ApplicationInfo appInfo;
	appInfo.pApplicationName   = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName        = "No Engine";
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = vk::ApiVersion14;

	// 先初始化 count
	uint32_t glfwExtensionCount = 0;
	// 同时获取数组和数量
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	// 将glfwExtensions 中的字符 转移到vector容器 extensions中
	char                      a     = 'a';
	const char               *b     = "b";
	const char               *arr[] = {"asd", "asd"};
	const char              **d     = arr;
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	for (auto result : extensions)
	{
		std::cout << "glfwExtensions: " << result << "\n";
	}

	vk::InstanceCreateInfo createInfo{};
	createInfo.pApplicationInfo = &appInfo;
	createInfo.pNext            = &debugCreateInfo;
	// 验证层
	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	//   扩展层
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledExtensionCount   = extensions.size();

	instance = vk::createInstance(createInfo);
	// enumerateInstanceExtensionProperties
	std::vector<vk::ExtensionProperties> deviceextensions =
		vk::enumerateInstanceExtensionProperties();

	std::cout << "Available Vulkan instance extensions:\n";
	for (const auto &ext : deviceextensions)
	{
		std::cout << "\t" << ext.extensionName << std::endl;
	}
}

bool VulkanContext::checkValidationLayerSupport()
{
	std::cout << "Validation layer check: \n";
	auto availableLayers = vk::enumerateInstanceLayerProperties();

	for (const char *layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto &layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	std::cout << "Validation layer is Support!\n";
	return true;
}

void VulkanContext::setupDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	debugCreateInfo.messageSeverity =
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

	debugCreateInfo.messageType =
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

	debugCreateInfo.pfnUserCallback = debugCallback;
	debugCreateInfo.pUserData       = nullptr;

	debugMessenger = instance.createDebugUtilsMessengerEXT(debugCreateInfo, nullptr, dldi);
}

void VulkanContext::pickPhysicalDevice()
{
	uint32_t                        deviceCount = 0;
	std::vector<vk::PhysicalDevice> devices     = instance.enumeratePhysicalDevices();
	if (devices.empty())
	{
		throw std::runtime_error("No GPUs with Vulkan support found.");
	}

	std::cout << "Available Vulkan Physical Devices:\n";
	for (const auto &device : devices)
	{
		vk::PhysicalDeviceProperties properties = device.getProperties();
		std::string                  deviceType;
		switch (properties.deviceType)
		{
			case vk::PhysicalDeviceType::eIntegratedGpu:
				deviceType = "Integrated GPU";
				break;
			case vk::PhysicalDeviceType::eDiscreteGpu:
				deviceType = "Discrete GPU";
				break;
			case vk::PhysicalDeviceType::eVirtualGpu:
				deviceType = "Virtual GPU";
				break;
			case vk::PhysicalDeviceType::eCpu:
				deviceType = "CPU";
				break;
			default:
				deviceType = "Other";
				break;
		}

		std::cout << "\t- " << properties.deviceName << " [" << deviceType << "]\n";
	}
	for (const auto &device : devices)
	{
		if (isDeviceSuitable(device))
		{
			physicalDevice = device;

			break;
		}
	}
	std::cout << "Selected device: " << physicalDevice.getProperties().deviceName << "\n";

}

bool VulkanContext::isDeviceSuitable(const vk::PhysicalDevice &device)
{
	// 获取设备属性和特性
	vk::PhysicalDeviceProperties properties = device.getProperties();
	vk::PhysicalDeviceFeatures   features   = device.getFeatures();

	// 检查图形队列是否存在
	bool hasGraphicsQueue = false;
	auto queueFamilies    = device.getQueueFamilyProperties();
	for (const auto &queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			hasGraphicsQueue = true;
			break;
		}
	}

	// 检查是否支持 swapchain 扩展
	std::vector<vk::ExtensionProperties> availableExtensions =
		device.enumerateDeviceExtensionProperties();

	const std::vector<const char *> requiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	bool extensionsSupported = std::all_of(
		requiredExtensions.begin(), requiredExtensions.end(),
		[&](const char *name) {
			return std::any_of(
				availableExtensions.begin(), availableExtensions.end(),
				[&](const vk::ExtensionProperties &ext) {
					return strcmp(ext.extensionName, name) == 0;
				});
		});

	// 总体判断
	return hasGraphicsQueue && extensionsSupported && features.geometryShader;
}

void VulkanContext::checkQueueFamilies()
{
	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	for (uint32_t i = 0; i < queueFamilies.size(); ++i)
	{
		const auto &qf = queueFamilies[i];
		std::cout << "QueueFamily[" << i << "]: "
				  << vk::to_string(qf.queueFlags)
				  << " (queues: " << qf.queueCount << ")\n";
	}
}

void VulkanContext::createLogicalDevice()
{
	std::set<uint32_t> uniqueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()};

	float                                  queuePriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	for (uint32_t family : uniqueFamilies)
	{
		vk::DeviceQueueCreateInfo queueInfo{};
		queueInfo.queueFamilyIndex = family;
		queueInfo.queueCount       = 1;
		queueInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueInfo);
	}

	// 启用哪些设备扩展（通常至少需要 swapchain）
	const std::vector<const char *> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	vk::PhysicalDeviceFeatures deviceFeatures{};        // 可配置需要的特性
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	vk::DeviceCreateInfo createInfo{};
	createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos       = queueCreateInfos.data();
	createInfo.pEnabledFeatures        = &deviceFeatures;
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	// 注意：验证层（pEnabledLayerNames）在 Vulkan 1.1+ 已不再强制要求设置（仅调试）

	device = physicalDevice.createDevice(createInfo);

	// 获取队列句柄
	graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
	presentQueue  = device.getQueue(indices.presentFamily.value(), 0);
}

vk::SurfaceKHR VulkanContext::createSurface()
{

	VkSurfaceKHR rawSurface;
	std::cout << "Create surface\n";
	if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window, nullptr, &rawSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
	std::cout << "Create surface2\n";
	return vk::SurfaceKHR(rawSurface);
}

QueueFamilyIndices VulkanContext::findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	auto queueFamilies = device.getQueueFamilyProperties();

	for (uint32_t i = 0; i < queueFamilies.size(); ++i)
	{
		if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;
		}

		if (device.getSurfaceSupportKHR(i, surface))
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
			break;
	}

	return indices;
}
}        // namespace ccn