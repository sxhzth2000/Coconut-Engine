

#include "Application.h"

#include "3dpart/imgui/backends/imgui_impl_vulkan.h"
#include "3dpart/imgui/imgui.h"
#include "3dpart/imgui/backends/imgui_impl_glfw.h"

#include "Buffer.h"
#include "ModelLoader.h"
#include "utils/utils.hpp"
#include "vulkan/VulkanGraphicsPipeline.h"
#include "vulkan/VulkanRenderPass.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <set>
using namespace ccn;
// Application.cpp ? main.cpp
float  lastX         = 400.0f;
float  lastY         = 300.0f;
bool   firstMouse    = true;
float  deltaTime     = 0.0f;
float  lastFrameTime = 0.0f;

 bool mouseLeftPressed = false;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


ccn::Application::Application()
{}


ccn::Application::~Application()
{
	cleanup();
}

void ccn::Application::run()
{
	initWindow();
	std::cout<<"initVulkan"<<std::endl;
	initVulkan();
	CreateCommandBuffer();
	preparerResourcs();
	std::cout<<"SetDescriptor"<<std::endl;
	SetDescriptor(context.device);


	std::cout<<"CreateGraphicsPipeline"<<std::endl;
	CreateGraphicsPipeline();
	std::cout<<"createDepthImageViews"<<std::endl;
	createDepthImageViews();
	std::cout<<"CreateFrameBuffer"<<std::endl;
	CreateFrameBuffer();
	initImGui();
	std::cout<<"CreateFrameBuffer"<<std::endl;
	CreateCommandBuffer();
	std::cout<<"CreateCommandBuffer"<<std::endl;
	CreateSyncObject();
	std::cout<<"CreateSyncObject"<<std::endl;


	mainLoop();
}

void ccn::Application::initWindow()
{
	constexpr uint32_t WIDTH  = 800;
	constexpr uint32_t HEIGHT = 600;
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	context.window = window;
	//
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// tell GLFW to capture our mouse



}


