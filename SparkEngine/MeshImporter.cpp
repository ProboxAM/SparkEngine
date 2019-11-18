#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

#define PAR_SHAPES_IMPLEMENTATION
#include "Par/par_shapes.h"

#include "ResourceMesh.h"

#include "MeshImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#define CUBE_ID 999998
#define SPHERE_ID 999997
#define CYLINDER_ID 999996
#define CONE_ID 999995
#define PLANE_ID 999994

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

bool MeshImporter::Start()
{
	ResourceMesh* mesh = (ResourceMesh*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_MESH, CUBE_ID);
	LoadPrimitive(PRIMITIVE_TYPE::P_CUBE, mesh);
	mesh->AddReference();
	mesh->SetFile("Cube");
	cube = mesh->GetID();

	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_MESH, SPHERE_ID);
	LoadPrimitive(PRIMITIVE_TYPE::P_SPHERE, mesh);
	mesh->AddReference();
	mesh->SetFile("Sphere");
	sphere = mesh->GetID();

	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_MESH, CYLINDER_ID);
	LoadPrimitive(PRIMITIVE_TYPE::P_CYLINDER, mesh);
	mesh->AddReference();
	mesh->SetFile("Cylinder");
	cylinder = mesh->GetID();

	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_MESH, CONE_ID);
	LoadPrimitive(PRIMITIVE_TYPE::P_CONE, mesh);
	mesh->AddReference();
	mesh->SetFile("Cone");
	cone = mesh->GetID();

	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_MESH, PLANE_ID);
	LoadPrimitive(PRIMITIVE_TYPE::P_PLANE, mesh);
	mesh->AddReference();
	mesh->SetFile("Plane");
	plane = mesh->GetID();

	return true;
}

bool MeshImporter::CleanUp()
{
	return true;
}

bool MeshImporter::Load(ResourceMesh* resource)
{
	char* buffer;
	uint size = App->fsystem->Load(resource->GetExportedFile(), &buffer);

	char* cursor = buffer;
	// amount of indices / vertices / colors / normals / texture_coords
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	resource->total_indices = ranges[0];
	resource->total_vertices = ranges[1];
	resource->total_normal = ranges[2];
	resource->total_uv = ranges[3];
	cursor += bytes;

	// Load indices
	bytes = sizeof(uint) * resource->total_indices;
	resource->indices = new uint[resource->total_indices];
	memcpy(resource->indices, cursor, bytes);
	cursor += bytes;

	// Load vertices
	bytes = sizeof(float3) * resource->total_vertices;
	resource->vertices = new float3[resource->total_vertices];
	memcpy(resource->vertices, cursor, bytes);
	cursor += bytes;

	if (resource->total_normal)
	{
		// Load normal
		bytes = sizeof(float3) * resource->total_normal;
		resource->normal = new float3[resource->total_normal];
		memcpy(resource->normal, cursor, bytes);
		cursor += bytes;
	}
	if (resource->total_uv)
	{
		// Load uv
		bytes = sizeof(float2) * resource->total_uv;
		resource->uv = new float2[resource->total_uv];
		memcpy(resource->uv, cursor, bytes);
		cursor += bytes;
	}
	resource->PrepareBuffers();

	RELEASE_ARRAY(buffer);

	return resource;
}

