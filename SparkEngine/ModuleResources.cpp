#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ModuleImporter.h"

#include "ModelMetaFile.h"
#include "TextureMetaFile.h"
#include "AnimationMetaFile.h"

#include "ModelImporter.h"
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "AnimatorControllerImporter.h"

#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceBone.h"
#include "ResourceAnimation.h"
#include "ResourceAnimatorController.h"

#include <fstream>
#include <iomanip>
#include <algorithm>

#include "ModuleResources.h"



ModuleResources::ModuleResources(bool active): Module(active)
{
}


ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
	RecursiveLoadAssets(ASSETS_FOLDER);
	LOG("Loaded all resources");

	CleanLibrary();

	return true;
}

bool ModuleResources::CleanUp()
{
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetType() == Resource::RESOURCE_TYPE::R_ANIMATOR)
			((ResourceAnimatorController*)it->second)->SaveAsset();
	}

	return true;
}

bool ModuleResources::ImportFileToAssets(const char * path)
{
	std::string extension, file;
	App->fsystem->SplitFilePath(path, nullptr, &file, &extension);
	file += "." + extension;
	std::string full_path;

	if (!App->fsystem->ExistsRecursive(file.c_str(), ASSETS_FOLDER, full_path))
	{
		LOG("Importing new file to Assets...");
		if (App->fsystem->CopyFromOutsideFS(path, App->editor->GetProjectPanelPath().c_str()))
		{
			Resource::RESOURCE_TYPE type = GetTypeFromExtension(extension);
			ImportFile(std::string(App->editor->GetProjectPanelPath() + file).c_str(), type);
			App->editor->ReloadProjectWindow();
		}
	}
	else
		LOG("File is already in Assets, not importing again.");

	return true;
}

uint ModuleResources::ImportFile(const char * new_file_in_assets, Resource::RESOURCE_TYPE type, MetaFile* meta)
{
	LOG("Importing file %s", new_file_in_assets);

	bool import_success = false;
	std::string output_file;
		
	switch (type)
	{
	case Resource::R_TEXTURE:
		import_success = App->importer->texture->Import(new_file_in_assets, output_file, (TextureMetaFile*&) meta);
		break;
	case Resource::R_MODEL:
		import_success = App->importer->model->Import(new_file_in_assets, output_file, (ModelMetaFile*&) meta);
		break;
	case Resource::R_SCENE:
		return 0;
		break;
	case Resource::R_ANIMATOR:
		import_success = App->importer->anim_controller->Import(new_file_in_assets, output_file, meta);
		break;
	case Resource::R_NONE:
		break;
	default:
		break;
	}

	if (import_success)
	{
		Resource* resource = CreateResource(type, meta->id);
		resource->SetFile(new_file_in_assets);
		resource->SetExportedFile(output_file);
		resource->meta = meta;
	}

	return meta->id;
}

uint ModuleResources::GenerateNewUID()
{
	return uint();
}

Resource* ModuleResources::GetAndReference(uint id)
{
	Resource* ret = nullptr;
	std::map<uint, Resource*>::iterator it = resources.find(id);

	if (it != resources.end())
	{
		ret = it->second;
		ret->AddReference();
	}

	return ret;
}

Resource* ModuleResources::Get(uint id)
{
	Resource* ret = nullptr;
	std::map<uint, Resource*>::iterator it = resources.find(id);

	if (it != resources.end())
		ret = it->second;

	return ret;
}

uint ModuleResources::GetID(std::string file)
{
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		if (it->second->meta && it->second->meta->original_file == file)
			return it->first;

	return 0;
}

Resource* ModuleResources::CreateResource(Resource::RESOURCE_TYPE type, uint id, bool& needs_reload)
{
	Resource* r = nullptr;
	
	std::map<uint,Resource*>::iterator it = resources.find(id);
	if (it != resources.end())
	{
		r = it->second;	
		if (r->IsLoaded())
		{
			r->UnLoad();
			needs_reload = true;
		}
	}
	else
	{
		switch (type)
		{
		case Resource::R_TEXTURE:
			r = new ResourceTexture(id);
			break;
		case Resource::R_MESH:
			r = new ResourceMesh(id);
			break;
		case Resource::R_MODEL:
			r = new ResourceModel(id);
			break;
		case Resource::R_SCENE:
			break;
		case Resource::R_ANIMATION:
			r = new ResourceAnimation(id);
			break;
		case Resource::R_ANIMATOR:
			r = new ResourceAnimatorController(id);
			break;
		case Resource::R_BONE:
			r = new ResourceBone(id);
			break;
		case Resource::R_NONE:
			break;
		default:
			break;
		}
		if (r != nullptr)
			resources.emplace(id, r);
	}

	return r;
}

