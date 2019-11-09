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

}