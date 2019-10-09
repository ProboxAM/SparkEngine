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
	float4 color;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<float3> debug_normals;

	Texture* tex;
	uint tex_id;

	uint vao, vbo, ebo, dg_nm_vbo;
	void PrepareMesh();
};

#endif // !_MESH_H_
