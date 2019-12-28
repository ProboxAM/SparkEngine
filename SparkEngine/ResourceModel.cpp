#include "Application.h"
#include "ModuleImporter.h"
#include "ModelImporter.h"
#include "ModuleResources.h"
#include "ModelMetaFile.h"
#include "AnimationMetaFile.h"
#include "ResourceModel.h"

ResourceModel::ResourceModel(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_MODEL)
{

}


ResourceModel::~ResourceModel()
{

}

void ResourceModel::UnLoad()
{
	nodes.clear();

	for each (uint mesh in ((ModelMetaFile*)meta)->meshes)
	{
		App->resources->Get(mesh)->RemoveReference();
	}
	for each (AnimationMetaFile* anim in ((ModelMetaFile*)meta)->animations)
	{
		App->resources->Get(anim->id)->RemoveReference();
	}
}

std::string ResourceModel::GetTypeString() const
{
	return "Model";
}

void ResourceModel::ReLoad()
{
	UnLoad();
	App->importer->model->Load(this);
}

void ResourceModel::Load()
{
	App->importer->model->Load(this);
	for each (uint mesh in ((ModelMetaFile*)meta)->meshes)
	{
		App->resources->GetAndReference(mesh);
	}
	for each (AnimationMetaFile* anim in ((ModelMetaFile*)meta)->animations)
	{
		App->resources->GetAndReference(anim->id);
	}
}
