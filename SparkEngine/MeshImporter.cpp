#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

#define PAR_SHAPES_IMPLEMENTATION
#include "Par/par_shapes.h"

#include "Mesh.h"

#include "MeshImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

MeshImporter::MeshImporter()
{
}


MeshImporter::~MeshImporter()
{
}

bool MeshImporter::Init()
{
	return true;
}

bool MeshImporter::CleanUp()
{
	return true;
}

Mesh* MeshImporter::Load(const char * exported_file)
{
	return nullptr;
}

Mesh* MeshImporter::Import(const aiScene* scene, const aiMesh* mesh)
{
	Mesh* resource = new Mesh();

	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vertex;
		resource->vertices.push_back(float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		resource->normal.push_back(float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));

		if (mesh->mTextureCoords[0]) //Only take in count first texture
		{
			resource->uv.push_back(float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}
		else
			resource->uv.push_back(float2(0.0f, 0.0f)); //Default to 0,0

														//DEBUG NORMAL VERTEX
		resource->debug_vertex_normals.push_back(resource->vertices[i]);
		resource->debug_vertex_normals.push_back(resource->vertices[i] + (resource->normal[i].Normalized() * DEBUG_NORMAL_LENGTH));

	}
	for (uint i = 0; i < mesh->mNumFaces; i++) //ASSUME FACE IS TRIANGLE
	{
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
		{
			resource->indices.push_back(face.mIndices[j]);
		}

		//Calculate center of face
		float center_x = (resource->vertices[face.mIndices[0]].x + resource->vertices[face.mIndices[1]].x + resource->vertices[face.mIndices[2]].x) / 3;
		float center_y = (resource->vertices[face.mIndices[0]].y + resource->vertices[face.mIndices[1]].y + resource->vertices[face.mIndices[2]].y) / 3;
		float center_z = (resource->vertices[face.mIndices[0]].z + resource->vertices[face.mIndices[1]].z + resource->vertices[face.mIndices[2]].z) / 3;

		float3 center = float3(center_x, center_y, center_z);

		//Calculate normal of face. Create 2 vector from face edges and calculate normal with cross product
		float3 edge_1 = (resource->vertices[face.mIndices[1]] - resource->vertices[face.mIndices[0]]);
		float3 edge_2 = (resource->vertices[face.mIndices[2]] - resource->vertices[face.mIndices[0]]);

		float3 normal;
		normal.x = (edge_1.y * edge_2.z) - (edge_1.z * edge_2.y);
		normal.y = (edge_1.z * edge_2.x) - (edge_1.x * edge_2.z);
		normal.z = (edge_1.x * edge_2.y) - (edge_1.y * edge_2.x);

		resource->debug_face_normals.push_back(center);
		resource->debug_face_normals.push_back(center + (normal.Normalized() * DEBUG_NORMAL_LENGTH));
	}

	LOG("New mesh with %d vertices", resource->vertices.size());
	resource->PrepareBuffers();

	return resource;
}


Mesh* MeshImporter::LoadPrimitive(PRIMITIVE_TYPE type)
{
	Mesh* new_mesh = new Mesh();
	par_shapes_mesh* primitive_mesh = nullptr;

	switch (type)
	{
	case P_CUBE:
		primitive_mesh = par_shapes_create_cube();
		break;
	case P_SPHERE:
		primitive_mesh = par_shapes_create_parametric_sphere(10, 10);
		break;
	case P_CYLINDER:
		primitive_mesh = par_shapes_create_cylinder(10, 10);
		break;
	case P_PLANE:
		primitive_mesh = par_shapes_create_plane(10, 10);
		break;
	case P_CONE:
		primitive_mesh = par_shapes_create_cone(10, 10);
		break;
	default:
		break;
	}

	for (int i = 0; i < primitive_mesh->npoints * 3;)
	{
		float3 vertex;
		vertex.x = primitive_mesh->points[i];
		vertex.y = primitive_mesh->points[i + 1];
		vertex.z = primitive_mesh->points[i + 2];
		new_mesh->vertices.push_back(vertex);

		if (primitive_mesh->normals)
		{
			float3 normal;
			normal.x = primitive_mesh->normals[i];
			normal.y = primitive_mesh->normals[i + 1];
			normal.z = primitive_mesh->normals[i + 2];
			new_mesh->normal.push_back(normal);

			new_mesh->debug_vertex_normals.push_back(vertex);
			new_mesh->debug_vertex_normals.push_back(vertex + (normal.Normalized() * DEBUG_NORMAL_LENGTH));
		}

		i += 3;
	}

	if (primitive_mesh->tcoords)
	{
		for (int i = 0; i < primitive_mesh->npoints * 2;)
		{
			float2 uv;
			uv.x = primitive_mesh->tcoords[i];
			uv.y = primitive_mesh->tcoords[i + 1];
			new_mesh->uv.push_back(uv);

			i += 2;
		}
	}

	new_mesh->indices.insert(new_mesh->indices.end(), &primitive_mesh->triangles[0], &primitive_mesh->triangles[primitive_mesh->ntriangles * 3]);

	//DEBUG	
	for (int i = 0; i < primitive_mesh->ntriangles * 3;)
	{
		//Calculate center of face
		float center_x = (new_mesh->vertices[primitive_mesh->triangles[i]].x + new_mesh->vertices[primitive_mesh->triangles[i + 1]].x + new_mesh->vertices[primitive_mesh->triangles[i + 2]].x) / 3;
		float center_y = (new_mesh->vertices[primitive_mesh->triangles[i]].y + new_mesh->vertices[primitive_mesh->triangles[i + 1]].y + new_mesh->vertices[primitive_mesh->triangles[i + 2]].y) / 3;
		float center_z = (new_mesh->vertices[primitive_mesh->triangles[i]].z + new_mesh->vertices[primitive_mesh->triangles[i + 1]].z + new_mesh->vertices[primitive_mesh->triangles[i + 2]].z) / 3;

		float3 center = float3(center_x, center_y, center_z);

		//Calculate normal of face. Create 2 vector from face edges and calculate normal with cross product
		float3 edge_1 = (new_mesh->vertices[primitive_mesh->triangles[i + 1]] - new_mesh->vertices[primitive_mesh->triangles[i]]);
		float3 edge_2 = (new_mesh->vertices[primitive_mesh->triangles[i + 2]] - new_mesh->vertices[primitive_mesh->triangles[i]]);

		float3 normal;
		normal.x = (edge_1.y * edge_2.z) - (edge_1.z * edge_2.y);
		normal.y = (edge_1.z * edge_2.x) - (edge_1.x * edge_2.z);
		normal.z = (edge_1.x * edge_2.y) - (edge_1.y * edge_2.x);

		new_mesh->debug_face_normals.push_back(center);
		new_mesh->debug_face_normals.push_back(center + (normal.Normalized() * DEBUG_NORMAL_LENGTH));

		i += 3;
	}

	par_shapes_free_mesh(primitive_mesh);
	new_mesh->PrepareBuffers();

	return new_mesh;
}