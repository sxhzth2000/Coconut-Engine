//
// Created by tanhao on 2025/8/29.
//
#include "main_camera_pass.h"

#include "glm/gtc/matrix_transform.hpp"
#include "runtime/core/base/macro.h"
#include "runtime/function/render/interface/vulkan/vulkan_util.h"
#include "runtime/function/render/render_common.h"
#include "runtime/function/render/render_mesh.h"
#include "stb/stb_image.h"

namespace Coconut
{
    void Coconut::MainCameraPass::setupAttachments()
    {
        // depth and swapchain pass is not in attachment,but why?,
        //  cause depht and swapchain is in Rendering pass not in framebuffer,and they have

        m_framebuffer.attachments.resize(_main_camera_pass_custom_attachment_count +
                                         _main_camera_pass_post_process_attachment_count);

        m_framebuffer.attachments[_main_camera_pass_gbuffer_a].format          = vk::Format::eR8G8B8A8Unorm;
        m_framebuffer.attachments[_main_camera_pass_gbuffer_b].format          = vk::Format::eR8G8B8A8Unorm;
        m_framebuffer.attachments[_main_camera_pass_gbuffer_c].format          = vk::Format::eR8G8B8A8Srgb;
        m_framebuffer.attachments[_main_camera_pass_backup_buffer_odd].format  = vk::Format::eR16G16B16A16Sfloat;
   //   m_framebuffer.attachments[_main_camera_pass_backup_buffer_even].format = vk::Format::eR16G16B16A16Sfloat;

        for (int buffer_index = 0; buffer_index < _main_camera_pass_custom_attachment_count; ++buffer_index)
        {
            if (buffer_index == _main_camera_pass_gbuffer_a)
            {
                vk::ImageCreateInfo image_info;
                image_info.imageType     = vk::ImageType::e2D;
                image_info.extent.width  = m_rhi->getSwapchainInfo().extent.width;
                image_info.extent.height = m_rhi->getSwapchainInfo().extent.height;
                image_info.extent.depth  = 1;
                image_info.mipLevels     = 1;
                image_info.arrayLayers   = 1;
                image_info.format        = m_framebuffer.attachments[_main_camera_pass_gbuffer_a].format;
                image_info.tiling        = vk::ImageTiling::eOptimal;
                image_info.initialLayout = vk::ImageLayout::eUndefined;
                image_info.usage = vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eColorAttachment |
                                   vk::ImageUsageFlagBits::eTransferSrc;
                image_info.sharingMode = vk::SharingMode::eExclusive;
                image_info.samples     = vk::SampleCountFlagBits::e1;
                image_info.flags       = {};

                m_rhi->createImageVMA(image_info, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
                                      m_framebuffer.attachments[buffer_index].image,
                                      m_framebuffer.attachments[buffer_index].allocation);
            }
            else
            {
                vk::ImageCreateInfo image_info;
                image_info.imageType     = vk::ImageType::e2D;
                image_info.extent.width  = m_rhi->getSwapchainInfo().extent.width;
                image_info.extent.height = m_rhi->getSwapchainInfo().extent.height;
                image_info.extent.depth  = 1;
                image_info.mipLevels     = 1;
                image_info.arrayLayers   = 1;
                image_info.format        = m_framebuffer.attachments[buffer_index].format;
                image_info.tiling        = vk::ImageTiling::eOptimal;
                image_info.initialLayout = vk::ImageLayout::eUndefined;
                image_info.usage = vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eColorAttachment |
                                   vk::ImageUsageFlagBits::eTransferSrc;
                image_info.sharingMode = vk::SharingMode::eExclusive;
                image_info.samples     = vk::SampleCountFlagBits::e1;
                image_info.flags       = {};

                m_rhi->createImageVMA(image_info, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
                                      m_framebuffer.attachments[buffer_index].image,
                                      m_framebuffer.attachments[buffer_index].allocation);
            }

            m_rhi->createImageView(m_framebuffer.attachments[buffer_index].image,
                                   m_framebuffer.attachments[buffer_index].format, vk::ImageAspectFlagBits::eColor,
                                   m_framebuffer.attachments[buffer_index].image_view);
        }

  //      m_framebuffer.attachments[_main_camera_pass_post_process_buffer_odd].format  = vk::Format::eR16G16B16A16Sfloat;
  //      m_framebuffer.attachments[_main_camera_pass_post_process_buffer_even].format = vk::Format::eR16G16B16A16Sfloat;

        for (int attachment_index = _main_camera_pass_custom_attachment_count;
             attachment_index <
             _main_camera_pass_custom_attachment_count + _main_camera_pass_post_process_attachment_count;
             ++attachment_index)
        {
            vk::ImageCreateInfo image_info;
            image_info.imageType     = vk::ImageType::e2D;
            image_info.extent.width  = m_rhi->getSwapchainInfo().extent.width;
            image_info.extent.height = m_rhi->getSwapchainInfo().extent.height;
            image_info.extent.depth  = 1;
            image_info.mipLevels     = 1;
            image_info.arrayLayers   = 1;
            image_info.format        = m_framebuffer.attachments[attachment_index].format;
            image_info.tiling        = vk::ImageTiling::eOptimal;
            image_info.initialLayout = vk::ImageLayout::eUndefined;
            image_info.usage = vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eColorAttachment |
                               vk::ImageUsageFlagBits::eSampled;
            image_info.sharingMode = vk::SharingMode::eExclusive; // (图形队列专用)
            image_info.samples     = vk::SampleCountFlagBits::e1; // (延迟渲染通常不用MSAA)
            image_info.flags       = {};

            m_rhi->createImageVMA(image_info, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
                                  m_framebuffer.attachments[attachment_index].image,
                                  m_framebuffer.attachments[attachment_index].allocation);

            m_rhi->createImageView(m_framebuffer.attachments[attachment_index].image,
                                   m_framebuffer.attachments[attachment_index].format, vk::ImageAspectFlagBits::eColor,
                                   m_framebuffer.attachments[attachment_index].image_view);
        }
    }

