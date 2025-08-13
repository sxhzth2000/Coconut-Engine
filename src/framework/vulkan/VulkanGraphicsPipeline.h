#include "VulkanContext.h"

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace ccn
{
class Shader;


std::vector<vk::PipelineShaderStageCreateInfo> createShaderModules(const vk::Device & device,
                                                                   const std::string &vertex_path,
                                                                   const std::string &fragment_path);

vk::PipelineDynamicStateCreateInfo setDynamicState(const std::vector<vk::DynamicState> &dynamicState);

vk::PipelineVertexInputStateCreateInfo setVertexInput(const std::vector<vk::VertexInputAttributeDescription> &attributeDescriptions,
                                                      const vk::VertexInputBindingDescription &               bindingDescription);


vk::PipelineLayout setPipelineLayout(const vk::Device &                       device,
                                              const vk::DescriptorSetLayout &          descriptorSetLayout,
                                              const std::vector<vk::PushConstantRange> ConstantRanges);

vk::Pipeline createGraphicsPipeline(const vk::RenderPass &         renderPass,
                                    const vk::DescriptorSetLayout &descriptorSetLayout);
};