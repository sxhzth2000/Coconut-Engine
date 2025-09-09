#pragma once

#include "runtime/function/render/render_type.h"
#include <optional>
#include <vulkan/vulkan.hpp>
namespace Coconut{


    enum {
        descriptor_pool_main_camera  =    0,

        descriptor_pool_count  =1
    };



    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;
        std::optional<uint32_t> m_compute_family;

        bool isComplete() { return graphics_family.has_value() && present_family.has_value() && m_compute_family.has_value();; }
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR        capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR>   presentModes;
    };
    struct SwapChainDesc
    {
        vk::Extent2D extent;
        vk::Format   image_format;
        vk::Viewport viewport;
        vk::Rect2D scissor;
        std::vector<vk::ImageView> imageViews;
    };

    struct DepthImageDesc
    {
        vk::Image depth_image = VK_NULL_HANDLE;
        vk::ImageView depth_image_view = VK_NULL_HANDLE;
        vk::Format        depth_image_format;
    };



}

