#include "render_pass_base.h"

namespace Coconut
{

    void RenderPassBase::setCommonInfo(RenderPassCommonInfo pass_common_info) {
        m_rhi=pass_common_info.rhi;
        m_render_resource = pass_common_info.render_resource;
    }
}