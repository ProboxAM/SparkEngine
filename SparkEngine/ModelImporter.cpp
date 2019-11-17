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


bool ModelImporter::Import(const char* file, std::string& output_file, ResourceModel::ModelMetaFile* meta)
{
	uint flags;

	flags = meta->GetImportSettings();

	const aiScene* scene = aiImportFile(file, flags);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<ResourceModel::ModelNode> nodes;
		ImportNode(scene->mRootNode, scene, 0, nodes, meta);

		output_file = LIBRARY_MODEL_FOLDER + std::to_string(meta->id) + MODEL_EXTENSION;
		Save(output_file, nodes);

		if (!meta->loaded)
		{
			for each (ResourceModel::ModelNode node in nodes)
			{
				meta->meshes.push_back(node.mesh);
			}
			App->fsystem->GetFileModificationDate(file, meta->modification_date);
			meta->exported_file = output_file;
			meta->original_file = file;
			meta->file = std::string(file) + ".meta";
		}

		SaveMeta(meta);

		aiReleaseImport(scene);
		return true;
	}

	LOG("Error importing file %s", file);
	return false;
}

void ModelImporter::ImportNode(const aiNode* node, const aiScene* scene, uint parent_id, std::vector<ResourceModel::ModelNode>& nodes, ResourceModel::ModelMetaFile* meta)
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
		resource_node.mesh = App->importer->mesh->Import(scene, current_mesh, meta->meshes.size() > 0 ? meta->meshes[index] : 0);

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
			ImportNode(node->mChildren[i], scene, index, nodes, meta);
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

bool ModelImporter::SaveMeta(ResourceModel::ModelMetaFile* meta)
{
	nlohmann::json meta_file;
	meta_file = {
		{ "original_file", meta->file },
		{ "exported_file", meta->exported_file},
		{ "id", meta->id },
		{ "modification_date", meta->modification_date },
		{ "setting", meta->setting },
		{ "find_instances", meta->find_instances },
		{ "validate_structures", meta->validate_structures },
		{ "optimize_meshes", meta->optimize_meshes },
		{ "calc_tangent_space", meta->calc_tangent_space },
		{ "gen_smooth_normals", meta->gen_smooth_normals },
		{ "join_vertices", meta->join_vertices },
		{ "improve_cache_locality", meta->improve_cache_locality },
		{ "limit_bone_weigths", meta->limit_bone_weigths },
		{ "remove_redundant_mats", meta->remove_redundant_mats },
		{ "split_large_meshes" ,meta->split_large_meshes },
		{ "triangulate", meta->triangulate },
		{ "gen_uv_coords", meta->gen_uv_coords },
		{ "sort_by_type", meta->sort_by_type },
		{ "find_degenerates", meta->find_degenerates },
		{ "find_invalid_data", meta->find_invalid_data },
		{ "meshes",{}}
	};

	nlohmann::json::iterator meshes = meta_file.find("meshes");
	for each (uint mesh in meta->meshes)
	{
		meshes.value().push_back(mesh);
	}

	std::ofstream o(meta->file);
	o << std::setw(4) << meta_file << std::endl;

	return true;
}

bool ModelImporter::LoadMeta(const char* file, ResourceModel::ModelMetaFile* meta)
{
	std::ifstream i(file);
	nlohmann::json json = nlohmann::json::parse(i);

	meta->file = file;
	meta->exported_file = json["exported_file"].get<std::string>();
	meta->original_file = json["original_file"].get<std::string>();
	meta->id = json["id"];
	meta->modification_date = json["modification_date"].get<std::string>();

	meta->setting = json["setting"];
	meta->find_instances = json["find_instances"];
	meta->validate_structures = json["validate_structures"];
	meta->optimize_meshes = json["optimize_meshes"];
	meta->calc_tangent_space = json["calc_tangent_space"];
	meta->gen_smooth_normals = json["gen_smooth_normals"];
	meta->join_vertices = json["join_vertices"];
	meta->improve_cache_locality = json["improve_cache_locality"];
	meta->limit_bone_weigths = json["limit_bone_weigths"];
	meta->remove_redundant_mats = json["remove_redundant_mats"];
	meta->split_large_meshes = json["split_large_meshes"];
	meta->triangulate = json["triangulate"];
	meta->gen_uv_coords = json["gen_uv_coords"];
	meta->sort_by_type = json["sort_by_type"];
	meta->find_degenerates = json["find_degenerates"];
	meta->find_invalid_data = json["find_invalid_data"];

	nlohmann::json meshes = json.find("meshes").value();
	for (nlohmann::json::iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		meta->meshes.push_back(it.value());
	}

	meta->loaded = true;

	return meta;
}