void Application::initVulkan()
{
	context.Init();

	///////////////////////SwapChina
	auto support       = querySwapChainSupport(context.physicalDevice, context.surface);
	auto surfaceFormat = chooseSwapSurfaceFormat(support.formats);

	swapChain = CreateSwapChain(context.device, context.physicalDevice,context.surface,context.indices,*window );
	//Renderpass
	swapChainImageFormat = surfaceFormat.format;
	swapChainImages= CreateSwapChainImages(context.device,swapChain);
	swapChainImageViews= CreateSwapChainImageViews(context.device,swapChainImages,surfaceFormat.format);

	auto result= context.physicalDevice.getSurfaceCapabilitiesKHR(context.surface);
	swapChainExtent= chooseSwapExtent(result,context.window);

	renderPass = CreateRenderPass(context, swapChainImageFormat);



}
void Application::initImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();


	{
		///////////////imgui  despritorpool

		std::vector<vk::DescriptorPoolSize> poolSizes = {
			{vk::DescriptorType::eSampler, 1000},
			{vk::DescriptorType::eCombinedImageSampler, 1000},
			{vk::DescriptorType::eSampledImage, 1000},
			{vk::DescriptorType::eStorageImage, 1000},
			{vk::DescriptorType::eUniformTexelBuffer, 1000},
			{vk::DescriptorType::eStorageTexelBuffer, 1000},
			{vk::DescriptorType::eUniformBuffer, 1000},
			{vk::DescriptorType::eStorageBuffer, 1000},
			{vk::DescriptorType::eUniformBufferDynamic, 1000},
			{vk::DescriptorType::eStorageBufferDynamic, 1000},
			{vk::DescriptorType::eInputAttachment, 1000}
		};

		vk::DescriptorPoolCreateInfo poolInfo{};
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		poolInfo.maxSets = 1000 * static_cast<uint32_t>(poolSizes.size());
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		descriptorPool_imgui = context.device.createDescriptorPool(poolInfo);
		if (!descriptorPool_imgui)
		{
			throw std::runtime_error("Failed to create ImGui descriptor pool!");
		}

	}

	{
		//commandpool
		vk::CommandPoolCreateInfo poolInfo{};
		poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex=context.indices.graphicsFamily.value();

		std::cout << "imgui CreateCommandPool" << std::endl;
		try
		{
			commandPool_imgui = context.device.createCommandPool(poolInfo);
		}
		catch (const vk::SystemError &err)
		{
			throw std::runtime_error("failed to create command pool!");
		}

		std::cout << "imgui Command Pool handle: " << static_cast<VkCommandPool>(commandPool_imgui) << std::endl;

	}

	{
		///imgui_renderpass
		vk::AttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment =0 ;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass{};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments= &colorAttachmentRef;

		vk::SubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcAccessMask = {};
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

		vk::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.dependencyCount=1;
		renderPassInfo.pDependencies = &dependency;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.pAttachments = &colorAttachment;

		renderPass_imgui = context.device.createRenderPass(renderPassInfo);

	}

	{
//command buffer
		commandBuffers_imgui.resize(swapChainImages.size());
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.commandBufferCount=commandBuffers_imgui.size();
		allocInfo.commandPool=commandPool_imgui;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		 commandBuffers_imgui = context.device.allocateCommandBuffers(allocInfo);
	}
	{
		//framebuffer-imgui

		framebuffers_imgui.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			vk::ImageView attachments[] = {
				swapChainImageViews[i],// ??? image view ??? framebuffer ? attachment0
			};

			vk::FramebufferCreateInfo framebufferInfo{};
			framebufferInfo.renderPass      = renderPass_imgui;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments    = attachments;
			framebufferInfo.width           = swapChainExtent.width;
			framebufferInfo.height          = swapChainExtent.height;
			framebufferInfo.layers          = 1;

			framebuffers_imgui[i] = context.device.createFramebuffer(framebufferInfo);
		}

	}







	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForVulkan(window, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
	init_info.Instance = context.instance;
	init_info.PhysicalDevice = context.physicalDevice;
	init_info.Device = context.device;
	init_info.QueueFamily = context.indices.graphicsFamily.value();
	init_info.Queue = context.graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE ;
	init_info.DescriptorPool = descriptorPool_imgui;
	init_info.RenderPass = renderPass_imgui;
	init_info.Subpass = 0;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info);






}

void Application::SetDescriptor(const vk::Device& device)
{
	////////////////////Descriptor
	std::vector<vk::DescriptorPoolSize> poolSize = {
		{vk::DescriptorType::eUniformBuffer, 1},
		{vk::DescriptorType::eCombinedImageSampler, 2}};
	descriptorPool = CreateDescriptorPool(context, poolSize);

	//ubo
	vk::DescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding            = 0;
	uboLayoutBinding.descriptorType     = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.descriptorCount    = 1;
	uboLayoutBinding.stageFlags         = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	// sampler
	vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding            = 1;
	samplerLayoutBinding.descriptorCount    = 2;
	samplerLayoutBinding.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags         = vk::ShaderStageFlagBits::eFragment;

	std::vector<vk::DescriptorSetLayoutBinding> bindings = {uboLayoutBinding, samplerLayoutBinding};
	//descriptorSetLayout
	descriptorSetLayout = CreateDescriptorSetLayout(context.device, bindings);

	//take 2 sampler and 1 uniform from pool
	descriptorSet = allocateDescriptorSets(context.device, descriptorSetLayout, descriptorPool);

	//
	std::cout<<"debug begin"<<std::endl;
	std::vector<vk::DescriptorImageInfo> imageInfos{};
	imageInfos.push_back(Cube_texture->getDescriptorInfo());
	imageInfos.push_back(Marry_texture->getDescriptorInfo());
	std::cout<<"debug begin"<<std::endl;
	std::cout<<"debug end 0"<<std::endl;
	auto uniformInfo = mvpc->getDescriptorInfo();



	//
	std::vector<vk::WriteDescriptorSet> writeDescriptorSets
	{
		vk::WriteDescriptorSet{
			descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uniformInfo},
		vk::WriteDescriptorSet{
			descriptorSet, 1, 0, static_cast<uint32_t>(imageInfos.size()), vk::DescriptorType::eCombinedImageSampler, imageInfos.data()}
	};

	updateDescriptorSets(context.device, writeDescriptorSets);


}

