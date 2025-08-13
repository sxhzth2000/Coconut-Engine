//
// Created by tanhao on 2025/7/12.
//

#ifndef COMMON_H
#define COMMON_H
#include "utils/stb_image.h"

#include <fstream>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
namespace ccn
{

uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice);




bool createBuffer( vk::Device& device, vk::PhysicalDevice physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
							   vk::Buffer &buffer, vk::DeviceMemory &memory, const void *data);



 std::vector<char> readFile(const std::string &filename);

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 color;        // 你已有的 color 也保留
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

struct triangle
{
	std::vector<Vertex> vertices = {
	    {{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},        // bottom center, red
	    {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},         // top right, green
	    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},        // top left, blue
	};
};

struct VertexPC
{
	float x, y, z, w;        // Position
	float r, g, b, a;        // Color
};

struct VertexPT
{
	float x, y, z, w;        // Position data
	float u, v;              // texture u,v
};

struct Cube_24
{

	//RightHand,Y_up;
	std::vector<Vertex> vertices = {
	    // Front face (white)
	    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {.0, .0, 1}},
	    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {.0, .0, 1}},
	    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {.0, .0, 1}},
	    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {.0, .0, 1}},

	    // Back face (red)
	    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {.0, .0, -1}},
	    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {.0, .0, -1}},
	    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {.0, .0, -1}},
	    {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {.0, .0, -1}},

	    // Left face (green)
	    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {-1, .0, .0}},
	    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {-1, .0, .0}},
	    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {-1, .0, .0}},
	    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {-1, .0, .0}},

	    // Right face (blue)
	    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1, .0, .0}},
	    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1, .0, .0}},
	    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1, .0, .0}},
	    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1, .0, .0}},

	    // Top face (yellow)
	    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {.0, 1, .0}},
	    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {.0, 1, .0}},
	    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {.0, 1, .0}},
	    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {.0, 1, .0}},

	    // Bottom face (cyan)
	    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {.0, -1, .0}},
	    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {.0, -1, .0}},
	    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {.0, -1, .0}},
	    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {.0, -1, .0}},
	};

	//eCounterClockwise
	std::vector<uint16_t> indices = {
	    0, 1, 2, 2, 3, 0,              // Front
	    4, 6, 5, 4, 7, 6,              // Back
	    8, 9, 10, 10, 11, 8,           // Left
	    12, 13, 14, 14, 15, 12,        // Right
	    16, 17, 18, 18, 19, 16,        // Top
	    20, 21, 22, 22, 23, 20         // Bottom
	};
};

};        // namespace ccn

#endif        // COMMON_H