    void Coconut::MainCameraPass::setupRenderPass()
    {
        LOG_INFO("");

        vk::AttachmentDescription attachments[_main_camera_pass_attachment_count] = {};

        vk::AttachmentDescription &gbuffer_normal_attachment_description = attachments[_main_camera_pass_gbuffer_a];
        gbuffer_normal_attachment_description.format  = m_framebuffer.attachments[_main_camera_pass_gbuffer_a].format;
        gbuffer_normal_attachment_description.samples = vk::SampleCountFlagBits::e1;
        gbuffer_normal_attachment_description.loadOp  = vk::AttachmentLoadOp::eClear;
        gbuffer_normal_attachment_description.storeOp = vk::AttachmentStoreOp::eStore;;
        gbuffer_normal_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        gbuffer_normal_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        gbuffer_normal_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        gbuffer_normal_attachment_description.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;


        vk::AttachmentDescription &gbuffer_metallic_roughness_shadingmodeid_attachment_description =
            attachments[_main_camera_pass_gbuffer_b];
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.format =
            m_framebuffer.attachments[_main_camera_pass_gbuffer_b].format;
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.samples = vk::SampleCountFlagBits::e1;
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.loadOp  = vk::AttachmentLoadOp::eClear;
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.storeOp = vk::AttachmentStoreOp::eStore;;
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.initialLayout = vk::ImageLayout::eUndefined;
        gbuffer_metallic_roughness_shadingmodeid_attachment_description.finalLayout   = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::AttachmentDescription &gbuffer_albedo_attachment_description = attachments[_main_camera_pass_gbuffer_c];
        gbuffer_albedo_attachment_description.format  = m_framebuffer.attachments[_main_camera_pass_gbuffer_c].format;
        gbuffer_albedo_attachment_description.samples = vk::SampleCountFlagBits::e1;
        gbuffer_albedo_attachment_description.loadOp  = vk::AttachmentLoadOp::eClear;
        gbuffer_albedo_attachment_description.storeOp = vk::AttachmentStoreOp::eDontCare;
        gbuffer_albedo_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        gbuffer_albedo_attachment_description.stencilStoreOp =  vk::AttachmentStoreOp::eDontCare;
        gbuffer_albedo_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        gbuffer_albedo_attachment_description.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::AttachmentDescription& backup_odd_color_attachment_description =
            attachments[_main_camera_pass_backup_buffer_odd];
        backup_odd_color_attachment_description.format =
            m_framebuffer.attachments[_main_camera_pass_backup_buffer_odd].format;
        backup_odd_color_attachment_description.samples        = vk::SampleCountFlagBits::e1;
        backup_odd_color_attachment_description.loadOp         = vk::AttachmentLoadOp::eClear;
        backup_odd_color_attachment_description.storeOp        = vk::AttachmentStoreOp::eDontCare;
        backup_odd_color_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        backup_odd_color_attachment_description.stencilStoreOp =  vk::AttachmentStoreOp::eDontCare;
        backup_odd_color_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        backup_odd_color_attachment_description.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;
/*

        vk::AttachmentDescription& backup_even_color_attachment_description =
            attachments[_main_camera_pass_backup_buffer_even];
        backup_even_color_attachment_description.format =
            m_framebuffer.attachments[_main_camera_pass_backup_buffer_even].format;
        backup_even_color_attachment_description.samples        = vk::SampleCountFlagBits::e1;
        backup_even_color_attachment_description.loadOp         = vk::AttachmentLoadOp::eClear;
        backup_even_color_attachment_description.storeOp        = vk::AttachmentStoreOp::eDontCare;
        backup_even_color_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        backup_even_color_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        backup_even_color_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        backup_even_color_attachment_description.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::AttachmentDescription& post_process_odd_color_attachment_description =
            attachments[_main_camera_pass_post_process_buffer_odd];
        post_process_odd_color_attachment_description.format =
            m_framebuffer.attachments[_main_camera_pass_post_process_buffer_odd].format;
        post_process_odd_color_attachment_description.samples        = vk::SampleCountFlagBits::e1;
        post_process_odd_color_attachment_description.loadOp         = vk::AttachmentLoadOp::eClear;
        post_process_odd_color_attachment_description.storeOp        = vk::AttachmentStoreOp::eDontCare;
        post_process_odd_color_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        post_process_odd_color_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        post_process_odd_color_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        post_process_odd_color_attachment_description.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::AttachmentDescription& post_process_even_color_attachment_description =
            attachments[_main_camera_pass_post_process_buffer_even];
        post_process_even_color_attachment_description.format =
            m_framebuffer.attachments[_main_camera_pass_post_process_buffer_even].format;
        post_process_even_color_attachment_description.samples        = vk::SampleCountFlagBits::e1;
        post_process_even_color_attachment_description.loadOp         = vk::AttachmentLoadOp::eClear;
        post_process_even_color_attachment_description.storeOp        = vk::AttachmentStoreOp::eDontCare;
        post_process_even_color_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        post_process_even_color_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        post_process_even_color_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        post_process_even_color_attachment_description.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;
*/

        vk::AttachmentDescription& depth_attachment_description = attachments[_main_camera_pass_depth];
        depth_attachment_description.format                   = m_rhi->getDepthImageInfo().depth_image_format;
        depth_attachment_description.samples                  = vk::SampleCountFlagBits::e1;
        depth_attachment_description.loadOp                   = vk::AttachmentLoadOp::eClear;
        depth_attachment_description.storeOp                  = vk::AttachmentStoreOp::eStore;
        depth_attachment_description.stencilLoadOp            = vk::AttachmentLoadOp::eDontCare;
        depth_attachment_description.stencilStoreOp           = vk::AttachmentStoreOp::eDontCare;
        depth_attachment_description.initialLayout            = vk::ImageLayout::eUndefined;
        depth_attachment_description.finalLayout              = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentDescription& swapchain_image_attachment_description =
            attachments[_main_camera_pass_swap_chain_image];
        swapchain_image_attachment_description.format         = m_rhi->getSwapchainInfo().image_format;
        swapchain_image_attachment_description.samples        = vk::SampleCountFlagBits::e1;
        swapchain_image_attachment_description.loadOp         = vk::AttachmentLoadOp::eClear;
        swapchain_image_attachment_description.storeOp        = vk::AttachmentStoreOp::eStore;
        swapchain_image_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        swapchain_image_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        swapchain_image_attachment_description.initialLayout  = vk::ImageLayout::eUndefined;
        swapchain_image_attachment_description.finalLayout    = vk::ImageLayout::ePresentSrcKHR;


        LOG_INFO("m_Renderpass_color_attachment_image_format:{}", to_string(swapchain_image_attachment_description.format));

        vk::SubpassDescription subpasses[_main_camera_subpass_count] = {};


        /// base pass
        vk::AttachmentReference base_pass_color_attachments_reference[3] ={};
        base_pass_color_attachments_reference[0].attachment = &gbuffer_normal_attachment_description - attachments;
        base_pass_color_attachments_reference[0].layout     = vk::ImageLayout::eColorAttachmentOptimal;
        base_pass_color_attachments_reference[1].attachment =
            &gbuffer_metallic_roughness_shadingmodeid_attachment_description - attachments;
        base_pass_color_attachments_reference[1].layout     = vk::ImageLayout::eColorAttachmentOptimal;
        base_pass_color_attachments_reference[2].attachment = &gbuffer_albedo_attachment_description - attachments;
        base_pass_color_attachments_reference[2].layout     = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference base_pass_depth_attachment_reference {};
        base_pass_depth_attachment_reference.attachment = &depth_attachment_description - attachments;
        base_pass_depth_attachment_reference.layout     = vk::ImageLayout::eDepthAttachmentOptimal;

        vk::SubpassDescription& base_pass = subpasses[_main_camera_subpass_basepass];
        base_pass.pipelineBindPoint     = vk::PipelineBindPoint::eGraphics;
        base_pass.colorAttachmentCount =
            sizeof(base_pass_color_attachments_reference) / sizeof(base_pass_color_attachments_reference[0]);
        base_pass.pColorAttachments       = &base_pass_color_attachments_reference[0];
        base_pass.pDepthStencilAttachment = &base_pass_depth_attachment_reference;
        base_pass.preserveAttachmentCount = 0;
        base_pass.pPreserveAttachments    = NULL;

        /// deferred
        vk::AttachmentReference deferred_lighting_pass_input_attachments_reference[4] = {};
        deferred_lighting_pass_input_attachments_reference[0].attachment =
            &gbuffer_normal_attachment_description - attachments;
        deferred_lighting_pass_input_attachments_reference[0].layout = vk::ImageLayout::eColorAttachmentOptimal;
        deferred_lighting_pass_input_attachments_reference[1].attachment =
            &gbuffer_metallic_roughness_shadingmodeid_attachment_description - attachments;
        deferred_lighting_pass_input_attachments_reference[1].layout = vk::ImageLayout::eShaderReadOnlyOptimal;
        deferred_lighting_pass_input_attachments_reference[2].attachment =
            &gbuffer_albedo_attachment_description - attachments;
        deferred_lighting_pass_input_attachments_reference[2].layout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        deferred_lighting_pass_input_attachments_reference[3].attachment = &depth_attachment_description - attachments;
        deferred_lighting_pass_input_attachments_reference[3].layout     = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::AttachmentReference deferred_lighting_pass_color_attachment_reference[1] = {};
        deferred_lighting_pass_color_attachment_reference[0].attachment =
            &backup_odd_color_attachment_description - attachments;
        deferred_lighting_pass_color_attachment_reference[0].layout = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::SubpassDescription& deferred_lighting_pass = subpasses[_main_camera_subpass_deferred_lighting];
        deferred_lighting_pass.pipelineBindPoint     = vk::PipelineBindPoint::eGraphics;
        deferred_lighting_pass.inputAttachmentCount  = sizeof(deferred_lighting_pass_input_attachments_reference) /
                                                      sizeof(deferred_lighting_pass_input_attachments_reference[0]);
        deferred_lighting_pass.pInputAttachments    = &deferred_lighting_pass_input_attachments_reference[0];
        deferred_lighting_pass.colorAttachmentCount = sizeof(deferred_lighting_pass_color_attachment_reference) /
                                                      sizeof(deferred_lighting_pass_color_attachment_reference[0]);
        deferred_lighting_pass.pColorAttachments       = &deferred_lighting_pass_color_attachment_reference[0];
        deferred_lighting_pass.pDepthStencilAttachment = NULL;
        deferred_lighting_pass.preserveAttachmentCount = 0;
        deferred_lighting_pass.pPreserveAttachments    = NULL;


        /// forward
        vk::AttachmentReference forward_lighting_pass_color_attachments_reference[1] = {};
        forward_lighting_pass_color_attachments_reference[0].attachment =
            &backup_odd_color_attachment_description - attachments;
        forward_lighting_pass_color_attachments_reference[0].layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference forward_lighting_pass_depth_attachment_reference {};
        forward_lighting_pass_depth_attachment_reference.attachment = &depth_attachment_description - attachments;
        forward_lighting_pass_depth_attachment_reference.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;;

        vk::SubpassDescription& forward_lighting_pass = subpasses[_main_camera_subpass_forward_lighting];
        forward_lighting_pass.pipelineBindPoint     = vk::PipelineBindPoint::eGraphics;
        forward_lighting_pass.inputAttachmentCount  = 0U;
        forward_lighting_pass.pInputAttachments     = NULL;
        forward_lighting_pass.colorAttachmentCount  = sizeof(forward_lighting_pass_color_attachments_reference) /
                                                     sizeof(forward_lighting_pass_color_attachments_reference[0]);
        forward_lighting_pass.pColorAttachments       = &forward_lighting_pass_color_attachments_reference[0];
        forward_lighting_pass.pDepthStencilAttachment = &forward_lighting_pass_depth_attachment_reference;
        forward_lighting_pass.preserveAttachmentCount = 0;
        forward_lighting_pass.pPreserveAttachments    = NULL;
/*

        /// tone mapping
        vk::AttachmentReference tone_mapping_pass_input_attachment_reference {};
        tone_mapping_pass_input_attachment_reference.attachment =
            &backup_odd_color_attachment_description - attachments;
        tone_mapping_pass_input_attachment_reference.layout = vk::ImageLayout::eReadOnlyOptimal;

        vk::AttachmentReference tone_mapping_pass_color_attachment_reference {};
        tone_mapping_pass_color_attachment_reference.attachment =
            &backup_even_color_attachment_description - attachments;
        tone_mapping_pass_color_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;


        vk::SubpassDescription& tone_mapping_pass   = subpasses[_main_camera_subpass_tone_mapping];
        tone_mapping_pass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
        tone_mapping_pass.inputAttachmentCount    = 1;
        tone_mapping_pass.pInputAttachments       = &tone_mapping_pass_input_attachment_reference;
        tone_mapping_pass.colorAttachmentCount    = 1;
        tone_mapping_pass.pColorAttachments       = &tone_mapping_pass_color_attachment_reference;
        tone_mapping_pass.pDepthStencilAttachment = NULL;
        tone_mapping_pass.preserveAttachmentCount = 0;
        tone_mapping_pass.pPreserveAttachments    = NULL;



        /// color_grading
        vk::AttachmentReference color_grading_pass_input_attachment_reference {};
        color_grading_pass_input_attachment_reference.attachment =
            &backup_even_color_attachment_description - attachments;
        color_grading_pass_input_attachment_reference.layout = vk::ImageLayout::eShaderReadOnlyOptimal;
        vk::AttachmentReference color_grading_pass_color_attachment_reference {};
        if (m_enable_fxaa)
        {
            color_grading_pass_color_attachment_reference.attachment =
                &post_process_odd_color_attachment_description - attachments;
        }
        else
        {
            color_grading_pass_color_attachment_reference.attachment =
                &backup_odd_color_attachment_description - attachments;
        }
        color_grading_pass_color_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;


        /// fxaa
        vk::AttachmentReference fxaa_pass_input_attachment_reference {};
        if (m_enable_fxaa)
        {
            fxaa_pass_input_attachment_reference.attachment =
                &post_process_odd_color_attachment_description - attachments;
        }
        else
        {
            fxaa_pass_input_attachment_reference.attachment = &backup_even_color_attachment_description - attachments;
        }
        fxaa_pass_input_attachment_reference.layout = vk::ImageLayout::eShaderReadOnlyOptimal;


        vk::AttachmentReference fxaa_pass_color_attachment_reference {};
        fxaa_pass_color_attachment_reference.attachment = &backup_odd_color_attachment_description - attachments;
        fxaa_pass_color_attachment_reference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription& fxaa_pass   = subpasses[_main_camera_subpass_fxaa];
        fxaa_pass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
        fxaa_pass.inputAttachmentCount    = 1;
        fxaa_pass.pInputAttachments       = &fxaa_pass_input_attachment_reference;
        fxaa_pass.colorAttachmentCount    = 1;
        fxaa_pass.pColorAttachments       = &fxaa_pass_color_attachment_reference;
        fxaa_pass.pDepthStencilAttachment = NULL;
        fxaa_pass.preserveAttachmentCount = 0;
        fxaa_pass.pPreserveAttachments    = NULL;

        /// ui
        vk::AttachmentReference ui_pass_color_attachment_reference {};
        ui_pass_color_attachment_reference.attachment = &backup_even_color_attachment_description - attachments;
        ui_pass_color_attachment_reference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

        uint32_t ui_pass_preserve_attachment = &backup_odd_color_attachment_description - attachments;

        vk::SubpassDescription& ui_pass  = subpasses[_main_camera_subpass_ui];
        ui_pass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
        ui_pass.inputAttachmentCount    = 0;
        ui_pass.pInputAttachments       = NULL;
        ui_pass.colorAttachmentCount    = 1;
        ui_pass.pColorAttachments       = &ui_pass_color_attachment_reference;
        ui_pass.pDepthStencilAttachment = NULL;
        ui_pass.preserveAttachmentCount = 1;
        ui_pass.pPreserveAttachments    = &ui_pass_preserve_attachment;
        /// combine ui
        vk::AttachmentReference combine_ui_pass_input_attachments_reference[2] = {};
        combine_ui_pass_input_attachments_reference[0].attachment =
            &backup_odd_color_attachment_description - attachments;
        combine_ui_pass_input_attachments_reference[0].layout = vk::ImageLayout::eShaderReadOnlyOptimal;
        combine_ui_pass_input_attachments_reference[1].attachment =
            &backup_even_color_attachment_description - attachments;
        combine_ui_pass_input_attachments_reference[1].layout = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::AttachmentReference combine_ui_pass_color_attachment_reference {};
        combine_ui_pass_color_attachment_reference.attachment = &swapchain_image_attachment_description - attachments;
        combine_ui_pass_color_attachment_reference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription& combine_ui_pass = subpasses[_main_camera_subpass_combine_ui];
        combine_ui_pass.pipelineBindPoint      = vk::PipelineBindPoint::eGraphics;
        combine_ui_pass.inputAttachmentCount   = sizeof(combine_ui_pass_input_attachments_reference) /
                                               sizeof(combine_ui_pass_input_attachments_reference[0]);
        combine_ui_pass.pInputAttachments       = combine_ui_pass_input_attachments_reference;
        combine_ui_pass.colorAttachmentCount    = 1;
        combine_ui_pass.pColorAttachments       = &combine_ui_pass_color_attachment_reference;
        combine_ui_pass.pDepthStencilAttachment = NULL;
        combine_ui_pass.preserveAttachmentCount = 0;
        combine_ui_pass.pPreserveAttachments    = NULL;
*/


        ///dependency

        /// 只保留三个subpass之间的dependency
        vk::SubpassDependency dependencies[3] = {};

        /// deferred lighting pass 依赖 base pass
        vk::SubpassDependency& deferred_lighting_pass_depend_on_base_pass = dependencies[0];
        deferred_lighting_pass_depend_on_base_pass.srcSubpass           = _main_camera_subpass_basepass;
        deferred_lighting_pass_depend_on_base_pass.dstSubpass           = _main_camera_subpass_deferred_lighting;
        deferred_lighting_pass_depend_on_base_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        deferred_lighting_pass_depend_on_base_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        deferred_lighting_pass_depend_on_base_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;
        deferred_lighting_pass_depend_on_base_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        deferred_lighting_pass_depend_on_base_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;

        /// forward lighting pass 依赖 deferred lighting pass
        vk::SubpassDependency& forward_lighting_pass_depend_on_deferred_lighting_pass = dependencies[1];
        forward_lighting_pass_depend_on_deferred_lighting_pass.srcSubpass = _main_camera_subpass_deferred_lighting;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dstSubpass = _main_camera_subpass_forward_lighting;
        forward_lighting_pass_depend_on_deferred_lighting_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        forward_lighting_pass_depend_on_deferred_lighting_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;

        /// forward lighting pass 输出到最终的swapchain image
        vk::SubpassDependency& final_output_dependency = dependencies[2];
        final_output_dependency.srcSubpass           = _main_camera_subpass_forward_lighting;
        final_output_dependency.dstSubpass           = vk::SubpassExternal;
        final_output_dependency.srcStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput;
        final_output_dependency.dstStageMask =
            vk::PipelineStageFlagBits::eBottomOfPipe;
        final_output_dependency.srcAccessMask =
            vk::AccessFlagBits::eColorAttachmentWrite;
        final_output_dependency.dstAccessMask =
            vk::AccessFlagBits::eMemoryRead;
        final_output_dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

        vk::RenderPassCreateInfo renderpass_create_info {};
        renderpass_create_info.attachmentCount = (sizeof(attachments) / sizeof(attachments[0]));
        renderpass_create_info.pAttachments    = attachments;
        renderpass_create_info.subpassCount    = 3; // 只保留三个subpass
        renderpass_create_info.pSubpasses      = subpasses;
        renderpass_create_info.dependencyCount = (sizeof(dependencies) / sizeof(dependencies[0]));
        renderpass_create_info.pDependencies   = dependencies;


        /*
        vk::SubpassDependency dependencies[8] = {};


        vk::SubpassDependency& deferred_lighting_pass_depend_on_shadow_map_pass = dependencies[0];
        deferred_lighting_pass_depend_on_shadow_map_pass.srcSubpass           = vk::SubpassExternal;
        deferred_lighting_pass_depend_on_shadow_map_pass.dstSubpass           = _main_camera_subpass_deferred_lighting;
        deferred_lighting_pass_depend_on_shadow_map_pass.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        deferred_lighting_pass_depend_on_shadow_map_pass.dstStageMask  = vk::PipelineStageFlagBits::eFragmentShader;
        deferred_lighting_pass_depend_on_shadow_map_pass.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        deferred_lighting_pass_depend_on_shadow_map_pass.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        deferred_lighting_pass_depend_on_shadow_map_pass.dependencyFlags = {}; // NOT BY REGION

        vk::SubpassDependency& deferred_lighting_pass_depend_on_base_pass = dependencies[1];
        deferred_lighting_pass_depend_on_base_pass.srcSubpass           = _main_camera_subpass_basepass;
        deferred_lighting_pass_depend_on_base_pass.dstSubpass           = _main_camera_subpass_deferred_lighting;
        deferred_lighting_pass_depend_on_base_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        deferred_lighting_pass_depend_on_base_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        deferred_lighting_pass_depend_on_base_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;;
        deferred_lighting_pass_depend_on_base_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        deferred_lighting_pass_depend_on_base_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;


        vk::SubpassDependency& forward_lighting_pass_depend_on_deferred_lighting_pass = dependencies[2];
        forward_lighting_pass_depend_on_deferred_lighting_pass.srcSubpass = _main_camera_subpass_deferred_lighting;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dstSubpass = _main_camera_subpass_forward_lighting;
        forward_lighting_pass_depend_on_deferred_lighting_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        forward_lighting_pass_depend_on_deferred_lighting_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        forward_lighting_pass_depend_on_deferred_lighting_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;


        vk::SubpassDependency& tone_mapping_pass_depend_on_lighting_pass = dependencies[3];
        tone_mapping_pass_depend_on_lighting_pass.srcSubpass           = _main_camera_subpass_forward_lighting;
        tone_mapping_pass_depend_on_lighting_pass.dstSubpass           = _main_camera_subpass_tone_mapping;
        tone_mapping_pass_depend_on_lighting_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        tone_mapping_pass_depend_on_lighting_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        tone_mapping_pass_depend_on_lighting_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;;
        tone_mapping_pass_depend_on_lighting_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        tone_mapping_pass_depend_on_lighting_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;

        vk::SubpassDependency& color_grading_pass_depend_on_tone_mapping_pass = dependencies[4];
        color_grading_pass_depend_on_tone_mapping_pass.srcSubpass           = _main_camera_subpass_tone_mapping;
        color_grading_pass_depend_on_tone_mapping_pass.dstSubpass           = _main_camera_subpass_color_grading;
        color_grading_pass_depend_on_tone_mapping_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        color_grading_pass_depend_on_tone_mapping_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        color_grading_pass_depend_on_tone_mapping_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;;
        color_grading_pass_depend_on_tone_mapping_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        color_grading_pass_depend_on_tone_mapping_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;

        vk::SubpassDependency& fxaa_pass_depend_on_color_grading_pass = dependencies[5];
        fxaa_pass_depend_on_color_grading_pass.srcSubpass           = _main_camera_subpass_color_grading;
        fxaa_pass_depend_on_color_grading_pass.dstSubpass           = _main_camera_subpass_fxaa;
        fxaa_pass_depend_on_color_grading_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        fxaa_pass_depend_on_color_grading_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        fxaa_pass_depend_on_color_grading_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;;
        fxaa_pass_depend_on_color_grading_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;

        vk::SubpassDependency& ui_pass_depend_on_fxaa_pass = dependencies[6];
        ui_pass_depend_on_fxaa_pass.srcSubpass           = _main_camera_subpass_fxaa;
        ui_pass_depend_on_fxaa_pass.dstSubpass           = _main_camera_subpass_ui;
        ui_pass_depend_on_fxaa_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        ui_pass_depend_on_fxaa_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        ui_pass_depend_on_fxaa_pass.srcAccessMask   = vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;;
        ui_pass_depend_on_fxaa_pass.dstAccessMask   = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        ui_pass_depend_on_fxaa_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;

        vk::SubpassDependency& combine_ui_pass_depend_on_ui_pass = dependencies[7];
        combine_ui_pass_depend_on_ui_pass.srcSubpass           = _main_camera_subpass_ui;
        combine_ui_pass_depend_on_ui_pass.dstSubpass           = _main_camera_subpass_combine_ui;
        combine_ui_pass_depend_on_ui_pass.srcStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        combine_ui_pass_depend_on_ui_pass.dstStageMask =
            vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        combine_ui_pass_depend_on_ui_pass.srcAccessMask =
            vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eColorAttachmentWrite;;
        combine_ui_pass_depend_on_ui_pass.dstAccessMask =
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead;
        combine_ui_pass_depend_on_ui_pass.dependencyFlags = vk::DependencyFlagBits::eByRegion;




        vk::RenderPassCreateInfo renderpass_create_info {};
        renderpass_create_info.attachmentCount = (sizeof(attachments) / sizeof(attachments[0]));
        renderpass_create_info.pAttachments    = attachments;
        renderpass_create_info.subpassCount    = (sizeof(subpasses) / sizeof(subpasses[0]));
        renderpass_create_info.pSubpasses      = subpasses;
        renderpass_create_info.dependencyCount = (sizeof(dependencies) / sizeof(dependencies[0]));
        renderpass_create_info.pDependencies   = dependencies;
*/
        // 5. 创建 RenderPass
        m_rhi->createRenderPass(&renderpass_create_info, m_framebuffer.render_pass);
    }

