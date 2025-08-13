//
// Created by tanhao on 2025/8/3.
//

#include "Shader.h"

#include <iostream>

namespace ccn
{

Shader::Shader(vk::Device indevice) :
    device(indevice)
{
}
Shader::~Shader()
{
}
void Shader::loadCode(std::string filename)
{
	code = readFile(filename);
}
void Shader::createShaderMoudle()
{
	if (code.size() % 4 != 0)
	{
		throw std::runtime_error("Shader code size must be a multiple of 4.");
	}

	vk::ShaderModuleCreateInfo create_info{};
	create_info.codeSize = code.size();
	create_info.pCode    = reinterpret_cast<const uint32_t *>(code.data());

	try
	{
		shader_module = device.createShaderModule(create_info);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Failed to create shader module: " << e.what()
		          << "\nCode size: " << code.size() << " bytes\n";
		throw;
	}
}

void Shader::setStageInfo(vk::ShaderStageFlagBits Flag)
{
	ShaderStageInfo.stage  = Flag;
	ShaderStageInfo.module = shader_module;
	ShaderStageInfo.pName  = "main";
}

}        // namespace ccn