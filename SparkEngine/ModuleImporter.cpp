#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"
#include "ModuleMeshes.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"



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

bool ModuleImporter::Init(const nlohmann::json::iterator& it)
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
	GameObject* parent_object;
	const aiScene* scene = aiImportFile(final_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	std::string name;
	App->fsystem->SplitFilePath(final_path.c_str(), nullptr, &name);
	parent_object = App->scene->CreateGameObject(nullptr, name);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
		{
			LoadNode(scene->mRootNode->mChildren[i], scene, parent_object);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading file %s", file);
}

void ModuleImporter::LoadNode(const aiNode* node, const aiScene* scene, GameObject* parent)
{	
	GameObject* new_object;

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	new_object = App->scene->CreateGameObject(parent, node->mName.C_Str(), pos, rot, scale/100);

	if (node->mNumMeshes > 0)
	{
		Mesh* new_mesh;
		Texture* new_texture;
		aiMesh* current_mesh = scene->mMeshes[node->mMeshes[0]];

		new_mesh = App->meshes->LoadMesh(scene, current_mesh);

		//Check for material, and then load texture if it has, otherwise apply default texture
		if (current_mesh->mMaterialIndex >= 0)
		{
			aiString texture_path;
			scene->mMaterials[current_mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
			if (texture_path.length > 0)
			{
				std::string file, extension;
				App->fsystem->SplitFilePath(texture_path.C_Str(), nullptr, &file, &extension);
				new_texture = App->textures->LoadTexture(std::string(file+"."+extension).c_str());
			}
			else
			{
				new_texture = App->textures->GetDefaultTexture();
				LOG("Default texture applied to %s", new_object->GetName().c_str());
			}
		}

		ComponentMesh* c_mesh = (ComponentMesh*)new_object->AddComponent(COMPONENT_TYPE::MESH);
		c_mesh->AddMesh(new_mesh);

		ComponentTexture* c_text = (ComponentTexture*)new_object->AddComponent(COMPONENT_TYPE::TEXTURE);
		c_text->AddTexture(new_texture);
	}

	if (node->mNumChildren > 0)
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			LoadNode(node->mChildren[i], scene, new_object);
		}
	}
}

void ModuleImporter::ImportFile(const char * path)
{
	std::string normalized_path = path;
	App->fsystem->NormalizePath(normalized_path);

	std::string extension, file;
	App->fsystem->SplitFilePath(path, nullptr, &file, &extension);
	file += "." + extension;

	if (!App->fsystem->Exists(std::string(ASSETS_FOLDER + file).c_str())) //if file doesnt exist we copy it
	{
		App->fsystem->CopyFromOutsideFS(normalized_path.c_str(), ASSETS_FOLDER);
	}

	if (extension == "fbx")
	{
		LoadFBXFile(file.c_str());
	}
	else if (extension == "png" || extension == "dds" || extension == "jpg")
	{
		if (App->scene->selected_gameobject)
		{
			ComponentTexture* c_tex = (ComponentTexture*)App->scene->selected_gameobject->GetComponent(COMPONENT_TYPE::TEXTURE);
			if (c_tex)
			{
				c_tex->AddTexture(App->textures->LoadTexture(file.c_str()));
				LOG("Applied texture: %s to GameObject: %s", file.c_str(), App->scene->selected_gameobject->GetName().c_str());
			}
		}	
	}	
}