//
// Created by tanhao on 2025/8/8.
//

#include "Mesh.h"

#include <utility>

#include "Reference.h"
ccn::MeshCube::MeshCube()
{
	Cube_24 cube;
	Mesh::setVertices(cube.vertices);
	Mesh::setIndices(cube.indices);
}

bool ccn::Mesh::setVertices(std::vector<ccn::Vertex> vertices)
{
	m_vertices = vertices;
	return true;
}
bool ccn::Mesh::setIndices(std::vector<uint16_t> indices)
{
	m_indices = indices;
	return true;
}