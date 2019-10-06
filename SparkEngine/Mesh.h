#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Globals.h"

struct Vertex
{
	float3 position;
	float3 normal;
	float2 uv;
	float4 color;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint> indices;

	unsigned int vao, vbo, ebo;
	void PrepareMesh();
};

#endif // !_MESH_H_
