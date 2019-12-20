#include "Application.h"
#include "BoneImporter.h"

#include "ResourceBone.h"



ResourceBone::ResourceBone(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_BONE)
{
}


ResourceBone::~ResourceBone()
{
}

std::string ResourceBone::GetTypeString() const
{
	return "Bone";
}

void ResourceBone::Load()
{

}

void ResourceBone::UnLoad()
{

}
