#include "Globals.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Mesh.h"
#include "ModuleImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")


void LogCallback(const char* text, char*)
{
	LOG(text);
}


ModuleImporter::ModuleImporter(bool start_enabled)
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init(nlohmann::json::iterator it)
{
	callback = LogCallback;
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	stream.callback = callback;

	return true;
}

update_status ModuleImporter::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

std::vector<Mesh> ModuleImporter::LoadFBXFile(const char * file)
{
	std::vector<Mesh> meshes;

	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh new_mesh = LoadMesh(scene->mMeshes[i]);
			new_mesh.PrepareMesh();
			meshes.push_back(new_mesh);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading file %s", file);

	return meshes;
}


Mesh ModuleImporter::LoadMesh(aiMesh* mesh)
{
	Mesh new_mesh;
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) //Only take in count first texture
		{
			vertex.uv.x = mesh->mTextureCoords[0][i].x;
			vertex.uv.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.uv = float2(0.0f, 0.0f); //Default to 0,0

		if (mesh->mColors[0]) //Only take in count first vertex color
		{
			vertex.color.x = mesh->mColors[0][i].r;
			vertex.color.y = mesh->mColors[0][i].g;
			vertex.color.z = mesh->mColors[0][i].b;
			vertex.color.w = mesh->mColors[0][i].a;
		}
		else
			vertex.color = float4(255.0f, 255.0f, 255.0f, 255.0f); //Default color white

		//DEBUG NORMAL VERTEX
		new_mesh.debug_normals.push_back(vertex.position);
		new_mesh.debug_normals.push_back(vertex.position + vertex.normal);

		new_mesh.vertices.push_back(vertex);
	}
	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
			new_mesh.indices.push_back(face.mIndices[j]);
	}
	LOG("New mesh with %d vertices", new_mesh.vertices.size());

	return new_mesh;
}