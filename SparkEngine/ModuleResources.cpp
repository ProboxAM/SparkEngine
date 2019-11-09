#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"

#include "SceneImporter.h"
#include "TextureImporter.h"

#include "ModuleResources.h"



ModuleResources::ModuleResources()
{
}


ModuleResources::~ModuleResources()
{
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
		Resource::RESOURCE_TYPE type = GetTypeFromExtension(extension.c_str());
		ImportFile(std::string(ASSETS_FOLDER + file).c_str(), type);
	}

	return true;
}

bool ModuleResources::ImportFile(const char * new_file_in_assets, Resource::RESOURCE_TYPE type)
{
	std::string output_file;

	bool import_success = false;
	switch (type)
	{
		case Resource::R_TEXTURE:
			import_success = App->importer->texture->Import(new_file_in_assets, output_file);
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

	return true;
}

uint ModuleResources::GenerateNewUID()
{
	return uint();
}

Resource * ModuleResources::Get(uint uid)
{
	return nullptr;
}

Resource * ModuleResources::CreateNewResource(Resource::RESOURCE_TYPE type, uint force_uid)
{
	return nullptr;
}

Resource::RESOURCE_TYPE ModuleResources::GetTypeFromExtension(const char * extension)
{
	if (extension != nullptr)
	{
		if (extension == "jpg")
			return Resource::RESOURCE_TYPE::R_TEXTURE;
		else if (extension == "png")
			return Resource::RESOURCE_TYPE::R_TEXTURE;
		else if (extension == "dds")
			return Resource::RESOURCE_TYPE::R_TEXTURE;
		else if (extension == "fbx")
			return Resource::RESOURCE_TYPE::R_MESH;
	}

	return Resource::RESOURCE_TYPE::R_NONE;
}
