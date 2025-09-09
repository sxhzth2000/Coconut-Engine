
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "runtime/function/render/interface/rhi.h"
#include "runtime/function/render/interface/rhi_struct.h"
#include "runtime/function/render/render_type.h"
#include "vulkan_rhi_struct.h"
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"

namespace Coconut
{

    class VulkanRHI final : public RHI
    {
       public:
        // initialize vulkan RHI
        void initialize(RHIInitInfo initialize_info) override;

        // renderLoop
        ///  set current framebuffer
        void prepareContext() override;

        //
        virtual bool prepareBeforePass() override;

        // command   and write command


        virtual void cmdBeginRenderPass(vk::CommandBuffer        command_buffer,
                                        vk::RenderPassBeginInfo& render_pass_begin_info) override;

        virtual void cmdEndRenderPass(vk::CommandBuffer commandBuffer) override;
        virtual void cmdSetViewport(vk::CommandBuffer commandBuffer, vk::Viewport viewPort) override;
        virtual void cmdSetScissor(vk::CommandBuffer commandBuffer, vk::Rect2D scissor) override;
        virtual void cmdBindPipeline(vk::CommandBuffer commandBuffer, vk::PipelineBindPoint bindPoint,
                                     vk::Pipeline pipeline) override;
        virtual void cmdDescriptorSets(vk::CommandBuffer commandBuffer, vk::PipelineBindPoint bindPoint,
                                       vk::PipelineLayout pipelineLayout, uint32_t firstSet,
                                       uint32_t descriptorSetCount, vk::DescriptorSet descriptorSet) override;

        virtual void cmdBindVertexBuffers(vk::CommandBuffer commandBuffer, vk::Buffer, vk::DeviceSize offsets) override;
        virtual void cmdBindIndexBuffer(vk::CommandBuffer commandBuffer, vk::Buffer buffer,
                                        vk::IndexType type) override;
        virtual void cmdDrewIndexed(vk::CommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount,
                                     uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;
        virtual void submitRendering() override;

        virtual void cmdMapMemory()  override;
        virtual void cmdUnMapMemory()override;

        virtual void mapBufferMemory(VmaAllocation allocation, const void* data, size_t size) override;



       private:
        void createInstance();
        void initializeDebugMessenger();
        void createWindowSurface();
        void initializePhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();
        void createCommandBuffers();
        void createDescriptorPool();

        void createSyncPrimitives();

        void createSwapChain();
        void createSwapChainImageViews();

        void createFramebufferImageAndView();
        void createDepthImageAndView();
        void createAssetAllocator();

       public:
        const SwapChainDesc& getSwapchainInfo() override;
        DepthImageDesc       getDepthImageInfo() override;
        vk::DescriptorPool   getDescriptorPool() override;
        vk::CommandBuffer    getCurrentCommandBuffer() override;

        uint8_t getMaxFramesInFlight() const override;

        uint8_t getCurrentFrameIndex() const override;
        void    setCurrentFrameIndex(uint8_t index) override;

       public:
        virtual bool createRenderPass(const vk::RenderPassCreateInfo* pCreateInfo,
                                      vk::RenderPass&                 pRenderPass) override;
        virtual bool createDescriptorSetLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings,
                                               vk::DescriptorSetLayout& DescriptorSetLayout) override;

        virtual bool createPipelineLayout(const vk::PipelineLayoutCreateInfo* pPipelineLayoutInfo,
                                          vk::PipelineLayout&                 pPipelineLayout) override;

        virtual vk::ShaderModule createShaderModule(const std::vector<unsigned char>& shader_code) override;
        virtual bool createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo,
                                            vk::Pipeline&                         pipeline) override;

       private:
        // debug
        vk::detail::DispatchLoaderDynamic dldi;
        vk::DebugUtilsMessengerEXT        m_debug_messenger = nullptr;
        //
        bool                     checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        bool                     isDeviceSuitable(const vk::PhysicalDevice& device);

        QueueFamilyIndices      findQueueFamilies();
        bool                    checkDeviceExtensionSupport(vk::PhysicalDevice physical_device);
        SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice physical_device);
        vk::SurfaceFormatKHR    chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR>& formats);
        vk::Extent2D            chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

        vk::PresentModeKHR VulkanRHI::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& modes);

        void createDescriptorSet(const vk::DescriptorSetAllocateInfo& dst_set_allocate_info,
                                 vk::DescriptorSet&                   descriptor_set) override;

        void updateDescriptorSets(const std::vector<vk::WriteDescriptorSet> write_dst_sets) override;
        bool createFramebuffer(const vk::FramebufferCreateInfo& framebuffer_create_info,
                               vk::Framebuffer&                 framebuffer) override;
        void VulkanRHI::createBufferVMA(vk::DeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                                        vk::Buffer& buffer, VmaAllocation& allocation);

        // semaphores

       public:
        static uint8_t const k_max_frames_in_flight{3};
        vk::Device           m_device{nullptr};
        vk::Queue            m_present_queue{nullptr};
        vk::Queue            m_graphics_queue{nullptr};
        vk::Queue            m_compute_queue{nullptr};

        vk::SwapchainKHR           m_swapchain;
        std::vector<vk::Image>     m_swapchain_images;
        vk::Format                 m_swapchain_image_format{vk::Format::eUndefined};
        std::vector<vk::ImageView> m_swapchain_imageviews;
        vk::Extent2D               m_swapchain_extent;
        uint32_t m_current_swapchain_image_index;


        vk::Image        m_depth_image{nullptr};
        vk::ImageView    m_depth_image_view{nullptr};
        vk::DeviceMemory m_depth_image_memory{nullptr};

        // asset allocator use VMA library
        VmaAllocator m_assets_allocator;

        vk::Viewport  m_viewport;
        vk::Rect2D    m_scissor;
        SwapChainDesc m_swpachain_desc;

       public:
        QueueFamilyIndices m_queue_indices;

        GLFWwindow*                    m_window{nullptr};
        vk::Instance                   m_instance{nullptr};
        vk::SurfaceKHR                 m_surface{nullptr};
        vk::PhysicalDevice             m_physical_device{nullptr};
        vk::CommandPool                m_command_pool{nullptr};
        std::vector<vk::CommandBuffer> m_command_buffers{k_max_frames_in_flight};
        vk::CommandBuffer              m_current_command_buffer;
        unsigned int                      m_current_frame_index;

        vk::DescriptorPool m_descriptor_pool;

        std::vector<vk::Semaphore> m_image_available_semaphore{k_max_frames_in_flight};
        std::vector<vk::Semaphore> m_render_finished_semaphore{k_max_frames_in_flight};
        std::vector<vk::Fence>     m_frame_in_flight_fence{k_max_frames_in_flight};




        vk::Format m_depth_image_format;

       private:
        const std::vector<char const*> m_validation_layers{"VK_LAYER_KHRONOS_validation"};
        uint32_t                       m_vulkan_api_version{VK_API_VERSION_1_0};
        std::vector<char const*>       m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

       private:
        bool m_enable_validation_Layers{true};
        bool m_enable_debug_utils_label{true};
        bool m_enable_point_light_shadow{true};

        // query
        vk::Format findDepthFormat();
        vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
                                       vk::FormatFeatureFlags features);

       public:
        ~VulkanRHI() override
        {
            ///////////////
            if (m_debug_messenger)
            {
                m_instance.destroyDebugUtilsMessengerEXT(m_debug_messenger, nullptr, dldi);
            }
            std::cout << "VulkanRHI destroyed\n";
        }
    };

} // namespace Coconut