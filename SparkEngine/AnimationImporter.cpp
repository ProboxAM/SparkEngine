#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "MathGeoLib\Math\Quat.h"
#include "ResourceAnimation.h"
#include "AnimationMetaFile.h"

#include "AnimationImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")


AnimationImporter::AnimationImporter()
{
}


AnimationImporter::~AnimationImporter()
{
}

AnimationMetaFile* AnimationImporter::Import(const char * file, const aiAnimation * anim, AnimationMetaFile* meta)
{
	bool new_meta = false;
	if (!meta)
	{
		new_meta = true;
		meta = new AnimationMetaFile();
	}

	bool needs_reload = false;
	ResourceAnimation* resource = (ResourceAnimation*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_ANIMATION, new_meta? App->GenerateID():meta->id, needs_reload);

	//Load Animation info
	if (!new_meta)
	{
		resource->name = meta->name;
		resource->tick_duration = meta->end_tick - meta->start_tick;
		resource->loops = meta->loops;
		resource->start_tick = meta->start_tick;
		resource->end_tick = meta->end_tick;		
	}
	else
	{
		resource->name = anim->mName.C_Str();
		resource->tick_duration = anim->mDuration;
		resource->loops = false;
		resource->start_tick = 0;
		resource->end_tick = anim->mDuration;

		meta->name = resource->name;
		meta->start_tick = 0;
		meta->end_tick = resource->tick_duration;
		meta->loops = false;
		meta->id = resource->GetID();
		meta->max_tick = anim->mDuration;
	}
	resource->ticks_per_second = anim->mTicksPerSecond;
	resource->max_tick = anim->mDuration;

	//Load Channels
	resource->num_channels = anim->mNumChannels;
	resource->channels = new ResourceAnimation::Channel[resource->num_channels];
	uint start_tick = meta->loaded ? meta->start_tick : 0;
	for (uint i = 0; i < resource->num_channels; i++)
	{
		ResourceAnimation::Channel channel;
		channel.name = anim->mChannels[i]->mNodeName.C_Str();

		//Load position keys
		channel.num_position_keys = anim->mChannels[i]->mNumPositionKeys;
		channel.position_keys = new float3Key[channel.num_position_keys];

		for (uint j = 0; j < channel.num_position_keys; j++)
		{
			float3 position = float3(anim->mChannels[i]->mPositionKeys[j].mValue.x, anim->mChannels[i]->mPositionKeys[j].mValue.y,
				anim->mChannels[i]->mPositionKeys[j].mValue.z);
			double value = anim->mChannels[i]->mPositionKeys[j].mTime;
			float3Key key = float3Key(position, value);

			memcpy(&channel.position_keys[j], &key, sizeof(float3Key));
		}

		//Load scaling keys
		channel.num_scale_keys = anim->mChannels[i]->mNumScalingKeys;
		channel.scale_keys = new float3Key[channel.num_scale_keys];

		for (uint j = 0; j < channel.num_scale_keys; j++)
		{
			float3 scale = float3(anim->mChannels[i]->mScalingKeys[j].mValue.x, anim->mChannels[i]->mScalingKeys[j].mValue.y,
				anim->mChannels[i]->mScalingKeys[j].mValue.z);
			double value = anim->mChannels[i]->mScalingKeys[j].mTime;
			float3Key key = float3Key(scale, value);

			memcpy(&channel.scale_keys[j], &key, sizeof(float3Key));
		}

		//Load rotation keys
		channel.num_rotation_keys = anim->mChannels[i]->mNumRotationKeys;
		channel.rotation_keys = new quatKey[channel.num_rotation_keys];

		for (uint j = 0; j < channel.num_rotation_keys; j++)
		{
			Quat rotation = { anim->mChannels[i]->mRotationKeys[j].mValue.x, anim->mChannels[i]->mRotationKeys[j].mValue.y,
				anim->mChannels[i]->mRotationKeys[j].mValue.z, anim->mChannels[i]->mRotationKeys[j].mValue.w };
			double value = anim->mChannels[i]->mRotationKeys[j].mTime;
			quatKey key = quatKey(rotation, value);

			memcpy(&channel.rotation_keys[j], &key, sizeof(quatKey));
		}

		resource->channels[i] = channel;
	}

	SaveAnimation(resource);
	if (!needs_reload) 
		resource->UnLoad();
	resource->SetFile(file);
	meta->loaded = true;

	return meta;
}

