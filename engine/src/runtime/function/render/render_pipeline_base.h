//
// Created by tanhao on 2025/8/29.
//

#ifndef COCONUT_RENDER_PIPELINE_BASE_H
#define COCONUT_RENDER_PIPELINE_BASE_H

#include "render_pass_base.h"
#include "runtime/function/render/interface/rhi.h"

namespace Coconut
{

    struct RenderPipelineInitInfo
    {
        bool                                enable_fxaa {false};
        std::shared_ptr<RenderResourceBase> render_resource;
    };

    class RenderPipelineBase
    {
        friend class RenderSystem;

    public:
        virtual ~RenderPipelineBase(){};
        virtual void initialize(RenderPipelineInitInfo init_info)=0;
        virtual void forwardRender(std::shared_ptr<RHI> rhi) ;

   protected:
    std::shared_ptr<RHI> m_rhi;
    std::shared_ptr<RenderPassBase> m_directional_light_pass;
    std::shared_ptr<RenderPassBase> m_main_camera_pass;




    };

} // namespace Coconut

#endif // COCONUT_RENDER_PIPELINE_BASE_H
