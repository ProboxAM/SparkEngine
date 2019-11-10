#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"

#include "SceneImporter.h"
#include "TextureImporter.h"
#include "MeshImporter.h"

#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include <fstream>
#include <iomanip>

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

uint ModuleResources::Find(const char * file_in_assets) const
{
	return uint();
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

bool ModuleResources::ImportFile(const char * new_file_in_assets, Resource::RESOURCE_TYPE type)
{
	std::string output_file;
	uint id = ++last_id;

	bool import_success = false;
	switch (type)
	{
		case Resource::R_TEXTURE:
			import_success = App->importer->texture->Import(new_file_in_assets, output_file, id);
			break;
		case Resource::R_MESH:
			import_success = App->importer->scene->Import(new_file_in_assets, output_file);
			break;
		case Resource::R_SCENE:
			break;
		case Resource::R_NONE:
			break;
		default:
			break;
	}

	if (import_success)
	{
		CreateMeta(new_file_in_assets, id);
		Resource* resource = CreateResource(type, id);
		resource->SetFile(new_file_in_assets);
		resource->SetExportedFile(output_file);
	}

	return true;
}

uint ModuleResources::GenerateNewUID()
{
	return uint();
}

Resource * ModuleResources::Get(uint id)
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

	return nullptr;
}

Resource * ModuleResources::CreateResource(Resource::RESOURCE_TYPE type, uint id)
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
	else if (extension == "png" || extension == "PNG")
		return Resource::RESOURCE_TYPE::R_TEXTURE;
	else if (extension == "dds" || extension == "DDS")
		return Resource::RESOURCE_TYPE::R_TEXTURE;
	else if (extension == "fbx" || extension == "FBX")
		return Resource::RESOURCE_TYPE::R_MESH;

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
			App->importer->mesh->Load(resource->GetExportedFile());
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
}

void ModuleResources::CreateMeta(std::string file, uint id)
{
	MetaInfo meta;
	meta.id = id;
	meta.original_file = file;

	nlohmann::json meta_file;
	meta_file = {
		{"original_file", meta.original_file},
		{"id",meta.id}
	};

	std::ofstream o(file+".meta");
	o << std::setw(4) << meta_file << std::endl;
}
