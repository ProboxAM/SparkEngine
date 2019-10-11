#include "glew\glew.h"
#include "Texture.h"
#include "Mesh.h"

void Mesh::PrepareMesh()
{
	//glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &nbo);
	glGenBuffers(1, &uvbo);

	//glBindVertexArray(vao);

	// vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float3), &vertices[0], GL_STATIC_DRAW);
	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);*/

	// vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(float3), &normal[0], GL_STATIC_DRAW);
	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);*/

	// vertex texture coords
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(float2), &uv[0], GL_STATIC_DRAW);
/*	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);*/


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
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