Resource* ModuleResources::CreateResource(Resource::RESOURCE_TYPE type, uint id)
{
	Resource* r = nullptr;

	std::map<uint, Resource*>::iterator it = resources.find(id);
	if (it != resources.end())
	{
		r = it->second;
	}
	else
	{
		switch (type)
		{
		case Resource::R_TEXTURE:
			r = new ResourceTexture(id);
			break;
		case Resource::R_MESH:
			r = new ResourceMesh(id);
			break;
		case Resource::R_MODEL:
			r = new ResourceModel(id);
			break;
		case Resource::R_SCENE:
			break;
		case Resource::R_ANIMATION:
			r = new ResourceAnimation(id);
			break;
		case Resource::R_ANIMATOR:
			r = new ResourceAnimatorController(id);
			break;
		case Resource::R_BONE:
			r = new ResourceBone(id);
			break;
		case Resource::R_NONE:
			break;
		default:
			break;
		}
		if (r != nullptr)
			resources.emplace(id, r);
	}

	return r;
}

Resource::RESOURCE_TYPE ModuleResources::GetTypeFromExtension(std::string extension)
{
	App->fsystem->NormalizePath(extension);
	if (extension == "png" || extension == "tga" || extension == "dds" || extension == "jpg")
		return Resource::RESOURCE_TYPE::R_TEXTURE;
	else if (extension == "fbx")
		return Resource::RESOURCE_TYPE::R_MODEL;
	else if (extension == "spk")
		return Resource::RESOURCE_TYPE::R_SCENE;
	else if (extension == "scontroller")
		return Resource::RESOURCE_TYPE::R_ANIMATOR;

	return Resource::RESOURCE_TYPE::R_NONE;
}

void ModuleResources::CreateAsset(Resource::RESOURCE_TYPE type)
{
	switch (type)
	{
	case Resource::R_TEXTURE:
		break;
	case Resource::R_MESH:
		break;
	case Resource::R_MODEL:
		break;
	case Resource::R_SCENE:
		break;
	case Resource::R_MATERIAL:
		break;
	case Resource::R_ANIMATION:
		break;
	case Resource::R_ANIMATOR:
	{
		ResourceAnimatorController* r = (ResourceAnimatorController*)CreateResource(type, App->GenerateID());
		r->meta = new MetaFile();
		std::string file;
		r->meta->original_file = App->fsystem->GetUniqueFile(App->editor->GetProjectPanelPath() + r->GetName() + ANIM_CONTROLLER_EXTENSION, file);
		r->name = file;
		r->SaveAsset();
		App->editor->ReloadProjectWindow();
	}
		break;
	case Resource::R_BONE:
		break;
	case Resource::R_NONE:
		break;
	default:
		break;
	}
}

