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
	std::string normalized_path = path;
	App->fsystem->NormalizePath(normalized_path);;
	
	std::string extension, file;
	App->fsystem->SplitFilePath(path, nullptr, &file, &extension);
	file += "." + extension;

	if (App->fsystem->CopyFromOutsideFS(normalized_path.c_str(), ASSETS_FOLDER))
	{
		Resource::RESOURCE_TYPE type = GetTypeFromExtension(extension);
		ImportFile(std::string(ASSETS_FOLDER + file).c_str(), type);
	}

	return true;
}

uint ModuleResources::ImportFile(const char * new_file_in_assets, Resource::RESOURCE_TYPE type)
{
	bool needs_import = true;
	bool import_success = false;
	std::string output_file;

	std::string meta_file = new_file_in_assets;
	meta_file.append(".meta");
	Resource::MetaFile* meta = CreateMeta(meta_file.c_str(), type);

	if (App->fsystem->Exists(meta_file.c_str()))
	{
		LoadMeta(meta_file.c_str(), meta, type);
		std::string mod_date;
		/*App->fsystem->GetFileModificationDate(new_file_in_assets, mod_date);
		if (meta->modification_date == mod_date)
		{
			needs_import = false;
			import_success = true;
			output_file = meta->exported_file;
		}	*/		
	}
		
	if (needs_import)
	{
		switch (type)
		{
		case Resource::R_TEXTURE:
			import_success = App->importer->texture->Import(new_file_in_assets, output_file, (ResourceTexture::TextureMetaFile*) meta);
			break;
		case Resource::R_MODEL:
			import_success = App->importer->model->Import(new_file_in_assets, output_file, (ResourceModel::ModelMetaFile*) meta);
			break;
		case Resource::R_SCENE:
			break;
		case Resource::R_NONE:
			break;
		default:
			break;
		}
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

Resource* ModuleResources::CreateResource(Resource::RESOURCE_TYPE type)
{
	Resource* r = nullptr;
	uint id = ++last_id;

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

	return r;
}

Resource* ModuleResources::CreateResource(Resource::RESOURCE_TYPE type, uint id)
{
	Resource* r = nullptr;

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
	// get all files, check if they have meta or not, if they have meta
	std::vector<std::string> files;
	App->fsystem->GetFilesFiltered(ASSETS_FOLDER, files, "meta");

	for each(std::string file in files)
	{
		std::string extension;
		App->fsystem->SplitFilePath(file.c_str(), nullptr, nullptr, &extension);
		ImportFile((ASSETS_FOLDER+file).c_str(), GetTypeFromExtension(extension));
	}

	LOG("imported all assets");
}

uint ModuleResources::GetIDFromMeta(std::string file)
{
	std::ifstream i(file);
	nlohmann::json j = nlohmann::json::parse(i);

	return j["id"];
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

	meta->id = ++last_id;

	return meta;
}