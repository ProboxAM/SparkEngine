#include "Application.h"
#include "ModuleImporter.h"
#include "MeshImporter.h"
#include "glew\glew.h"
#include "ResourceMesh.h"

ResourceMesh::~ResourceMesh()
{
}

ResourceMesh::ResourceMesh(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_MESH)
{
}

ResourceMesh::ResourceMesh(uint id, ResourceMesh * m): Resource(id, Resource::RESOURCE_TYPE::R_MESH)
{
	total_indices = m->total_indices;
	total_vertices = m->total_vertices;
	total_normal = m->total_normal;
	total_uv = m->total_uv;

	// Load indices
	uint bytes = sizeof(uint) * total_indices;
	indices = new uint[total_indices];
	memcpy(indices, m->indices, bytes);

	// Load vertices
	bytes = sizeof(float3) * total_vertices;
	vertices = new float3[total_vertices];
	memcpy(vertices, m->vertices, bytes);

	if (total_normal)
	{
		// Load normal
		bytes = sizeof(float3) * total_normal;
		normal = new float3[total_normal];
		memcpy(normal, m->normal, bytes);
	}
	if (total_uv)
	{
		// Load uv
		bytes = sizeof(float2) * total_uv;
		uv = new float2[total_uv];
		memcpy(uv, m->uv, bytes);
	}

	PrepareBuffers();
}

std::string ResourceMesh::GetTypeString() const
{
	return "Mesh";
}

void ResourceMesh::PrepareBuffers()
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

void ResourceMesh::UnLoad()
{
	LOG("Unloading Mesh %s", exported_file.c_str());
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	if (normal)
	{
		delete[] normal;
		normal = nullptr;
	}

	if (uv)
	{
		delete[] uv;
		uv = nullptr;
	}

	glDeleteBuffers(BUFF_TOTAL, buffers);
}

void ResourceMesh::Load()
{
	App->importer->mesh->Load(this);
}