void ModuleResources::RecursiveLoadAssets(std::string directory)
{
	LOG("Loading resources in %s", directory.c_str());
	// Get all files in assets that are not meta
	std::vector<std::string> files, directories;
	App->fsystem->DiscoverFiles(directory.c_str(), files, directories, "meta");

	for each(std::string file in files)
	{
		LOG("Loading resource %s", file.c_str());
		std::string asset_file = (directory + file).c_str();
		std::string meta_file = file + ".meta";
		std::string extension;
		App->fsystem->SplitFilePath(file.c_str(), nullptr, nullptr, &extension);
		Resource::RESOURCE_TYPE type = GetTypeFromExtension(extension);

		if (App->fsystem->ExistsRecursive(meta_file.c_str(), ASSETS_FOLDER, meta_file)) //File has meta.
		{
			LOG("Resource has meta, loading meta file...");
			//Check if meta is in same folder as asset
			std::string expected_meta_file = asset_file + ".meta";
			if (meta_file != expected_meta_file)
			{
				LOG("Asset and meta not in same path. Moving meta file...");
				App->fsystem->Cut(meta_file.c_str(), expected_meta_file.c_str());
				meta_file = expected_meta_file;
			}
			//Load meta
			MetaFile* meta = CreateMeta(meta_file.c_str(), type);
			LoadMeta(meta_file.c_str(), meta, type);

			if (meta->original_file != asset_file)
				meta->original_file = asset_file;
			int mod_date;
			App->fsystem->GetFileModificationDate(asset_file.c_str(), mod_date);
			bool is_modified = meta->modification_date != mod_date; // Check if asset file has been modified
			bool missing_files = !ImportedLibraryFilesExist(meta, type); // Check if any file in library is missing

			if (is_modified || missing_files)
			{
				LOG("Resource needs reimporting");
				ImportFile(asset_file.c_str(), type, meta); //Reimport asset with ids from meta.
			}		
			else
				CreateResourcesFromMeta(meta, type); //No need to import, just create resources
		}
		else
			ImportFile(asset_file.c_str(), GetTypeFromExtension(extension)); //New asset, needs import
	}

	for each (std::string folder in directories)
		RecursiveLoadAssets(directory + folder + "/");
}

bool ModuleResources::ImportedLibraryFilesExist(MetaFile* meta, Resource::RESOURCE_TYPE type)
{
	bool ret = false;

	switch (type)
	{
	case Resource::RESOURCE_TYPE::R_TEXTURE:
		ret = App->fsystem->Exists(meta->exported_file.c_str());
		break;
	case Resource::RESOURCE_TYPE::R_MODEL:
	{
		ModelMetaFile* model_meta = (ModelMetaFile*) meta;
		for each (uint mesh in model_meta->meshes)
		{
			if (mesh != 0)
			{
				std::string mesh_file = LIBRARY_MESH_FOLDER + std::to_string(mesh) + MESH_EXTENSION;
				ret = App->fsystem->Exists(mesh_file.c_str());

				if (!ret)
					return false;
			}
		}
		for each (AnimationMetaFile* anim in model_meta->animations)
		{
			if (anim->id != 0)
			{
				std::string anim_file = LIBRARY_ANIMATION_FOLDER + std::to_string(anim->id) + ANIM_EXTENSION;
				ret = App->fsystem->Exists(anim_file.c_str());

				if (!ret)
					return false;
			}
		}
		for (std::map<std::string, uint>::iterator it = model_meta->bones.begin(); it != model_meta->bones.end(); ++it)
		{
			if (it->second != 0)
			{
				std::string bone_file = LIBRARY_BONE_FOLDER + std::to_string(it->second) + BONE_EXTENSION;
				ret = App->fsystem->Exists(bone_file.c_str());

				if (!ret)
					return false;
			}
		}
	}
		break;
	case Resource::RESOURCE_TYPE::R_SCENE:
		break;
	case Resource::RESOURCE_TYPE::R_ANIMATOR:
		ret = App->fsystem->Exists(meta->exported_file.c_str());
		break;
	case Resource::RESOURCE_TYPE::R_NONE:
		break;
	default:
		break;
	}

	return ret;
}

uint ModuleResources::GetIDFromMeta(std::string file)
{
	std::ifstream i(file);
	nlohmann::json j = nlohmann::json::parse(i);

	return j["id"];
}

std::map<uint, Resource*> ModuleResources::GetResources()
{
	return resources;
}

std::vector<Resource*> ModuleResources::GetResources(Resource::RESOURCE_TYPE type)
{
	std::vector<Resource*> ret;

	for(std::map<uint,Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if(it->second->GetType() == type)
			ret.push_back(it->second);
	}

	return ret;
}

bool ModuleResources::LoadMeta(const char* file, MetaFile* meta, Resource::RESOURCE_TYPE type)
{
	switch (type)
	{
	case Resource::RESOURCE_TYPE::R_TEXTURE:
		App->importer->texture->LoadMeta(file, (TextureMetaFile*) meta);
		break;
	case Resource::RESOURCE_TYPE::R_MODEL:
		App->importer->model->LoadMeta(file, (ModelMetaFile*) meta);
		break;
	case Resource::RESOURCE_TYPE::R_ANIMATOR:
		App->importer->anim_controller->LoadMeta(file, meta);
		break;
	case Resource::RESOURCE_TYPE::R_SCENE:
		break;
	case Resource::RESOURCE_TYPE::R_NONE:
		break;
	default:
		break;
	}

	return true;
}

