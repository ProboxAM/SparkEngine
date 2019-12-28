#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"

#include "MeshImporter.h"
#include "TextureImporter.h"
#include "AnimationImporter.h"
#include "BoneImporter.h"

#include "ResourceAnimation.h"

#include "ModelImporter.h"

#include "ModelMetaFile.h"
#include "AnimationMetaFile.h"

#include <fstream>
#include <iomanip>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


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
	node.bone = (*it)["bone"];
	node.root_bone = (*it)["root_bone"];
	node.mesh_binded = (*it)["mesh_binded"];

	resource->nodes.push_back(node);

	return true;
}


bool ModelImporter::Import(const char* file, std::string& output_file, ModelMetaFile*& meta)
{
	LOG("Importing Model %s", file);
	uint flags = aiProcessPreset_TargetRealtime_MaxQuality;
	if(meta)
		flags = meta->GetImportSettings();
	else //If there was no meta, we create a new one for this resource and generate id.
	{
		meta = new ModelMetaFile();
		meta->id = App->GenerateID();
		meta->import_animation = true;
	}

	const aiScene* scene = aiImportFile(file, flags);
	if (!scene)
	{
		LOG("Error importing file %s", file);
		return false;
	}

	//import meshes
	if (scene->HasMeshes())
	{
		uint num_meshes = scene->mNumMeshes;
		for (int i = 0; i < num_meshes; i++)
		{
			uint mesh_uid = meta->loaded ? meta->meshes[i] : App->GenerateID();
			meshes.push_back(App->importer->mesh->Import(file, scene->mMeshes[i], mesh_uid, scene->mMeshes[i]->mName.C_Str()));
			//import bones of mesh
			if (scene->mMeshes[i]->HasBones())
			{
				uint num_bones = scene->mMeshes[i]->mNumBones;
				for (int j = 0; j < num_bones; j++)
				{
					uint bone_uid = meta->loaded ? meta->bones[std::string(scene->mMeshes[i]->mBones[j]->mName.C_Str())] : App->GenerateID();
					bones.emplace(scene->mMeshes[i]->mBones[j]->mName.C_Str(), App->importer->bone->Import(file, scene->mMeshes[i]->mBones[j], bone_uid));
				}
			}
		}
	}

	//import animations
	if (scene->HasAnimations() && meta->import_animation)
	{
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			if (meta->loaded)
			{
				for each (AnimationMetaFile* anim in meta->animations)
				{
					animations.push_back(anim);
					App->importer->animation->Import(file, scene->mAnimations[i], anim);
				}
			}
			else
			{
				animations.push_back(App->importer->animation->Import(file, scene->mAnimations[i]));
			}		
		}
		meta->max_ticks = animations[0]->max_tick;
	}

	//create nodes
	std::vector<ResourceModel::ModelNode> nodes;
	ImportNode(scene->mRootNode, scene, 0, file, nodes);

	std::string root_name;
	App->fsystem->SplitFilePath(file, nullptr, &root_name, nullptr);
	nodes[0].name = root_name;

	output_file = LIBRARY_MODEL_FOLDER + std::to_string(meta->id) + MODEL_EXTENSION;
	Save(output_file, nodes);

	meta->meshes = meshes;
	meta->bones = bones;
	meta->animations = animations;

	App->fsystem->GetFileModificationDate(file, meta->modification_date);
	meta->exported_file = output_file;
	meta->original_file = file;
	meta->file = std::string(file) + ".meta";
	meta->loaded = true;
	SaveMeta(meta);

	aiReleaseImport(scene);
	meshes.clear();
	bones.clear();
	animations.clear();

	return true;
}

