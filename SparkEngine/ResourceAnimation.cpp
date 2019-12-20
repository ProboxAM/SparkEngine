#include "Application.h"
#include "ModuleImporter.h"
#include "AnimationImporter.h"
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

void ResourceAnimation::UnLoad()
{
	LOG("Unloading Animation %s", name.c_str());

	for (int i = 0; i < num_channels; i++)
	{
		delete[] channels[i].position_keys;
		delete[] channels[i].scale_keys;
		delete[] channels[i].rotation_keys;
	}
	channels = nullptr;
}

void ResourceAnimation::Load()
{
	App->importer->animation->Load(this);
}