MetaFile* ModuleResources::CreateMeta(const char* file, Resource::RESOURCE_TYPE type)
{
	MetaFile* meta;
	switch (type)
	{
	case Resource::RESOURCE_TYPE::R_TEXTURE:
		meta = new TextureMetaFile();
		break;
	case Resource::RESOURCE_TYPE::R_MODEL:
		meta = new ModelMetaFile();
		break;
	case Resource::RESOURCE_TYPE::R_SCENE:

		break;
	case Resource::RESOURCE_TYPE::R_ANIMATOR:
		meta = new MetaFile();
		break;
	case Resource::RESOURCE_TYPE::R_NONE:
		break;
	default:
		break;
	}

	return meta;
}

void ModuleResources::CreateResourcesFromMeta(MetaFile* meta, Resource::RESOURCE_TYPE type)
{
	Resource* resource;
	switch (type)
	{
	case Resource::RESOURCE_TYPE::R_TEXTURE:
		resource = CreateResource(type, meta->id);
		break;
	case Resource::RESOURCE_TYPE::R_MODEL:
	{
		resource = CreateResource(type, meta->id);

		ModelMetaFile* model_meta = (ModelMetaFile*) meta;
		for each (uint mesh in model_meta->meshes)
		{
			if (mesh != 0)
			{
				Resource* mesh_resource = CreateResource(Resource::RESOURCE_TYPE::R_MESH, mesh);
				mesh_resource->SetExportedFile(LIBRARY_MESH_FOLDER + std::to_string(mesh) + MESH_EXTENSION);
				mesh_resource->SetFile(meta->original_file);
			}			
		}
		for each (AnimationMetaFile* anim_meta in model_meta->animations)
		{
			if (anim_meta->id != 0)
			{
				Resource* animation_resource = CreateResource(Resource::RESOURCE_TYPE::R_ANIMATION, anim_meta->id);
				animation_resource->SetExportedFile(LIBRARY_ANIMATION_FOLDER + std::to_string(anim_meta->id) + ANIM_EXTENSION);
				animation_resource->SetFile(meta->original_file);
				animation_resource->Load();
				animation_resource->UnLoad();
			}
		}
		for (std::map<std::string, uint>::iterator it = model_meta->bones.begin(); it != model_meta->bones.end(); ++it)
		{
			Resource* bone_resource = CreateResource(Resource::RESOURCE_TYPE::R_BONE, it->second);
			bone_resource->SetExportedFile(LIBRARY_BONE_FOLDER + std::to_string(it->second) + BONE_EXTENSION);
			bone_resource->SetFile(meta->original_file);
		}
	}
		break;
	case Resource::RESOURCE_TYPE::R_SCENE:

		break;
	case Resource::RESOURCE_TYPE::R_ANIMATOR:
		resource = CreateResource(type, meta->id);
		resource->SetFile(meta->original_file);
		resource->SetExportedFile(meta->exported_file);
	default:
		break;
	}

	resource->SetFile(meta->original_file);
	resource->SetExportedFile(meta->exported_file);
	resource->meta = meta;
}

void ModuleResources::CleanLibrary()
{
	std::vector<std::string> files, directories;
	App->fsystem->DiscoverFiles(LIBRARY_FOLDER, files, directories);
	for each (std::string folder in directories)
	{
		std::string folder_path = LIBRARY_FOLDER+folder;
		folder_path += "/";
		CleanLibraryFolder(folder_path.c_str());
	}
}
void ModuleResources::CleanLibraryFolder(const char* folder)
{
	std::vector<std::string> files, directories;
	App->fsystem->DiscoverFiles(folder, files, directories);

	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		std::string file_path = folder + (*it);
		bool found = false;
		for (std::map<uint, Resource*>::iterator resource_it = resources.begin(); resource_it != resources.end(); ++resource_it)
		{
			if (resource_it->second->GetExportedFile() == file_path)
				found = true;
		}

		if (!found)
		{
			LOG("%s doesn't have a resource associated. Removing it.", file_path.c_str());
			App->fsystem->Remove(file_path.c_str());
		}

	}
}