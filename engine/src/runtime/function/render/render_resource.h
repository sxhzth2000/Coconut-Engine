#pragma once
#include "render_resource.h"
#include "render_resource_base.h"
#include <map>
#include "vulkan/vulkan.hpp"
#include "render_common.h"
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"
namespace Coconut{


    struct
    VmaBuffer
    {
        vk::Buffer _buffer_;
        VmaAllocation  vmaAllocation;
    };

    struct
    VmaBuffer_index_buffer
        {
            vk::Buffer _buffer_;
            VmaAllocation  vmaAllocation;
            std::vector<uint16_t> _indices;
        };


    struct GlobalRenderResource
    {
        VmaBuffer         _uniform_buffer;
        VmaBuffer         _vertex_buffer;
        VmaBuffer_index_buffer         _indices_buffer;
    };


    class RenderResource : public RenderResourceBase
    {
    public:


       virtual void init() override;



    public:

        GlobalRenderResource m_global_render_resource;

    private:




    };

}