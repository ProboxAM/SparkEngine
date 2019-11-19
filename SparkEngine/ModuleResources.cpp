#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"

#include "ModelImporter.h"
#include "TextureImporter.h"
#include "MeshImporter.h"

#include "ResourceTexture.h"
#include "ResourceMesh.h"

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
	LoadAssets();

	return true;
}

bool ModuleResources::ImportFileToAssets(const char * path)
{
	std::string extension, file;
	App->fsystem->SplitFilePath(path, nullptr, &file, &extension);
	file += "." + extension;

	if (!App->fsystem->Exists(std::string(ASSETS_FOLDER + file).c_str()))
	{
		LOG("Importing new file to Assets...");
		if (App->fsystem->CopyFromOutsideFS(path, ASSETS_FOLDER))
		{
			Resource::RESOURCE_TYPE type = GetTypeFromExtension(extension);
			ImportFile(std::string(ASSETS_FOLDER + file).c_str(), type);
		}
	}
	else
		LOG("File is already in Assets, not importing again.");

	return true;
}

uint ModuleResources::ImportFile(const char * new_file_in_assets, Resource::RESOURCE_TYPE type, Resource::MetaFile* meta)
{
	LOG("Importing file %s", new_file_in_assets);

	bool import_success = false;
	std::string output_file;
		
	switch (type)
	{
	case Resource::R_TEXTURE:
		import_success = App->importer->texture->Import(new_file_in_assets, output_file, (ResourceTexture::TextureMetaFile*&) meta);
		break;
	case Resource::R_MODEL:
		import_success = App->importer->model->Import(new_file_in_assets, output_file, (ResourceModel::ModelMetaFile*&) meta);
		break;
	case Resource::R_SCENE:
		return 0;
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

Resource* ModuleResources::GetAndLoad(uint id)
{
	Resource* ret = nullptr;
	std::map<uint, Resource*>::iterator it = resources.find(id);

	if (it != resources.end())
	{
		ret = it->second;
		if (!it->second->IsLoaded())
			LoadResource(ret);

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
		if (it->second->GetFile() == file)
			return it->first;

	return 0;
}

Resource* ModuleResources::CreateResource(Resource::RESOURCE_TYPE type, uint id)
{
	Resource* r = nullptr;
	
	std::map<uint,Resource*>::iterator it = resources.find(id);
	if (it != resources.end())
	{
		r = it->second;
		if (r->IsLoaded())
		{
			LOG("Reloading resource %u", id);
			r->ReLoad();
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
	if (extension == "jpg" || extension == "JPG")
		return Resource::RESOURCE_TYPE::R_TEXTURE;
	else if (extension == "png" || extension == "PNG" || extension == "tga")
		return Resource::RESOURCE_TYPE::R_TEXTURE;
	else if (extension == "dds" || extension == "DDS")
		return Resource::RESOURCE_TYPE::R_TEXTURE;
	else if (extension == "fbx" || extension == "FBX")
		return Resource::RESOURCE_TYPE::R_MODEL;
	else if (extension=="scene")
		return Resource::RESOURCE_TYPE::R_SCENE;

	return Resource::RESOURCE_TYPE::R_NONE;
}

bool ModuleResources::LoadResource(Resource* resource)
{
	switch (resource->GetType())
	{
		case Resource::RESOURCE_TYPE::R_TEXTURE:
			App->importer->texture->Load((ResourceTexture*)resource);
			break;
		case Resource::RESOURCE_TYPE::R_MESH:
			App->importer->mesh->Load((ResourceMesh*)resource);
			break;
		case Resource::RESOURCE_TYPE::R_MODEL:
			App->importer->model->Load((ResourceModel*)resource);
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

void ModuleResources::LoadAssets()
{
	LOG("Loading resources...");
	// Get all files in assets that are not meta
	std::vector<std::string> files;
	App->fsystem->GetFilesFiltered(ASSETS_FOLDER, files, "meta");

	for each(std::string file in files)
	{
		LOG("Loading resource %s", file.c_str());
		std::string asset_file = (ASSETS_FOLDER + file).c_str();
		std::string meta_file = asset_file + ".meta";
		std::string extension;
		App->fsystem->SplitFilePath(file.c_str(), nullptr, nullptr, &extension);
		Resource::RESOURCE_TYPE type = GetTypeFromExtension(extension);

		if (App->fsystem->Exists(meta_file.c_str())) //File has meta.
		{
			LOG("Resource has meta, loading meta file...");
			//Load meta
			Resource::MetaFile* meta = CreateMeta(meta_file.c_str(), type);
			LoadMeta(meta_file.c_str(), meta, type);

			std::string mod_date;
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

	LOG("Loaded all assets");
}

bool ModuleResources::ImportedLibraryFilesExist(Resource::MetaFile* meta, Resource::RESOURCE_TYPE type)
{
	bool ret = false;

	switch (type)
	{
	case Resource::RESOURCE_TYPE::R_TEXTURE:
		ret = App->fsystem->Exists(meta->exported_file.c_str());
		break;
	case Resource::RESOURCE_TYPE::R_MODEL:
	{
		ResourceModel::ModelMetaFile* model_meta = (ResourceModel::ModelMetaFile*) meta;
		for each (uint mesh in model_meta->meshes)
		{
			if (mesh != 0)
			{
				std::string mesh_file = LIBRARY_MESH_FOLDER + std::to_string(mesh) + MESH_EXTENSION;
				ret = App->fsystem->Exists(mesh_file.c_str());

				if (!ret)
					break;
			}
		}
	}
		break;
	case Resource::RESOURCE_TYPE::R_SCENE:
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

bool ModuleResources::LoadMeta(const char* file, Resource::MetaFile* meta, Resource::RESOURCE_TYPE type)
{
	switch (type)
	{
	case Resource::RESOURCE_TYPE::R_TEXTURE:
		App->importer->texture->LoadMeta(file, (ResourceTexture::TextureMetaFile*) meta);
		break;
	case Resource::RESOURCE_TYPE::R_MODEL:
		App->importer->model->LoadMeta(file, (ResourceModel::ModelMetaFile*) meta);
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

Resource::MetaFile* ModuleResources::CreateMeta(const char* file, Resource::RESOURCE_TYPE type)
{
	Resource::MetaFile* meta;
	switch (type)
	{
	case Resource::RESOURCE_TYPE::R_TEXTURE:
		meta = new ResourceTexture::TextureMetaFile();
		break;
	case Resource::RESOURCE_TYPE::R_MODEL:
		meta = new ResourceModel::ModelMetaFile();
		break;
	case Resource::RESOURCE_TYPE::R_SCENE:

		break;
	case Resource::RESOURCE_TYPE::R_NONE:
		break;
	default:
		break;
	}

	return meta;
}

void ModuleResources::CreateResourcesFromMeta(Resource::MetaFile* meta, Resource::RESOURCE_TYPE type)
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

		ResourceModel::ModelMetaFile* model_meta = (ResourceModel::ModelMetaFile*) meta;
		for each (uint mesh in model_meta->meshes)
		{
			if (mesh != 0)
			{
				Resource* mesh_resource = CreateResource(Resource::RESOURCE_TYPE::R_MESH, mesh);
				mesh_resource->SetExportedFile(LIBRARY_MESH_FOLDER + std::to_string(mesh) + MESH_EXTENSION);
			}			
		}

	}
		break;
	case Resource::RESOURCE_TYPE::R_SCENE:

		break;
	case Resource::RESOURCE_TYPE::R_NONE:
		break;
	default:
		break;
	}

	resource->SetFile(meta->original_file);
	resource->SetExportedFile(meta->exported_file);
	resource->meta = meta;
}