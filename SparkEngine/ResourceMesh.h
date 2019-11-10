#ifndef _RESOURCE_MESH_H_
#define _RESOURCE_MESH_H_

#include "Resource.h"

enum BUFFER_TYPE
{
	BUFF_VERT,
	BUFF_UV,
	BUFF_NORM,
	BUFF_IND,
	//BUFF_DEBUG_VERT_NORM,
	//BUFF_DEBUG_FACE_NORM,
	BUFF_TOTAL
};

class ResourceMesh : public Resource
{
public:
	~ResourceMesh();
	ResourceMesh(uint id);

	float3* vertices;
	float2* uv;
	float3* normal;
	uint* indices;

	uint total_vertices;
	uint total_uv;
	uint total_normal;
	uint total_indices;

	//std::vector<float3> debug_vertex_normals;
	//std::vector<float3> debug_face_normals;

	uint buffers[BUFF_TOTAL];
	void PrepareBuffers();
};

#endif // !_MESH_H_
