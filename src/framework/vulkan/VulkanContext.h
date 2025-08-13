#ifndef COCONUT_VULKANCONTEX_H
#define COCONUT_VULKANCONTEX_H

#include "VulkanSwapChain.h"
#include "framework/struct.h"
#include <vulkan/vulkan.hpp>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

namespace ccn
{
class VulkanContext
{
public:
	VulkanContext();
	~VulkanContext();

	// 禁止拷贝构造和拷贝赋值，防止错误复制 Vulkan 资源
	VulkanContext(const VulkanContext&) = delete;
	VulkanContext& operator=(const VulkanContext&) = delete;

	// 允许移动构造和移动赋值（如果需要）
	VulkanContext(VulkanContext&&) noexcept = default;
	VulkanContext& operator=(VulkanContext&&) noexcept = default;

	void Init();

	// SetUp
	/// Instance
	void createInstance();
	bool checkValidationLayerSupport();
	void setupDebugMessenger();

	/// PhysicalDevice
	void pickPhysicalDevice();
	bool isDeviceSuitable(const vk::PhysicalDevice &device);

	/// LogicalDevice
	void checkQueueFamilies();
	void createLogicalDevice();

	/// WindowSurface
	vk::SurfaceKHR createSurface();
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);

	GLFWwindow* window = nullptr;

	vk::Instance instance = nullptr;

	vk::detail::DispatchLoaderDynamic dldi;
	vk::DebugUtilsMessengerEXT debugMessenger;
	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	vk::PhysicalDevice physicalDevice;
	vk::SurfaceKHR surface;

	QueueFamilyIndices indices;
	vk::Device device;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;




};
} // namespace ccn

#endif // COCONUT_VULKANCONTEX_H
