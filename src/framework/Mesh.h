//
// Created by tanhao on 2025/8/8.
//

#ifndef MESH_H
#define MESH_H
#include "common.h"

#include <glm/fwd.hpp>
#include <vector>
namespace ccn
{
class Mesh
{
  public:
	 Mesh() = default;
	~Mesh() = default;

	virtual bool setVertices(std::vector<ccn::Vertex> m_vertices);
	virtual bool setIndices(std::vector<uint16_t> m_indices);


  public:
	std::vector<ccn::Vertex> m_vertices;
	std::vector<uint16_t>    m_indices;
};

class MeshCube : public Mesh
{
  public:
	 MeshCube();
	~MeshCube() = default;
};
}        // namespace ccn
#endif        // MESH_H
