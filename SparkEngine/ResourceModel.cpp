#include "Application.h"
#include "ModuleImporter.h"
#include "ModelImporter.h"
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
}
