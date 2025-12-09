

#pragma once
#include "runtime/function/render/render_pass.h"



namespace Coconut
{
//main_pass_attachment
    enum {
        _color_attachment_index  = 0,
        _depth_attachment_index  = 1,
        _color_attachment_count_ = 1,
        _depth_attachment_count_ = 1
    };

// subpass
    // 1. model
    // 2. sky box
    // 3. axis
    // 4. billboard type particle
    enum RenderPipeLineType : uint8_t
    {
        _render_pipeline_type_mesh_gbuffer = 0,
        _render_pipeline_type_deferred_lighting,
//        _render_pipeline_type_mesh_lighting,
//        _render_pipeline_type_skybox,
//        _render_pipeline_type_axis,
//        _render_pipeline_type_particle,
        _render_pipeline_type_count
    };




    // 1: per mesh layout
    // 2: global layout
    // 3: mesh per material layout
    // 4: sky box layout
    // 5: axis layout
    // 6: billboard type particle layout
    // 7: gbuffer lighting
    enum LayoutType : uint8_t
    {
//        _per_mesh = 0,
        _mesh_global,
        _mesh_per_material,
//        _skybox,
//        _axis,
//        _particle,
        _deferred_lighting,
        _layout_type_count
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
        ///   set  frame buffer attachments
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

        bool                                         m_enable_fxaa{ false };

       private:
        std::vector<vk::Framebuffer> m_swapchain_framebuffers;
    };
} // namespace Coconut