uint MeshImporter::Import(const aiScene* scene, const aiMesh* mesh, uint id)
{
	ResourceMesh* resource = (ResourceMesh*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_MESH, id);

	//Load vertices
	resource->total_vertices = mesh->mNumVertices;
	resource->vertices = new float3[resource->total_vertices];
	memcpy(resource->vertices, mesh->mVertices, sizeof(float3) * resource->total_vertices);

	//Load uv
	if (mesh->mTextureCoords[0])
	{
		resource->total_uv = mesh->mNumVertices;
		resource->uv = new float2[resource->total_uv];
		memcpy(resource->vertices, mesh->mVertices, sizeof(float2) * resource->total_vertices);
		for (unsigned int i = 0; i < resource->total_uv; i++)
		{
			memcpy(&resource->uv[i], &mesh->mTextureCoords[0][i], sizeof(float2));
		}
	}
	if (mesh->HasNormals())
	{
		resource->total_normal = mesh->mNumVertices;
		resource->normal = new float3[resource->total_normal];
		memcpy(resource->normal, mesh->mNormals, sizeof(float3) * resource->total_normal);
	}

	if (mesh->HasFaces())
	{
		resource->total_indices = mesh->mNumFaces * 3;
		resource->indices = new uint[resource->total_indices];
		for (uint i = 0; i < mesh->mNumFaces; i++) //ASSUME FACE IS TRIANGLE
		{
			memcpy(&resource->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	}


	//DEBUG NORMAL VERTEX
	//resource->debug_vertex_normals.push_back(resource->vertices[i]);
	//resource->debug_vertex_normals.push_back(resource->vertices[i] + (resource->normal[i].Normalized() * DEBUG_NORMAL_LENGTH));

	//Calculate center of face
	/*float center_x = (resource->vertices[face.mIndices[0]].x + resource->vertices[face.mIndices[1]].x + resource->vertices[face.mIndices[2]].x) / 3;
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

	resource->debug_face_normals.push_back(center);*/

	LOG("New mesh with %d vertices", resource->total_vertices);
	SaveMesh(resource);
	resource->UnLoad();

	return resource->GetID();
}

bool MeshImporter::SaveMesh(ResourceMesh* mesh)
{
	// amount of indices / vertices / colors / normals / texture_coords / AABB
	uint ranges[4] = { mesh->total_indices, mesh->total_vertices, mesh->total_normal, mesh->total_uv };
	uint size = sizeof(ranges) + sizeof(uint) * mesh->total_indices + sizeof(float3) * mesh->total_vertices
		+ sizeof(float3) * mesh->total_normal+ sizeof(float2) * mesh->total_uv;

	// Allocate
	char* data = new char[size]; 
	char* cursor = data;

	// First store ranges
	uint bytes = sizeof(ranges); 
	memcpy(cursor, ranges, bytes);
	cursor += bytes;
	
	// Store indices
	bytes = sizeof(uint) * mesh->total_indices;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	// Store vertices
	bytes = sizeof(float3) * mesh->total_vertices;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;

	if (mesh->total_normal > 0)
	{
		//Store normal
		bytes = sizeof(float3) * mesh->total_normal;
		memcpy(cursor, mesh->normal, bytes);
		cursor += bytes;
	}
	if (mesh->total_uv > 0)
	{
		//Store uv
		bytes = sizeof(float2) * mesh->total_uv;
		memcpy(cursor, mesh->uv, bytes);
		cursor += bytes;
	}

	std::string file = LIBRARY_MESH_FOLDER;
	file += std::to_string(mesh->GetID()) + MESH_EXTENSION;
	uint ret = App->fsystem->Save(file.c_str(), data, size);
	RELEASE_ARRAY(data);

	mesh->SetExportedFile(file);

	return true;
}


ResourceMesh* MeshImporter::LoadPrimitive(PRIMITIVE_TYPE type, ResourceMesh* mesh)
{
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

	//Load vertices
	mesh->total_vertices = primitive_mesh->npoints;
	mesh->vertices = new float3[mesh->total_vertices];
	memcpy(mesh->vertices, primitive_mesh->points, sizeof(float3) * mesh->total_vertices);

	//Load uv
	if (primitive_mesh->tcoords)
	{
		mesh->total_uv = primitive_mesh->npoints;
		mesh->uv = new float2[mesh->total_uv];
		memcpy(mesh->uv, primitive_mesh->tcoords, sizeof(float2) * mesh->total_uv);
	}
	if (primitive_mesh->normals)
	{
		mesh->total_normal = primitive_mesh->npoints;
		mesh->normal = new float3[mesh->total_normal];
		memcpy(mesh->normal, primitive_mesh->normals, sizeof(float3) * mesh->total_normal);
	}	

	mesh->total_indices = primitive_mesh->ntriangles * 3;
	mesh->indices = new uint[mesh->total_indices];
	memcpy(mesh->indices, primitive_mesh->triangles, sizeof(uint) * mesh->total_indices);

	//DEBUG	
	/*for (int i = 0; i < primitive_mesh->ntriangles * 3;)
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
	}*/

	par_shapes_free_mesh(primitive_mesh);
	mesh->PrepareBuffers();

	return mesh;
}
