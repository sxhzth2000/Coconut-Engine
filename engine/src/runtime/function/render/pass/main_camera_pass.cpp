//
// Created by tanhao on 2025/8/29.
//
#include "main_camera_pass.h"

#include "runtime/core/base/macro.h"

#include "runtime/function/render/interface/vulkan/vulkan_util.h"

#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_common.h"
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
        std::vector<vk::AttachmentDescription> attachments {colorAttachment,depthAttachment };
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount =  static_cast<uint32_t>(attachments.size());;
        renderPassInfo.pAttachments    = attachments.data();
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subpass;
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
            setupFramebufferDescriptorSet();
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

            // sampler
            vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding            = 1;
            samplerLayoutBinding.descriptorCount    = 2;
            samplerLayoutBinding.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags         = vk::ShaderStageFlagBits::eFragment;

           // std::vector<vk::DescriptorSetLayoutBinding> bindings = {uboLayoutBinding, samplerLayoutBinding};

            std::vector<vk::DescriptorSetLayoutBinding> bindings = {uboLayoutBinding};


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
    vk::VertexInputBindingDescription vert_binding_description{};
    vert_binding_description.binding=0;
    vert_binding_description.stride = sizeof(Vertex);
    vert_binding_description.inputRate = vk::VertexInputRate::eVertex;

    vk::VertexInputAttributeDescription vert_attribute_description{};
    vert_attribute_description.binding = 0;
    vert_attribute_description.location =0 ;
    vert_attribute_description.format = vk::Format::eR32G32B32Sfloat;
    vert_attribute_description.offset = offsetof(Vertex,position);

    vk::PipelineVertexInputStateCreateInfo vert_state_create_info{};

    vert_state_create_info.vertexBindingDescriptionCount = 1;
    vert_state_create_info.pVertexBindingDescriptions = &vert_binding_description;
    vert_state_create_info.vertexAttributeDescriptionCount =1;
    vert_state_create_info.pVertexAttributeDescriptions = & vert_attribute_description;

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

     setupMeshDescriptorSet();


    }
    void MainCameraPass::setupMeshDescriptorSet() {

        vk::DescriptorSetAllocateInfo dst_set_allocate_info{};
        dst_set_allocate_info.descriptorPool = m_rhi->getDescriptorPool();
        dst_set_allocate_info.descriptorSetCount = 1;
        dst_set_allocate_info.pSetLayouts = &m_descriptor_infos[0].layout;

        m_rhi->createDescriptorSet(dst_set_allocate_info,m_descriptor_infos[0].descriptor_set);

        vk::DescriptorBufferInfo color_ubo_info;

        // buffer
        {
            auto &buffer = m_global_render_resource->_uniform_buffer;
            m_rhi->createBufferVMA(sizeof(Color), vk::BufferUsageFlagBits::eUniformBuffer,
                                   VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, buffer._buffer_, buffer.vmaAllocation);
            color_ubo_info.buffer = buffer._buffer_;
            color_ubo_info.offset = 0;
            color_ubo_info.range  = sizeof(Color);
        }



        std::vector< vk::WriteDescriptorSet> writeDescriptorSets{
            vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set,0,0,1,vk::DescriptorType::eUniformBuffer, nullptr,&color_ubo_info }
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




                // buffer
                {
                    auto &buffer = m_global_render_resource->_uniform_buffer;

                    auto time=glfwGetTime();
                    auto pi  = 3.1415926;
                    Color color{ glm::vec3(sin(time+pi*0.5), sin(time+pi),sin(time+pi*1.5)) };

                    m_rhi->mapBufferMemory(buffer.vmaAllocation,&(color),sizeof(Color)); //
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
        m_rhi->cmdDescriptorSets(m_rhi->getCurrentCommandBuffer(),vk::PipelineBindPoint::eGraphics,
                                 m_render_pipelines[0].layout,0,1,m_descriptor_infos[0].descriptor_set);




        m_rhi->cmdBindVertexBuffers(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_vertex_buffer._buffer_,offsets);
        m_rhi->cmdBindIndexBuffer(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_indices_buffer._buffer_,vk::IndexType::eUint16);
        m_rhi->cmdDrewIndexed(m_rhi->getCurrentCommandBuffer(),m_global_render_resource->_indices_buffer._indices.size(),1,0,0,0);

        m_rhi->cmdEndRenderPass(m_rhi->getCurrentCommandBuffer());


    }




    void MainCameraPass::draw() {
        LOG_INFO("");


        uint8_t current_swapchain_image_index=m_rhi->getCurrentFrameIndex();
        LOG_INFO("current_swapchain_image_index {}",current_swapchain_image_index);


        m_rhi->prepareBeforePass();
        drawForward(current_swapchain_image_index);
        m_rhi->submitRendering();

    }

    void MainCameraPass::preparePassData() {
        LOG_INFO("");


        // 硬编码的三角形顶点数据
        const std::vector<Vertex> vertices = {
            {{ 0.0f,  0.5f, 0.0f}}, // 顶点 0: 顶部中间
            {{-0.5f, -0.5f, 0.0f}}, // 顶点 1: 左下角
            {{ 0.5f, -0.5f, 0.0f}}  // 顶点 2: 右下角
        };

        auto &vertexBuffer=m_global_render_resource->_vertex_buffer;
        m_rhi->createBufferVMA(sizeof(Vertex) * vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer,
                               VMA_MEMORY_USAGE_CPU_TO_GPU,vertexBuffer._buffer_,vertexBuffer.vmaAllocation );

        m_rhi->mapBufferMemory(vertexBuffer.vmaAllocation,vertices.data(),sizeof(Vertex) * vertices.size());


        const std::vector<uint16_t> indices = {
            0, 1, 2 // 组成一个三角形
        };


        auto &indexBuffer = m_global_render_resource->_indices_buffer;
        m_rhi->createBufferVMA(sizeof(uint16_t) * indices.size(),vk::BufferUsageFlagBits::eIndexBuffer,
                               VMA_MEMORY_USAGE_CPU_TO_GPU,indexBuffer._buffer_,indexBuffer.vmaAllocation);
        m_rhi->mapBufferMemory(indexBuffer.vmaAllocation,indices.data(),sizeof(uint16_t) * indices.size());

        m_global_render_resource->_indices_buffer._indices=indices;



    }

}