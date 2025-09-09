#include "vulkan_rhi.h"

#include "vulkan_rhi_resource.h"
#include "runtime/core/base/macro.h"
#include "runtime/function/render/interface/rhi_struct.h"
#include "runtime/function/render/window_system.h"
#include "vulkan_util.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <set>

#define VMA_IMPLEMENTATION
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"


namespace Coconut
{
    // debug callback
    static vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT             messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*                                         pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    void Coconut::VulkanRHI::initialize(RHIInitInfo init_info)
    {
        LOG_INFO("");

        m_window = init_info.window_system->getWindow();

        createInstance();
        initializeDebugMessenger();
        createWindowSurface();
        initializePhysicalDevice();
        createLogicalDevice();
        createCommandPool();
        createCommandBuffers();

        createDescriptorPool();

        createSyncPrimitives();

        createSwapChain();

        createSwapChainImageViews();

        createFramebufferImageAndView();

        createAssetAllocator();
    }
    void Coconut::VulkanRHI::createInstance()
    {

        LOG_INFO("");
        if (m_enable_validation_Layers && !checkValidationLayerSupport())
        {
            LOG_ERROR("validation layers requested, but not available!");
        }
        // app info
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName   = "Coconut_renderer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "Coconut";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = m_vulkan_api_version;

        // create info
        vk::InstanceCreateInfo instance_create_info{};
        instance_create_info.pApplicationInfo = &appInfo; // the appInfo is stored here
        auto extensions                       = getRequiredExtensions();
        for (auto result : extensions)
        {
            std::cout << "glfwExtensions: " << result << "\n";
        }

        instance_create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        instance_create_info.ppEnabledExtensionNames = extensions.data();

        if (m_enable_validation_Layers)
        {
            instance_create_info.enabledLayerCount   = static_cast<uint32_t>(m_validation_layers.size());
            instance_create_info.ppEnabledLayerNames = m_validation_layers.data();
        }
        else
        {
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.pNext             = nullptr;
        }

        // create m_vulkan_context._instance
        try
        {
            m_instance = vk::createInstance(instance_create_info);
            // enumerateInstanceExtensionProperties
            std::vector<vk::ExtensionProperties> deviceextensions = vk::enumerateInstanceExtensionProperties();

            std::cout << "Available Vulkan instance extensions:\n";
            for (const auto& ext : deviceextensions)
            {
                std::cout << "\t" << ext.extensionName << std::endl;
            }
        }
        catch (vk::SystemError& err)
        {
            LOG_ERROR("vk create instance failed: {}", err.what());
            return;
        }
    }
    bool Coconut::VulkanRHI::checkValidationLayerSupport()
    {

        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

        for (const char* layerName : m_validation_layers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
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

        return true;
    }
    std::vector<const char*> Coconut::VulkanRHI::getRequiredExtensions()
    {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (m_enable_validation_Layers || m_enable_debug_utils_label)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void Coconut::VulkanRHI::initializeDebugMessenger()
    {
        LOG_INFO("");

        dldi = vk::detail::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);

        if (m_enable_validation_Layers)
        {
            vk::DebugUtilsMessengerCreateInfoEXT debug_createInfo{};
            debug_createInfo.messageSeverity =
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
            debug_createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                                           | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                                           | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;
            debug_createInfo.pUserData       = nullptr;
            debug_createInfo.pfnUserCallback = debugCallback;

            m_debug_messenger = m_instance.createDebugUtilsMessengerEXT(debug_createInfo, nullptr, dldi);

            if (!m_debug_messenger)
            {
                LOG_ERROR("failed to set up debug messenger!");
            }
        }
    }

    void Coconut::VulkanRHI::createWindowSurface()
    {

        LOG_INFO("");
        VkResult result = glfwCreateWindowSurface(m_instance, m_window, nullptr, (VkSurfaceKHR*)&m_surface);

        if (result != VK_SUCCESS)
        {
            LOG_ERROR("glfwCreateWindowSurface failed!");
        }
    }

    void Coconut::VulkanRHI::initializePhysicalDevice()
    {
        LOG_INFO("");

        std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
        if (devices.empty())
        {
            throw std::runtime_error("No GPUs with Vulkan support found.");
        }

        std::vector<std::pair<int, vk::PhysicalDevice>> ranked_physical_devices;
        std::cout << "Available Vulkan Physical Devices:\n";
        for (const auto& device : devices)
        {
            vk::PhysicalDeviceProperties properties = device.getProperties();
            std::string                  deviceType;
            int                          score = 0;
            switch (properties.deviceType)
            {
                case vk::PhysicalDeviceType::eIntegratedGpu:
                    deviceType = "Integrated GPU";
                    score += 100;
                    break;

                case vk::PhysicalDeviceType::eDiscreteGpu:
                    deviceType = "Discrete GPU";
                    score += 1000;
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
            ranked_physical_devices.push_back({score, device});
            std::cout << "\t- " << properties.deviceName << " [" << deviceType << "]\n";
        }

        // rank device
        std::sort(
            ranked_physical_devices.begin(),
            ranked_physical_devices.end(),
            [](const std::pair<int, VkPhysicalDevice>& p1, const std::pair<int, VkPhysicalDevice>& p2) {
                return p1 > p2;
            });
        for (const auto& device : ranked_physical_devices)
        {
            if (isDeviceSuitable(device.second))
            {
                m_physical_device = device.second;
                break;
            }
        }
    }
    void VulkanRHI::createFramebufferImageAndView()
    {

        // framebuffer need  color and depth attachment,color attachment imageview come from swapchain imageview

        // for depth imageview, we need manual create

        createDepthImageAndView();
    }
    void VulkanRHI::createLogicalDevice()
    {

        LOG_INFO("");
        m_queue_indices = findQueueFamilies();
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

        std::set<uint32_t> uniqueFamilies = {
            m_queue_indices.graphics_family.value(), m_queue_indices.present_family.value()};

        float queuePriority = 1.0f;
        for (uint32_t family : uniqueFamilies)
        {
            vk::DeviceQueueCreateInfo queueInfo{};
            queueInfo.queueFamilyIndex = family;
            queueInfo.queueCount       = 1;
            queueInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueInfo);
        }

        const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};

        vk::PhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = vk::True;

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures        = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        m_device = m_physical_device.createDevice(deviceCreateInfo);

        m_graphics_queue = m_device.getQueue(m_queue_indices.graphics_family.value(), 0);
        m_present_queue  = m_device.getQueue(m_queue_indices.present_family.value(), 0);
    }

