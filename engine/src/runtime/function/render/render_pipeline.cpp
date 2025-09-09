//
// Created by tanhao on 2025/9/1.
//

#include "render_pipeline.h"

#include "runtime/core/base/macro.h"
#include "runtime/function/render/pass/main_camera_pass.h"
namespace Coconut
{
    void Coconut::RenderPipeline::initialize(Coconut::RenderPipelineInitInfo init_info)
    {
        m_main_camera_pass = std::make_shared<MainCameraPass>();

        RenderPassCommonInfo pass_common_info;
        pass_common_info.rhi             = m_rhi;
        pass_common_info.render_resource = init_info.render_resource;

        m_main_camera_pass->setCommonInfo(pass_common_info);
        m_main_camera_pass->initialize();
        m_main_camera_pass->preparePassData();







    }
    void Coconut::RenderPipeline::forwardRender()
    {
        LOG_INFO("");

        auto resutl = m_main_camera_pass.get();
        dynamic_cast<MainCameraPass*>(resutl)->draw();

    }
}