#pragma once

#include "common.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace ccn
{
class ModelLoader
{
  public:
	bool loadModel(const std::string &path);

	const std::vector<Vertex> &getVertices() const
	{
		return vertices;
	}
	const std::vector<uint16_t> &getIndices() const
	{
		return indices;
	}
	const std::vector<std::string> &getTextures() const
	{
		return textures;
	}

  private:
	void processNode(aiNode *node, const aiScene *scene);
	void processMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<ccn::Vertex> vertices;
	std::vector<uint16_t>    indices;
	std::vector<std::string> textures;
};
}        // namespace ccn