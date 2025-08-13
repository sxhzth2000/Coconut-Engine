#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"

vk::CommandBuffer Texture::beginSingleTimeCommands()
{
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.level              = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandPool        = commandPool;
	allocInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer;
	device.allocateCommandBuffers(&allocInfo, &commandBuffer);

	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void Texture::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo{};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;

	graphicsQueue.submit(submitInfo, nullptr);
	graphicsQueue.waitIdle();

	device.freeCommandBuffers(commandPool, commandBuffer);
}
vk::DescriptorImageInfo Texture::getDescriptorInfo()
{
	std::cout << "get Texture DescriptorInfo" << std::endl;
	std::cout << "sampler handle: " << (void*)(VkSampler)sampler << std::endl;
	std::cout << "imageView handle: " << (void*)(VkImageView)imageView << std::endl;
	return vk::DescriptorImageInfo{
		sampler, imageView, vk::ImageLayout::eShaderReadOnlyOptimal};
}

Texture::Texture(vk::Device         device,
                 vk::PhysicalDevice physicalDevice,
                 vk::CommandPool    commandPool,
                 vk::Queue          graphicsQueue) :
    device(device),
    physicalDevice(physicalDevice),
    commandPool(commandPool),
    graphicsQueue(graphicsQueue),
    image(VK_NULL_HANDLE),
    imageMemory(VK_NULL_HANDLE),
    imageView(VK_NULL_HANDLE),
    sampler(VK_NULL_HANDLE),
    mipLevels(0)
{
	// 这里只是初始化成员变量，不执行任何 Vulkan 创建操作
}
Texture::~Texture()
{
}

void Texture::loadFromFile(const std::string &filename)
{
	int      texWidth, texHeight, texChannels;
	stbi_uc *pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (!pixels)
	{
		throw std::runtime_error("Failed to load texture image: " + filename);
	}

	vk::DeviceSize imageSize = texWidth * texHeight * 4;        // STBI_rgb_alpha = 4 channels (RGBA)
	mipLevels                = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	vk::Buffer       stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	std::cout<<"Create Texture buffer"<<std::endl;
	ccn::createBuffer(device,
	                  physicalDevice,
	                  imageSize,
	                  vk::BufferUsageFlagBits::eTransferSrc,
	                  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	                  stagingBuffer,
	                  stagingBufferMemory,
	                  pixels);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, mipLevels, vk::Format::eR8G8B8A8Srgb,
	            vk::ImageTiling::eOptimal,
	            vk::ImageUsageFlagBits::eTransferSrc |
	                vk::ImageUsageFlagBits::eTransferDst |
	                vk::ImageUsageFlagBits::eSampled,
	            vk::MemoryPropertyFlagBits::eDeviceLocal,
	            image, imageMemory);

	transitionImageLayout(image, vk::Format::eR8G8B8A8Srgb,
	                      vk::ImageLayout::eUndefined,
	                      vk::ImageLayout::eTransferDstOptimal, mipLevels);

	copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	// transition from TransferDstOptimal to ShaderReadOnlyOptimal for all mip levels
	generateMipmaps(image, vk::Format::eR8G8B8A8Srgb, texWidth, texHeight, mipLevels);

	// Create view and sampler
	createImageView(image, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
	createSampler();

	// Cleanup staging
	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

void Texture::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::Format format,
                          vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                          vk::MemoryPropertyFlags properties,
                          vk::Image &image, vk::DeviceMemory &imageMemory)
{
	vk::ImageCreateInfo imageInfo{};
	imageInfo.imageType     = vk::ImageType::e2D;
	imageInfo.extent.width  = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth  = 1;
	imageInfo.mipLevels     = mipLevels;
	imageInfo.arrayLayers   = 1;
	imageInfo.format        = format;
	imageInfo.tiling        = tiling;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.usage         = usage;
	imageInfo.sharingMode   = vk::SharingMode::eExclusive;
	imageInfo.samples       = vk::SampleCountFlagBits::e1;
	imageInfo.flags         = {};

	image = device.createImage(imageInfo);

	vk::MemoryRequirements memRequirements = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = ccn::findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

	imageMemory = device.allocateMemory(allocInfo);
	device.bindImageMemory(image, imageMemory, 0);
}
void Texture::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.image                           = image;
	viewInfo.viewType                        = vk::ImageViewType::e2D;
	viewInfo.format                          = format;
	viewInfo.subresourceRange.aspectMask     = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;

	imageView = device.createImageView(viewInfo);
}
void Texture::createSampler()
{
	vk::SamplerCreateInfo samplerInfo{};
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.minFilter = vk::Filter::eLinear;

	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

	samplerInfo.anisotropyEnable = VK_TRUE;

	vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
	samplerInfo.maxAnisotropy               = properties.limits.maxSamplerAnisotropy;

	samplerInfo.borderColor             = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp     = vk::CompareOp::eAlways;

	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.minLod     = 0.0f;
	samplerInfo.maxLod     = static_cast<float>(mipLevels);
	samplerInfo.mipLodBias = 0.0f;

	sampler = device.createSampler(samplerInfo);
}