void Application::CreateGraphicsPipeline()
{

	///////////////////////////////pipeline
	//shader modules
	shaderStagesInfo = createShaderModules(context.device, "shaders/vert_cube.spv", "shaders/frag_cube.spv");
	//dynamic state
	std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor}; // ????????
	dynamicStateInfo= setDynamicState(dynamicStates);
	// Vertex input
	vk::VertexInputBindingDescription
	BindingDescription = {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
	std::vector<vk::VertexInputAttributeDescription>    // ????
	attributeDescriptions = {
		vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
		vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
		vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord)},
		vk::VertexInputAttributeDescription{3, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)}};
	vertexInputInfo=setVertexInput(attributeDescriptions,BindingDescription);
	//	Input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.topology               = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = false;
	inputAssemblyInfo=inputAssembly;
	//
	//view port
	//
	vk::PipelineViewportStateCreateInfo viewportState = {{}, 1, {}, 1,{}};
	viewportStateInfo=viewportState;
	//
	//Raster
	//
	vk::PipelineRasterizationStateCreateInfo rasterization;
	rasterization.flags                   = {};
	rasterization.depthClampEnable        = false;
	rasterization.rasterizerDiscardEnable = false;
	rasterization.polygonMode             = vk::PolygonMode::eFill;
	rasterization.cullMode                = vk::CullModeFlagBits::eBack;
	rasterization.frontFace               = vk::FrontFace::eCounterClockwise;
	rasterization.depthBiasEnable         = false;
	rasterization.lineWidth               = 1.0f;
	rasterization.depthBiasConstantFactor = 0.0f;
	rasterization.depthBiasClamp          = 0.0f;
	rasterization.depthBiasSlopeFactor    = 0.0f;
	rasterizerInfo =rasterization;
	//
	// Multisample
	//
	vk::PipelineMultisampleStateCreateInfo multisampling;
	multisampling.flags                 = {};
	multisampling.rasterizationSamples  = vk::SampleCountFlagBits::e1;
	multisampling.sampleShadingEnable   = false;
	multisampling.minSampleShading      = 1.0f;
	multisampling.pSampleMask           = nullptr;
	multisampling.alphaToCoverageEnable = false;
	multisampling.alphaToOneEnable      = false;
	multisamplingInfo=multisampling;
	//
	//DepthStencil
	//
	vk::PipelineDepthStencilStateCreateInfo depthStencil;
	depthStencil.depthTestEnable       = true;
	depthStencil.depthWriteEnable      = true;
	depthStencil.depthCompareOp        = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable = false;
	depthStencil.stencilTestEnable     = false;
	depthStencilInfo=depthStencil;



	// ????????????????
	//  std::vector<vk::PipelineColorBlendAttachmentState> attachments = {colorBlendAttachment1, colorBlendAttachment2, ...};
	//  colorBlending.setAttachments(attachments);
	// ColorBlending
	vk::PipelineColorBlendAttachmentState   colorBlendAttachment;
	colorBlendAttachment.colorWriteMask =
		vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE; // ?????
	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp       = vk::LogicOp::eCopy;
	colorBlending.setAttachments(colorBlendAttachment); // ???? attachment
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
	colorBlendingInfo=colorBlending;
	//
	//PilinleLayout
	//
	std::vector<vk::PushConstantRange> pushConstantRanges;
	pushConstantRanges.push_back(vk::PushConstantRange{});
	pushConstantRanges.back().stageFlags = vk::ShaderStageFlagBits::eFragment;
	pushConstantRanges.back().offset     = 0;
	pushConstantRanges.back().size       = sizeof(unsigned int);
	auto lineLayout= setPipelineLayout(context.device,descriptorSetLayout,pushConstantRanges);
	pipelineLayout =lineLayout;
	//

	pipelineInfo.stageCount          = 2;
	pipelineInfo.pStages             = shaderStagesInfo.data();
	pipelineInfo.pDynamicState       = &dynamicStateInfo;
	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState      = &viewportStateInfo;
	pipelineInfo.pRasterizationState = &rasterizerInfo;
	pipelineInfo.pMultisampleState   = &multisamplingInfo;
	pipelineInfo.pDepthStencilState  = &depthStencilInfo;
	pipelineInfo.pColorBlendState    = &colorBlendingInfo;
	pipelineInfo.layout     = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass    = 0;

	try
	{
		pipeline = context.device.createGraphicsPipeline(nullptr, pipelineInfo).value;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Failed to create graphics pipeline: " << e.what() << std::endl;
		throw;
	}


}

