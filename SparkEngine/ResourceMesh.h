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
	ResourceMesh(uint id, ResourceMesh* m);
	std::string name;

	float3* vertices = nullptr;
	float2* uv = nullptr;
	float3* normal = nullptr;
	uint* indices = nullptr;

	uint total_vertices = 0;
	uint total_uv = 0;
	uint total_normal = 0;
	uint total_indices = 0;

	//std::vector<float3> debug_vertex_normals;
	//std::vector<float3> debug_face_normals;
	std::string GetTypeString() const;
	uint buffers[BUFF_TOTAL];
	void PrepareBuffers();
	void UnLoad();
	void Load();
	void Reset();
};

#endif // !_MESH_H_
