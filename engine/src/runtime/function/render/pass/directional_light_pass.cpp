#include "directional_light_pass.h"

#include "runtime/function/render/interface/vulkan/vulkan_util.h"

void Coconut::DirectionalLightShadowPass::initialize() {
    LOG_INFO("");
    RenderPass::initialize(nullptr);
    setupAttachments();
    setupRenderPass();
    setupFramebuffer();
    setupDescriptorSetLayout();


}
void Coconut::DirectionalLightShadowPass::setupAttachments() {

    // shadow
    // depth
    m_framebuffer.attachments.resize(1);
    //depth
    m_framebuffer.attachments[0].format = vk::Format::eD32Sfloat;

    vk::ImageCreateInfo image_info{};
    image_info.imageType     = vk::ImageType::e2D;
    image_info.extent.width  = SHADOW_MAP_SIZE;
    image_info.extent.height = SHADOW_MAP_SIZE;
    image_info.extent.depth  = 1;
    image_info.mipLevels     = 1;
    image_info.arrayLayers   = 1;
    image_info.format        = vk::Format::eD32Sfloat;
    image_info.tiling        = vk::ImageTiling::eOptimal;
    image_info.initialLayout = vk::ImageLayout::eUndefined;
    image_info.usage         =
        vk::ImageUsageFlagBits::eDepthStencilAttachment |
        vk::ImageUsageFlagBits::eSampled;
    image_info.sharingMode   = vk::SharingMode::eExclusive;
    image_info.samples       = vk::SampleCountFlagBits::e1;
    image_info.flags         = {};

    m_rhi->createImageVMA(image_info,VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,m_framebuffer.attachments[0].image,
                          m_framebuffer.attachments[0].allocation);

    m_rhi->createImageView(m_framebuffer.attachments[0].image,    vk::Format::eD32Sfloat,vk::ImageAspectFlagBits::eDepth,
                           m_framebuffer.attachments[0].image_view);

}
void Coconut::DirectionalLightShadowPass::setupRenderPass() {
    // --- attachments ---
    vk::AttachmentDescription directional_light_shadow_color_attachment_description{};
    vk::AttachmentReference     directional_light_shadow_color_attachment_reference{};

    vk::AttachmentDescription directional_light_shadow_depth_attachment_description{};
    vk::AttachmentReference   directional_light_shadow_depth_attachment_reference{};

    // Color attachment (如果你不需要 color，可以删除此 attachment)
    directional_light_shadow_color_attachment_description.format  = m_rhi->getSwapchainInfo().image_format;
    directional_light_shadow_color_attachment_description.samples = vk::SampleCountFlagBits::e1;
    directional_light_shadow_color_attachment_description.loadOp  = vk::AttachmentLoadOp::eClear;
    directional_light_shadow_color_attachment_description.storeOp = vk::AttachmentStoreOp::eStore;
    directional_light_shadow_color_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    directional_light_shadow_color_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    directional_light_shadow_color_attachment_description.initialLayout = vk::ImageLayout::eUndefined;
    directional_light_shadow_color_attachment_description.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal; // 如果用于调试展示，或改为 eShaderReadOnlyOptimal / ePresentSrcKHR 视需求调整

    // Depth attachment
    directional_light_shadow_depth_attachment_description.format  = vk::Format::eD32Sfloat;
    directional_light_shadow_depth_attachment_description.samples = vk::SampleCountFlagBits::e1;
    directional_light_shadow_depth_attachment_description.loadOp  = vk::AttachmentLoadOp::eClear;
    // 渲染完后我们通常希望 depth 可被采样 => storeOp 可以使用 eStore 或 eStore（若不需要采样可用 eDontCare）
    directional_light_shadow_depth_attachment_description.storeOp = vk::AttachmentStoreOp::eStore;
    directional_light_shadow_depth_attachment_description.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    directional_light_shadow_depth_attachment_description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    directional_light_shadow_depth_attachment_description.initialLayout = vk::ImageLayout::eUndefined;
    directional_light_shadow_depth_attachment_description.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    // attachment references in the subpass
   // directional_light_shadow_color_attachment_reference.attachment = 0;
   // directional_light_shadow_color_attachment_reference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

    directional_light_shadow_depth_attachment_reference.attachment = 0;
    directional_light_shadow_depth_attachment_reference.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    // --- subpass ---
    vk::SubpassDescription shadow_pass{};
    shadow_pass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    // 如果你不使用 color，设置 colorAttachmentCount = 0 且 pColorAttachments = nullptr
    shadow_pass.colorAttachmentCount = 0;
    shadow_pass.pColorAttachments    = nullptr;
    shadow_pass.pDepthStencilAttachment = &directional_light_shadow_depth_attachment_reference;

    // --- subpass dependencies ---
    // 依赖 1: external -> subpass(0) : 确保外部读（shader read）在进入写 depth 前同步，并处理 layout 转换
    vk::SubpassDependency dependency0{};
    dependency0.srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependency0.dstSubpass      = 0;
    dependency0.srcStageMask    = vk::PipelineStageFlagBits::eFragmentShader;
    dependency0.dstStageMask    = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    dependency0.srcAccessMask   = vk::AccessFlagBits::eShaderRead;
    dependency0.dstAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    dependency0.dependencyFlags = vk::DependencyFlagBits::eByRegion;

    // 依赖 2: subpass(0) -> external : 渲染结束后保证 depth 写入对后续 shader 可见（用于后续采样）
    vk::SubpassDependency dependency1{};
    dependency1.srcSubpass      = 0;
    dependency1.dstSubpass      = VK_SUBPASS_EXTERNAL;
    dependency1.srcStageMask    = vk::PipelineStageFlagBits::eLateFragmentTests;
    dependency1.dstStageMask    = vk::PipelineStageFlagBits::eFragmentShader;
    dependency1.srcAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    dependency1.dstAccessMask   = vk::AccessFlagBits::eShaderRead;
    dependency1.dependencyFlags = vk::DependencyFlagBits::eByRegion;

    // --- render pass create info ---
    std::array<vk::AttachmentDescription, 1> attachments = {
        //directional_light_shadow_color_attachment_description,
        directional_light_shadow_depth_attachment_description
    };

    std::array<vk::SubpassDescription, 1> subpasses = { shadow_pass };
    std::array<vk::SubpassDependency, 2> dependencies = { dependency0, dependency1 };

    vk::RenderPassCreateInfo createInfo{};
    createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    createInfo.pAttachments    = attachments.data();
    createInfo.subpassCount    = static_cast<uint32_t>(subpasses.size());
    createInfo.pSubpasses      = subpasses.data();
    createInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    createInfo.pDependencies   = dependencies.data();

    // 创建 render pass（假设 m_rhi->createRenderPass 接受 vk::RenderPassCreateInfo*）
    m_rhi->createRenderPass(&createInfo, m_framebuffer.render_pass);
}
void Coconut::DirectionalLightShadowPass::setupFramebuffer() {

    vk::ImageView attachments[1]={m_framebuffer.attachments[0].image_view};

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.renderPass=m_framebuffer.render_pass;
    framebufferInfo.attachmentCount =1;
    framebufferInfo.pAttachments =attachments;
    framebufferInfo.width           = SHADOW_MAP_SIZE;
    framebufferInfo.height          = SHADOW_MAP_SIZE;
    framebufferInfo.layers          = 1;
    m_rhi->createFramebuffer(framebufferInfo, m_framebuffer.framebuffer);
}
void Coconut::DirectionalLightShadowPass::setupDescriptorSetLayout() {

    m_descriptor_infos.resize(1);

    {
     //lightViewProj;
    vk::DescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding            = 0;
    uboLayoutBinding.descriptorType     = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount    = 1;
    uboLayoutBinding.stageFlags         = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    uboLayoutBinding.pImmutableSamplers = nullptr;
     //
    std::vector<vk::DescriptorSetLayoutBinding> bindings = {uboLayoutBinding};


    //descriptorSetLayout
    m_rhi->createDescriptorSetLayout(bindings, m_descriptor_infos[0].layout);
    }

}
void Coconut::DirectionalLightShadowPass::postInitialize() {

    LOG_INFO("");

    setupPipelines();
    setupDescriptorSet();
}
void Coconut::DirectionalLightShadowPass::setupPipelines() {

    m_render_pipelines.resize(1);
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

    auto vert_code_01_triangle = vkutil::readSPVFile("shaders/directional_light_shadow_vert.spv");
    auto frag_code_01_triangle = vkutil::readSPVFile("shaders/directional_light_shadow_frag.spv");

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
    // Shadow Pass 的视口和裁剪设置
    vk::Viewport shadowViewport{};
    shadowViewport.x = 0.0f;
    shadowViewport.y = 0.0f;
    shadowViewport.width = SHADOW_MAP_SIZE;
    shadowViewport.height = SHADOW_MAP_SIZE;
    shadowViewport.minDepth = 0.0f;
    shadowViewport.maxDepth = 1.0f;

    vk::Rect2D shadowScissor{};
    shadowScissor.offset = vk::Offset2D{0, 0};
    shadowScissor.extent = vk::Extent2D{SHADOW_MAP_SIZE, SHADOW_MAP_SIZE};


    vk::PipelineViewportStateCreateInfo view_port_create_info{};
    view_port_create_info.viewportCount = 1;
    view_port_create_info.pViewports = &shadowViewport;
    view_port_create_info.scissorCount = 1;
    view_port_create_info.pScissors =  &shadowScissor;

    //rasterization

    vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info{};
    rasterization_state_create_info.depthClampEnable = vk::False;
    rasterization_state_create_info.rasterizerDiscardEnable =vk::False;
    rasterization_state_create_info.polygonMode = vk::PolygonMode::eFill;
    rasterization_state_create_info.lineWidth = 1.0f;
    rasterization_state_create_info.cullMode =vk::CullModeFlagBits::eNone;
    rasterization_state_create_info.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization_state_create_info.depthBiasEnable = vk::True;
    rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_create_info.depthBiasClamp = 0.0f;
    rasterization_state_create_info.depthBiasSlopeFactor =0.0f;

    //  multisample
    vk::PipelineMultisampleStateCreateInfo multisampler_state_create_info{};
    multisampler_state_create_info.sampleShadingEnable = vk::False;
    multisampler_state_create_info.rasterizationSamples =vk::SampleCountFlagBits::e1;

    // color blend


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

    graphics_pipeline_create_info.pDepthStencilState = & depth_stencil_create_info;
    graphics_pipeline_create_info.layout = m_render_pipelines[0].layout;
    graphics_pipeline_create_info.renderPass          = m_framebuffer.render_pass;
    graphics_pipeline_create_info.subpass             = 0;
    graphics_pipeline_create_info.pDynamicState       = &dynamic_state_create_info;

    m_rhi->createGraphicsPipeline(graphics_pipeline_create_info,m_render_pipelines[0].pipeline);
}


