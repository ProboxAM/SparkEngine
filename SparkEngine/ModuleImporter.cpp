#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTexture.h"
#include "Texture.h"

#include "Importer.h"
#include "SceneImporter.h"
#include "TextureImporter.h"
#include "MeshImporter.h"

#include "ModuleImporter.h"


ModuleImporter::ModuleImporter(bool start_enabled)
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init(const nlohmann::json::iterator& it)
{
	// init importers
	scene = new SceneImporter();
	texture = new TextureImporter();
	mesh = new MeshImporter();

	scene->Init();
	texture->Init();
	mesh->Init();

	return true;
}

bool ModuleImporter::CleanUp()
{	
	//clean importers

	return true;
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

	if (extension == "fbx" || extension == "FBX")
	{
		scene->Import(file.c_str());
	}
	else if (extension == "png" || extension == "dds" || extension == "jpg" ||
			 extension == "PNG" || extension == "DDS" || extension == "JPG")
	{
		std::string output_file;
		texture->Import(std::string(ASSETS_FOLDER + file).c_str(), output_file);

		if (App->scene->selected_gameobject)
		{
			ComponentTexture* c_tex = (ComponentTexture*) App->scene->selected_gameobject->GetComponent(COMPONENT_TYPE::TEXTURE);
			if (c_tex)
			{
				c_tex->AddTexture(texture->Load(output_file.c_str()));
				LOG("Applied texture: %s to GameObject: %s", output_file.c_str(), App->scene->selected_gameobject->GetName().c_str());
			}
		}	
	}	
}