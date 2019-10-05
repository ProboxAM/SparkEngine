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

	const aiScene* scene = aiImportFile(file, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh new_mesh;
			new_mesh.total_vertices = scene->mMeshes[i]->mNumVertices * 3;
			new_mesh.vertices = new float[new_mesh.total_vertices];
			memcpy(new_mesh.vertices, scene->mMeshes[i]->mVertices, sizeof(float) * new_mesh.total_vertices);

			if (scene->mMeshes[i]->HasFaces())
			{
				new_mesh.total_indices = scene->mMeshes[i]->mNumFaces * 3;
				new_mesh.indices = new uint[new_mesh.total_indices]; // assume each face is a triangle
				for (uint y = 0; y < scene->mMeshes[i]->mNumFaces; ++y)
				{
					if (scene->mMeshes[i]->mFaces[y].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}			
					else
						memcpy(&new_mesh.indices[y * 3], scene->mMeshes[i]->mFaces[y].mIndices, 3 * sizeof(uint));
				}
			}
			if (scene->mMeshes[i]->HasNormals())
			{
				new_mesh.total_normals = new_mesh.total_vertices;
				new_mesh.normals = new float[new_mesh.total_vertices];
				memcpy(new_mesh.normals, scene->mMeshes[i]->mNormals, sizeof(float) * new_mesh.total_normals);
			}

			meshes.push_back(new_mesh);
			LOG("New mesh with %d vertices", new_mesh.total_vertices);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading file %s", file);

	return meshes;
}
