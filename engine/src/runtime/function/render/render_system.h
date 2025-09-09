//
// Created by tanhao on 2025/8/28.
//

#ifndef COCONUT_RENDER_SYSTEM_H
#define COCONUT_RENDER_SYSTEM_H

#include <memory>

namespace Coconut
{
    class RHI;
    class VulkanRHI;
    class WindowSystem;
    class RenderPipelineBase;
    class RenderResourceBase;

    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
      //  std::shared_ptr<DebugDrawManager> debugdraw_manager;
    };


    class RenderSystem
    {
    public:

        void initialize(RenderSystemInitInfo init_info);

        void tick(float delta_time);






        RenderSystem() =default;
        ~RenderSystem();

    private:
        std::shared_ptr<RHI>                m_rhi;
        std::shared_ptr<RenderPipelineBase> m_render_pipeline;
        std::shared_ptr<RenderResourceBase> m_render_resource;

    };
}
#endif // COCONUT_RENDER_SYSTEM_H
