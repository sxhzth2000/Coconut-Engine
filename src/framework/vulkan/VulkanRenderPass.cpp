//
// Created by tanhao on 2025/8/11.
//

#include "VulkanRenderPass.h"

#include "utils/utils.hpp"

namespace ccn {



vk::RenderPass CreateRenderPass(const VulkanContext& context ,const vk::Format swapChainImageFormat)
{
	vk::AttachmentDescription colorAttachment;
	vk::AttachmentReference colorAttachmentRef;


	vk::AttachmentDescription depthAttachment;
	vk::AttachmentReference depthAttachmentRef;


// 1. 设置颜色附件

	colorAttachment.format         = swapChainImageFormat;
	colorAttachment.samples        = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout  = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

	colorAttachmentRef.attachment = 0;        // 与上面 colorAttachment 的 index 对应
	colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

	// 2.深度附件

	depthAttachment.format         = vk::Format::eD32Sfloat; // 你需要实现这个函数，选择合适的深度格式，比如VK_FORMAT_D32_SFLOAT
	depthAttachment.samples        = vk::SampleCountFlagBits::e1;
	depthAttachment.loadOp         = vk::AttachmentLoadOp::eClear;  // 清除深度缓冲
	depthAttachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
	depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.initialLayout  = vk::ImageLayout::eUndefined;
	depthAttachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;



	depthAttachmentRef.attachment = 1; // 第二个附件索引
	depthAttachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;



	// 3. 设置子通道（Subpass）
	vk::SubpassDescription subpass{};
	subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments    = &colorAttachmentRef;   // 1
	subpass.pDepthStencilAttachment = &depthAttachmentRef;  // 2

//3.子通道依赖
	vk::SubpassDependency dependency{};
	dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass    = 0;
	dependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
	dependency.srcAccessMask = {};
	dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;



	// 4. 填充 RenderPassCreateInfo  2个附件
	std::vector<vk::AttachmentDescription> attachments {colorAttachment, depthAttachment};
	vk::RenderPassCreateInfo renderPassInfo{};
	renderPassInfo.attachmentCount =  static_cast<uint32_t>(attachments.size());;
	renderPassInfo.pAttachments    = attachments.data();
	renderPassInfo.subpassCount    = 1;
	renderPassInfo.pSubpasses      = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies   = &dependency;

	// 5. 创建 RenderPass
	return   context.device.createRenderPass(renderPassInfo);

}
} // ccn