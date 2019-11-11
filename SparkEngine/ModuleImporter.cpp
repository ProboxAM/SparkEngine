#include "ModelImporter.h"
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
	model = new ModelImporter();
	texture = new TextureImporter();
	mesh = new MeshImporter();

	model->Init();
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