void Application::createDepthImageViews()
{
		//depthimageview
		depthExtent.width=swapChainExtent.width;
		depthExtent.height=swapChainExtent.height;
		depthExtent.depth=1;

		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType     = vk::ImageType::e2D;
		imageInfo.extent       = depthExtent;
		imageInfo.mipLevels    = 1;
		imageInfo.arrayLayers  = 1;
		imageInfo.format       = vk::Format::eD32Sfloat;
		imageInfo.tiling       = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout= vk::ImageLayout::eUndefined;
		imageInfo.usage        = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		imageInfo.samples      = vk::SampleCountFlagBits::e1;
		imageInfo.sharingMode  = vk::SharingMode::eExclusive;

		depthImage = context.device.createImage(imageInfo);

		vk::MemoryRequirements memRequirements = context.device.getImageMemoryRequirements(depthImage);

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
												  vk::MemoryPropertyFlagBits::eDeviceLocal,context.physicalDevice);

		depthImageMemory = context.device.allocateMemory(allocInfo);

		context.device.bindImageMemory(depthImage, depthImageMemory, 0);

		// ????????
		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.image = depthImage;
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format =  vk::Format::eD32Sfloat;
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		depthImageView = context.device.createImageView(viewInfo);

}

void Application::CreateFrameBuffer()
{

	framebuffers.resize(swapChainImageViews.size());




	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		vk::ImageView attachments[] = {
			swapChainImageViews[i],// ??? image view ??? framebuffer ? attachment0
			depthImageView
		};

		vk::FramebufferCreateInfo framebufferInfo{};
		framebufferInfo.renderPass      = renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments    = attachments;
		framebufferInfo.width           = swapChainExtent.width;
		framebufferInfo.height          = swapChainExtent.height;
		framebufferInfo.layers          = 1;

		framebuffers[i] = context.device.createFramebuffer(framebufferInfo);
	}

}

void Application::CreateCommandBuffer()
{

	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolInfo.queueFamilyIndex = context.indices.graphicsFamily.value();
	std::cout << "CreateCommandPool" << std::endl;
	try
	{
		commandPool = context.device.createCommandPool(poolInfo);
	}
	catch (const vk::SystemError &err)
	{
		throw std::runtime_error("failed to create command pool!");
	}

	std::cout << "Command Pool handle: " << static_cast<VkCommandPool>(commandPool) << std::endl;


	commandBuffers.resize(swapChainImages.size());        //

	vk::CommandBufferAllocateInfo allocateInfo{};
	allocateInfo.commandPool        = commandPool;
	allocateInfo.level              = vk::CommandBufferLevel::ePrimary;
	allocateInfo.commandBufferCount = (static_cast<uint32_t>(commandBuffers.size()));
	commandBuffers = context.device.allocateCommandBuffers(allocateInfo);
	std::cout << "allocateCommandBuffers" << std::endl;
}