    void MainCameraPass::initialize()
    {
        LOG_INFO("");

        RenderPass::initialize(nullptr);

        setupAttachments();
        setupRenderPass();

        setupDescriptorSetLayout();
        setupPipelines();

        setupDescriptorSet();
        //    setupFramebufferDescriptorSet();
        setupSwapchainFramebuffers();

        //            setupParticlePass();
    }
    void MainCameraPass::setupDescriptorSetLayout()
    {
        LOG_INFO("");
        // for mesh lighting

        m_descriptor_infos.resize(_layout_type_count);

        {
            ///_mesh_global
            //
            std::array<vk::DescriptorSetLayoutBinding,2> mesh_global_layout_bindings;

            //proj_view_matrix , camera_position , etc
            vk::DescriptorSetLayoutBinding &mesh_global_layout_perframe_storage_buffer_binding =
                mesh_global_layout_bindings[0];
            mesh_global_layout_perframe_storage_buffer_binding.binding = 0;
            mesh_global_layout_perframe_storage_buffer_binding.descriptorType = vk::DescriptorType::eStorageBufferDynamic;
            mesh_global_layout_perframe_storage_buffer_binding.descriptorCount = 1;
            mesh_global_layout_perframe_storage_buffer_binding.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
            mesh_global_layout_perframe_storage_buffer_binding.pImmutableSamplers = NULL;
            // shadow-map
            vk::DescriptorSetLayoutBinding& mesh_global_layout_directional_light_shadow_texture_binding = mesh_global_layout_bindings[1];
            mesh_global_layout_directional_light_shadow_texture_binding.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
            mesh_global_layout_directional_light_shadow_texture_binding.descriptorCount    = 1;
            mesh_global_layout_directional_light_shadow_texture_binding.stageFlags         = vk::ShaderStageFlagBits::eFragment;
            mesh_global_layout_directional_light_shadow_texture_binding.pImmutableSamplers = NULL;
            mesh_global_layout_directional_light_shadow_texture_binding.binding = 1;
            m_rhi->createDescriptorSetLayout(std::vector(
                                                 mesh_global_layout_bindings.begin(),mesh_global_layout_bindings.end()),
                                             m_descriptor_infos[_mesh_global].layout);
        }

        {
            /// g-buffer-deferred lighting

            std::array< vk::DescriptorSetLayoutBinding,4> gbuffer_lighting_global_layout_bindings;
            vk::DescriptorSetLayoutBinding& gbuffer_normal_global_layout_input_attachment_binding =
                gbuffer_lighting_global_layout_bindings[0];
            gbuffer_normal_global_layout_input_attachment_binding.binding         = 0;
            gbuffer_normal_global_layout_input_attachment_binding.descriptorType  = vk::DescriptorType::eInputAttachment;
            gbuffer_normal_global_layout_input_attachment_binding.descriptorCount = 1;
            gbuffer_normal_global_layout_input_attachment_binding.stageFlags      = vk::ShaderStageFlagBits::eFragment;

            vk::DescriptorSetLayoutBinding&
                gbuffer_metallic_roughness_shadingmodeid_global_layout_input_attachment_binding =
                    gbuffer_lighting_global_layout_bindings[1];
            gbuffer_metallic_roughness_shadingmodeid_global_layout_input_attachment_binding.binding = 1;
            gbuffer_metallic_roughness_shadingmodeid_global_layout_input_attachment_binding.descriptorType =
                vk::DescriptorType::eInputAttachment;
            gbuffer_metallic_roughness_shadingmodeid_global_layout_input_attachment_binding.descriptorCount = 1;
            gbuffer_metallic_roughness_shadingmodeid_global_layout_input_attachment_binding.stageFlags =
                vk::ShaderStageFlagBits::eFragment;

            vk::DescriptorSetLayoutBinding& gbuffer_albedo_global_layout_input_attachment_binding =
                gbuffer_lighting_global_layout_bindings[2];
            gbuffer_albedo_global_layout_input_attachment_binding.binding         = 2;
            gbuffer_albedo_global_layout_input_attachment_binding.descriptorType  = vk::DescriptorType::eInputAttachment;
            gbuffer_albedo_global_layout_input_attachment_binding.descriptorCount = 1;
            gbuffer_albedo_global_layout_input_attachment_binding.stageFlags      = vk::ShaderStageFlagBits::eFragment;

            vk::DescriptorSetLayoutBinding& gbuffer_depth_global_layout_input_attachment_binding =
                gbuffer_lighting_global_layout_bindings[3];
            gbuffer_depth_global_layout_input_attachment_binding.binding         = 3;
            gbuffer_depth_global_layout_input_attachment_binding.descriptorType  = vk::DescriptorType::eInputAttachment;
            gbuffer_depth_global_layout_input_attachment_binding.descriptorCount = 1;
            gbuffer_depth_global_layout_input_attachment_binding.stageFlags      = vk::ShaderStageFlagBits::eFragment;

             m_rhi->createDescriptorSetLayout(std::vector(
                                                    gbuffer_lighting_global_layout_bindings.begin(), gbuffer_lighting_global_layout_bindings.end()),
                                                m_descriptor_infos[_deferred_lighting].layout);
        }

        {
            //mesh_per_material
            std::array<vk::DescriptorSetLayoutBinding,8> mesh_material_layout_bindings;

            // (set = 2, binding = 0 in fragment shader)
            vk::DescriptorSetLayoutBinding& mesh_material_layout_uniform_buffer_binding =
                mesh_material_layout_bindings[0];
            mesh_material_layout_uniform_buffer_binding.binding            = 0;
            mesh_material_layout_uniform_buffer_binding.descriptorType     = vk::DescriptorType::eUniformBuffer;
            mesh_material_layout_uniform_buffer_binding.descriptorCount    = 1;
            mesh_material_layout_uniform_buffer_binding.stageFlags         = vk::ShaderStageFlagBits::eFragment;
            mesh_material_layout_uniform_buffer_binding.pImmutableSamplers = nullptr;

            // (set = 2, binding = 1 in fragment shader)
            vk::DescriptorSetLayoutBinding& mesh_material_layout_base_color_texture_binding =
                mesh_material_layout_bindings[1];
            mesh_material_layout_base_color_texture_binding.binding         = 1;
            mesh_material_layout_base_color_texture_binding.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            mesh_material_layout_base_color_texture_binding.descriptorCount = 1;
            mesh_material_layout_base_color_texture_binding.stageFlags      = vk::ShaderStageFlagBits::eFragment;
            mesh_material_layout_base_color_texture_binding.pImmutableSamplers = nullptr;

            // (set = 2, binding = 2 in fragment shader)
            vk::DescriptorSetLayoutBinding& mesh_material_layout_metallic_roughness_texture_binding =
                mesh_material_layout_bindings[2];
            mesh_material_layout_metallic_roughness_texture_binding = mesh_material_layout_base_color_texture_binding;
            mesh_material_layout_metallic_roughness_texture_binding.binding = 2;

            // (set = 2, binding = 3 in fragment shader)
            vk::DescriptorSetLayoutBinding& mesh_material_layout_normal_roughness_texture_binding =
                mesh_material_layout_bindings[3];
            mesh_material_layout_normal_roughness_texture_binding = mesh_material_layout_base_color_texture_binding;
            mesh_material_layout_normal_roughness_texture_binding.binding = 3;

            // (set = 2, binding = 4 in fragment shader)
            vk::DescriptorSetLayoutBinding& mesh_material_layout_occlusion_texture_binding =
                mesh_material_layout_bindings[4];
            mesh_material_layout_occlusion_texture_binding         = mesh_material_layout_base_color_texture_binding;
            mesh_material_layout_occlusion_texture_binding.binding = 4;

            // (set = 2, binding = 5 in fragment shader)
            vk::DescriptorSetLayoutBinding& mesh_material_layout_emissive_texture_binding =
                mesh_material_layout_bindings[5];
            mesh_material_layout_emissive_texture_binding         = mesh_material_layout_base_color_texture_binding;
            mesh_material_layout_emissive_texture_binding.binding = 5;


            m_rhi->createDescriptorSetLayout(std::vector(
                                                 mesh_material_layout_bindings.begin(), mesh_material_layout_bindings.end()),
                                             m_descriptor_infos[_mesh_per_material].layout);
        }

    }

