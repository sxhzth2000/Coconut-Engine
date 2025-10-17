

#pragma once
#include "runtime/function/render/render_pass.h"



namespace Coconut
{

    enum {
        _color_attachment_index  = 0,
        _depth_attachment_index  = 1,
        _color_attachment_count_ = 1,
        _depth_attachment_count_ = 1
    };

    enum RenderPipeLineType : uint8_t { _render_pipeline_type_mesh_lighting = 0, _render_pipeline_type_count };
    // 1: per mesh layout
    // 2: global layout
    // 3: mesh per material layout
    // 4: sky box layout
    // 5: axis layout
    // 6: billboard type particle layout
    // 7: gbuffer lighting
    enum LayoutType : uint8_t {
        _per_mesh = 0,

    };

    class MainCameraPass : public RenderPass
    {
       public:
        // use in RenderPipeline init
        void         initialize();
        virtual void preparePassData() override;

       public:
        virtual void draw() override;
        void         drawForward(uint32_t current_swapchain_image_index);



       private:
        ///   set  frame buffer attachments, now is empty
        void setupAttachments();

        /// initialize
        void setupRenderPass();
        void setupDescriptorSetLayout();
        void setupPipelines();
        void setupDescriptorSet();
        void setupFramebufferDescriptorSet();
        void setupSwapchainFramebuffers();

        //
        void setupMeshDescriptorSet();
        // tick

        // draw
        void drawMeshTest();
       public:

        vk::ImageView* m_directional_light_shadow_depth_image_view;
        vk::Sampler shadow_map_sampler;

        UBO* shadow_pass_ubo;

       private:
        std::vector<vk::Framebuffer> m_swapchain_framebuffers;
    };
} // namespace Coconut