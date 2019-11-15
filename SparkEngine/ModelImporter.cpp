#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"
#include "MeshImporter.h"
#include "TextureImporter.h"

#include "ResourceModel.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

#include "ModelImporter.h"

#include <fstream>
#include <iomanip>

#pragma comment (lib, "Assimp/libx86/assimp.lib")


void LogCallback(const char* text, char* data)
{
	std::string temp_string = text;
	temp_string.erase(std::remove(temp_string.begin(), temp_string.end(), '%'), temp_string.end());
	LOG(temp_string.c_str());
}

ModelImporter::ModelImporter()
{
}

ModelImporter::~ModelImporter()
{
}

bool ModelImporter::Init()
{
	aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool ModelImporter::CleanUp()
{
	aiDetachAllLogStreams();

	return false;
}

bool ModelImporter::Load(ResourceModel* resource)
{
	std::string path = resource->GetExportedFile();

	std::ifstream i(path);
	nlohmann::json j = nlohmann::json::parse(i);

	for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it)
	{
		LoadNode(it, resource);
	}

	return true;
}

bool ModelImporter::LoadNode(nlohmann::json::iterator it, ResourceModel* resource)
{
	ResourceModel::ModelNode node;

	node.name = (*it)["name"].get<std::string>();

	node.position.x = (*it)["position"][0];
	node.position.y = (*it)["position"][1];
	node.position.z = (*it)["position"][2];

	node.scale.x = (*it)["scale"][0];
	node.scale.y = (*it)["scale"][1];
	node.scale.z = (*it)["scale"][2];

	node.rotation.x = (*it)["rotation"][0];
	node.rotation.y = (*it)["rotation"][1];
	node.rotation.z = (*it)["rotation"][2];
	node.rotation.w = (*it)["rotation"][3];

	node.parent = (*it)["parent"];
	node.texture = (*it)["texture"];
	node.mesh = (*it)["mesh"];

	resource->nodes.push_back(node);

	return true;
}


bool ModelImporter::Import(const char* file, std::string& output_file, uint id)
{
	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	std::vector<uint> meshes_id;
	std::string meta_file = file;
	meta_file += ".meta";
	if (App->fsystem->Exists(meta_file.c_str())) //Read meta to get meshes ids
		GetMeshesID(meta_file, meshes_id);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<ResourceModel::ModelNode> nodes;
		ImportNode(scene->mRootNode, scene, 0, nodes, meshes_id);
		output_file = LIBRARY_MODEL_FOLDER + std::to_string(id) + MODEL_EXTENSION;
		Save(output_file, nodes);
		CreateMeta(std::string(file), id, nodes);
		aiReleaseImport(scene);
		return true;
	}

	LOG("Error importing file %s", file);
	return false;
}

void ModelImporter::GetMeshesID(std::string file, std::vector<uint>& meshes_id)
{
	std::ifstream i(file);
	nlohmann::json json = nlohmann::json::parse(i);

	nlohmann::json meshes = json.find("meshes").value();
	for (nlohmann::json::iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		meshes_id.push_back(it.value());
	}
}

void ModelImporter::ImportNode(const aiNode* node, const aiScene* scene, uint parent_id, std::vector<ResourceModel::ModelNode>& nodes, std::vector<uint> meshes_id)
{
	uint index = nodes.size();
	ResourceModel::ModelNode resource_node;

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	FixScaleUnits(scale);

	resource_node.position = pos;
	resource_node.scale = scale;
	resource_node.rotation = rot;
	resource_node.parent = parent_id;
	resource_node.name = node->mName.C_Str();

	if (node->mNumMeshes > 0)
	{
		aiMesh* current_mesh = scene->mMeshes[node->mMeshes[0]]; //only one mesh for object for now, sry
		resource_node.mesh = App->importer->mesh->Import(scene, current_mesh, meshes_id.size() > 0 ? meshes_id[index] : 0);

		//Check for material, and then load texture if it has, otherwise apply default texture	
		aiString texture_path;
		scene->mMaterials[current_mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
		if (texture_path.length > 0)
		{
			std::string assets_path = ASSETS_FOLDER;
			std::string file, extension;
			App->fsystem->SplitFilePath(texture_path.C_Str(), nullptr, &file, &extension);
			assets_path += file + "." + extension;
			resource_node.texture = App->resources->ImportFile(assets_path.c_str(), Resource::RESOURCE_TYPE::R_TEXTURE);
		}
		else
			resource_node.texture = App->importer->texture->checkers;
	}
	nodes.push_back(resource_node);

	if (node->mNumChildren > 0)
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			ImportNode(node->mChildren[i], scene, index, nodes, meshes_id);
		}
	}
}


void ModelImporter::FixScaleUnits(float3 &scale)
{
	if (scale.x >= 1000) scale /= 1000;
	if (scale.x >= 100) scale /= 100;
	if (scale.x >= 10) scale /= 10;
}


bool ModelImporter::Save(std::string file, const std::vector<ResourceModel::ModelNode>& nodes)
{
	nlohmann::json json;

	for each (ResourceModel::ModelNode node in nodes)
	{
		nlohmann::json object;
		object = {
			{ "name", node.name},
			{ "position", { node.position.x, node.position.y, node.position.z} },
			{ "scale",{ node.scale.x, node.scale.y, node.scale.z } },
			{ "rotation",{ node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w } },
			{ "parent", node.parent },
			{ "mesh", node.mesh },
			{ "texture", node.texture }
		};
		json.push_back(object);
	}

	std::ofstream o(file);
	o << std::setw(4) << json << std::endl;

	return true;
}

bool ModelImporter::CreateMeta(std::string file, uint id, const std::vector<ResourceModel::ModelNode>& nodes)
{
	nlohmann::json meta_file;
	meta_file = {
		{ "original_file", file },
		{ "id", id },
		{ "meshes",{}}
	};
	nlohmann::json::iterator meshes = meta_file.find("meshes");
	for each (ResourceModel::ModelNode node in nodes)
	{
		meshes.value().push_back(node.mesh);
	}

	std::ofstream o(file + ".meta");
	o << std::setw(4) << meta_file << std::endl;

	return true;
}
