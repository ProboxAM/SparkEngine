#include "glew\glew.h"
#include "Texture.h"
#include "Mesh.h"

void Mesh::PrepareMesh()
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[BUFF_IND]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

	//glBindVertexArray(0);

	/*glGenBuffers(1, &dg_nm_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, dg_nm_vbo);
	glBufferData(GL_ARRAY_BUFFER, debug_normals.size() * sizeof(float3), &debug_normals[0], GL_STATIC_DRAW);

	// debug vertex normals
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), (void*)0);

	glBindVertexArray(0);*/
}