void Coconut::DirectionalLightShadowPass::setupDescriptorSet() {

    vk::DescriptorSetAllocateInfo dst_set_allocate_info{};
    dst_set_allocate_info.descriptorPool = m_rhi->getDescriptorPool();
    dst_set_allocate_info.descriptorSetCount = 1;
    dst_set_allocate_info.pSetLayouts = &m_descriptor_infos[0].layout;

    m_rhi->createDescriptorSet(dst_set_allocate_info,m_descriptor_infos[0].descriptor_set);

    vk::DescriptorBufferInfo ubo_info;
    vk::DescriptorImageInfo sampler_info;
    // buffer
    {
        //this buffer is already created in main pass , just use it
        auto &buffer = m_global_render_resource->_uniform_buffer_directional_light_pass;
        m_rhi->createBufferVMA(sizeof(UBO), vk::BufferUsageFlagBits::eUniformBuffer,
                               VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, buffer._buffer_, buffer.vmaAllocation);
        ubo_info.buffer = buffer._buffer_;
        ubo_info.offset = 0;
        ubo_info.range  = sizeof(UBO);
    }



    std::vector< vk::WriteDescriptorSet> writeDescriptorSets{
        vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set,0,0,1,vk::DescriptorType::eUniformBuffer, nullptr,&ubo_info},
  //      vk::WriteDescriptorSet{m_descriptor_infos[0].descriptor_set,1,0,1,vk::DescriptorType::eCombinedImageSampler, &sampler_info}
    };

    m_rhi->updateDescriptorSets(writeDescriptorSets);
}


