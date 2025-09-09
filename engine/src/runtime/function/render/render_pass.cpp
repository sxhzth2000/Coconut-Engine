#include "render_pass.h"


namespace Coconut
{
    void RenderPass::initialize(const RenderPassInitInfo* init_info)
    {
        m_global_render_resource =
            &(std::static_pointer_cast<RenderResource>(m_render_resource)->m_global_render_resource);





    }
    void RenderPass::draw() {};
}