    void MainCameraPass::setupPipelines()
    {
        LOG_INFO("");
        m_render_pipelines.resize(_render_pipeline_type_count);
        /// g-buffer-pipeline
        {
            vk::DescriptorSetLayout descriptorset_Layouts[2]={
                m_descriptor_infos[_mesh_global].layout,
                m_descriptor_infos[_mesh_per_material].layout
            };

            // pipeline layout

            vk::PipelineLayoutCreateInfo pipelineLayout;
            pipelineLayout.setLayoutCount         = 1;
            pipelineLayout.pSetLayouts            = descriptorset_Layouts;
            pipelineLayout.pushConstantRangeCount = 0;
            pipelineLayout.pPushConstantRanges    = nullptr;

            m_rhi->createPipelineLayout(&pipelineLayout, m_render_pipelines[0].layout);

            // shaderModule

            auto vert_code_01_triangle = vkutil::readSPVFile("shaders/01_triangle_vert.spv");
            auto frag_code_01_triangle = vkutil::readSPVFile("shaders/01_triangle_frag.spv");

            auto vert_shader_module = m_rhi->createShaderModule(vert_code_01_triangle);
            auto frag_shader_module = m_rhi->createShaderModule(frag_code_01_triangle);

            vk::PipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info;
            vk::PipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info;

            vert_pipeline_shader_stage_create_info.stage  = vk::ShaderStageFlagBits::eVertex;
            vert_pipeline_shader_stage_create_info.pName  = "main";
            vert_pipeline_shader_stage_create_info.module = vert_shader_module;

            frag_pipeline_shader_stage_create_info.stage  = vk::ShaderStageFlagBits::eFragment;
            frag_pipeline_shader_stage_create_info.pName  = "main";
            frag_pipeline_shader_stage_create_info.module = frag_shader_module;

            vk::PipelineShaderStageCreateInfo shader_stages[] = {vert_pipeline_shader_stage_create_info,
                                                                 frag_pipeline_shader_stage_create_info};

            // vertex bindings

            vk::VertexInputAttributeDescription position_attr{};
            position_attr.binding  = 0;
            position_attr.location = 0;
            position_attr.format   = vk::Format::eR32G32B32Sfloat;
            position_attr.offset   = offsetof(MeshVertexData, position);

            vk::VertexInputAttributeDescription normal_attr{};
            normal_attr.binding  = 0;
            normal_attr.location = 1; // shader layout(location = 1)
            normal_attr.format   = vk::Format::eR32G32B32Sfloat;
            normal_attr.offset   = offsetof(MeshVertexData, normal);

            vk::VertexInputAttributeDescription tangent_attr{};
            tangent_attr.binding  = 0;
            tangent_attr.location = 2; // shader layout(location = 2)
            tangent_attr.format   = vk::Format::eR32G32B32Sfloat;
            tangent_attr.offset   = offsetof(MeshVertexData, tangent);

            vk::VertexInputAttributeDescription uv_attr{};
            uv_attr.binding  = 0;
            uv_attr.location = 3; // shader layout(location = 3)
            uv_attr.format   = vk::Format::eR32G32Sfloat;
            uv_attr.offset   = offsetof(MeshVertexData, uv);

            std::array<vk::VertexInputBindingDescription, 1> binding_desc = {vk::VertexInputBindingDescription{
                0,                           // binding
                sizeof(MeshVertexData),      // stride
                vk::VertexInputRate::eVertex // per-vertex
            }};

            std::array<vk::VertexInputAttributeDescription, 4> attribute_desc = {position_attr, normal_attr,
                                                                                 tangent_attr, uv_attr};

            vk::PipelineVertexInputStateCreateInfo vert_state_create_info{};
            vert_state_create_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_desc.size());
            ;
            vert_state_create_info.pVertexBindingDescriptions      = binding_desc.data();
            vert_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_desc.size());
            ;
            vert_state_create_info.pVertexAttributeDescriptions = attribute_desc.data();

