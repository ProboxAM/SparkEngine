#ifndef _MESH_H_
#define _MESH_H_

#include "Globals.h"

class Mesh
{
public:
	float* vertices;
	uint* indices;

	uint total_vertices;
	uint total_indices;

	uint id_indices;
	uint id_vertices;
};

#endif // !_MESH_H_
