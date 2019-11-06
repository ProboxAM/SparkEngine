#include "glew\glew.h"
#include "Texture.h"
#include "Mesh.h"

void Mesh::PrepareBuffers()
{
	glGenBuffers(BUFF_TOTAL, buffers);

	// vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_VERT]);
	glBufferData(GL_ARRAY_BUFFER, total_vertices * sizeof(float3), vertices, GL_STATIC_DRAW);

	// vertex normals
	if (total_normal > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_NORM]);
		glBufferData(GL_ARRAY_BUFFER, total_normal * sizeof(float3), normal, GL_STATIC_DRAW);

		// debug vertex normals
		//glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_DEBUG_VERT_NORM]);
		//glBufferData(GL_ARRAY_BUFFER, debug_vertex_normals.size() * sizeof(float3), &debug_vertex_normals[0], GL_STATIC_DRAW);
	}

	if (total_uv > 0)
	{
		// vertex texture coords
		glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_UV]);
		glBufferData(GL_ARRAY_BUFFER, total_uv * sizeof(float2), uv, GL_STATIC_DRAW);
	}

	//faces
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[BUFF_IND]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, total_indices * sizeof(uint), indices, GL_STATIC_DRAW);

	// debug face normals
	//glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_DEBUG_FACE_NORM]);
	//glBufferData(GL_ARRAY_BUFFER, debug_face_normals.size() * sizeof(float3), &debug_face_normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
