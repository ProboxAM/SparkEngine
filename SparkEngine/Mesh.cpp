#include "glew\glew.h"
#include "Texture.h"
#include "Mesh.h"

void Mesh::PrepareBuffers()
{
	glGenBuffers(4, buffers);

	// vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_VERT]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float3), &vertices[0], GL_STATIC_DRAW);

	// vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_NORM]);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(float3), &normal[0], GL_STATIC_DRAW);

	// vertex texture coords
	glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_UV]);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(float2), &uv[0], GL_STATIC_DRAW);

	//faces
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[BUFF_IND]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

	// debug vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_DEBUG_VERT_NORM]);
	glBufferData(GL_ARRAY_BUFFER, debug_vertex_normals.size() * sizeof(float3), &debug_vertex_normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
