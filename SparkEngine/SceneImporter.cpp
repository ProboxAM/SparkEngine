#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleImporter.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "Mesh.h"
#include "Texture.h"

#include "Importer.h"
#include "TextureImporter.h"
#include "MeshImporter.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

#include "SceneImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")


void LogCallback(const char* text, char* data)
{
	std::string temp_string = text;
	temp_string.erase(std::remove(temp_string.begin(), temp_string.end(), '%'), temp_string.end());
	LOG(temp_string.c_str());
}

SceneImporter::SceneImporter()
{
}

SceneImporter::~SceneImporter()
{
}

bool SceneImporter::Init()
{
	aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool SceneImporter::CleanUp()
{
	aiDetachAllLogStreams();

	return false;
}

bool SceneImporter::Load(const char * exported_file, GameObject * resource)
{
	return false;
}


bool SceneImporter::Import(const char* file)
{
	std::string final_path = ASSETS_FOLDER + std::string(file);
	GameObject* parent_object;
	AABB bb;
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

		return true;
	}

	LOG("Error loading file %s", file);
	return false;
}

void SceneImporter::LoadNode(const aiNode* node, const aiScene* scene, GameObject* parent)
{
	GameObject* new_object;

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	FixScaleUnits(scale);

	new_object = App->scene->CreateGameObject(parent, node->mName.C_Str(), pos, rot, scale);

	if (node->mNumMeshes > 0)
	{
		Mesh* new_mesh;
		Texture* new_texture;
		aiMesh* current_mesh = scene->mMeshes[node->mMeshes[0]];

		new_mesh = App->importer->mesh->Load("mesh.spk");
		new_object->bounding_box.SetNegativeInfinity();
		new_object->bounding_box.Enclose(new_mesh->vertices, new_mesh->total_vertices);

		//Check for material, and then load texture if it has, otherwise apply default texture
		if (current_mesh->mMaterialIndex >= 0)
		{
			aiString texture_path;
			scene->mMaterials[current_mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
			if (texture_path.length > 0)
			{
				std::string file;
				App->fsystem->SplitFilePath(texture_path.C_Str(), nullptr, &file);
				new_texture = App->importer->texture->Load(std::string(LIBRARY_TEXTURES_FOLDER + file + ".dds").c_str());
			}
			else
			{
				new_texture = App->importer->texture->LoadDefault();
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


void SceneImporter::FixScaleUnits(float3 &scale)
{
	if (scale.x >= 1000) scale /= 1000;
	if (scale.x >= 100) scale /= 100;
	if (scale.x >= 10) scale /= 10;
}