    QueueFamilyIndices VulkanRHI::findQueueFamilies()
    {

        QueueFamilyIndices indices;

        auto queueFamilies = m_physical_device.getQueueFamilyProperties();
        for (uint32_t i = 0; i < queueFamilies.size(); ++i)
        {
            const auto& qf = queueFamilies[i];
            if (qf.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphics_family = i;
            }
            if (m_physical_device.getSurfaceSupportKHR(i, m_surface))
            {
                indices.present_family = i;
            }
            if (indices.isComplete())
                break;
            std::cout << "QueueFamily[" << i << "]: " << vk::to_string(qf.queueFlags) << " (queues: " << qf.queueCount
                      << ")\n";
        }

        return indices;
    }

    vk::Format VulkanRHI::findDepthFormat()
    {
        return findSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    vk::Format VulkanRHI::findSupportedFormat(
        const std::vector<vk::Format>& candidates,
        vk::ImageTiling                tiling,
        vk::FormatFeatureFlags         features)
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties props;

            m_physical_device.getFormatProperties(format, &props);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        LOG_ERROR("findSupportedFormat failed");

        return vk::Format();
    }
    bool VulkanRHI::checkDeviceExtensionSupport(vk::PhysicalDevice physical_device)
    {
        uint32_t                             extension_count;
        std::vector<vk::ExtensionProperties> available_extensions =
            physical_device.enumerateDeviceExtensionProperties();

        std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());
        for (const auto& extension : available_extensions)
        {
            required_extensions.erase(extension.extensionName);
        }

