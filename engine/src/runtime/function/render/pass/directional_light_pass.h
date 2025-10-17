#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "runtime/function/render/render_pass.h"

namespace Coconut
{

    class DirectionalLightShadowPass : public RenderPass
    {
       public:
        void initialize() override final;
        void postInitialize() override final;
        virtual void preparePassData() override;

       public:
        virtual void draw() override;

        UBO ubo;

        const uint32_t SHADOW_MAP_SIZE = 1024;
       private:
        void setupAttachments();
        void setupRenderPass();
        void setupFramebuffer();
        void setupDescriptorSetLayout();

        void setupPipelines();
        void setupDescriptorSet();




    };

}

