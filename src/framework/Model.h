//
// Created by tanhao on 2025/8/6.
//

#ifndef MODEL_H
#define MODEL_H
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

namespace ccn
{

class Model
{
  public:
	 Model();
	~Model();

  public:
	std::string              path;
	std::vector<Vertex>      vertices;
	std::vector<uint32_t>    indices;
	std::vector<std::string> textures;
};

}        // namespace ccn

#endif        // MODEL_H
