//
// Created by tanhao on 2025/8/3.
//

#include "VulkanGraphicsPipeline.h"

#include "../Shader.h"


namespace ccn
{


vk::PipelineLayout setPipelineLayout(const vk::Device& device ,
	const vk::DescriptorSetLayout &descriptorSetLayout,
	const std::vector<vk::PushConstantRange>  ConstantRanges)
{

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount         = 1;
	pipelineLayoutInfo.pSetLayouts            = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges    = ConstantRanges.data();

	auto result= device.createPipelineLayout(pipelineLayoutInfo);

	if (!result)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	return result;
}

std::vector<vk::PipelineShaderStageCreateInfo> createShaderModules(const vk::Device& device,const std::string& vertex_path,const  std::string& fragment_path)
{
	std::vector<Shader> shaders;
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
	shaders.emplace_back(device); // vertex
	shaders.back().loadCode(vertex_path);
	shaders.back().createShaderMoudle();
	shaders.back().setStageInfo(shader_type::vertexShader);
	shaderStages.push_back(shaders.back().ShaderStageInfo);

	shaders.emplace_back(device); // fragment
	shaders.back().loadCode(fragment_path);
	shaders.back().createShaderMoudle();
	shaders.back().setStageInfo(shader_type::fragmentShader);
	shaderStages.push_back(shaders.back().ShaderStageInfo);

	return shaderStages;
}

vk::PipelineDynamicStateCreateInfo setDynamicState(const std::vector<vk::DynamicState>& dynamicStates)
{
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates    = dynamicStates.data();
	return dynamicStateInfo;
}

 vk::PipelineVertexInputStateCreateInfo setVertexInput(	const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions,
 	const vk::VertexInputBindingDescription  &       bindingDescription)
{
vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.vertexBindingDescriptionCount   = 1;
	vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
	return vertexInputInfo;
}





} // namespace ccn