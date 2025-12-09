//
// Created by tanhao on 2025/9/1.
//

#include "render_pipeline.h"

#include "runtime/core/base/macro.h"
#include "runtime/function/render/pass/directional_light_pass.h"
#include "runtime/function/render/pass/main_camera_pass.h"
namespace Coconut
{
    void Coconut::RenderPipeline::initialize(Coconut::RenderPipelineInitInfo init_info)
    {
        LOG_INFO("");
        m_main_camera_pass =        std::make_shared<MainCameraPass>();
        m_directional_light_pass=   std::make_shared <DirectionalLightShadowPass>();


        RenderPassCommonInfo pass_common_info;
        pass_common_info.rhi             = m_rhi;
        pass_common_info.render_resource = init_info.render_resource;

        m_directional_light_pass->setCommonInfo(pass_common_info);
        m_main_camera_pass->setCommonInfo(pass_common_info);



        m_directional_light_pass->initialize();
        m_directional_light_pass->postInitialize();

        std::shared_ptr<MainCameraPass> main_camera_pass = std::static_pointer_cast<MainCameraPass>(m_main_camera_pass);
        std::shared_ptr<DirectionalLightShadowPass> shadow_pass = std::static_pointer_cast<DirectionalLightShadowPass>(m_directional_light_pass);

        main_camera_pass->shadow_pass_ubo = &shadow_pass->ubo;
        main_camera_pass->m_directional_light_shadow_depth_image_view =
            &std::static_pointer_cast<RenderPass>(m_directional_light_pass)->m_framebuffer.attachments[0].image_view;

        m_main_camera_pass->initialize();
        m_main_camera_pass->preparePassData();

    }
    void Coconut::RenderPipeline::forwardRender(std::shared_ptr<RHI> rhi)
    {
        LOG_INFO("");
        m_rhi->prepareBeforePass();

        dynamic_cast<DirectionalLightShadowPass*>(m_directional_light_pass.get())->draw();

        auto resutl = m_main_camera_pass.get();
        auto shadowpass =m_directional_light_pass.get();

        dynamic_cast<MainCameraPass*>(resutl)->draw();

    }
}