void Application::RecordCommandBuffer(vk::CommandBuffer& commandBuffer,uint32_t imageIndex)
{

	std::cout<<"RecordCommandBuffer_MAIN"<<std::endl;
// 1. ?????????
	vk::Viewport viewport{};
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>(swapChainExtent.width);
	viewport.height   = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
//
	std::cout<<"viewport:"<<viewport.width<<"x"<<viewport.height<<std::endl;

	vk::Rect2D scissor;
	scissor.offset = vk::Offset2D{0, 0};
	scissor.extent = swapChainExtent;

	std::cout<<"scissor.extent:"<<scissor.extent.width<<"x"<<scissor.extent.height<<std::endl;


	// beginInfo
	vk::CommandBufferBeginInfo beginInfo = {};

	beginInfo.flags = vk::CommandBufferUsageFlags();        // ?????


	unsigned int BufferIndex = 0;

	commandBuffer.begin(beginInfo);



	// ???color attachment index = 0, depth attachment index = 1
	// 2. set clear color
	std::array<vk::ClearValue, 2> clearValues;
	clearValues[0].color = vk::ClearColorValue(std::array<float,4>{0.0f, 0.0f, 0.0f, 1.0f});
	clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0u}; // <-- ??? [0,1]


	//  3. ?? render pass
	vk::RenderPassBeginInfo passBeginInfo{};
	passBeginInfo.renderPass        = renderPass;
	 passBeginInfo.framebuffer       = framebuffers[imageIndex];
	passBeginInfo.renderArea.offset = vk::Offset2D{0, 0};
	passBeginInfo.renderArea.extent = swapChainExtent;

	passBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());;
	passBeginInfo.pClearValues = clearValues.data();


	commandBuffer.beginRenderPass(passBeginInfo, vk::SubpassContents::eInline);

	//
	commandBuffer.setViewport(0, viewport);
	commandBuffer.setScissor(0, scissor);

	// 4. ??????
	//	std::cout << "Pipeline handle: " << static_cast<void *>(static_cast<VkPipeline>(graphics_pipeline->getPipeline())) << std::endl;




	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	commandBuffer.bindDescriptorSets(
	vk::PipelineBindPoint::eGraphics,
	pipelineLayout,
	0,
	{descriptorSet},
	nullptr);

	VkDeviceSize offsets[] = {0};
	unsigned int texIndex = 0;

	// draw cube
	commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer_Cube, offsets);
	commandBuffer.bindIndexBuffer(indexBuffer_Cube, 0, vk::IndexType::eUint16);
	commandBuffer.pushConstants(pipelineLayout,vk::ShaderStageFlagBits::eFragment,0,sizeof(unsigned int),&texIndex);
	commandBuffer.drawIndexed(static_cast<uint32_t>(cube.m_indices.size()), 1, 0, 0, 0);
	// draw marry
	texIndex=1;
	commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer_Marry, offsets);
	commandBuffer.bindIndexBuffer(indexBuffer_Marry, 0, vk::IndexType::eUint16);
	commandBuffer.pushConstants(pipelineLayout,vk::ShaderStageFlagBits::eFragment,0,sizeof(unsigned int),&texIndex);

	commandBuffer.bindDescriptorSets(
	    vk::PipelineBindPoint::eGraphics ,
	    pipelineLayout,
	    0,
	    {descriptorSet},
	    nullptr);


	commandBuffer.drawIndexed(static_cast<uint32_t>(marry.m_indices.size()), 1, 0, 0, 0);
	// 6. end of render pass
	commandBuffer.endRenderPass();
	// 7. end
	commandBuffer.end();

}
void Application::RecordCommandBufferImgui(vk::CommandBuffer &commandBuffer, unsigned int imageIndex)
{
	std::cout<<"RecordCommandBuffer_IMGUI"<<std::endl;
	// 开始命令缓冲录制（如果没提前开始）
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
	commandBuffer.begin(beginInfo);

	// 开始 RenderPass（你的 RenderPass，framebuffer等必须和交换链对应）
	vk::RenderPassBeginInfo renderPassInfo{};
	renderPassInfo.renderPass = renderPass_imgui;                    // ImGui使用的RenderPass，通常和主渲染一致
	renderPassInfo.framebuffer = framebuffers_imgui[imageIndex];     // 绑定当前交换链对应的 framebuffer
	renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
	renderPassInfo.renderArea.extent = swapChainExtent;

	std::array<vk::ClearValue, 1> clearValues{};
	clearValues[0].color = vk::ClearColorValue(std::array<float,4>{0.0f, 0.0f, 0.0f, 1.0f});
	//clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	// ImGui 渲染绘制命令写入命令缓冲区
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.operator VkCommandBuffer(),nullptr);



	commandBuffer.endRenderPass();

	commandBuffer.end();
}

