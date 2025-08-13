#include "ModelLoader.h"
#include <assimp/postprocess.h>
#include <iostream>

bool ccn::ModelLoader::loadModel(const std::string &path)
{
	std::cout << "loadmodel\n";
	Assimp::Importer importer;
	std::cout << "importer\n";
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs );
	if (!scene || !scene->mRootNode)
	{
		std::cerr << "Assimp failed: " << importer.GetErrorString() << std::endl;
		return false;
	}

	processNode(scene->mRootNode, scene);
	return true;
}

void ccn::ModelLoader::processNode(aiNode *node, const aiScene *scene)
{
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene);
}

void ccn::ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene)
{
	uint32_t indexOffset = static_cast<uint32_t>(vertices.size());
	int stop =0;
	for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
	{

		Vertex vertex;
		vertex.position = glm::vec3(
		    mesh->mVertices[i].x,
		    mesh->mVertices[i].y,
		    mesh->mVertices[i].z);

		vertex.color = glm::vec3(1.0, 1.0, 1.0);

		vertex.normal = mesh->HasNormals() ?
		                    glm::vec3(
		                        mesh->mNormals[i].x,
		                        mesh->mNormals[i].y,
		                        mesh->mNormals[i].z) :
		                    glm::vec3(0.0f);

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y);

			// 打印纹理坐标
			if (stop<20)
			{	std::cout << "Vertex " << i
						  << " TexCoord: (" << vertex.texCoord.x
						  << ", " << vertex.texCoord.y << ")\n";
				stop++;
			}


		}


		else
		{
			vertex.texCoord = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace &face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; ++j)
			indices.push_back(indexOffset + face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		aiString    str;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS)
		{
			textures.push_back(str.C_Str());
		}
	}
}