#include "Globals.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Mesh.h"
#include "Texture.h"

#define ILUT_USE_OPENGL
#include "DeviL/include/ilut.h"

#include "ModuleImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib,"DeviL/lib/DevIL.lib")
#pragma comment (lib,"DeviL/lib/ILUT.lib")
#pragma comment (lib,"DeviL/lib/ILU.lib")


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

	ilInit();
	iluInit();
	ilutInit();
	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);
	ilutRenderer(ILUT_OPENGL);

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
			Mesh new_mesh = LoadMesh(scene, scene->mMeshes[i]);
			new_mesh.PrepareMesh();
			meshes.push_back(new_mesh);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading file %s", file);

	return meshes;
}


Mesh ModuleImporter::LoadMesh(const aiScene* scene, aiMesh* mesh)
{
	Mesh new_mesh;
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vertex;
		new_mesh.vertices.push_back(float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		new_mesh.normal.push_back(float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));

		if (mesh->mTextureCoords[0]) //Only take in count first texture
		{
			new_mesh.uv.push_back(float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}
		else
			new_mesh.uv.push_back(float2(0.0f, 0.0f)); //Default to 0,0

		//DEBUG NORMAL VERTEX
		//new_mesh.debug_normals.push_back(vertex.position);
		//new_mesh.debug_normals.push_back(vertex.position + vertex.normal);

		//new_mesh.vertices.push_back(vertex);
	}
	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
			new_mesh.indices.push_back(face.mIndices[j]);
	}

	/*if (mesh->mMaterialIndex >= 0)
	{
		new_mesh.tex = new Texture();
		new_mesh.tex->LoadTexture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);
	}*/


	LOG("New mesh with %d vertices", new_mesh.vertices.size());

	return new_mesh;
}