#ifndef _MESH_H_
#define _MESH_H_

#include "Globals.h"

class Mesh
{
public:
	float* vertices;
	uint* indices;
	float* normals;

	uint total_vertices;
	uint total_indices;
	uint total_normals;

	uint id_indices;
	uint id_vertices;
	uint id_normals;
};

#endif // !_MESH_H_
