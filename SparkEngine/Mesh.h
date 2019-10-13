#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Globals.h"

enum BUFFER_TYPE
{
	BUFF_VERT,
	BUFF_UV,
	BUFF_NORM,
	BUFF_IND,
	BUFF_DEBUG_VERT_NORM,
	BUFF_DEBUG_FACE_NORM,
	BUFF_TOTAL
};

class Mesh
{
public:
	std::vector<float3> vertices;
	std::vector<float2> uv;
	std::vector<float3> normal;
	std::vector<uint> indices;

	std::vector<float3> debug_vertex_normals;

	uint buffers[BUFF_TOTAL];
	void PrepareBuffers();
};

#endif // !_MESH_H_
