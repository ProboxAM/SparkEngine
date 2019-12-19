#ifndef _RESOURCE_ANIMATION_H_
#define _RESOURCE_ANIMATION_H_

#include "Resource.h"
#include <map>
#include "Key.h"

class ResourceAnimation : public Resource
{
public:
	struct Channel {
		std::string name = "";
		uint num_position_keys = 0;
		uint num_scale_keys = 0;
		uint num_rotation_keys = 0;

		float3Key* position_keys;
		float3Key* scale_keys;
		quatKey* rotation_keys;
	};

	struct AnimationMetaFile : public MetaFile
	{
		bool loops = false;
	};

public:

	ResourceAnimation(uint id);
	~ResourceAnimation();

	std::string GetTypeString() const;

public:
	std::string name = "";
	uint tick_duration = 0;
	uint ticks_per_second = 0;

	uint num_channels = 0;
	Channel* channels;
};


#endif // !_RESOURCE_ANIMATION_H_


