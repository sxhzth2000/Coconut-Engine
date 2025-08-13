//
// Created by tanhao on 2025/8/11.
//

#ifndef COCONUT_VULKANRENDERPASS_H
#define COCONUT_VULKANRENDERPASS_H
#include "VulkanContext.h"
#include "framework/Reference.h"

#include <vulkan/vulkan.hpp>
namespace ccn {


	vk::RenderPass CreateRenderPass(const VulkanContext& context ,const vk::Format swapChainImageFormat);




    };


#endif //COCONUT_VULKANRENDERPASS_H