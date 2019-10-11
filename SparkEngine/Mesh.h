#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Globals.h"

class Texture;

struct Vertex
{
	float3 position;
	float3 normal;
	float2 uv;
};

class Mesh
{
public:
	std::vector<float3> vertices;
	std::vector<float2> uv;
	std::vector<float3> normal;

	std::vector<uint> indices;
	std::vector<float3> debug_normals;

	Texture* tex;
	uint tex_id;

	uint vao, vbo, ebo, nbo, uvbo;
	void PrepareMesh();
};

#endif // !_MESH_H_