        return required_extensions.empty();
    }

    SwapChainSupportDetails VulkanRHI::querySwapChainSupport(vk::PhysicalDevice physical_device)
    {
        SwapChainSupportDetails details_result;

        // capabilities
        details_result.capabilities = physical_device.getSurfaceCapabilitiesKHR(m_surface);

        // formats
        details_result.formats = physical_device.getSurfaceFormatsKHR(m_surface);

        // present modes
        details_result.presentModes = physical_device.getSurfacePresentModesKHR(m_surface);

        return details_result;
    }

    void VulkanRHI::prepareContext() {

        m_current_command_buffer=m_command_buffers[m_current_frame_index];
    };

    bool VulkanRHI::isDeviceSuitable(const vk::PhysicalDevice& device)
    {
        //
        vk::PhysicalDeviceProperties properties = device.getProperties();
        vk::PhysicalDeviceFeatures   features   = device.getFeatures();

        //
        bool hasGraphicsQueue = false;
        auto queueFamilies    = device.getQueueFamilyProperties();
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                hasGraphicsQueue = true;
                break;
            }
        }

        //  swap chain
        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

        const std::vector<const char*> requiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        bool extensionsSupported =
            std::all_of(requiredExtensions.begin(), requiredExtensions.end(), [&](const char* name) {
                return std::any_of(
                    availableExtensions.begin(), availableExtensions.end(), [&](const vk::ExtensionProperties& ext) {
                        return strcmp(ext.extensionName, name) == 0;
                    });
            });

        //
        return hasGraphicsQueue && extensionsSupported && features.geometryShader;
    }
    void VulkanRHI::createCommandPool()
    {

        vk::CommandPoolCreateInfo createInfo;
        createInfo.queueFamilyIndex = m_queue_indices.graphics_family.value();
        createInfo.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        m_command_pool = m_device.createCommandPool(createInfo);
    }
    void VulkanRHI::createCommandBuffers()
    {

        vk::CommandBufferAllocateInfo allocateInfo{};
        allocateInfo.commandPool        = m_command_pool;
        allocateInfo.commandBufferCount = static_cast<uint32_t>(k_max_frames_in_flight);
        allocateInfo.level              = vk::CommandBufferLevel::ePrimary;

        m_command_buffers = m_device.allocateCommandBuffers(allocateInfo);
    }
    void VulkanRHI::createDescriptorPool()
    {

        {
            std::vector<vk::DescriptorPoolSize> pool_size{
                {vk::DescriptorType::eUniformBuffer, 10},
                {vk::DescriptorType::eStorageBufferDynamic, 10},
                {vk::DescriptorType::eCombinedImageSampler, 20}};

            vk::DescriptorPoolCreateInfo main_pool_createInfo{};
            main_pool_createInfo.pPoolSizes    = pool_size.data();
            main_pool_createInfo.poolSizeCount = pool_size.size();
            main_pool_createInfo.maxSets       = 10;

            m_descriptor_pool = m_device.createDescriptorPool(main_pool_createInfo);
        }
    }
    void VulkanRHI::createSyncPrimitives()
    {

        vk::SemaphoreCreateInfo semaphoreInfo;
        vk::FenceCreateInfo     fenceInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        for (size_t i = 0; i < k_max_frames_in_flight; i++)
        {
            m_image_available_semaphore[i] = m_device.createSemaphore(semaphoreInfo);
            m_render_finished_semaphore[i] = m_device.createSemaphore(semaphoreInfo);
            m_frame_in_flight_fence[i]     = m_device.createFence(fenceInfo);
        }
    }
    void VulkanRHI::createSwapChain()
    {

        auto support       = querySwapChainSupport(m_physical_device);
        auto surfaceFormat = chooseSwapSurfaceFormat(support.formats);
        auto presentMode   = chooseSwapPresentMode(support.presentModes);
        auto extent        = chooseSwapExtent(support.capabilities, m_window);

        LOG_INFO(
            "minImageCount:{0},maxImageCount:{1}",
            support.capabilities.minImageCount,
            support.capabilities.maxImageCount)

        uint32_t imageCount = support.capabilities.minImageCount + 1;

        if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount)
        {
            imageCount = support.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo;
        createInfo.surface          = m_surface;
        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = vk::ImageUsageFlagBits::eColorAttachment;

        std::cout << "SwapChainImageCount: " << imageCount << "\n";

        uint32_t queueFamilyIndices[] = {
            m_queue_indices.graphics_family.value(), m_queue_indices.present_family.value()};

        if (m_queue_indices.graphics_family != m_queue_indices.present_family)
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

        m_swapchain = m_device.createSwapchainKHR(createInfo);

        m_swapchain_image_format = surfaceFormat.format;
        m_swapchain_extent       = extent;
        LOG_INFO("m_swapchain_image_format:{}", to_string(m_swapchain_image_format));


    }

    vk::SurfaceFormatKHR VulkanRHI::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR>& formats)
    {

        for (const auto& format : formats)
        {
            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return format;
            }
        }
        return formats[0];
    }

    vk::Extent2D VulkanRHI::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)

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

            actualExtent.width =
                std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height =
                std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            return actualExtent;
        }
    }

    vk::PresentModeKHR VulkanRHI::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& modes)
    {
        for (const auto& mode : modes)
        {
            if (mode == vk::PresentModeKHR::eMailbox)
            {
                return mode;
            }
        }
        return vk::PresentModeKHR::eFifo;
    }
    void VulkanRHI::createSwapChainImageViews()
    {

        uint32_t imageCount = 3;

        m_swapchain_images.resize(imageCount);

        uint32_t swapchain_imageCount = 0;
        m_device.getSwapchainImagesKHR(m_swapchain, &swapchain_imageCount, nullptr);
        LOG_INFO("  SwapChain image count :  goal:  {0},  current {1}", imageCount, swapchain_imageCount);

        if (swapchain_imageCount != imageCount)
        {
            LOG_ERROR("SwapChain image count error!");
        }
        auto result = m_device.getSwapchainImagesKHR(m_swapchain, &imageCount, m_swapchain_images.data());

        if (result != vk::Result::eSuccess)
        {
            LOG_ERROR("failed to create get  swapchain images!")
        }

        //
        m_swapchain_imageviews.resize(swapchain_imageCount);

        for (size_t i = 0; i < swapchain_imageCount; i++)
        {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.image      = m_swapchain_images[i];
            createInfo.viewType   = vk::ImageViewType::e2D;
            createInfo.format     = m_swapchain_image_format; // 和 swapchain 格式一致
            createInfo.components = {
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity};
            createInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;
            m_swapchain_imageviews[i]                  = m_device.createImageView(createInfo);
        }
    }

    void VulkanRHI::createDepthImageAndView()
    {
        vk::Extent3D depthExtent;

        depthExtent.width  = m_swapchain_extent.width;
        depthExtent.height = m_swapchain_extent.height;
        depthExtent.depth  = 1;

        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType     = vk::ImageType::e2D;
        imageInfo.extent        = depthExtent;
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = vk::Format::eD32Sfloat;
        imageInfo.tiling        = vk::ImageTiling::eOptimal;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        imageInfo.samples       = vk::SampleCountFlagBits::e1;
        imageInfo.sharingMode   = vk::SharingMode::eExclusive;

        m_depth_image = m_device.createImage(imageInfo);

        m_depth_image_format = vk::Format::eD32Sfloat;

        vk::MemoryRequirements memRequirements = m_device.getImageMemoryRequirements(m_depth_image);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = vkutil::findMemoryType(
            memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, m_physical_device);

        m_depth_image_memory = m_device.allocateMemory(allocInfo);

        m_device.bindImageMemory(m_depth_image, m_depth_image_memory, 0);

        // ????????
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image                           = m_depth_image;
        viewInfo.viewType                        = vk::ImageViewType::e2D;
        viewInfo.format                          = vk::Format::eD32Sfloat;
        viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eDepth;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        m_depth_image_view = m_device.createImageView(viewInfo);
    }
    void VulkanRHI::createAssetAllocator()
    {

        VmaVulkanFunctions vulkanFunctions    = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion       = m_vulkan_api_version;
        allocatorCreateInfo.physicalDevice         = m_physical_device;
        allocatorCreateInfo.device                 = m_device;
        allocatorCreateInfo.instance               = m_instance;
        allocatorCreateInfo.pVulkanFunctions       = &vulkanFunctions;

        vmaCreateAllocator(&allocatorCreateInfo, &m_assets_allocator);
    }

    const SwapChainDesc& VulkanRHI::getSwapchainInfo()
    {

        m_swpachain_desc.extent       = m_swapchain_extent;
        m_swpachain_desc.imageViews   = m_swapchain_imageviews;
        m_swpachain_desc.image_format = m_swapchain_image_format;
        m_swpachain_desc.scissor      = m_scissor;
        m_swpachain_desc.viewport     = m_viewport;
        return m_swpachain_desc;
    }

    bool VulkanRHI::createRenderPass(const vk::RenderPassCreateInfo* pCreateInfo, vk::RenderPass& pRenderPass)
    {

        pRenderPass = m_device.createRenderPass(*pCreateInfo);

        return true;
    }
    bool VulkanRHI::createDescriptorSetLayout(
        const std::vector<vk::DescriptorSetLayoutBinding>& bindings,
        vk::DescriptorSetLayout&                           DescriptorSetLayout)
    {

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = static_cast<uint32_t>((bindings).size());
        layoutInfo.pBindings    = (bindings).data();

        DescriptorSetLayout = m_device.createDescriptorSetLayout(layoutInfo);
        return true;
    }
    bool VulkanRHI::createPipelineLayout(
        const vk::PipelineLayoutCreateInfo* pPipelineLayoutInfo,
        vk::PipelineLayout&                 pPipelineLayout)
    {
        pPipelineLayout = m_device.createPipelineLayout(*pPipelineLayoutInfo);
        return true;
    }

    vk::ShaderModule VulkanRHI::createShaderModule(const std::vector<unsigned char>& shader_code)
    {
        auto result = vkutil::createShaderModule(m_device, shader_code);

        return result;
    }
    bool VulkanRHI::createGraphicsPipeline(
        const vk::GraphicsPipelineCreateInfo& pipeline_create_info,
        vk::Pipeline&                         pipeline)
    {

        pipeline = m_device.createGraphicsPipeline(nullptr, pipeline_create_info).value;

        return true;
    }
    vk::DescriptorPool VulkanRHI::getDescriptorPool() { return m_descriptor_pool; }
    void               VulkanRHI::createDescriptorSet(
        const vk::DescriptorSetAllocateInfo& dst_set_allocate_info,
        vk::DescriptorSet&                   descriptor_set)
    {

        descriptor_set = m_device.allocateDescriptorSets(dst_set_allocate_info)[0];
    }
    void VulkanRHI::updateDescriptorSets(const std::vector<vk::WriteDescriptorSet> write_dst_sets)
    {

        m_device.updateDescriptorSets(static_cast<uint32_t>(write_dst_sets.size()), write_dst_sets.data(), 0, nullptr);
    }
    DepthImageDesc VulkanRHI::getDepthImageInfo()
    {

        DepthImageDesc depthImageDesc{};
        depthImageDesc.depth_image        = m_depth_image;
        depthImageDesc.depth_image_view   = m_depth_image_view;
        depthImageDesc.depth_image_format = m_depth_image_format;
        return depthImageDesc;
    }
    bool
    VulkanRHI::createFramebuffer(const vk::FramebufferCreateInfo& framebuffer_create_info, vk::Framebuffer& framebuffer)
    {
        framebuffer = m_device.createFramebuffer(framebuffer_create_info);
        return true;
    }


    void VulkanRHI::createBufferVMA(
        vk::DeviceSize       size,
        vk::BufferUsageFlags usage,
        VmaMemoryUsage       memoryUsage,
        vk::Buffer&          buffer,
        VmaAllocation&       allocation)
    {
        // 1. 填 buffer 描述
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.size        = size;
        bufferInfo.usage       = usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        // 2. 填分配策略
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage; // 比如 VMA_MEMORY_USAGE_AUTO, HOST_VISIBLE 等

        // 3. 调 VMA 创建
        VkBuffer           cBuffer;
        VkBufferCreateInfo cBufferInfo = static_cast<VkBufferCreateInfo>(bufferInfo);

        if (vmaCreateBuffer(m_assets_allocator, &cBufferInfo, &allocInfo, &cBuffer, &allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer with VMA!");
        }
        buffer = cBuffer;
    }
    void VulkanRHI::cmdBeginRenderPass(vk::CommandBuffer       command_buffer,
                                       vk::RenderPassBeginInfo& render_pass_begin_info)
    {
        command_buffer.beginRenderPass(render_pass_begin_info,vk::SubpassContents::eInline);
    }

    vk::CommandBuffer VulkanRHI::getCurrentCommandBuffer() {

        return m_current_command_buffer;

    }
    uint8_t   VulkanRHI::getMaxFramesInFlight() const { return k_max_frames_in_flight;; }
    uint8_t   VulkanRHI::getCurrentFrameIndex() const { return m_current_frame_index;; }
    void      VulkanRHI::setCurrentFrameIndex(uint8_t index) { m_current_frame_index = index;}


    void VulkanRHI::cmdEndRenderPass(vk::CommandBuffer commandBuffer) {
        commandBuffer.endRenderPass(); }
    void VulkanRHI::cmdSetViewport(vk::CommandBuffer commandBuffer, vk::Viewport viewPort) {

        commandBuffer.setViewport(0,viewPort);

    }
    void VulkanRHI::cmdSetScissor(vk::CommandBuffer commandBuffer, vk::Rect2D scissor) {

        commandBuffer.setScissor(0,scissor);
    }

    void VulkanRHI::cmdBindPipeline(vk::CommandBuffer commandBuffer, vk::PipelineBindPoint bindPoint,
                                    vk::Pipeline pipeline)
    {
        commandBuffer.bindPipeline(bindPoint,pipeline);
    }
    void VulkanRHI::cmdDescriptorSets(vk::CommandBuffer commandBuffer, vk::PipelineBindPoint bindPoint,
                                      vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t descriptorSetCount,
                                      vk::DescriptorSet descriptorSet)
    {
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout,0,descriptorSet, nullptr);
    }
    void VulkanRHI::cmdBindVertexBuffers(vk::CommandBuffer commandBuffer,vk::Buffer vertexBuffer,vk::DeviceSize offsets) {
        commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer, &offsets);
    }
    void VulkanRHI::cmdBindIndexBuffer(vk::CommandBuffer commandBuffer,vk::Buffer buffer,vk::IndexType type) {


        commandBuffer.bindIndexBuffer(buffer,0,type);
    }
    void VulkanRHI::cmdDrewIndexed(vk::CommandBuffer commandBuffer,
                                    uint32_t           indexCount,
                                    uint32_t           instanceCount,
                                    uint32_t           firstIndex,
                                    int32_t            vertexOffset,
                                    uint32_t           firstInstance) {
        commandBuffer.drawIndexed(indexCount,instanceCount,firstIndex,vertexOffset,firstInstance);
    }
    void VulkanRHI::submitRendering() {
        std::cout<<"m_current_frame_index :"<<m_current_frame_index<<std::endl;



        m_command_buffers[m_current_frame_index].end();


                vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
                vk::SubmitInfo         submitInfo{};
                submitInfo.waitSemaphoreCount    = 1;
                submitInfo.pWaitSemaphores       = &m_image_available_semaphore[m_current_frame_index];
                vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                submitInfo.pWaitDstStageMask     = waitStages;
                submitInfo.commandBufferCount    = 1;
                submitInfo.pCommandBuffers       = &m_command_buffers[m_current_frame_index];
                submitInfo.signalSemaphoreCount  = 1;
                submitInfo.pSignalSemaphores     = &m_render_finished_semaphore[m_current_frame_index];

                m_graphics_queue.submit(submitInfo, m_frame_in_flight_fence[m_current_frame_index]);



                // ???????
                vk::PresentInfoKHR presentInfo{};
                presentInfo.waitSemaphoreCount = 1;
                presentInfo.pWaitSemaphores    = &m_render_finished_semaphore[m_current_frame_index];
                presentInfo.swapchainCount     = 1;
                presentInfo.pSwapchains        = &m_swapchain;
                presentInfo.pImageIndices      = &m_current_swapchain_image_index;
                presentInfo.pResults           = nullptr;

                vk::Result presentResult = m_present_queue.presentKHR(presentInfo);
                // 8. ?????
                // ???????????????????
                if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
                {
                    // recreateSwapChain();
                }
                else if (presentResult != vk::Result::eSuccess)
                {
                    throw std::runtime_error("failed to present swap chain image!");
                }



        m_current_frame_index = (m_current_frame_index + 1) % k_max_frames_in_flight;

    }



    bool VulkanRHI::prepareBeforePass()
    {
        // 1. 等待当前帧槽的 fence（确保 GPU 完成上一帧）
        m_device.waitForFences(m_frame_in_flight_fence[m_current_frame_index], VK_TRUE, UINT64_MAX);

        // 2. 获取 swapchain image
        auto acquireResult =
            m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, m_image_available_semaphore[m_current_frame_index],
                                         nullptr, &m_current_swapchain_image_index);


        if (acquireResult == vk::Result::eErrorOutOfDateKHR)
        {
            //	recreateSwapchain();
        }
        else if (acquireResult != vk::Result::eSuccess && acquireResult != vk::Result::eSuboptimalKHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // 3. reset 当前帧的 fence（准备本帧提交）
        m_device.resetFences(m_frame_in_flight_fence[m_current_frame_index]);



        // Command buffer begin info
        vk::CommandBufferBeginInfo commandbuffer_begin_info{};
        commandbuffer_begin_info.flags = vk::CommandBufferUsageFlags();
        m_command_buffers[m_current_frame_index].begin(commandbuffer_begin_info);
    }


    void VulkanRHI::cmdUnMapMemory() {}
    void VulkanRHI::cmdMapMemory() {}
    void VulkanRHI::mapBufferMemory(VmaAllocation allocation, const void* data, size_t size)
    {
        void* mappedData = nullptr;
        vmaMapMemory(m_assets_allocator, allocation, &mappedData);
        memcpy(mappedData, data, size);
        vmaUnmapMemory(m_assets_allocator, allocation);
    }

}
    // namespace Coconut