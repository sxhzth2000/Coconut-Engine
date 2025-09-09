//
// Created by tanhao on 2025/9/1.
//

#ifndef COCONUT_RENDER_PIPELINE_H
#define COCONUT_RENDER_PIPELINE_H

#include "render_pipeline_base.h"
namespace Coconut
{

    class RenderPipeline : public RenderPipelineBase
    {
    public:
        virtual void initialize(RenderPipelineInitInfo init_info) override final;
        virtual void forwardRender() override final;
    };
}

#endif // COCONUT_RENDER_PIPELINE_H
