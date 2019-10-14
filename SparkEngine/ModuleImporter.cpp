#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"



#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "Mesh.h"
#include "Texture.h"


#include "ModuleRenderer3D.h"

#define ILUT_USE_OPENGL
#include "DeviL/include/ilut.h"

#include "ModuleImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib,"DeviL/lib/DevIL.lib")
#pragma comment (lib,"DeviL/lib/ILUT.lib")
#pragma comment (lib,"DeviL/lib/ILU.lib")

void LogCallback(const char* text, char* data)
{
	std::string temp_string = text;
	temp_string.erase(std::remove(temp_string.begin(), temp_string.end(), '%'), temp_string.end());
	LOG(temp_string.c_str());
}


ModuleImporter::ModuleImporter(bool start_enabled)
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init(nlohmann::json::iterator it)
{
	ilInit();
	iluInit();
	ilutInit();
	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);
	ilutRenderer(ILUT_OPENGL);

	aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);


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

void ModuleImporter::LoadFBXFile(const char * file)
{
	std::string final_path = ASSETS_FOLDER + std::string(file);

	const aiScene* scene = aiImportFile(final_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			GameObject* new_object = App->scene->CreateGameObject();

			ComponentMesh* c_mesh = (ComponentMesh*) new_object->AddComponent(COMPONENT_TYPE::MESH);
			c_mesh->AddMesh(LoadMesh(scene, scene->mMeshes[i], new_object));
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading file %s", file);
}

void ModuleImporter::ImportFile(const char * path)
{
	std::string normalized_path = path;
	App->fsystem->NormalizePath(normalized_path);

	std::string extension, file;
	App->fsystem->SplitFilePath(path, nullptr, &file, &extension);

	if (!App->fsystem->Exists(std::string(ASSETS_FOLDER + file).c_str())) //if file doesnt exist we copy it
		App->fsystem->CopyFromOutsideFS(normalized_path.c_str(), ASSETS_FOLDER);

	if (extension == "fbx")
	{
		LoadFBXFile(file.c_str());
	}
	else if (extension == "png" || extension == "dds")
	{
		//TODO: IMPORT TEXTURE AND APPLY TO SELECTED GAMEOBJECT
		//App->renderer3D->test_texture = LoadTexture(file.c_str());
	}	
}

Texture* ModuleImporter::LoadTexture(const char* path)
{
	Texture* tex = new Texture();
	uint image_id;

	std::string final_path = ASSETS_FOLDER + std::string(path);

	ilGenImages(1, &image_id); // Grab a new image name.
	ilBindImage(image_id);
	ilLoadImage(final_path.c_str());
	tex->id = ilutGLBindTexImage();
	tex->width = ilGetInteger(IL_IMAGE_WIDTH);
	tex->height = ilGetInteger(IL_IMAGE_HEIGHT);
	tex->path = path;
	glBindTexture(GL_TEXTURE_2D, 0);

	ilDeleteImages(1, &image_id);

	LOG("Loaded Texture %s", path);

	return tex;
}

Mesh* ModuleImporter::LoadMesh(const aiScene* scene, aiMesh* mesh, GameObject* object)
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
		new_mesh->debug_vertex_normals.push_back(float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		new_mesh->debug_vertex_normals.push_back(float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z) + float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
	}
	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
			new_mesh->indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiString texture_path;
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

		ComponentTexture* c_text = (ComponentTexture*)object->AddComponent(COMPONENT_TYPE::TEXTURE);

		if (texture_path.length > 0)
			c_text->AddTexture(LoadTexture(texture_path.C_Str()));
	}

	LOG("New mesh with %d vertices", new_mesh->vertices.size());
	new_mesh->PrepareBuffers();

	return new_mesh;
}