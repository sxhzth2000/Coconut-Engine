//
// Created by tanhao on 2025/8/28.
//

#ifndef COCONUT_RHI_H
#define COCONUT_RHI_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#include "rhi_struct.h"
#include "runtime/function/render/interface/vulkan/vulkan_rhi_struct.h"
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"

namespace Coconut
{
    class WindowSystem;

    struct RHIInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    class RHI
    {
       public:
        virtual ~RHI()                                       = 0;
        virtual void initialize(RHIInitInfo initialize_info) = 0;
        //
        virtual void prepareContext() = 0;
        //


        //tick

        virtual bool prepareBeforePass()=0;  // begin command  buffer  and set
        //command buffer

       virtual void cmdBeginRenderPass(vk::CommandBuffer command_buffer,vk::RenderPassBeginInfo & render_pass_begin_info)=0;
       virtual void cmdEndRenderPass(vk::CommandBuffer commandBuffer) =0;
       virtual void cmdSetViewport(vk::CommandBuffer commandBuffer, vk::Viewport viewPort)=0;
       virtual void cmdSetScissor(vk::CommandBuffer commandBuffer, vk::Rect2D scissor)=0;
       virtual void cmdBindPipeline(vk::CommandBuffer commandBuffer,vk::PipelineBindPoint bindPoint,vk::Pipeline pipeline)=0;
       virtual void cmdDescriptorSets(vk::CommandBuffer commandBuffer,vk::PipelineBindPoint bindPoint,vk::PipelineLayout pipelineLayout, uint32_t firstSet,
                                      uint32_t descriptorSetCount,vk::DescriptorSet descriptorSet)=0;

       virtual void cmdBindVertexBuffers(vk::CommandBuffer commandBuffer,vk::Buffer,vk::DeviceSize offsets) =0;
       virtual void cmdBindIndexBuffer(vk::CommandBuffer commandBuffer,vk::Buffer,vk::IndexType type) =0;
       virtual void cmdDrewIndexed(vk::CommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount,
                                    uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) =0;



        //submit
       virtual void submitRendering()=0;
       virtual void cmdMapMemory() =0;
       virtual void cmdUnMapMemory()=0;
       virtual void mapBufferMemory(VmaAllocation allocation, const void* data, size_t size) =0;



        virtual const SwapChainDesc& getSwapchainInfo()                                                         = 0;
        virtual DepthImageDesc       getDepthImageInfo()                                                        = 0;
        virtual vk::CommandBuffer getCurrentCommandBuffer() =0;
        virtual uint8_t getMaxFramesInFlight() const = 0;
        virtual uint8_t getCurrentFrameIndex() const = 0;
        virtual void setCurrentFrameIndex(uint8_t index) = 0;

        virtual bool createRenderPass(const vk::RenderPassCreateInfo* pCreateInfo, vk::RenderPass& pRenderPass) = 0;
        virtual bool createDescriptorSetLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings,
                                               vk::DescriptorSetLayout& DescriptorSetLayout)                    = 0;
        virtual bool createPipelineLayout(const vk::PipelineLayoutCreateInfo* pPipelineLayoutInfo,
                                          vk::PipelineLayout&                 pPipelineLayout)                                  = 0;
        virtual vk::ShaderModule createShaderModule(const std::vector<unsigned char>& shader_code)              = 0;
        virtual bool createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo,
                                            vk::Pipeline&                         pipeline)                                             = 0;
        virtual vk::DescriptorPool getDescriptorPool()                                                          = 0;
        virtual void               createDescriptorSet(const vk::DescriptorSetAllocateInfo& dst_set_allocate_info,
                                                       vk::DescriptorSet&                   descriptor_set)                       = 0;
        virtual void               updateDescriptorSets(const std::vector<vk::WriteDescriptorSet> dst_sets)     = 0;

        virtual bool createFramebuffer(const vk::FramebufferCreateInfo& framebuffer_create_info,
                                       vk::Framebuffer&                 framebuffer)                = 0;
        virtual void createBufferVMA(vk::DeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                                     vk::Buffer& buffer, VmaAllocation& allocation) = 0;
    };
    inline RHI::~RHI() = default;
} // namespace Coconut
#endif // COCONUT_RHI_H