void Application::DrawFrame()
{


std::cout << "Draw frame" << std::endl;
	// per-frame time logic
	// --------------------
	float currentFrameTime = static_cast<float>(glfwGetTime());
	deltaTime              = currentFrameTime - lastFrameTime;
	lastFrameTime          = currentFrameTime;

	float time = static_cast<float>(glfwGetTime());

	ubo.color = glm::vec3(
	    std::sin(time) * 0.5f + 0.5f,
	    std::cos(time) * 0.5f + 0.5f,
	    0.5f);


	ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.view = camera.GetViewMatrix();

	ubo.proj = glm::perspective(glm::radians(camera.Zoom),
	                              float(swapChainExtent.width) / float(swapChainExtent.height),
	                            0.1f, 100.0f);
	ubo.proj[1][1]*=-1;

	//

	void *data;
	context.device.mapMemory(mvpc->bufferMemory, 0, sizeof(ubo), {}, &data);
	memcpy(data, &ubo, sizeof(ubo));
	context.device.unmapMemory(mvpc->bufferMemory);

	std::cout <<"mapMemory"<<std::endl;
	// 1 Wait for last frame
	std::cout <<"acquireNextImageKHR"<<std::endl;
	context.device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);


	uint32_t imageIndex;

	auto acquireResult = context.device.acquireNextImageKHR(
	    swapChain, UINT64_MAX,
	    imageAvailableSemaphores[currentFrame],
	    nullptr,
	    &imageIndex);

	if (imagesInFlight[imageIndex])
	{
		context.device.waitForFences(imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	if (acquireResult == vk::Result::eErrorOutOfDateKHR)
	{
		//	recreateSwapchain();
		return;
	}
	else if (acquireResult != vk::Result::eSuccess && acquireResult != vk::Result::eSuboptimalKHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// 3 reset fence and command buffer
	context.device.resetFences(inFlightFences[currentFrame]);

	std::cout<<"	"<<std::endl;
	// 4  ????? fence ????? image




	{
		////imgui

	ImGui_ImplVulkan_NewFrame();


	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();



	std::cout<<"	ImGui::Begin(Hello);	"<<std::endl;
	// 这里写你的ImGui界面代码，例如：
	ImGui::Begin("Hello");
	ImGui::Text("ImGui frame");

	static char inputBuf[256] = "fuck";
	ImGui::InputText("input", inputBuf, sizeof(inputBuf));





	ImGui::End();
	std::cout<<"		ImGui::End();	"<<std::endl;
	}



	RecordCommandBuffer(commandBuffers[currentFrame], imageIndex);


	RecordCommandBufferImgui(commandBuffers_imgui[currentFrame], imageIndex); // 确保你有这个函数录制 ImGui 命令


	std::vector<vk::CommandBuffer> final_commandBuffers = { commandBuffers[currentFrame],commandBuffers_imgui[currentFrame] };

	//  5 submitInfo
	vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	vk::SubmitInfo         submitInfo{};
	submitInfo.waitSemaphoreCount    = 1;
	submitInfo.pWaitSemaphores       = &imageAvailableSemaphores[currentFrame];
	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	submitInfo.pWaitDstStageMask     = waitStages;
	submitInfo.commandBufferCount    = static_cast<uint32_t>(final_commandBuffers.size());
	submitInfo.pCommandBuffers       = final_commandBuffers.data();
	submitInfo.signalSemaphoreCount  = 1;
	submitInfo.pSignalSemaphores     = &renderFinishedSemaphores[currentFrame];

	context.graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	// ???????
	vk::PresentInfoKHR presentInfo{};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = &renderFinishedSemaphores[currentFrame];
	presentInfo.swapchainCount     = 1;
	presentInfo.pSwapchains        = &swapChain;
	presentInfo.pImageIndices      = &imageIndex;
	presentInfo.pResults           = nullptr;

	vk::Result presentResult = context.presentQueue.presentKHR(presentInfo);
	// 8. ?????
	currentFrame = (currentFrame + 1) % maxFramesInFlight;
	// ???????????????????
	if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
	{
		// recreateSwapChain();
	}
	else if (presentResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}





}


void ccn::Application::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		processInput(window); // ? ?????????????
		DrawFrame();
	}
	context.device.waitIdle();
}


