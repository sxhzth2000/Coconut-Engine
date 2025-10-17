#pragma once
#include "render_resource.h"
#include "render_resource_base.h"
#include <map>
#include "vulkan/vulkan.hpp"
#include "render_common.h"
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"
namespace Coconut{


    struct VmaBuffer
    {
        vk::Buffer _buffer_;
        VmaAllocation  vmaAllocation;
    };



    struct VmaBuffer_index_buffer
        {
            vk::Buffer _buffer_;
            VmaAllocation  vmaAllocation;
            std::vector<uint16_t> _indices;
        };

    struct Texture
    {
        vk::Image   _image_;
        vk::ImageView _image_view;
        vk::Sampler _sampler_;
        VmaAllocation vmaAllocation;
    };

    struct GlobalRenderResource
    {
        VmaBuffer         _uniform_buffer_main_pass;
        VmaBuffer         _uniform_buffer_directional_light_pass;
        VmaBuffer         _uniform_buffer_point_light_pass;

        VmaBuffer         _vertex_buffer_marry;
        VmaBuffer_index_buffer         _indices_buffer_marry;

        VmaBuffer         _vertex_buffer_floor;
        VmaBuffer_index_buffer         _indices_buffer_floor;

        Texture             _texture_;
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