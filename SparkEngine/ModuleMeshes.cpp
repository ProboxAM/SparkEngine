#include "GameObject.h"
#include "ModuleTextures.h"
#include "Mesh.h"
#include "Texture.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#define PAR_SHAPES_IMPLEMENTATION
#include "Par/par_shapes.h"

#include "ModuleMeshes.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleMeshes::ModuleMeshes(bool active): Module(active)
{
}


ModuleMeshes::~ModuleMeshes()
{

}


Mesh* ModuleMeshes::LoadMesh(const aiScene* scene, aiMesh* mesh)
{
	Mesh* new_mesh = new Mesh();
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vertex;
		new_mesh->vertices.push_back(float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		new_mesh->normal.push_back(float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));

		if (mesh->mTextureCoords[0]) //Only take in count first texture
		{
			new_mesh->uv.push_back(float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}
		else
			new_mesh->uv.push_back(float2(0.0f, 0.0f)); //Default to 0,0

		//DEBUG NORMAL VERTEX
		new_mesh->debug_vertex_normals.push_back(new_mesh->vertices[i]);
		new_mesh->debug_vertex_normals.push_back(new_mesh->vertices[i] + (new_mesh->normal[i].Normalized() * DEBUG_NORMAL_LENGTH));

	}
	for (uint i = 0; i < mesh->mNumFaces; i++) //ASSUME FACE IS TRIANGLE
	{
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
		{
			new_mesh->indices.push_back(face.mIndices[j]);
		}

		//Calculate center of face
		float center_x = (new_mesh->vertices[face.mIndices[0]].x + new_mesh->vertices[face.mIndices[1]].x + new_mesh->vertices[face.mIndices[2]].x) / 3;
		float center_y = (new_mesh->vertices[face.mIndices[0]].y + new_mesh->vertices[face.mIndices[1]].y + new_mesh->vertices[face.mIndices[2]].y) / 3;
		float center_z = (new_mesh->vertices[face.mIndices[0]].z + new_mesh->vertices[face.mIndices[1]].z + new_mesh->vertices[face.mIndices[2]].z) / 3;

		float3 center = float3(center_x, center_y, center_z);

		//Calculate normal of face. Create 2 vector from face edges and calculate normal with cross product
		float3 edge_1 = (new_mesh->vertices[face.mIndices[1]] - new_mesh->vertices[face.mIndices[0]]);
		float3 edge_2 = (new_mesh->vertices[face.mIndices[2]] - new_mesh->vertices[face.mIndices[0]]);

		float3 normal;
		normal.x = (edge_1.y * edge_2.z) - (edge_1.z * edge_2.y);
		normal.y = (edge_1.z * edge_2.x) - (edge_1.x * edge_2.z);
		normal.z = (edge_1.x * edge_2.y) - (edge_1.y * edge_2.x);

		new_mesh->debug_face_normals.push_back(center);
		new_mesh->debug_face_normals.push_back(center + (normal.Normalized() * DEBUG_NORMAL_LENGTH));
	}

	LOG("New mesh with %d vertices", new_mesh->vertices.size());
	new_mesh->PrepareBuffers();

	return new_mesh;
}

Mesh * ModuleMeshes::CreateSphereMesh()
{
	Mesh* new_mesh = new Mesh();
	par_shapes_mesh* sphere = par_shapes_create_parametric_sphere(10, 10);
	
	for (int i = 0; i < sphere->npoints * 3;)
	{
		float3 vertex;
		vertex.x = sphere->points[i];
		vertex.y = sphere->points[i + 1];
		vertex.z = sphere->points[i + 2];
		new_mesh->vertices.push_back(vertex);

		float3 normal;
		normal.x = sphere->normals[i];
		normal.y = sphere->normals[i + 1];
		normal.z = sphere->normals[i + 2];
		new_mesh->normal.push_back(normal);

		new_mesh->debug_vertex_normals.push_back(vertex);
		new_mesh->debug_vertex_normals.push_back(vertex + (normal.Normalized() * DEBUG_NORMAL_LENGTH));

		i += 3;
	}
	for (int i = 0; i < sphere->npoints * 2;)
	{
		float2 uv;
		uv.x = sphere->tcoords[i];
		uv.y = sphere->tcoords[i + 1];
		new_mesh->uv.push_back(uv);

		i += 2;
	}

	new_mesh->indices.insert(new_mesh->indices.end(), &sphere->triangles[0], &sphere->triangles[sphere->ntriangles*3]);

	//DEBUG	
	for (int i = 0; i < sphere->ntriangles * 3;)
	{
		//Calculate center of face
		float center_x = (new_mesh->vertices[sphere->triangles[i]].x + new_mesh->vertices[sphere->triangles[i+1]].x + new_mesh->vertices[sphere->triangles[i+2]].x) / 3;
		float center_y = (new_mesh->vertices[sphere->triangles[i]].y + new_mesh->vertices[sphere->triangles[i+1]].y + new_mesh->vertices[sphere->triangles[i+2]].y) / 3;
		float center_z = (new_mesh->vertices[sphere->triangles[i]].z + new_mesh->vertices[sphere->triangles[i+1]].z + new_mesh->vertices[sphere->triangles[i+2]].z) / 3;

		float3 center = float3(center_x, center_y, center_z);

		//Calculate normal of face. Create 2 vector from face edges and calculate normal with cross product
		float3 edge_1 = (new_mesh->vertices[sphere->triangles[i+1]] - new_mesh->vertices[sphere->triangles[i]]);
		float3 edge_2 = (new_mesh->vertices[sphere->triangles[i+2]] - new_mesh->vertices[sphere->triangles[i]]);

		float3 normal;
		normal.x = (edge_1.y * edge_2.z) - (edge_1.z * edge_2.y);
		normal.y = (edge_1.z * edge_2.x) - (edge_1.x * edge_2.z);
		normal.z = (edge_1.x * edge_2.y) - (edge_1.y * edge_2.x);

		new_mesh->debug_face_normals.push_back(center);
		new_mesh->debug_face_normals.push_back(center + (normal.Normalized() * DEBUG_NORMAL_LENGTH));

		i += 3;
	}

	par_shapes_free_mesh(sphere);
	new_mesh->PrepareBuffers();

	return new_mesh;
}

Mesh * ModuleMeshes::CreateSquareMesh()
{
	Mesh* new_mesh = new Mesh();

	return new_mesh;
}