bool AnimationImporter::SaveAnimation(ResourceAnimation* anim)
{
	uint size = sizeof(uint) + anim->name.size() + sizeof(uint) * 6;
	for (uint i = 0; i < anim->num_channels; i++)
	{
		size += sizeof(uint) + anim->channels[i].name.size() + sizeof(uint) * 3 +
			sizeof(float3Key) * anim->channels[i].num_position_keys + sizeof(float3Key) * anim->channels[i].num_scale_keys + sizeof(quatKey) * anim->channels[i].num_rotation_keys;
	}
	// Allocate
	char* data = new char[size];
	char* cursor = data;

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size = anim->name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes = anim->name.size();
	memcpy(cursor, anim->name.c_str(), bytes);
	cursor += bytes;

	// Store duration, ticks and num channels
	bytes = sizeof(uint);
	memcpy(cursor, &anim->start_tick, bytes);
	cursor += bytes;
	memcpy(cursor, &anim->end_tick, bytes);
	cursor += bytes;
	memcpy(cursor, &anim->tick_duration, bytes);
	cursor += bytes;
	memcpy(cursor, &anim->ticks_per_second, bytes);
	cursor += bytes;
	memcpy(cursor, &anim->max_tick, bytes);
	cursor += bytes;
	memcpy(cursor, &anim->num_channels, bytes);
	cursor += bytes;

	// Store channels
	for (uint i = 0; i < anim->num_channels; i++)
	{
		//Store channel name size and name
		bytes = sizeof(uint);
		name_size = anim->channels[i].name.size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = anim->channels[i].name.size();
		memcpy(cursor, anim->channels[i].name.c_str(), bytes);
		cursor += bytes;

		//Store num position, scale, rotation keys
		bytes = sizeof(uint);
		memcpy(cursor, &anim->channels[i].num_position_keys, bytes);
		cursor += bytes;
		memcpy(cursor, &anim->channels[i].num_scale_keys, bytes);
		cursor += bytes;
		memcpy(cursor, &anim->channels[i].num_rotation_keys, bytes);
		cursor += bytes;

		//Store position keys
		bytes = sizeof(float3Key) * anim->channels[i].num_position_keys;
		memcpy(cursor, anim->channels[i].position_keys, bytes);
		cursor += bytes;

		//Store scale keys
		bytes = sizeof(float3Key) * anim->channels[i].num_scale_keys;
		memcpy(cursor, anim->channels[i].scale_keys, bytes);
		cursor += bytes;

		//Store rotation keys
		bytes = sizeof(quatKey) * anim->channels[i].num_rotation_keys;
		memcpy(cursor, anim->channels[i].rotation_keys, bytes);
		cursor += bytes;
	}

	std::string file = LIBRARY_ANIMATION_FOLDER;
	file += std::to_string(anim->GetID()) + ANIM_EXTENSION;
	uint ret = App->fsystem->Save(file.c_str(), data, size);
	RELEASE_ARRAY(data);

	anim->SetExportedFile(file);

	return true;
}

bool AnimationImporter::Load(ResourceAnimation* resource)
{
	char* buffer;
	uint size = App->fsystem->Load(resource->GetExportedFile(), &buffer);
	char* cursor = buffer;

	//Load name size
	uint bytes = sizeof(uint);
	uint name_size;
	memcpy(&name_size, cursor, bytes);
	cursor += bytes;

	//Load name
	bytes = name_size;
	resource->name.resize(bytes);
	memcpy(&resource->name[0], cursor, bytes);
	cursor += bytes;

	//Load channel nums, duration and ticks per second
	bytes = sizeof(uint);
	memcpy(&resource->start_tick, cursor, bytes);
	cursor += bytes;
	memcpy(&resource->end_tick, cursor, bytes);
	cursor += bytes;
	memcpy(&resource->tick_duration, cursor, bytes);
	cursor += bytes;
	memcpy(&resource->ticks_per_second, cursor, bytes);
	cursor += bytes;
	memcpy(&resource->max_tick, cursor, bytes);
	cursor += bytes;
	memcpy(&resource->num_channels, cursor, bytes);
	cursor += bytes;

	resource->channels = new ResourceAnimation::Channel[resource->num_channels];
	// Load channels
	for (uint i = 0; i < resource->num_channels; i++)
	{
		//Load channel name size
		bytes = sizeof(uint);
		name_size;
		memcpy(&name_size, cursor, bytes);
		cursor += bytes;

		//Load name
		bytes = name_size;
		resource->channels[i].name.resize(bytes);
		memcpy(&resource->channels[i].name[0], cursor, bytes);
		cursor += bytes;

		//Load num position, scale, rotation keys
		bytes = sizeof(uint);
		memcpy(&resource->channels[i].num_position_keys, cursor, bytes);
		cursor += bytes;
		memcpy(&resource->channels[i].num_scale_keys, cursor, bytes);
		cursor += bytes;
		memcpy(&resource->channels[i].num_rotation_keys, cursor, bytes);
		cursor += bytes;

		//Load position keys
		bytes = sizeof(float3Key) * resource->channels[i].num_position_keys;
		resource->channels[i].position_keys = new float3Key[resource->channels[i].num_position_keys];
		memcpy(resource->channels[i].position_keys, cursor, bytes);
		cursor += bytes;

		//Load scale keys
		bytes = sizeof(float3Key) * resource->channels[i].num_scale_keys;
		resource->channels[i].scale_keys = new float3Key[resource->channels[i].num_scale_keys];
		memcpy(resource->channels[i].scale_keys, cursor, bytes);
		cursor += bytes;

		//Load rotation keys
		bytes = sizeof(quatKey) * resource->channels[i].num_rotation_keys;
		resource->channels[i].rotation_keys = new quatKey[resource->channels[i].num_rotation_keys];
		memcpy(resource->channels[i].rotation_keys, cursor, bytes);
		cursor += bytes;
	}

	LOG("Loaded Animation %s", resource->GetExportedFile());

	RELEASE_ARRAY(buffer);

	return resource;
}