            // input assembly
            vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
            input_assembly_create_info.topology               = vk::PrimitiveTopology::eTriangleList;
            input_assembly_create_info.primitiveRestartEnable = vk::False;

            // view port
            vk::PipelineViewportStateCreateInfo view_port_create_info{};
            view_port_create_info.viewportCount = 1;
            view_port_create_info.pViewports    = &(m_rhi->getSwapchainInfo().viewport);
            view_port_create_info.scissorCount  = 1;
            view_port_create_info.pScissors     = &(m_rhi->getSwapchainInfo().scissor);

            // rasterization

            vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info{};
            rasterization_state_create_info.depthClampEnable        = vk::False;
            rasterization_state_create_info.rasterizerDiscardEnable = vk::False;
            rasterization_state_create_info.polygonMode             = vk::PolygonMode::eFill;
            rasterization_state_create_info.lineWidth               = 1.0f;
            rasterization_state_create_info.cullMode                = vk::CullModeFlagBits::eNone;
            rasterization_state_create_info.frontFace               = vk::FrontFace::eCounterClockwise;
            rasterization_state_create_info.depthBiasEnable         = vk::False;
            rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
            rasterization_state_create_info.depthBiasClamp          = 0.0f;
            rasterization_state_create_info.depthBiasSlopeFactor    = 0.0f;

