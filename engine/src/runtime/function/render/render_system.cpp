//
// Created by tanhao on 2025/8/28.
//

#include "runtime/function/render/render_system.h"
#include "runtime/function/render/interface/vulkan/vulkan_rhi.h"

#include "render_pipeline.h"
#include "render_resource.h"
#include "runtime/core/base/macro.h"

namespace Coconut
{

    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {

        LOG_INFO("");

        // render context initialize
        RHIInitInfo rhi_init_info;
        rhi_init_info.window_system = init_info.window_system;

        m_rhi = std::make_shared<VulkanRHI>();
        m_render_pipeline = std::make_shared<RenderPipeline>();
        m_render_resource = std::make_shared<RenderResource>();


        m_rhi->initialize(rhi_init_info);


        m_render_pipeline->m_rhi=m_rhi;

        RenderPipelineInitInfo pipeline_init_info;
        pipeline_init_info.render_resource = m_render_resource;


        m_render_pipeline->initialize(pipeline_init_info);






    }

    void RenderSystem::tick(float delta_time) {


        // prepare render command context
        m_rhi->prepareContext();

        m_render_pipeline->forwardRender();

        // update per-frame buffer
        /// camera  lighting
       // m_render_resource->updatePerFrameBuffer(m_render_scene, m_render_camera);


        //update per-resources

        //update per-scene

        /// prepare pipeline's render passes data
        //m_render_pipeline->preparePassData(m_render_resource);

        //m_render_pipeline->forwardRender(m_rhi, m_render_resource);

    }

    RenderSystem::~RenderSystem() {}
}