void Texture::transitionImageLayout(vk::Image image, vk::Format format,
                                    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                    uint32_t mipLevels)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::ImageMemoryBarrier barrier{};
	barrier.oldLayout           = oldLayout;
	barrier.newLayout           = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image                           = image;
	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = 1;

	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		if (hasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
	         newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage      = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		throw std::invalid_argument("Unsupported layout transition!");
	}

	commandBuffer.pipelineBarrier(
	    sourceStage, destinationStage,
	    {}, {}, {}, barrier);

	endSingleTimeCommands(commandBuffer);
}
bool Texture::hasStencilComponent(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}
void Texture::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::BufferImageCopy region{};
	region.bufferOffset      = 0;
	region.bufferRowLength   = 0;        // tightly packed
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;

	region.imageOffset = vk::Offset3D(0, 0, 0);
	region.imageExtent = vk::Extent3D{width, height, 1};

	commandBuffer.copyBufferToImage(
	    buffer, image,
	    vk::ImageLayout::eTransferDstOptimal,
	    region);

	endSingleTimeCommands(commandBuffer);
}
void Texture::generateMipmaps(vk::Image image, vk::Format imageFormat,
                              int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// Check format supports linear blitting
	vk::FormatProperties formatProperties = physicalDevice.getFormatProperties(imageFormat);
	if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
	{
		throw std::runtime_error("Texture image format does not support linear blitting!");
	}

	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	int32_t mipWidth  = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++)
	{
		vk::ImageMemoryBarrier barrier{};
		barrier.image                           = image;
		barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel   = i - 1;
		barrier.subresourceRange.levelCount     = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount     = 1;

		barrier.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout     = vk::ImageLayout::eTransferSrcOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		commandBuffer.pipelineBarrier(
		    vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
		    {}, {}, {}, barrier);

		vk::ImageBlit blit{};
		blit.srcOffsets[0]                 = vk::Offset3D{0, 0, 0};
		blit.srcOffsets[1]                 = vk::Offset3D{mipWidth, mipHeight, 1};
		blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
		blit.srcSubresource.mipLevel       = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount     = 1;

		blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
		blit.dstOffsets[1] = vk::Offset3D{
		    mipWidth > 1 ? mipWidth / 2 : 1,
		    mipHeight > 1 ? mipHeight / 2 : 1,
		    1};
		blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
		blit.dstSubresource.mipLevel       = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount     = 1;

		commandBuffer.blitImage(
		    image, vk::ImageLayout::eTransferSrcOptimal,
		    image, vk::ImageLayout::eTransferDstOptimal,
		    blit, vk::Filter::eLinear);

		// Transition current mip to read-only
		barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
		barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer.pipelineBarrier(
		    vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
		    {}, {}, {}, barrier);

		if (mipWidth > 1)
			mipWidth /= 2;
		if (mipHeight > 1)
			mipHeight /= 2;
	}

	// Transition last mip level
	vk::ImageMemoryBarrier barrier{};
	barrier.image                           = image;
	barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseMipLevel   = mipLevels - 1;
	barrier.subresourceRange.levelCount     = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = 1;

	barrier.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
	barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

	commandBuffer.pipelineBarrier(
	    vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
	    {}, {}, {}, barrier);

	endSingleTimeCommands(commandBuffer);
}