            //  multisample
            vk::PipelineMultisampleStateCreateInfo multisampler_state_create_info{};
            multisampler_state_create_info.sampleShadingEnable  = vk::False;
            multisampler_state_create_info.rasterizationSamples = vk::SampleCountFlagBits::e1;

            // color blend
            vk::PipelineColorBlendAttachmentState color_blend_attachments[1];
            color_blend_attachments[0].colorWriteMask = vk::ColorComponentFlagBits::eR |
                                                        vk::ColorComponentFlagBits::eG |
                                                        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

            color_blend_attachments[0].blendEnable = vk::False;

            vk::PipelineColorBlendStateCreateInfo color_blend_state_info{};
            color_blend_state_info.logicOpEnable = vk::False;
            color_blend_state_info.logicOp       = vk::LogicOp::eCopy;
            color_blend_state_info.setAttachments(color_blend_attachments);
            color_blend_state_info.blendConstants[0] = 0.0f;
            color_blend_state_info.blendConstants[1] = 0.0f;
            color_blend_state_info.blendConstants[2] = 0.0f;
            color_blend_state_info.blendConstants[3] = 0.0f;

            // DepthStencil
            vk::PipelineDepthStencilStateCreateInfo depth_stencil_create_info{};
            depth_stencil_create_info.depthTestEnable       = vk::True;
            depth_stencil_create_info.depthWriteEnable      = vk::True;
            depth_stencil_create_info.depthCompareOp        = vk::CompareOp::eLess;
            depth_stencil_create_info.depthBoundsTestEnable = vk::False;
            depth_stencil_create_info.stencilTestEnable     = vk::False;

            // DynamicState

            std::vector<vk::DynamicState> dynamic_states{vk::DynamicState::eViewport, vk::DynamicState::eScissor}; //
            vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{};
            dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
            dynamic_state_create_info.pDynamicStates    = dynamic_states.data();

            // graphicsPipeline create info
            vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info{};
            graphics_pipeline_create_info.stageCount          = 2;
            graphics_pipeline_create_info.pStages             = shader_stages;
            graphics_pipeline_create_info.pVertexInputState   = &vert_state_create_info;
            graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
            graphics_pipeline_create_info.pViewportState      = &view_port_create_info;
            graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
            graphics_pipeline_create_info.pMultisampleState   = &multisampler_state_create_info;
            graphics_pipeline_create_info.pColorBlendState    = &color_blend_state_info;
            graphics_pipeline_create_info.pDepthStencilState  = &depth_stencil_create_info;
            graphics_pipeline_create_info.layout              = m_render_pipelines[0].layout;
            graphics_pipeline_create_info.renderPass          = m_framebuffer.render_pass;
            graphics_pipeline_create_info.subpass             = 0;
            graphics_pipeline_create_info.pDynamicState       = &dynamic_state_create_info;

