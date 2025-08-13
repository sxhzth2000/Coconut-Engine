//
// Created by tanhao on 2025/7/12.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include "Buffer.h"
#include "Camera.h"
#include "Texture.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "vulkan/VulkanDescriptor.h"

#include "Mesh.h"

#include "common.h"

#include "vulkan/VulkanContext.h"


#include <memory>

class Texture;


// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
extern Camera camera;

extern float lastX;
extern float lastY;
extern bool  firstMouse;
extern float deltaTime;
extern float lastFrame;
extern bool mouseLeftPressed;
extern void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
extern void mouse_callback(GLFWwindow *window, double xpos, double ypos);
extern void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
extern void processInput(GLFWwindow *window);



static void check_vk_result(VkResult err)
{
	if (err == 0) return; // VK_SUCCESS
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}


namespace ccn
{
class Application
{
  public:
	         Application();
	virtual ~Application();

	void run();

	void initWindow();
	void initVulkan();
	void initImGui();
//////pipeline
	void SetDescriptor(const vk::Device& device);


	void CreateGraphicsPipeline();
	void createDepthImageViews();
	void CreateFrameBuffer();
	///// commandbuffer
	void CreateCommandBuffer();
	void RecordCommandBuffer(vk::CommandBuffer& commandBuffer,unsigned int imageIndex);
	void RecordCommandBufferImgui(vk::CommandBuffer& commandBuffer,unsigned int imageIndex);

	void DrawFrame();

	void mainLoop();
	void cleanup();



	void preparerResourcs();







public:
	GLFWwindow  *window   = nullptr;

	VulkanContext context;




	//////////////////////////////////////////////////
	///				Imgui
	///////////////////////////////////////////////////
	vk::DescriptorPool 	descriptorPool_imgui;
	vk::CommandPool		commandPool_imgui;
	vk::RenderPass		renderPass_imgui;
	std::vector<vk::CommandBuffer> commandBuffers_imgui;
	std::vector<vk::Framebuffer> framebuffers_imgui;


//////////////////////////////////////////////////
///					SwapChain
///////////////////////////////////////////////////
	vk::SwapchainKHR swapChain;
	std::vector<vk::Image>     swapChainImages;
	std::vector<vk::ImageView> swapChainImageViews;
	vk::Format                 swapChainImageFormat;
	vk::Extent2D               swapChainExtent;

//////////////////////////////////////////////////
///////				RenderPass
////////////////////////////////////////////////////
	vk::RenderPass renderPass;
/////////////////////////////////////////////////////
///					Descriptor
////////////////////////////////////////////////////
	vk::DescriptorPool descriptorPool;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorSet descriptorSet;


/////////////////////////////////////////////////////
///					GraphicsPipeline
////////////////////////////////////////////////////

	// pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo{};
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline pipeline;

	// shader
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStagesInfo;
	// DynamicState
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
	// vertexinput
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	// inputAssembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	// Viewport
	vk::PipelineViewportStateCreateInfo viewportStateInfo{};
	// rasterizer
	vk::PipelineRasterizationStateCreateInfo rasterizerInfo{};
	//  multisampling
	vk::PipelineMultisampleStateCreateInfo multisamplingInfo{};
	//
	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};

	vk::PipelineColorBlendStateCreateInfo colorBlendingInfo{};

	/////////////////////////////////////////////////////
	///					FrameBuffer
	////////////////////////////////////////////////////
	vk::Extent3D depthExtent;
	vk::Image depthImage;
	vk::ImageView depthImageView;
	vk::DeviceMemory depthImageMemory;
	std::vector<vk::Framebuffer> framebuffers;
	/////////////////////////////////////////////////////
	///					FrameBuffer
	////////////////////////////////////////////////////
	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;
	unsigned int  imageIndex=0;
/////////////
////
////////////
	void  CreateSyncObject();

	size_t                     currentFrame      = 0;        // 当前帧索引
	constexpr static int       maxFramesInFlight = 2;
	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence>     inFlightFences;
	std::vector<vk::Fence>     imagesInFlight;





	vk::VertexInputBindingDescription                  vertexBindingDescription; // 持久的绑定描述












////////////////////////////////
	MeshCube cube;
	Mesh     marry;
	// index buffer
	vk::Buffer       indexBuffer_Cube;
	vk::DeviceMemory indexMemory_Cube;
	// vertex buffer
	vk::Buffer       vertexBuffer_Cube;
	vk::DeviceMemory vertexMemory_Cube;



	// index buffer
	vk::Buffer       indexBuffer_Marry;
	vk::DeviceMemory indexMemory_Marry;
	// vertex buffer
	vk::Buffer       vertexBuffer_Marry;
	vk::DeviceMemory vertexMemory_Marry;

	// texture
	std::unique_ptr<Texture> Cube_texture;

	// texture
	std::unique_ptr<Texture> Marry_texture;

	// uniform
	std::unique_ptr<UniformBuffer> mvpc;
	// graphics pipeline











	UniformBufferObject ubo;
};
}        // namespace ccn

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------

#endif        // APPLICATION_H