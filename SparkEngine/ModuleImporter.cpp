#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"

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

#include "ModuleImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")


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
		std::string file;
		App->fsystem->SplitFilePath(final_path.c_str(), nullptr, &file);
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			GameObject* new_object = App->scene->CreateGameObject(nullptr, file + std::to_string(i));

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
	{
		App->fsystem->CopyFromOutsideFS(normalized_path.c_str(), ASSETS_FOLDER);
	}

	if (extension == "fbx")
	{
		LoadFBXFile(file.c_str());
	}
	else if (extension == "png" || extension == "dds")
	{
		if (App->scene->selected_gameobject)
		{
			ComponentTexture* c_tex = (ComponentTexture*)App->scene->selected_gameobject->GetComponent(COMPONENT_TYPE::TEXTURE);
			c_tex->AddTexture(App->textures->LoadTexture(file.c_str()));
			LOG("Applied texture: %s to GameObject: %s", file.c_str(), App->scene->selected_gameobject->GetName().c_str());
		}	
	}	
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
		new_mesh->debug_vertex_normals.push_back(new_mesh->vertices[i]);
		new_mesh->debug_vertex_normals.push_back(new_mesh->vertices[i] + (new_mesh->normal[i].Normalized() * DEBUG_NORMAL_LENGTH));

		//DEBUG NORMAL FACE
		//sacar centro triangulo 

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

	if (mesh->mMaterialIndex >= 0)
	{
		aiString texture_path;
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

		ComponentTexture* c_text = (ComponentTexture*)object->AddComponent(COMPONENT_TYPE::TEXTURE);

		if (texture_path.length > 0)
		{
			c_text->AddTexture(App->textures->LoadTexture(texture_path.C_Str()));
		}
		else
		{
			c_text->AddTexture(App->textures->GetDefaultTexture());
			LOG("Default texture applied to %s", object->GetName().c_str());
		}
			
	}
	LOG("New mesh with %d vertices", new_mesh->vertices.size());
	new_mesh->PrepareBuffers();

	return new_mesh;
}