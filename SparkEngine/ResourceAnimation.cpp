#include "ResourceAnimation.h"


ResourceAnimation::ResourceAnimation(uint id): Resource(id, Resource::RESOURCE_TYPE::R_ANIMATION)
{
}

ResourceAnimation::~ResourceAnimation()
{
}


std::string ResourceAnimation::GetTypeString() const
{
	return "Animation";
}