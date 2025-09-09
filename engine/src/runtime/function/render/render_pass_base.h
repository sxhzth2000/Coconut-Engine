#pragma once
#include "runtime/function/render/interface/rhi.h"
namespace Coconut{

    class RHI;
    class RenderResourceBase;
    struct RenderPassInitInfo
    {};
    struct RenderPassCommonInfo
    {
        std::shared_ptr<RHI>                rhi;
        std::shared_ptr<RenderResourceBase> render_resource;
    };
    class RenderPassBase
    {

    public:
    virtual void initialize()=0;
    virtual void  setCommonInfo(RenderPassCommonInfo pass_common_info);
    virtual void preparePassData()=0;
    private:

    protected:
        std::shared_ptr<RHI>                m_rhi;
        std::shared_ptr<RenderResourceBase> m_render_resource;

    };
}