void ccn::Application::cleanup()
{
	std::cout << "start cleanup \n";

	// ???????????????
context.device.waitIdle();

	if (window)
	{
		glfwDestroyWindow(window);
	}

	glfwTerminate();
}


void Application::preparerResourcs()
{
	std::cout << "Before ModelLoader" << std::endl;
	ModelLoader loader;
	std::cout << "After ModelLoader" << std::endl;
	if (loader.loadModel("assets/marry/Marry.obj"))
	{
		auto verts = loader.getVertices();
		auto inds  = loader.getIndices();
		auto texs  = loader.getTextures();

		std::cout << "Loaded " << verts.size() << " vertices, "
			<< inds.size() / 3 << " triangles.\n";

		for (const auto &tex : texs)
			std::cout << "Texture: " << tex << "\n";

		marry.m_vertices = verts;
		marry.m_indices  = inds;
	}

	// vertex buffe
	createBuffer(context.device,context.physicalDevice,sizeof(Vertex) * cube.m_vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, vertexBuffer_Cube, vertexMemory_Cube, cube.m_vertices.data());
	// inedx buffer
	createBuffer(context.device,context.physicalDevice,sizeof(uint16_t) * cube.m_indices.size(), vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, indexBuffer_Cube, indexMemory_Cube, cube.m_indices.data());

	// marry model
	//  vertex buffer
	createBuffer(context.device,context.physicalDevice,sizeof(Vertex) * marry.m_vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, vertexBuffer_Marry, vertexMemory_Marry, marry.m_vertices.data());
	// inedx buffer
	createBuffer(context.device,context.physicalDevice,sizeof(uint16_t) * marry.m_indices.size(), vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, indexBuffer_Marry, indexMemory_Marry, marry.m_indices.data());

	std::cout << "1111111111" << std::endl;
	// uniform
	mvpc = std::make_unique<UniformBuffer>();
	mvpc->setContex(context.physicalDevice, context.device);
	mvpc->create(sizeof(UniformBufferObject), nullptr);
	std::cout << "10000000" << std::endl;

	// texture
	std::cout << "Cube_texture" << std::endl;
	Cube_texture = std::make_unique<Texture>(context.device, context.physicalDevice, commandPool, context.graphicsQueue);
	Cube_texture->loadFromFile("textures/clockSide.jpg");
	//texture
	Marry_texture = std::make_unique<Texture>(context.device, context.physicalDevice, commandPool, context.graphicsQueue);
	Marry_texture->loadFromFile("assets/marry/MC003_Kozakura_Mari.png");


}
void Application::CreateSyncObject()
{
	vk::SemaphoreCreateInfo semaphoreInfo{};
	vk::FenceCreateInfo     fenceInfo{};
	fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled; // 初始为已信号状态，方便第一次使用

	imageAvailableSemaphores.resize(maxFramesInFlight);
	renderFinishedSemaphores.resize(maxFramesInFlight);
	inFlightFences.resize(maxFramesInFlight);
	imagesInFlight.resize(swapChainImages.size(), nullptr); // 一般和swapchain image数量相同

	for (size_t i = 0; i < maxFramesInFlight; i++)
	{
		imageAvailableSemaphores[i] = context.device.createSemaphore(semaphoreInfo);
		renderFinishedSemaphores[i] = context.device.createSemaphore(semaphoreInfo);
		inFlightFences[i] = context.device.createFence(fenceInfo);
	}

}

// 鼠标按钮回调，监听左键按下/释放
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			mouseLeftPressed = true;
			firstMouse = true;  // 重置，避免跳跃
		}
		else if (action == GLFW_RELEASE)
		{
			mouseLeftPressed = false;
		}
	}
}


void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
	if (!mouseLeftPressed)
		return; // 左键没按，不处理视角旋转

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return; // ImGui 界面上操作，禁止摄像机旋转

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX      = xpos;
		lastY      = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
	std::cout << "scroll_callback" << std::endl;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return; // ImGui 界面

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}