            m_rhi->createGraphicsPipeline(graphics_pipeline_create_info, m_render_pipelines[0].pipeline);
        }

        /// deferred lighting - pipeline
        {
            vk::DescriptorSetLayout      descriptorset_layouts[2] = {m_descriptor_infos[_mesh_global].layout,
                                                                m_descriptor_infos[_deferred_lighting].layout};

                vk::PipelineLayoutCreateInfo pipeline_layout_create_info {};
                pipeline_layout_create_info.setLayoutCount =
                    sizeof(descriptorset_layouts) / sizeof(descriptorset_layouts[0]);
                pipeline_layout_create_info.pSetLayouts = descriptorset_layouts;

                if ( m_rhi->createPipelineLayout(&pipeline_layout_create_info,
                                                               m_render_pipelines[_render_pipeline_type_deferred_lighting].layout))
                {
                    throw std::runtime_error("create deferred lighting pipeline layout");
                }

                RHIShader* vert_shader_module = m_rhi->createShaderModule(DEFERRED_LIGHTING_VERT);
                RHIShader* frag_shader_module = m_rhi->createShaderModule(DEFERRED_LIGHTING_FRAG);

                RHIPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info {};
                vert_pipeline_shader_stage_create_info.sType  = RHI_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vert_pipeline_shader_stage_create_info.stage  = RHI_SHADER_STAGE_VERTEX_BIT;
                vert_pipeline_shader_stage_create_info.module = vert_shader_module;
                vert_pipeline_shader_stage_create_info.pName  = "main";
                // vert_pipeline_shader_stage_create_info.pSpecializationInfo

                RHIPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info {};
                frag_pipeline_shader_stage_create_info.sType  = RHI_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                frag_pipeline_shader_stage_create_info.stage  = vk::ShaderStageFlagBits::eFragment;
                frag_pipeline_shader_stage_create_info.module = frag_shader_module;
                frag_pipeline_shader_stage_create_info.pName  = "main";

                RHIPipelineShaderStageCreateInfo shader_stages[] = {vert_pipeline_shader_stage_create_info,
                                                                    frag_pipeline_shader_stage_create_info};

                auto                                 vertex_binding_descriptions   = MeshVertex::getBindingDescriptions();
                auto                                 vertex_attribute_descriptions = MeshVertex::getAttributeDescriptions();
                RHIPipelineVertexInputStateCreateInfo vertex_input_state_create_info {};
                vertex_input_state_create_info.sType = RHI_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
                vertex_input_state_create_info.pVertexBindingDescriptions    = NULL;
                vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
                vertex_input_state_create_info.pVertexAttributeDescriptions  = NULL;

                RHIPipelineInputAssemblyStateCreateInfo input_assembly_create_info {};
                input_assembly_create_info.sType    = RHI_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                input_assembly_create_info.topology = RHI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                input_assembly_create_info.primitiveRestartEnable = RHI_FALSE;

                RHIPipelineViewportStateCreateInfo viewport_state_create_info {};
                viewport_state_create_info.sType         = RHI_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewport_state_create_info.viewportCount = 1;
                viewport_state_create_info.pViewports    = m_rhi->getSwapchainInfo().viewport;
                viewport_state_create_info.scissorCount  = 1;
                viewport_state_create_info.pScissors     = m_rhi->getSwapchainInfo().scissor;

                RHIPipelineRasterizationStateCreateInfo rasterization_state_create_info {};
                rasterization_state_create_info.sType = RHI_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterization_state_create_info.depthClampEnable        = RHI_FALSE;
                rasterization_state_create_info.rasterizerDiscardEnable = RHI_FALSE;
                rasterization_state_create_info.polygonMode             = RHI_POLYGON_MODE_FILL;
                rasterization_state_create_info.lineWidth               = 1.0f;
                rasterization_state_create_info.cullMode                = RHI_CULL_MODE_BACK_BIT;
                rasterization_state_create_info.frontFace               = RHI_FRONT_FACE_CLOCKWISE;
                rasterization_state_create_info.depthBiasEnable         = RHI_FALSE;
                rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
                rasterization_state_create_info.depthBiasClamp          = 0.0f;
                rasterization_state_create_info.depthBiasSlopeFactor    = 0.0f;

                RHIPipelineMultisampleStateCreateInfo multisample_state_create_info {};
                multisample_state_create_info.sType = RHI_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisample_state_create_info.sampleShadingEnable  = RHI_FALSE;
                multisample_state_create_info.rasterizationSamples = RHI_SAMPLE_COUNT_1_BIT;

                RHIPipelineColorBlendAttachmentState color_blend_attachments[1] = {};
                color_blend_attachments[0].colorWriteMask = RHI_COLOR_COMPONENT_R_BIT | RHI_COLOR_COMPONENT_G_BIT |
                                                            RHI_COLOR_COMPONENT_B_BIT | RHI_COLOR_COMPONENT_A_BIT;
                color_blend_attachments[0].blendEnable         = RHI_FALSE;
                color_blend_attachments[0].srcColorBlendFactor = RHI_BLEND_FACTOR_ONE;
                color_blend_attachments[0].dstColorBlendFactor = RHI_BLEND_FACTOR_ONE;
                color_blend_attachments[0].colorBlendOp        = RHI_BLEND_OP_ADD;
                color_blend_attachments[0].srcAlphaBlendFactor = RHI_BLEND_FACTOR_ONE;
                color_blend_attachments[0].dstAlphaBlendFactor = RHI_BLEND_FACTOR_ONE;
                color_blend_attachments[0].alphaBlendOp        = RHI_BLEND_OP_ADD;

                RHIPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
                color_blend_state_create_info.sType         = RHI_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                color_blend_state_create_info.logicOpEnable = RHI_FALSE;
                color_blend_state_create_info.logicOp       = RHI_LOGIC_OP_COPY;
                color_blend_state_create_info.attachmentCount =
                    sizeof(color_blend_attachments) / sizeof(color_blend_attachments[0]);
                color_blend_state_create_info.pAttachments      = &color_blend_attachments[0];
                color_blend_state_create_info.blendConstants[0] = 0.0f;
                color_blend_state_create_info.blendConstants[1] = 0.0f;
                color_blend_state_create_info.blendConstants[2] = 0.0f;
                color_blend_state_create_info.blendConstants[3] = 0.0f;

                RHIPipelineDepthStencilStateCreateInfo depth_stencil_create_info {};
                depth_stencil_create_info.sType            = RHI_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depth_stencil_create_info.depthTestEnable  = RHI_FALSE;
                depth_stencil_create_info.depthWriteEnable = RHI_FALSE;
                depth_stencil_create_info.depthCompareOp   = RHI_COMPARE_OP_ALWAYS;
                depth_stencil_create_info.depthBoundsTestEnable = RHI_FALSE;
                depth_stencil_create_info.stencilTestEnable     = RHI_FALSE;

                RHIDynamicState                   dynamic_states[] = {RHI_DYNAMIC_STATE_VIEWPORT, RHI_DYNAMIC_STATE_SCISSOR};
                RHIPipelineDynamicStateCreateInfo dynamic_state_create_info {};
                dynamic_state_create_info.sType             = RHI_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamic_state_create_info.dynamicStateCount = 2;
                dynamic_state_create_info.pDynamicStates    = dynamic_states;

                RHIGraphicsPipelineCreateInfo pipelineInfo {};
                pipelineInfo.sType               = RHI_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount          = 2;
                pipelineInfo.pStages             = shader_stages;
                pipelineInfo.pVertexInputState   = &vertex_input_state_create_info;
                pipelineInfo.pInputAssemblyState = &input_assembly_create_info;
                pipelineInfo.pViewportState      = &viewport_state_create_info;
                pipelineInfo.pRasterizationState = &rasterization_state_create_info;
                pipelineInfo.pMultisampleState   = &multisample_state_create_info;
                pipelineInfo.pColorBlendState    = &color_blend_state_create_info;
                pipelineInfo.pDepthStencilState  = &depth_stencil_create_info;
                pipelineInfo.layout              = m_render_pipelines[_render_pipeline_type_deferred_lighting].layout;
                pipelineInfo.renderPass          = m_framebuffer.render_pass;
                pipelineInfo.subpass             = _main_camera_subpass_deferred_lighting;
                pipelineInfo.basePipelineHandle  = RHI_NULL_HANDLE;
                pipelineInfo.pDynamicState       = &dynamic_state_create_info;

                if (RHI_SUCCESS != m_rhi->createGraphicsPipelines(RHI_NULL_HANDLE,
                                                                  1,
                                                                  &pipelineInfo,
                                                                  m_render_pipelines[_render_pipeline_type_deferred_lighting].pipeline))
                {
                    throw std::runtime_error("create deferred lighting graphics pipeline");
                }


        }

        /// mesh lighting   forward
        {
            vk::DescriptorSetLayout      descriptorset_layouts[3] = {m_descriptor_infos[_mesh_global].layout,
                                                                m_descriptor_infos[_per_mesh].layout,
                                                                m_descriptor_infos[_mesh_per_material].layout};
        }

        /// sky box  - pipeline

        {
            vk::DescriptorSetLayout      descriptorset_layouts[1] = {m_descriptor_infos[_skybox].layout};

        }

        ///
        {

        }













    }

    void MainCameraPass::setupDescriptorSet()
    {
        LOG_INFO("");
        auto  texture_data = m_render_resource->loadTexture("engine/asset/marry/MC003_Kozakura_Mari.png");
        auto &texture      = m_global_render_resource->_texture_;
        {
            // pix
            vk::Buffer    staging_buffer;
            VmaAllocation vmaAllocation;
            m_rhi->createBufferVMA(texture_data->m_width * texture_data->m_height * 4,
                                   vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY, staging_buffer,
                                   vmaAllocation);
            m_rhi->mapBufferMemory(vmaAllocation, texture_data->m_pixels,
                                   texture_data->m_width * texture_data->m_height * 4);

            vk::ImageCreateInfo image_info{};
            image_info.imageType     = vk::ImageType::e2D;
            image_info.extent.width  = texture_data->m_width;
            image_info.extent.height = texture_data->m_height;
            image_info.extent.depth  = 1;
            image_info.mipLevels     = texture_data->m_mip_levels;
            image_info.arrayLayers   = 1;
            image_info.format        = vk::Format::eR8G8B8A8Srgb;
            image_info.tiling        = vk::ImageTiling::eOptimal;
            image_info.initialLayout = vk::ImageLayout::eUndefined;
            image_info.usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            image_info.sharingMode   = vk::SharingMode::eExclusive;
            image_info.samples       = vk::SampleCountFlagBits::e1;
            image_info.flags         = {};

            m_rhi->createImageVMA(image_info, VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, texture._image_,
                                  texture.vmaAllocation);

            m_rhi->transitionImageLayout(texture._image_, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eTransferDstOptimal, texture_data->m_mip_levels);

            m_rhi->copyBufferToImage(staging_buffer, texture._image_, texture_data->m_width, texture_data->m_height);

            m_rhi->transitionImageLayout(texture._image_, vk::Format::eR8G8B8A8Srgb,
                                         vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
                                         texture_data->m_mip_levels);

            m_rhi->createImageView(texture._image_, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor,
                                   texture._image_view);

            m_rhi->createSampler(texture._sampler_);
        }

        setupMeshDescriptorSet();
    }

    void MainCameraPass::setupMeshDescriptorSet()
    {
        vk::DescriptorSetAllocateInfo dst_set_allocate_info{};
        dst_set_allocate_info.descriptorPool     = m_rhi->getDescriptorPool();
        dst_set_allocate_info.descriptorSetCount = 1;
        dst_set_allocate_info.pSetLayouts        = &m_descriptor_infos[0].layout;

        m_rhi->createDescriptorSet(dst_set_allocate_info, m_descriptor_infos[0].descriptor_set);

        vk::DescriptorBufferInfo color_ubo_info;
        vk::DescriptorBufferInfo shadow_pass_light_info;
        vk::DescriptorImageInfo  texture_sampler_info;
        vk::DescriptorImageInfo  shadow_map_sampler_info{};
        // buffer
        {
            auto &buffer = m_global_render_resource->_uniform_buffer_main_pass;
            m_rhi->createBufferVMA(sizeof(UBO), vk::BufferUsageFlagBits::eUniformBuffer,
                                   VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, buffer._buffer_, buffer.vmaAllocation);
            color_ubo_info.buffer = buffer._buffer_;
            color_ubo_info.offset = 0;
            color_ubo_info.range  = sizeof(UBO);
        }
        {
            auto &buffer = m_global_render_resource->_uniform_buffer_directional_light_pass;

            shadow_pass_light_info.buffer = buffer._buffer_;
            shadow_pass_light_info.offset = 0;
            shadow_pass_light_info.range  = sizeof(UBO);
        }
        // sampler
        {
            auto &texture                    = m_global_render_resource->_texture_;
            texture_sampler_info.sampler     = texture._sampler_;
            texture_sampler_info.imageView   = texture._image_view;
            texture_sampler_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        // shadow map
        {
            vk::SamplerCreateInfo samplerInfo{};
            samplerInfo.magFilter    = vk::Filter::eLinear;
            samplerInfo.minFilter    = vk::Filter::eLinear;
            samplerInfo.mipmapMode   = vk::SamplerMipmapMode::eLinear;
            samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
            samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
            samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
            samplerInfo.borderColor  = vk::BorderColor::eFloatOpaqueWhite;

            samplerInfo.compareEnable = VK_TRUE;                     // ✅ 关键
            samplerInfo.compareOp     = vk::CompareOp::eLessOrEqual; // 使用深度比较
            samplerInfo.mipLodBias    = 0.0f;
            samplerInfo.minLod        = 0.0f;
            samplerInfo.maxLod        = 1.0f;

            m_rhi->createSampler(shadow_map_sampler, samplerInfo);

            shadow_map_sampler_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            shadow_map_sampler_info.imageView   = *m_directional_light_shadow_depth_image_view;
            shadow_map_sampler_info.sampler     = shadow_map_sampler;
        }

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets{
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set, 0, 0, 1, vk::DescriptorType::eUniformBuffer,
                                   nullptr, &color_ubo_info},
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set, 1, 0, 1,
                                   vk::DescriptorType::eCombinedImageSampler, &texture_sampler_info},
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set, 2, 0, 1,
                                   vk::DescriptorType::eCombinedImageSampler, &shadow_map_sampler_info},
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set, 3, 0, 1, vk::DescriptorType::eUniformBuffer,
                                   nullptr, &shadow_pass_light_info}};

        m_rhi->updateDescriptorSets(writeDescriptorSets);
    }
    void MainCameraPass::setupFramebufferDescriptorSet()
    {
        // normal map
        // g-buffer map    descriptorset image info
    }
    void MainCameraPass::setupSwapchainFramebuffers()
    {
        m_swapchain_framebuffers.resize(m_rhi->getSwapchainInfo().imageViews.size());

        for (size_t i = 0; i < m_rhi->getSwapchainInfo().imageViews.size(); i++)
        {
            std::vector<vk::ImageView> attachments_for_image_view = {m_rhi->getSwapchainInfo().imageViews[i],
                                                                     m_rhi->getDepthImageInfo().depth_image_view};

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.renderPass      = m_framebuffer.render_pass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments_for_image_view.size());
            framebufferInfo.pAttachments    = attachments_for_image_view.data();
            framebufferInfo.width           = m_rhi->getSwapchainInfo().extent.width;
            framebufferInfo.height          = m_rhi->getSwapchainInfo().extent.height;
            framebufferInfo.layers          = 1;

            m_rhi->createFramebuffer(framebufferInfo, m_swapchain_framebuffers[i]);
        }
    }
    void MainCameraPass::drawForward(uint32_t current_swapchain_image_index)
    {
        // ubo buffer
        {
            auto &buffer = m_global_render_resource->_uniform_buffer_main_pass;

            auto time = glfwGetTime();
            auto x    = 5 * sin(time * 0.1);
            auto z    = 5 * cos(time * 0.1);

            UBO ubo;

            ubo.model = glm::mat4(1.0f);

            float zNear  = 1.0f;
            float zFar   = 100.0f;
            float fov    = 45;
            float width  = 1600;
            float height = 900;

            // auto ortho = glm::orthoZO(-1.0f,1.0f,-1.0f,1.0f,zNera,zFar);

            auto proj_matrix = glm::perspectiveRH_ZO(glm::radians(fov), width / height, zNear, zFar);

            // Vulkan NDC Y 坐标翻转
            proj_matrix[1][1] *= -1;

            //  glm::vec3 camera_position =glm::vec3 (0.0f,5.0f,5.0f);
            glm::vec3 camera_position = glm::vec3(x, 5.0f, z);

            auto target_position = glm::vec3(0.0f, 2.0f, 0.0f);
            auto world_up        = glm::vec3(0.0f, 1.0f, 0.0f);
            auto view_matrix     = glm::lookAt(camera_position, target_position, world_up);

            ubo.view_proj = proj_matrix * view_matrix;

            m_rhi->mapBufferMemory(buffer.vmaAllocation, &ubo, sizeof(UBO)); //
        }

        vk::RenderPassBeginInfo renderpass_begin_info{};

        vk::Viewport viewport{};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(m_rhi->getSwapchainInfo().extent.width);
        viewport.height   = static_cast<float>(m_rhi->getSwapchainInfo().extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vk::Rect2D scissor;
        scissor.offset         = vk::Offset2D{0, 0};
        scissor.extent         = m_rhi->getSwapchainInfo().extent;
        vk::DeviceSize offsets = {0};

        // Renderpass begin info
        renderpass_begin_info.renderPass        = m_framebuffer.render_pass;
        renderpass_begin_info.framebuffer       = m_swapchain_framebuffers[current_swapchain_image_index];
        renderpass_begin_info.renderArea.offset = vk::Offset2D{0, 0};
        renderpass_begin_info.renderArea.extent = m_rhi->getSwapchainInfo().extent;
        std::array<vk::ClearValue, 2> clearValues;
        clearValues[0].color                  = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
        clearValues[1].depthStencil           = vk::ClearDepthStencilValue{1.0f, 0u}; // <-- ??? [0,1]
        renderpass_begin_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderpass_begin_info.pClearValues    = clearValues.data();

        m_rhi->cmdBeginRenderPass(m_rhi->getCurrentCommandBuffer(), renderpass_begin_info);

        m_rhi->cmdSetViewport(m_rhi->getCurrentCommandBuffer(), viewport);
        m_rhi->cmdSetScissor(m_rhi->getCurrentCommandBuffer(), scissor);
        m_rhi->cmdBindPipeline(m_rhi->getCurrentCommandBuffer(), vk::PipelineBindPoint::eGraphics,
                               m_render_pipelines[0].pipeline);
        m_rhi->cmdBindDescriptorSets(m_rhi->getCurrentCommandBuffer(), vk::PipelineBindPoint::eGraphics,
                                     m_render_pipelines[0].layout, 0, 1, m_descriptor_infos[0].descriptor_set);

        m_rhi->cmdBindVertexBuffers(m_rhi->getCurrentCommandBuffer(),
                                    m_global_render_resource->_vertex_buffer_marry._buffer_, offsets);
        m_rhi->cmdBindIndexBuffer(m_rhi->getCurrentCommandBuffer(),
                                  m_global_render_resource->_indices_buffer_marry._buffer_, vk::IndexType::eUint16);
        m_rhi->cmdDrewIndexed(m_rhi->getCurrentCommandBuffer(),
                              m_global_render_resource->_indices_buffer_marry._indices.size(), 1, 0, 0, 0);

        m_rhi->cmdBindVertexBuffers(m_rhi->getCurrentCommandBuffer(),
                                    m_global_render_resource->_vertex_buffer_floor._buffer_, offsets);
        m_rhi->cmdBindIndexBuffer(m_rhi->getCurrentCommandBuffer(),
                                  m_global_render_resource->_indices_buffer_floor._buffer_, vk::IndexType::eUint16);
        m_rhi->cmdDrewIndexed(m_rhi->getCurrentCommandBuffer(),
                              m_global_render_resource->_indices_buffer_floor._indices.size(), 1, 0, 0, 0);

        m_rhi->cmdEndRenderPass(m_rhi->getCurrentCommandBuffer());
    }

    void MainCameraPass::draw()
    {
        LOG_INFO("");

        uint8_t current_swapchain_image_index = m_rhi->getCurrentFrameIndex();
        LOG_INFO("current_swapchain_image_index {}", current_swapchain_image_index);

        // begin command buffer
        //   m_rhi->prepareBeforePass();
        drawForward(current_swapchain_image_index);
        m_rhi->submitRendering();
    }

    // load mesh and store in buffer
    void MainCameraPass::preparePassData()
    {
        LOG_INFO("");
        // marry
        auto mesh_data_marry = m_render_resource->loadStaticMesh("engine/asset/marry/Marry.obj");
        //        auto mesh_data_marry = m_render_resource->loadStaticMesh("engine/asset/cube/cube.obj");

        auto &vertexBuffer_marry = m_global_render_resource->_vertex_buffer_marry;
        auto &indexBuffer_marry  = m_global_render_resource->_indices_buffer_marry;

        m_rhi->createBufferVMA(sizeof(MeshVertexData) * mesh_data_marry.m_vertex_buffer_data->size(),
                               vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                               vertexBuffer_marry._buffer_, vertexBuffer_marry.vmaAllocation);

        m_rhi->mapBufferMemory(vertexBuffer_marry.vmaAllocation, mesh_data_marry.m_vertex_buffer_data->data(),
                               sizeof(MeshVertexData) * mesh_data_marry.m_vertex_buffer_data->size());

        m_rhi->createBufferVMA(sizeof(uint16_t) * mesh_data_marry.m_index_buffer_data->size(),
                               vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                               indexBuffer_marry._buffer_, indexBuffer_marry.vmaAllocation);
        m_rhi->mapBufferMemory(indexBuffer_marry.vmaAllocation, mesh_data_marry.m_index_buffer_data->data(),
                               sizeof(uint16_t) * mesh_data_marry.m_index_buffer_data->size());

        m_global_render_resource->_indices_buffer_marry._indices = *mesh_data_marry.m_index_buffer_data;
        // floor
        auto mesh_data_floor = m_render_resource->loadStaticMesh("engine/asset/floor/floor.obj");

        auto &vertexBuffer_floor = m_global_render_resource->_vertex_buffer_floor;
        auto &indexBuffer_floor  = m_global_render_resource->_indices_buffer_floor;

        m_rhi->createBufferVMA(sizeof(MeshVertexData) * mesh_data_floor.m_vertex_buffer_data->size(),
                               vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                               vertexBuffer_floor._buffer_, vertexBuffer_floor.vmaAllocation);

        m_rhi->mapBufferMemory(vertexBuffer_floor.vmaAllocation, mesh_data_floor.m_vertex_buffer_data->data(),
                               sizeof(MeshVertexData) * mesh_data_floor.m_vertex_buffer_data->size());

        m_rhi->createBufferVMA(sizeof(uint16_t) * mesh_data_floor.m_index_buffer_data->size(),
                               vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                               indexBuffer_floor._buffer_, indexBuffer_floor.vmaAllocation);
        m_rhi->mapBufferMemory(indexBuffer_floor.vmaAllocation, mesh_data_floor.m_index_buffer_data->data(),
                               sizeof(uint16_t) * mesh_data_floor.m_index_buffer_data->size());

        m_global_render_resource->_indices_buffer_floor._indices = *mesh_data_floor.m_index_buffer_data;
    }

} // namespace Coconut