void ModelImporter::ImportNode(const aiNode* node, const aiScene* scene, uint parent_id, const char* file, std::vector<ResourceModel::ModelNode>& nodes)
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
		resource_node.mesh = meshes[node->mMeshes[0]]; //only one mesh for object for now

		//Check for material, and then load texture if it has, otherwise apply default texture	
		aiString texture_path;
		scene->mMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
		if (texture_path.length > 0)
		{
			std::string tex_file, extension, path, texture_full_path;
			App->fsystem->SplitFilePath(texture_path.C_Str(), nullptr, &tex_file, &extension);
			App->fsystem->SplitFilePath(file, &path);
			texture_full_path = path + tex_file + "." + extension;
			tex_file += "." + extension;

			if (App->fsystem->ExistsRecursive(tex_file.c_str(), ASSETS_FOLDER, texture_full_path)) //Check if asset of texture exists in project
			{
				std::string meta_file = texture_full_path + ".meta";
				if (App->fsystem->Exists(meta_file.c_str())) //Check if meta exista associated to the asset
				{
					resource_node.texture = App->resources->GetIDFromMeta(meta_file);
				}
				else
					resource_node.texture = App->resources->ImportFile(texture_full_path.c_str(), Resource::RESOURCE_TYPE::R_TEXTURE);
			}
		}
		else
			resource_node.texture = App->importer->texture->checkers;
	}
	if (bones.find(std::string(node->mName.C_Str())) != bones.end())
	{
		resource_node.bone = bones[std::string(node->mName.C_Str())];

		//find if its a root bone
		uint num_meshes = scene->mNumMeshes;
		for (int i = 0; i < num_meshes; i++) //check each mesh in scene for this bone.
		{
			bool not_root = false;
			if (scene->mMeshes[i]->HasBones())
			{
				bool found = false;
				uint num_bones = scene->mMeshes[i]->mNumBones;
				for (int j = 0; j < num_bones; j++)
				{
					if (scene->mMeshes[i]->mBones[j]->mName == node->mParent->mName) //if parent is a bone, we cant be root
					{
						not_root = true;
						break;
					}
					if (scene->mMeshes[i]->mBones[j]->mName == node->mName) //if bone in this mesh
						found = true;
				}
				if (found && !not_root) //if bone was in mesh and is root
				{
					resource_node.root_bone = true;
					resource_node.mesh_binded = meshes[i];
				}
			}
			if (not_root)
				break;
		}
	}

	nodes.push_back(resource_node);

	if (node->mNumChildren > 0)
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			ImportNode(node->mChildren[i], scene, index, file, nodes);
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
			{ "bone", node.bone },
			{ "texture", node.texture },
			{ "root_bone", node.root_bone},
			{ "mesh_binded", node.mesh_binded}
		};
		json.push_back(object);
	}

	std::ofstream o(file);
	o << std::setw(4) << json << std::endl;

	return true;
}

bool ModelImporter::SaveMeta(ModelMetaFile* meta)
{
	nlohmann::json meta_file;

	nlohmann::json animations_obj;
	for each (AnimationMetaFile* anim_meta in meta->animations)
	{
		nlohmann::json anim_obj = {
			{ "id",anim_meta->id },
			{ "name",anim_meta->name },
			{ "start_tick",anim_meta->start_tick },
			{ "end_tick",anim_meta->end_tick },
			{ "max_tick",anim_meta->max_tick},
			{ "loops",anim_meta->loops },
		};
		animations_obj.push_back(anim_obj);
	}

	meta_file = {
		{ "original_file", meta->original_file },
		{ "exported_file", meta->exported_file},
		{ "id", meta->id },
		{ "modification_date", meta->modification_date },
		{ "import_animation", meta->import_animation},
		{ "max_ticks", meta->max_ticks},
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
		{ "meshes", meta->meshes},
		{ "animations", animations_obj},
		{ "bones", meta->bones }
	};

	std::ofstream o(meta->file);
	o << std::setw(4) << meta_file << std::endl;

	return true;
}

bool ModelImporter::LoadMeta(const char* file, ModelMetaFile* meta)
{
	std::ifstream i(file);
	nlohmann::json json = nlohmann::json::parse(i);

	meta->file = file;
	meta->exported_file = json["exported_file"].get<std::string>();
	meta->original_file = json["original_file"].get<std::string>();
	meta->id = json["id"];
	meta->modification_date = json["modification_date"];

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

	meta->import_animation = json["import_animation"];
	meta->max_ticks = json["max_ticks"];

	meta->meshes = json["meshes"].get<std::vector<uint>>();
	meta->bones = json["bones"].get<std::map<std::string, uint>>();

	nlohmann::json animations_obj = json["animations"];
	for(nlohmann::json::iterator anim = animations_obj.begin(); anim!=animations_obj.end(); ++anim)
	{
		AnimationMetaFile* anim_meta = new AnimationMetaFile();
		anim_meta->id = anim.value()["id"];
		anim_meta->name = anim.value()["name"].get<std::string>();
		anim_meta->start_tick = anim.value()["start_tick"];
		anim_meta->end_tick = anim.value()["end_tick"];
		anim_meta->max_tick = anim.value()["max_tick"];
		anim_meta->loops = anim.value()["loops"];
		anim_meta->loaded = true;

		meta->animations.push_back(anim_meta);
	}

	meta->loaded = true;

	return meta;
}