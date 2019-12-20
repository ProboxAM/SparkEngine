#include "ModelImporter.h"
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "AnimationImporter.h"
#include "BoneImporter.h"

#include "ModuleImporter.h"


ModuleImporter::ModuleImporter(bool start_enabled)
{
	name = "Importer";
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
	animation = new AnimationImporter();
	bone = new BoneImporter();

	model->Init();
	texture->Init();
	mesh->Init();
	animation->Init();
	bone->Init();

	Load(it);

	return true;
}

bool ModuleImporter::Start()
{
	model->Start();
	texture->Start();
	mesh->Start();
	animation->Start();
	bone->Start();

	return true;
}

bool ModuleImporter::CleanUp()
{	
	//clean importers
	return true;
}

bool ModuleImporter::Load(const nlohmann::json::iterator & it)
{
	//set ids for engine resources. Since they dont have meta and we want them to always have same id, we get id from Settings.
	texture->checkers = (*it)["checkers"];
	mesh->cube = (*it)["cube"];
	mesh->sphere = (*it)["sphere"];
	mesh->cylinder = (*it)["cylinder"];
	mesh->cone = (*it)["cone"];
	mesh->plane = (*it)["plane"];

	return true;
}

bool ModuleImporter::Save(nlohmann::json & it)
{
	//set ids for engine resources. Since they dont have meta and we want them to always have same id, we get id from Settings.
	it[name] = {
		{ "checkers", texture->checkers },
		{ "cube", mesh->cube },
		{ "sphere", mesh->sphere },
		{ "cylinder", mesh->cylinder },
		{ "cone", mesh->cone },
		{ "plane", mesh->plane }
	};

	return true;
}

void ModuleImporter::ImportFile(const char * path)
{

}