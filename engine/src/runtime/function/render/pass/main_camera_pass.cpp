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

    // for now  we dont need to set external attachments, swap chain  and  depth  attachments is set in rhi,


    }
    void Coconut::MainCameraPass::setupRenderPass()
    {

        LOG_INFO("");
         vk::AttachmentDescription colorAttachment;

        vk::AttachmentReference    colorAttachmentRef;


        vk::AttachmentDescription depthAttachment;
        vk::AttachmentReference depthAttachmentRef;




        depthAttachment.format         = vk::Format::eD32Sfloat;
        depthAttachment.samples        = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout  = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;




            colorAttachment.format  = m_rhi->getSwapchainInfo().image_format;
        colorAttachment.samples        = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout  = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;
        colorAttachment.format        =  m_rhi->getSwapchainInfo().image_format;

        LOG_INFO("m_Renderpass_color_attachment_image_format:{}", to_string(colorAttachment.format));


/// framebuffer create info color attachment 与此处的 render pass 的attachment 一致

                depthAttachmentRef.attachment = 1; // 第一个附件
                depthAttachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        colorAttachmentRef.attachment = 0;        // 传给 pipeline 的imageview
        colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;




        // 3. 设置子通道（Subpass）
        vk::SubpassDescription subpasses[2];
        subpasses[0].pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
        subpasses[0].colorAttachmentCount = 1;
        subpasses[0].pColorAttachments    = &colorAttachmentRef;   // 1
        subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;  // 2




        //3.子通道依赖
        vk::SubpassDependency dependency{};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.srcAccessMask = {};
        dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;



        // 4. 填充 RenderPassCreateInfo  2个附件
        std::vector<vk::AttachmentDescription> attachments {colorAttachment,depthAttachment };
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount =  static_cast<uint32_t>(attachments.size());;
        renderPassInfo.pAttachments    = attachments.data();
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = subpasses;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        // 5. 创建 RenderPass
        m_rhi->createRenderPass(&renderPassInfo,m_framebuffer.render_pass);

    }


    void MainCameraPass::initialize() {
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
    void MainCameraPass::setupDescriptorSetLayout() {
        LOG_INFO("");
        //for mesh lighting
        m_descriptor_infos.resize(_render_pipeline_type_count);
        {
            //ubo
            vk::DescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding            = 0;
            uboLayoutBinding.descriptorType     = vk::DescriptorType::eUniformBuffer;
            uboLayoutBinding.descriptorCount    = 1;
            uboLayoutBinding.stageFlags         = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
            uboLayoutBinding.pImmutableSamplers = nullptr;

            // sampler texture
            vk::DescriptorSetLayoutBinding samplerLayoutBinding_texture{};
            samplerLayoutBinding_texture.binding            = 1;
            samplerLayoutBinding_texture.descriptorCount    = 1;
            samplerLayoutBinding_texture.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
            samplerLayoutBinding_texture.pImmutableSamplers = nullptr;
            samplerLayoutBinding_texture.stageFlags         = vk::ShaderStageFlagBits::eFragment;
            // sampler shadow map
            vk::DescriptorSetLayoutBinding samplerLayoutBinding_shadow_map{};
            samplerLayoutBinding_shadow_map.binding            = 2;
            samplerLayoutBinding_shadow_map.descriptorCount    = 1;
            samplerLayoutBinding_shadow_map.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
            samplerLayoutBinding_shadow_map.pImmutableSamplers = nullptr;
            samplerLayoutBinding_shadow_map.stageFlags         = vk::ShaderStageFlagBits::eFragment;
            //
            //ubo_shadow_pass
            vk::DescriptorSetLayoutBinding uboLayoutBinding_shadow{};
            uboLayoutBinding_shadow.binding            = 3;
            uboLayoutBinding_shadow.descriptorType     = vk::DescriptorType::eUniformBuffer;
            uboLayoutBinding_shadow.descriptorCount    = 1;
            uboLayoutBinding_shadow.stageFlags         = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
            uboLayoutBinding_shadow.pImmutableSamplers = nullptr;



           std::vector<vk::DescriptorSetLayoutBinding> bindings = {uboLayoutBinding, samplerLayoutBinding_texture,samplerLayoutBinding_shadow_map,uboLayoutBinding_shadow};

            //std::vector<vk::DescriptorSetLayoutBinding> bindings = {uboLayoutBinding};


            //descriptorSetLayout
            m_rhi->createDescriptorSetLayout(bindings, m_descriptor_infos[0].layout);

        }


    }


    void MainCameraPass::setupPipelines() {

        LOG_INFO("");
     m_render_pipelines.resize(_render_pipeline_type_count);

     vk::DescriptorSetLayout descriptorset_Layouts{
        m_descriptor_infos[0].layout,
     };

    // pipeline layout

    vk::PipelineLayoutCreateInfo pipelineLayout;
    pipelineLayout.setLayoutCount = 1;
    pipelineLayout.pSetLayouts = &descriptorset_Layouts;
    pipelineLayout.pushConstantRangeCount = 0;
    pipelineLayout.pPushConstantRanges = nullptr;

     m_rhi->createPipelineLayout(&pipelineLayout,m_render_pipelines[0].layout) ;


// shaderModule

     auto vert_code_01_triangle = vkutil::readSPVFile("shaders/01_triangle_vert.spv");
     auto frag_code_01_triangle = vkutil::readSPVFile("shaders/01_triangle_frag.spv");

     auto vert_shader_module =m_rhi->createShaderModule(vert_code_01_triangle);
     auto frag_shader_module = m_rhi->createShaderModule(frag_code_01_triangle);

    vk::PipelineShaderStageCreateInfo  vert_pipeline_shader_stage_create_info;
    vk::PipelineShaderStageCreateInfo  frag_pipeline_shader_stage_create_info;


    vert_pipeline_shader_stage_create_info.stage=vk::ShaderStageFlagBits::eVertex;
    vert_pipeline_shader_stage_create_info.pName = "main";
    vert_pipeline_shader_stage_create_info.module = vert_shader_module;

    frag_pipeline_shader_stage_create_info.stage=vk::ShaderStageFlagBits::eFragment;
    frag_pipeline_shader_stage_create_info.pName = "main";
    frag_pipeline_shader_stage_create_info.module = frag_shader_module;

    vk::PipelineShaderStageCreateInfo  shader_stages[]={
        vert_pipeline_shader_stage_create_info,
        frag_pipeline_shader_stage_create_info
    };

//vertex bindings

    vk::VertexInputAttributeDescription position_attr{};
    position_attr.binding = 0;
    position_attr.location =0 ;
    position_attr.format = vk::Format::eR32G32B32Sfloat;
    position_attr.offset = offsetof(MeshVertexData,position);

    vk::VertexInputAttributeDescription normal_attr{};
    normal_attr.binding = 0;
    normal_attr.location = 1; // shader layout(location = 1)
    normal_attr.format = vk::Format::eR32G32B32Sfloat;
    normal_attr.offset = offsetof(MeshVertexData, normal);

    vk::VertexInputAttributeDescription tangent_attr{};
    tangent_attr.binding = 0;
    tangent_attr.location = 2; // shader layout(location = 2)
    tangent_attr.format = vk::Format::eR32G32B32Sfloat;
    tangent_attr.offset = offsetof(MeshVertexData, tangent);

    vk::VertexInputAttributeDescription uv_attr{};
    uv_attr.binding = 0;
    uv_attr.location = 3; // shader layout(location = 3)
    uv_attr.format = vk::Format::eR32G32Sfloat;
    uv_attr.offset = offsetof(MeshVertexData, uv);

    std::array<vk::VertexInputBindingDescription, 1> binding_desc = {
        vk::VertexInputBindingDescription{
            0,                                      // binding
            sizeof(MeshVertexData),                 // stride
            vk::VertexInputRate::eVertex            // per-vertex
        }
    };

    std::array<vk::VertexInputAttributeDescription, 4> attribute_desc = {
        position_attr,
        normal_attr,
        tangent_attr,
        uv_attr
    };

    vk::PipelineVertexInputStateCreateInfo vert_state_create_info{};
    vert_state_create_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_desc.size());;
    vert_state_create_info.pVertexBindingDescriptions = binding_desc.data();
    vert_state_create_info.vertexAttributeDescriptionCount =static_cast<uint32_t>(attribute_desc.size());;
    vert_state_create_info.pVertexAttributeDescriptions =  attribute_desc.data();

//input assembly
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
    input_assembly_create_info.topology = vk::PrimitiveTopology::eTriangleList;
    input_assembly_create_info.primitiveRestartEnable = vk::False;

//view port
    vk::PipelineViewportStateCreateInfo view_port_create_info{};
    view_port_create_info.viewportCount = 1;
    view_port_create_info.pViewports = &(m_rhi->getSwapchainInfo().viewport);
    view_port_create_info.scissorCount = 1;
    view_port_create_info.pScissors =   &(m_rhi->getSwapchainInfo().scissor);

//rasterization

    vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info{};
    rasterization_state_create_info.depthClampEnable = vk::False;
    rasterization_state_create_info.rasterizerDiscardEnable =vk::False;
    rasterization_state_create_info.polygonMode = vk::PolygonMode::eFill;
    rasterization_state_create_info.lineWidth = 1.0f;
    rasterization_state_create_info.cullMode =vk::CullModeFlagBits::eNone;
    rasterization_state_create_info.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization_state_create_info.depthBiasEnable = vk::False;
    rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_create_info.depthBiasClamp = 0.0f;
    rasterization_state_create_info.depthBiasSlopeFactor =0.0f;

//  multisample
    vk::PipelineMultisampleStateCreateInfo multisampler_state_create_info{};
    multisampler_state_create_info.sampleShadingEnable = vk::False;
    multisampler_state_create_info.rasterizationSamples =vk::SampleCountFlagBits::e1;

// color blend
    vk::PipelineColorBlendAttachmentState color_blend_attachments[1];
    color_blend_attachments[0].colorWriteMask=
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;

    color_blend_attachments[0].blendEnable = vk::False;

    vk::PipelineColorBlendStateCreateInfo color_blend_state_info{};
    color_blend_state_info.logicOpEnable = vk::False;
    color_blend_state_info.logicOp =  vk::LogicOp::eCopy;
    color_blend_state_info.setAttachments(color_blend_attachments);
    color_blend_state_info.blendConstants[0] = 0.0f;
    color_blend_state_info.blendConstants[1] = 0.0f;
    color_blend_state_info.blendConstants[2] = 0.0f;
    color_blend_state_info.blendConstants[3] = 0.0f;

//DepthStencil
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_create_info{};
    depth_stencil_create_info.depthTestEnable  = vk::True;
    depth_stencil_create_info.depthWriteEnable = vk::True;
    depth_stencil_create_info.depthCompareOp   = vk::CompareOp::eLess;
    depth_stencil_create_info.depthBoundsTestEnable = vk::False;
    depth_stencil_create_info.stencilTestEnable     = vk::False;

//DynamicState

    std::vector<vk::DynamicState> dynamic_states{vk::DynamicState::eViewport, vk::DynamicState::eScissor}; //
    vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{};
    dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
    dynamic_state_create_info.pDynamicStates = dynamic_states.data();


//graphicsPipeline create info
    vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info{};
    graphics_pipeline_create_info.stageCount =2;
    graphics_pipeline_create_info.pStages   =  shader_stages;
    graphics_pipeline_create_info.pVertexInputState =&vert_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = & input_assembly_create_info;
    graphics_pipeline_create_info.pViewportState =& view_port_create_info;
    graphics_pipeline_create_info.pRasterizationState = & rasterization_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &multisampler_state_create_info;
    graphics_pipeline_create_info.pColorBlendState = & color_blend_state_info;
    graphics_pipeline_create_info.pDepthStencilState = & depth_stencil_create_info;
    graphics_pipeline_create_info.layout = m_render_pipelines[0].layout;
   graphics_pipeline_create_info.renderPass          = m_framebuffer.render_pass;
   graphics_pipeline_create_info.subpass             = 0;
   graphics_pipeline_create_info.pDynamicState       = &dynamic_state_create_info;


   m_rhi->createGraphicsPipeline(graphics_pipeline_create_info,m_render_pipelines[0].pipeline);

    }


    void MainCameraPass::setupDescriptorSet() {
        LOG_INFO("");
        auto texture_data =  m_render_resource->loadTexture("engine/asset/marry/MC003_Kozakura_Mari.png");
        auto &texture = m_global_render_resource->_texture_;
        {
            // pix
            vk::Buffer staging_buffer;
            VmaAllocation vmaAllocation;
            m_rhi->createBufferVMA(texture_data->m_width*texture_data->m_height*4,vk::BufferUsageFlagBits::eTransferSrc,
                                   VMA_MEMORY_USAGE_CPU_ONLY,staging_buffer,vmaAllocation);
            m_rhi->mapBufferMemory(vmaAllocation,texture_data->m_pixels,texture_data->m_width*texture_data->m_height*4);


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
            image_info.usage         =
                vk::ImageUsageFlagBits::eTransferDst |
                vk::ImageUsageFlagBits::eSampled;
            image_info.sharingMode   = vk::SharingMode::eExclusive;
            image_info.samples       = vk::SampleCountFlagBits::e1;
            image_info.flags         = {};

            m_rhi->createImageVMA(image_info,VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,texture._image_,texture.vmaAllocation);

            m_rhi->transitionImageLayout(texture._image_,vk::Format::eR8G8B8A8Srgb,vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eTransferDstOptimal,texture_data->m_mip_levels);


            m_rhi->copyBufferToImage(staging_buffer,texture._image_,texture_data->m_width,texture_data->m_height);

            m_rhi->transitionImageLayout(texture._image_, vk::Format::eR8G8B8A8Srgb,
                                         vk::ImageLayout::eTransferDstOptimal,
                                         vk::ImageLayout::eShaderReadOnlyOptimal,
                                         texture_data->m_mip_levels);

            m_rhi->createImageView(texture._image_,vk::Format::eR8G8B8A8Srgb,
                                   vk::ImageAspectFlagBits::eColor,texture._image_view);

            m_rhi->createSampler(texture._sampler_);

        }





     setupMeshDescriptorSet();


    }
    void MainCameraPass::setupMeshDescriptorSet() {

        vk::DescriptorSetAllocateInfo dst_set_allocate_info{};
        dst_set_allocate_info.descriptorPool = m_rhi->getDescriptorPool();
        dst_set_allocate_info.descriptorSetCount = 1;
        dst_set_allocate_info.pSetLayouts = &m_descriptor_infos[0].layout;

        m_rhi->createDescriptorSet(dst_set_allocate_info,m_descriptor_infos[0].descriptor_set);

        vk::DescriptorBufferInfo color_ubo_info;
        vk::DescriptorBufferInfo shadow_pass_light_info;
        vk::DescriptorImageInfo  texture_sampler_info;
        vk::DescriptorImageInfo shadow_map_sampler_info{};
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
           auto &texture= m_global_render_resource->_texture_;
           texture_sampler_info.sampler=texture._sampler_;
           texture_sampler_info.imageView=texture._image_view;
           texture_sampler_info.imageLayout=vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        // shadow map
        {
            vk::SamplerCreateInfo samplerInfo{};
            samplerInfo.magFilter = vk::Filter::eLinear;
            samplerInfo.minFilter = vk::Filter::eLinear;
            samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
            samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
            samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
            samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
            samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;

            samplerInfo.compareEnable = VK_TRUE;                       // ✅ 关键
            samplerInfo.compareOp = vk::CompareOp::eLessOrEqual;       // 使用深度比较
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 1.0f;


            m_rhi->createSampler(shadow_map_sampler,samplerInfo);


            shadow_map_sampler_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            shadow_map_sampler_info.imageView   = *m_directional_light_shadow_depth_image_view;
            shadow_map_sampler_info.sampler     = shadow_map_sampler;

        }




        std::vector< vk::WriteDescriptorSet> writeDescriptorSets{
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set,0,0,1,vk::DescriptorType::eUniformBuffer, nullptr,&color_ubo_info },
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set,1,0,1,vk::DescriptorType::eCombinedImageSampler, &texture_sampler_info},
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set,2,0,1,vk::DescriptorType::eCombinedImageSampler, &shadow_map_sampler_info},
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set,3,0,1,vk::DescriptorType::eUniformBuffer,nullptr,&shadow_pass_light_info}
        };



        m_rhi->updateDescriptorSets(writeDescriptorSets);

    }
    void MainCameraPass::setupFramebufferDescriptorSet() {
        // normal map
        // g-buffer map    descriptorset image info

    }
    void MainCameraPass::setupSwapchainFramebuffers() {
        m_swapchain_framebuffers.resize(m_rhi->getSwapchainInfo().imageViews.size());


        for (size_t i =0 ; i<m_rhi->getSwapchainInfo().imageViews.size();i++)
        {
            std::vector<vk::ImageView>  attachments_for_image_view = {
                m_rhi->getSwapchainInfo().imageViews[i],
                m_rhi->getDepthImageInfo().depth_image_view
            };

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.renderPass      = m_framebuffer.render_pass;
            framebufferInfo.attachmentCount =static_cast<uint32_t> (attachments_for_image_view.size());
            framebufferInfo.pAttachments    = attachments_for_image_view.data();
            framebufferInfo.width           = m_rhi->getSwapchainInfo().extent.width;
            framebufferInfo.height          = m_rhi->getSwapchainInfo().extent.height;
            framebufferInfo.layers          = 1;

             m_rhi->createFramebuffer(framebufferInfo, m_swapchain_framebuffers[i]);
        }

    }
    void MainCameraPass::drawForward(uint32_t current_swapchain_image_index) {

        // ubo buffer
        {
            auto &buffer = m_global_render_resource->_uniform_buffer_main_pass;

            auto time=glfwGetTime();
            auto x = 5*sin(time*0.1);
            auto z = 5*cos(time*0.1);

            UBO ubo;

            ubo.model= glm::mat4 (1.0f);

            float zNear= 1.0f;
            float zFar = 100.0f;
            float fov = 45 ;
            float width =1600;
            float height=900;

            //auto ortho = glm::orthoZO(-1.0f,1.0f,-1.0f,1.0f,zNera,zFar);

            auto proj_matrix = glm::perspectiveRH_ZO(glm::radians(fov),width/height,zNear,zFar);

            // Vulkan NDC Y 坐标翻转
            proj_matrix[1][1] *= -1;


          //  glm::vec3 camera_position =glm::vec3 (0.0f,5.0f,5.0f);
            glm::vec3 camera_position =glm::vec3 (x,5.0f,z);

            auto target_position = glm::vec3 (0.0f,2.0f,0.0f);
            auto world_up = glm::vec3 (0.0f,1.0f,0.0f);
            auto view_matrix = glm::lookAt(camera_position,target_position,world_up);

            ubo.view_proj=proj_matrix*view_matrix;


            m_rhi->mapBufferMemory(buffer.vmaAllocation,&ubo,sizeof(UBO)); //
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
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = m_rhi->getSwapchainInfo().extent;
        vk::DeviceSize offsets = {0};

        //Renderpass begin info
        renderpass_begin_info.renderPass = m_framebuffer.render_pass;
        renderpass_begin_info.framebuffer = m_swapchain_framebuffers[current_swapchain_image_index];
        renderpass_begin_info.renderArea.offset =vk::Offset2D {0,0};
        renderpass_begin_info.renderArea.extent = m_rhi->getSwapchainInfo().extent;
        std::array<vk::ClearValue, 2> clearValues;
        clearValues[0].color = vk::ClearColorValue(std::array<float,4>{0.0f, 0.0f, 0.0f, 1.0f});
        clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0u}; // <-- ??? [0,1]
        renderpass_begin_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderpass_begin_info.pClearValues =  clearValues.data();


        m_rhi->cmdBeginRenderPass(m_rhi->getCurrentCommandBuffer(),renderpass_begin_info);

        m_rhi->cmdSetViewport(m_rhi->getCurrentCommandBuffer(),viewport);
        m_rhi->cmdSetScissor(m_rhi->getCurrentCommandBuffer(),scissor);
        m_rhi->cmdBindPipeline(m_rhi->getCurrentCommandBuffer(),vk::PipelineBindPoint::eGraphics,m_render_pipelines[0].pipeline);
        m_rhi->cmdBindDescriptorSets(m_rhi->getCurrentCommandBuffer(),vk::PipelineBindPoint::eGraphics,
                                 m_render_pipelines[0].layout,0,1,m_descriptor_infos[0].descriptor_set);








        m_rhi->cmdBindVertexBuffers(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_vertex_buffer_marry._buffer_,offsets);
        m_rhi->cmdBindIndexBuffer(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_indices_buffer_marry._buffer_,vk::IndexType::eUint16);
        m_rhi->cmdDrewIndexed(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_indices_buffer_marry._indices.size(),1,0,0,0);

        m_rhi->cmdBindVertexBuffers(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_vertex_buffer_floor._buffer_,offsets);
        m_rhi->cmdBindIndexBuffer(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_indices_buffer_floor._buffer_,vk::IndexType::eUint16);
        m_rhi->cmdDrewIndexed(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_indices_buffer_floor._indices.size(),1,0,0,0);


        m_rhi->cmdEndRenderPass(m_rhi->getCurrentCommandBuffer());


    }




    void MainCameraPass::draw() {
        LOG_INFO("");


        uint8_t current_swapchain_image_index=m_rhi->getCurrentFrameIndex();
        LOG_INFO("current_swapchain_image_index {}",current_swapchain_image_index);

        // begin command buffer
      //   m_rhi->prepareBeforePass();
        drawForward(current_swapchain_image_index);
        m_rhi->submitRendering();

    }


    // load mesh and store in buffer
    void MainCameraPass::preparePassData() {
        LOG_INFO("");
//marry
        auto mesh_data_marry = m_render_resource->loadStaticMesh("engine/asset/marry/Marry.obj");
//        auto mesh_data_marry = m_render_resource->loadStaticMesh("engine/asset/cube/cube.obj");


        auto &vertexBuffer_marry=m_global_render_resource->_vertex_buffer_marry;
        auto &indexBuffer_marry = m_global_render_resource->_indices_buffer_marry;

        m_rhi->createBufferVMA(sizeof(MeshVertexData) * mesh_data_marry.m_vertex_buffer_data->size(), vk::BufferUsageFlagBits::eVertexBuffer,
                               VMA_MEMORY_USAGE_CPU_TO_GPU,vertexBuffer_marry._buffer_,vertexBuffer_marry.vmaAllocation );

        m_rhi->mapBufferMemory(vertexBuffer_marry.vmaAllocation, mesh_data_marry.m_vertex_buffer_data->data(),sizeof(MeshVertexData) * mesh_data_marry.m_vertex_buffer_data->size());

        m_rhi->createBufferVMA(sizeof(uint16_t) * mesh_data_marry.m_index_buffer_data->size(),vk::BufferUsageFlagBits::eIndexBuffer,
                               VMA_MEMORY_USAGE_CPU_TO_GPU,
                               indexBuffer_marry._buffer_, indexBuffer_marry.vmaAllocation);
        m_rhi->mapBufferMemory(indexBuffer_marry.vmaAllocation, mesh_data_marry.m_index_buffer_data->data(),
                               sizeof(uint16_t) * mesh_data_marry.m_index_buffer_data->size());

            m_global_render_resource->_indices_buffer_marry._indices= *mesh_data_marry.m_index_buffer_data;
//floor
            auto mesh_data_floor    = m_render_resource->loadStaticMesh("engine/asset/floor/floor.obj");

            auto &vertexBuffer_floor =m_global_render_resource->_vertex_buffer_floor;
            auto &indexBuffer_floor  = m_global_render_resource->_indices_buffer_floor;

            m_rhi->createBufferVMA(sizeof(MeshVertexData) * mesh_data_floor.m_vertex_buffer_data->size(), vk::BufferUsageFlagBits::eVertexBuffer,
                                   VMA_MEMORY_USAGE_CPU_TO_GPU,vertexBuffer_floor._buffer_,vertexBuffer_floor.vmaAllocation );

            m_rhi->mapBufferMemory(vertexBuffer_floor.vmaAllocation, mesh_data_floor.m_vertex_buffer_data->data(),sizeof(MeshVertexData) * mesh_data_floor.m_vertex_buffer_data->size());

            m_rhi->createBufferVMA(sizeof(uint16_t) * mesh_data_floor.m_index_buffer_data->size(),vk::BufferUsageFlagBits::eIndexBuffer,
                                   VMA_MEMORY_USAGE_CPU_TO_GPU,
                                   indexBuffer_floor._buffer_, indexBuffer_floor.vmaAllocation);
            m_rhi->mapBufferMemory(indexBuffer_floor.vmaAllocation, mesh_data_floor.m_index_buffer_data->data(),
                                   sizeof(uint16_t) * mesh_data_floor.m_index_buffer_data->size());

            m_global_render_resource->_indices_buffer_floor._indices= *mesh_data_floor.m_index_buffer_data;






    }

}