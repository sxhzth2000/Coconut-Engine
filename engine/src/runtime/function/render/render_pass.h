
#include "render_pass_base.h"
#include "render_resource.h"
#include "vulkan/vulkan.hpp"
#include "runtime/function/render/interface/rhi_struct.h"
namespace Coconut
{

    enum
    {
        _main_camera_pass_depth                         = 0,
        _main_camera_pass_swap_chain_image              = 1,
        _main_camera_pass_attachment_count              = 2,

    };



class RenderPass:public RenderPassBase
{
public:
    struct Descriptor
    {
        vk::DescriptorSetLayout layout;
        vk::DescriptorSet       descriptor_set;
    };
    struct FrameBufferAttachment
    {
        vk::Image*        image;
        vk::DeviceMemory* mem;
        vk::ImageView*    view;
        vk::Format       format;
    };
    struct Framebuffer
    {
        int           width;
        int           height;
        vk::Framebuffer* framebuffer;
        vk::RenderPass  render_pass;

        std::vector<FrameBufferAttachment> attachments;
    };
    struct RenderPipelineBase
    {
        vk::PipelineLayout layout;
        vk::Pipeline       pipeline;
    };

    void initialize(const RenderPassInitInfo* init_info);

    GlobalRenderResource*      m_global_render_resource {nullptr};

    virtual void draw();
public:
    std::vector<Descriptor>         m_descriptor_infos;
    std::vector<RenderPipelineBase> m_render_pipelines;
    Framebuffer                     m_framebuffer;

};
};