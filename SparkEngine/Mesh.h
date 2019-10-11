#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Globals.h"

class Texture;

enum BUFFER_TYPE
{
	BUFF_VERT,
	BUFF_UV,
	BUFF_NORM,
	BUFF_IND,
	BUFF_TOTAL
};

class Mesh
{
public:
	std::vector<float3> vertices;
	std::vector<float2> uv;
	std::vector<float3> normal;
	std::vector<uint> indices;
	//std::vector<float3> debug_normals;

	Texture* tex;

	uint buffers[BUFF_TOTAL];
	void PrepareMesh();
};

#endif // !_MESH_H_
