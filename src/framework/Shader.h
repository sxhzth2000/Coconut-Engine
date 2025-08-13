//
// Created by tanhao on 2025/8/3.
//

#ifndef SHADER_H
#define SHADER_H
#include <vulkan/vulkan.hpp>

#include "common.h"
#include <vector>
// 在某个公共头文件中，比如 Shader.h 顶部
namespace shader_type
{
constexpr vk::ShaderStageFlagBits vertexShader   = vk::ShaderStageFlagBits::eVertex;
constexpr vk::ShaderStageFlagBits fragmentShader = vk::ShaderStageFlagBits::eFragment;
}        // namespace shader_type

namespace ccn
{

class Shader
{
  public:
	 Shader(vk::Device indevice);        // 声明构造函数
	~Shader();

	void loadCode(std::string filename);
	void createShaderMoudle();
	void setStageInfo(vk::ShaderStageFlagBits Flag);

	std::vector<char>                 code;
	vk::ShaderModule                  shader_module;
	vk::PipelineShaderStageCreateInfo ShaderStageInfo;

	vk::Device device;
};

}        // namespace ccn

#endif        // SHADER_H