void Coconut::DirectionalLightShadowPass::draw() {
    LOG_INFO("");

    // ubo buffer for lighting
    {
        auto &buffer = m_global_render_resource->_uniform_buffer_directional_light_pass;

        ubo.model = glm::mat4(1.0f);

        float zNear = 0.01f;
        float zFar =10.0f;

        float orthoSize =10.0f; // 覆盖场景范围

    // 正交投影矩阵 - 使用方形宽高比
    // 得到的深度[0,1]
    //        glm::orthoZO 的输出是：
    //
    //            x, y ∈ [-1, 1]
    //
    //            z ∈ [0, 1]
        auto proj_matrix = glm::orthoZO(-orthoSize, orthoSize, -orthoSize, orthoSize, zNear, zFar);

        // Vulkan NDC Y 坐标翻转
        proj_matrix[1][1] *= -1;

        auto world_up = glm::vec3(0.0f, 1.0f, 0.0f);

        auto target_position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto time= glfwGetTime();


        glm::vec3 lightDir = glm::normalize(glm::vec3(sin(time), -1.0f, cos(time)*2));

      //  glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));

        glm::vec3 lightPos = -lightDir *2.0f + target_position;
        auto view_matrix = glm::lookAtRH(lightPos, target_position, world_up);


        ubo.view_proj = proj_matrix * view_matrix;

        m_rhi->mapBufferMemory(buffer.vmaAllocation, &ubo, sizeof(UBO));
    }

    vk::RenderPassBeginInfo renderpass_begin_info{};

    // === 关键修正：使用阴影贴图尺寸而不是交换链尺寸 ===
    const uint32_t SHADOW_MAP_SIZE = 1024;

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(SHADOW_MAP_SIZE);
    viewport.height = static_cast<float>(SHADOW_MAP_SIZE);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D{0, 0};
    scissor.extent = vk::Extent2D{SHADOW_MAP_SIZE, SHADOW_MAP_SIZE};

    vk::DeviceSize offsets = {0};

    // Renderpass begin info
    renderpass_begin_info.renderPass = m_framebuffer.render_pass;
    renderpass_begin_info.framebuffer = m_framebuffer.framebuffer;

    // === 关键修正：渲染区域使用阴影贴图尺寸 ===
    renderpass_begin_info.renderArea.offset = vk::Offset2D{0, 0};
    renderpass_begin_info.renderArea.extent = vk::Extent2D{SHADOW_MAP_SIZE, SHADOW_MAP_SIZE};

    std::array<vk::ClearValue, 1> clearValues;
    clearValues[0].depthStencil = vk::ClearDepthStencilValue{1.0f, 0u};
    renderpass_begin_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderpass_begin_info.pClearValues = clearValues.data();

    m_rhi->cmdBeginRenderPass(m_rhi->getCurrentCommandBuffer(), renderpass_begin_info);

    m_rhi->cmdSetViewport(m_rhi->getCurrentCommandBuffer(), viewport);
    m_rhi->cmdSetScissor(m_rhi->getCurrentCommandBuffer(), scissor);
    m_rhi->cmdBindPipeline(m_rhi->getCurrentCommandBuffer(), vk::PipelineBindPoint::eGraphics, m_render_pipelines[0].pipeline);
    m_rhi->cmdBindDescriptorSets(m_rhi->getCurrentCommandBuffer(), vk::PipelineBindPoint::eGraphics,
                                 m_render_pipelines[0].layout, 0, 1, m_descriptor_infos[0].descriptor_set);

    m_rhi->cmdBindVertexBuffers(m_rhi->getCurrentCommandBuffer(), m_global_render_resource->_vertex_buffer_marry._buffer_, offsets);
    m_rhi->cmdBindIndexBuffer(m_rhi->getCurrentCommandBuffer(), m_global_render_resource->_indices_buffer_marry._buffer_, vk::IndexType::eUint16);
    m_rhi->cmdDrewIndexed(m_rhi->getCurrentCommandBuffer(), m_global_render_resource->_indices_buffer_marry._indices.size(), 1, 0, 0, 0);

    m_rhi->cmdEndRenderPass(m_rhi->getCurrentCommandBuffer());

    // Layout Transition: write to read
    vk::ImageMemoryBarrier barrier{};
    barrier.oldLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_framebuffer.attachments[0].image;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    m_rhi->getCurrentCommandBuffer().pipelineBarrier(
        vk::PipelineStageFlagBits::eLateFragmentTests,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        nullptr, nullptr,
        barrier);
}


void Coconut::DirectionalLightShadowPass::preparePassData() {}

